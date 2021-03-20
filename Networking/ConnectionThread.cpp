/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>
*/
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include "ConnectionThread.h"


/**	@brief		Constructor
*	@param		connection							Object containing the gateway connection that will be used by the thread. It will be persistent during the whole lifetime of the object.
*	@param		finishedSendingCallback				Callback function which is called whenever the connection has finished a sending trial
*	@param		exceptionCallback					Callback function which is called when an exception has occured in the thread (and the thread has finished silently)
*	@exception	std::runtime_error					Thrown if one of input parameters is an invalid pointer
*	@remarks 										None
*/
External::CConnectionThread::CConnectionThread( std::unique_ptr<CAlarmGateway> connection, std::function<void()> finishedSendingCallback, std::function<void( const std::exception_ptr& )> exceptionCallback )
	: connection( std::move( connection ) ),
	isNewMessageReceived( false ),
	isTerminateThread( false ),
	finishedSendingCallback( finishedSendingCallback ),
	exceptionCallback( exceptionCallback )
{
	if ( CConnectionThread::connection == nullptr ) {
		throw std::runtime_error( "The connection must not be a nullpointer." );
	}
	if ( ( finishedSendingCallback == nullptr ) || ( exceptionCallback == nullptr ) ) {
		throw std::runtime_error( "The callback functions must not be empty." );
	}

	connectionThread = std::thread( [&]() { CConnectionThread::ConnectionThread(); } );
}


/**	@brief		Destructor
*	@remarks 										None
*/
External::CConnectionThread::~CConnectionThread()
{
	{
		std::lock_guard<std::mutex> lock( messageProcessingMutex );
		isTerminateThread = true;
		isNewMessageReceived = true;
	}
	newMessageReceived.notify_all();

	connectionThread.join();
}


/**	@brief		Sends a new message asychronously
*	@param		message								Message to be sent
*	@param		numTrials							Number of the already performed sending trials
*	@return 										None
*	@exception 	std::runtime_errorr					Thrown if the message is empty
*	@remarks 										The method usually returns immediately. But if another message is currently processed, it will be blocked until the other message has been processed.
*/
void External::CConnectionThread::SendMessage( std::unique_ptr<Message> message, const unsigned int& numTrials )
{
	if ( message == nullptr ) {
		throw std::runtime_error( "The message is empty." );
	}

	{
		std::lock( messageProcessingMutex, dataMutex );
		std::lock_guard<std::mutex> lock( messageProcessingMutex, std::adopt_lock );
		std::lock_guard<std::mutex> dataLock( dataMutex, std::adopt_lock );
		CConnectionThread::message = std::move( message );
		isNewMessageReceived = true;
		status.code = Utilities::Message::IN_PROCESSING;
		status.text.clear();
		sequence = CConnectionThread::message->sequence;
		time = CConnectionThread::message->time;
		isRealAlarm = CConnectionThread::message->isRealAlarm;
		messageData = CConnectionThread::message->message->Clone();
		login = CConnectionThread::message->login->Clone();
		audioFile = CConnectionThread::message->audioFile;
		CConnectionThread::numTrials = numTrials + 1;
	}
	newMessageReceived.notify_all();
}


/**	@brief		Obtains the status of the current or last sent message
*	@param		status								Status of the current or last sent message
*	@param		time								Start time of the sequence for which the message is sent
*	@param		sequence							Code of the sequence for which the message is sent
*	@param		numTrials							Number of finished sending trials (currently running sending trials are not counted)
*	@param		currMessage							Current message
*	@return 										True if the last message has already been sent, if it is currently sent the return value is false
*	@exception 										None
*	@remarks 										None
*/
bool External::CConnectionThread::GetStatus( Utilities::Message::SendStatus& status, Utilities::CDateTime& time, std::vector<int>& sequence, unsigned int& numTrials, std::unique_ptr<Message>& currMessage )
{
	std::lock_guard<std::mutex> dataLock( dataMutex );
	status = CConnectionThread::status;
	time = CConnectionThread::time;
	sequence = CConnectionThread::sequence;
	numTrials = CConnectionThread::numTrials;

	currMessage = std::make_unique<Message>();
	currMessage->audioFile = CConnectionThread::audioFile;
	currMessage->isRealAlarm = CConnectionThread::isRealAlarm;
	currMessage->login = CConnectionThread::login->Clone();
	currMessage->message = CConnectionThread::messageData->Clone();
	currMessage->sequence = CConnectionThread::sequence;
	currMessage->time = CConnectionThread::time;

	if ( status.code == Utilities::Message::IN_PROCESSING ) {
		return false;
	} else {
		return true;
	}
}


/**	@brief		Thread implementing the access to the network resource
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void External::CConnectionThread::ConnectionThread()
{
	using namespace std;
	using namespace Utilities::Message;

	SendStatus currStatus;

	try {
		while ( true ) {
			unique_lock<mutex> lock( messageProcessingMutex );
			newMessageReceived.wait( lock, [=]() { return isNewMessageReceived; } );
			isNewMessageReceived = false;
			if ( isTerminateThread ) {
				break;
			}

			try {
				// send the alarm via the gateway
				connection->Send( message->sequence, message->time, message->isRealAlarm, message->login->Clone(), message->message->Clone(), message->audioFile ); // the function can throw exceptions (std::domain_error in case of missing internet connection)
				currStatus.code = SUCCESS;
				currStatus.text = "";
			} catch ( std::domain_error& e ) {
				// internet connection error
				currStatus.code = NONFATAL_FAILURE;		// non-fatal error - further trials are possible
				currStatus.text = e.what();
			} catch ( std::exception& e ) {
				// any other error
				currStatus.code = FATAL_FAILURE;		// fatal error - stop trials immediately
				currStatus.text = e.what();
			}

			{
				lock_guard<mutex> dataLock( dataMutex );
				status = currStatus;
			}

			callbackFutures.erase( remove_if( begin( callbackFutures ), end( callbackFutures ), []( auto&& currFuture ) { return( currFuture.wait_for( 0s ) == future_status::ready ); } ), callbackFutures.end() );
			callbackFutures.push_back( async( launch::async, finishedSendingCallback ) );
		}
	} catch ( ... ) {
		// inform the caller about the exception before terminating the thread
		exceptionCallback( current_exception() );
	}
}
