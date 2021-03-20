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

#include <boost/date_time/posix_time/ptime.hpp>
#include "ConnectionManager.h"


/**	@brief		Constructor
*	@param		gateway								Instance of the required gateway. The gateway needs to be clonable.
*	@param		login								Login for the required gateway. The caller must ensure that it is of the correct type for the present gateway.
*	@param		statusCallback						Callback function for sending the status change informations for the messages. Ensure that this function returns fast not to block the thread.
*	@param		exceptionCallback					Callback function which is called when an exception has occured in the thread (and the thread has finished silently)
*	@exception	std::runtime_error					Thrown if one of the input parameters contains an invalid pointer
*	@remarks 										None
*/
External::CConnectionManager::CConnectionManager( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> login, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback, std::function<void( const std::exception_ptr& )> exceptionCallback )
	: login( std::move( login ) ),
	isResumeThread( false ),
	statusCallback( statusCallback ),
	isTerminateThread( false ),
	exceptionCallback( exceptionCallback )
{
	unsigned int numTrials, maxNumConnections;
	float timeDistTrial;

	if ( ( CConnectionManager::login == nullptr ) || ( statusCallback == nullptr ) || ( exceptionCallback == nullptr ) ) {
		throw std::runtime_error( "Nullpointers are not allowed as parameters." );
	}

	CConnectionManager::login->GetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );

	// construct the connections
	for ( size_t ID = 0; ID < maxNumConnections; ID++ ) {
		availableConnections.push_back( std::make_unique< CConnectionThread >( gateway->Clone(), std::bind( &CConnectionManager::OnConnectionFinished, this ), exceptionCallback ) );
	}

	managerThread = std::thread( [&]() { CConnectionManager::ManagerThread(); } );
}


/**	@brief		Destructor
*	@remarks 										None
*/
External::CConnectionManager::~CConnectionManager()
{
	{
		std::lock_guard<std::mutex> lock( managerThreadMutex );
		isTerminateThread = true;
		isResumeThread = true;
	}
	managerThreadWaitCondition.notify_all();

	managerThread.join();
}


/**	@brief		Adds a message to the queue of the connection manager
*	@param		newSequence							FME-sequence code, for which alarm sending is required
*	@param		time								Detection time of the code (UTC)
*	@param		isRealAlarm							Flag stating if this is a real (default) or test alarm
*	@param		alarmMessageDataset					Alarm message dataset. The caller must ensure that it is of the correct type for the present gateway.
*	@param		audioFile							Audio file of the alarm message
*	@return 										None
*	@exception 	std::runtime_error					Thrown if either "time" or "alarmMessageDataset" is empty
*	@remarks 										The message will be automatically deleted. It will be finished when either the sending was successful or if it has been finally aborted.
*/
void External::CConnectionManager::AddMessage( const std::vector<int>& newSequence, const Utilities::CDateTime & time, const bool & isRealAlarm, std::unique_ptr<CAlarmMessage> alarmMessageDataset, const Utilities::CMediaFile& audioFile )
{
	using namespace std;

	if ( ( !time.IsValid() ) || ( alarmMessageDataset == nullptr ) ) {
		throw std::runtime_error( "Input parameters time or alarmMessageDataset are empty." );
	}

	unique_ptr<Message> newMessage{ new Message };

	newMessage->sequence = newSequence;
	newMessage->time = time;
	newMessage->isRealAlarm = isRealAlarm;
	newMessage->message = move( alarmMessageDataset );
	newMessage->login = login->Clone();
	newMessage->audioFile = audioFile;

	{
		lock_guard<mutex> lock( managerThreadMutex );
		messageQueue[chrono::steady_clock::now()] = make_pair( move( newMessage ), 0 );
		isResumeThread = true;
	}
	managerThreadWaitCondition.notify_all();
}


/**	@brief		Callback method signalling that a connection thread has finished its sending trial
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void External::CConnectionManager::OnConnectionFinished()
{
	{
		std::lock_guard<std::mutex> lock( managerThreadMutex );
		isResumeThread = true;
	}
	managerThreadWaitCondition.notify_all();
}


/**	@brief		Regains the finished connections within the manager thread
*	@param		maxNumTrials						Maximum number of connections trials until the message sending is assumed to have failed
*	@param		timeDistTrials						Time between the connection trials
*	@return 										None
*	@exception 										None
*	@remarks 										This method is not thread-safe by itself
*/
void External::CConnectionManager::RegainFinishedConnections( const unsigned int& maxNumTrials, const std::chrono::milliseconds& timeDistTrials )
{
	using namespace std;
	using namespace std::chrono;
	using namespace boost::posix_time;
	using namespace Utilities::Message;

	SendStatus status;
	Utilities::CDateTime time;
	unique_ptr<Message> currMessage;
	unique_ptr<External::CAlarmMessage> currAlarmMessage;
	time_point<steady_clock> currMessageTimepoint;
	unique_ptr<CConnectionThread> currConnection;
	vector<int> code;
	unsigned int currNumTrials;

	for ( auto&& connection : unavailableConnections ) {
		connection->GetStatus( status, time, code, currNumTrials, currMessage );
		if ( status.code != IN_PROCESSING ) {
			// make the connection again available
			currConnection = move( connection );
			availableConnections.push_back( move( currConnection ) );

			// put the message back to the queue if a non-fatal failure occured (which implies that the number of trials has not yet been exceeded)
			currAlarmMessage = currMessage->message->Clone();
			if ( status.code == NONFATAL_FAILURE ) {
				if ( currNumTrials < maxNumTrials ) {
					messageQueue[steady_clock::now() + timeDistTrials] = make_pair( move( currMessage ), currNumTrials );
				} else {
					status.code = TIMEOUT_FAILURE;
				}
			}

			// send the status information to the caller
			statusCallback( make_unique<CSendStatusMessage<CAlarmMessage> >( ptime( microsec_clock::universal_time() ), time, code, move( currAlarmMessage ), status, currNumTrials, duration_cast<chrono::seconds>( timeDistTrials ) ) );
		}
	}

	// remove all regained connections from the queue
	unavailableConnections.erase( remove_if( begin( unavailableConnections ), end( unavailableConnections ), [&]( auto&& connection ) { return ( connection == nullptr ); } ), end( unavailableConnections ) );
}


/**	@brief		Sends the next due message (i.e. the first message within the queue) within the manager thread
*	@return 										Flag stating true if no available connection was found, false otherwise
*	@exception 										None
*	@remarks 										This method is not thread-safe by itself
*/
bool External::CConnectionManager::SendNextDueMessage()
{
	using namespace std;
	using namespace std::chrono;

	bool isNoConnectionsAvailable = false;
	unsigned int currNumTrials;
	unique_ptr<Message> currMessage;
	unique_ptr<CConnectionThread> currConnection;
	time_point<steady_clock> currMessageTimepoint;

	if ( !messageQueue.empty() ) {
		currMessageTimepoint = begin( messageQueue )->first;
		if ( ( steady_clock::now() - currMessageTimepoint ) >= 0ms ) { // ensures that preemptive wake-ups of the manager thread (due to finish message sending messages) have no effect
			if ( !availableConnections.empty() ) {
				currMessage = move( get< unique_ptr<Message> >( messageQueue.at( currMessageTimepoint ) ) );
				currNumTrials = get<unsigned int>( messageQueue.at( currMessageTimepoint ) );
				messageQueue.erase( currMessageTimepoint );

				// obtain a connection thread from the list of available threads
				currConnection = move( availableConnections.front() );
				availableConnections.pop_front();
				currConnection->SendMessage( move( currMessage ), currNumTrials ); // returns immediately because it is ensured that always the connection is only used once at a time
				unavailableConnections.push_back( move( currConnection ) );
			} else {
				isNoConnectionsAvailable = true; // wait until a connection has been finished
			}
		}
	}

	return isNoConnectionsAvailable;
}


/**	@brief		Thread managing the connections
*	@return 										None
*	@exception 										None
*	@remarks 										This thread is continuously running
*/
void External::CConnectionManager::ManagerThread()
{
	using namespace std;
	using namespace std::chrono;

	float timeDistTrialsFloat;
	chrono::milliseconds timeDistTrials;
	unsigned int maxNumTrials, maxNumConnections;
	nanoseconds currSleepDuration;
	bool isNoConnectionsAvailable = false;

	try {
		login->GetConnectionTrialInfos( maxNumTrials, timeDistTrialsFloat, maxNumConnections );
		timeDistTrials = chrono::milliseconds( static_cast<long long>( timeDistTrialsFloat * 1000 ) );

		while ( true ) {
			// sleeping of the thread (the thread will be reactivated if either the next message is due, a new message has been added, a connection thread has finished or in case of terminating)
			std::unique_lock<mutex> lock( managerThreadMutex );
			if ( !messageQueue.empty() && !isNoConnectionsAvailable ) {
				currSleepDuration = begin( messageQueue )->first - steady_clock::now();
				managerThreadWaitCondition.wait_for( lock, currSleepDuration, [=]() { return isResumeThread; } );
			} else {
				managerThreadWaitCondition.wait( lock, [=]() { return isResumeThread; } );
			}

			isResumeThread = false;
			if ( isTerminateThread ) {
				break;
			}

			// regain finished (i.e. again available) connections
			RegainFinishedConnections( maxNumTrials, timeDistTrials );

			// send the currently due message (i.e. the first in the queue)
			isNoConnectionsAvailable = SendNextDueMessage();
		}
	} catch ( ... ) {
		// inform the caller about the exception before terminating the thread
		exceptionCallback( current_exception() );
	}
}
