/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)

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
		#define Middleware_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define Middleware_API __declspec(dllexport)
	#endif
#endif

#include "BoostStdTimeConverter.h"
#include "SendStatusMessage.h"
#include "AlarmMessage.h"
#include "AudioSettings.h"
#include "ExecutionRuntime.h"



/**	@brief		Constructor
*	@param		newParams							Parameter for the detection / sever operation
*	@param		appSettingsDir						Directory containing the basic general (audio, ...) settings files
*	@param		runtimeErrorCallback				Function called if an error occurs during the execution in one of the threads (and the thread has finished silently)
*	@param		messageFromDetectorCallback			Function called if the detector emits a message that should be logged by the caller. Ensure that this function returns fast not to block the thread.
*	@exception 										None
*	@remarks 										This function is the only public function and controls the whole functionality of the class
*/
Middleware::CExecutionRuntime::CExecutionRuntime(const CSettingsParam& newParams, const boost::filesystem::path& appSettingsDir, std::function<void(const std::string&)> runtimeErrorCallback, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> messageFromDetectorCallback )
	: gateways( messageFromDetectorCallback, std::bind( &CExecutionRuntime::OnRuntimeException, this, std::placeholders::_1 ) ),
	  runtimeErrorCallback( runtimeErrorCallback ),
	  statusMessageCallback( messageFromDetectorCallback )
{
	using namespace std;

	External::CAlarmMessageDatabase alarmMessagesDatabase;
	External::CGatewayLoginDatabase loginDatabase;

	// set new parameters
	CExecutionRuntime::params = newParams;
	CExecutionRuntime::appSettingsDir = appSettingsDir;

	// initialize the alarm gateways
	params.GetGatewaySettings( loginDatabase, alarmMessagesDatabase );	
	gateways.ResetGatewayLoginDatabase( loginDatabase );
	gateways.ResetAlarmMessagesDatabase( alarmMessagesDatabase );
}



/**	@brief		Destructor
*	@remarks 										None
*/
Middleware::CExecutionRuntime::~CExecutionRuntime()
{
}



/**	@brief		Obtain the set parameters
*	@return 										Currently valid parameter set
*	@exception 										None
*	@remarks 										None
*/
Middleware::CSettingsParam Middleware::CExecutionRuntime::GetParams()
{
	return params;
}



/**	@brief		Basic processing of sequences (identical on detectors and servers)
*	@param		sequenceData						Data of the new FME sequence
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void Middleware::CExecutionRuntime::BasicSequenceProcessing( const Utilities::CSeqData& sequenceData ) {
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace External;
	using namespace Utilities::Message;
	using namespace Utilities::Time;

	Core::Processing::CAudioDevice device;
	bool isPlayTone, isDefaultRecording;
	float recordingLength, minDistanceRepetition, deleteTimespan;
	ptime time;
	vector<int> sequenceCode;
	vector< vector<int> > sequencesWhitelist;
	tuple< ptime, vector<int> > previousSequence;
	string audioFormatID;
	stringstream ss, timeStream;
	SendStatus status;

	GetParams().GetRecordingSettings( recordingLength, isDefaultRecording, audioFormatID );
	GetParams().GetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );
	sequencesWhitelist = GetParams().GetWhitelist();

	try {
		time = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( sequenceData.GetStartTime() );
	} catch (std::out_of_range e) {
		// set to "not a date / time"
		time = ptime();
	}

	// store the current sequence
	std::unique_lock<std::mutex> lockLastSequence( lastSequenceMutex );
	previousSequence = lastSequence;
	lastSequence = std::make_tuple( time, sequenceData.GetCode() );
	lockLastSequence.unlock();
	
	// delete all too old sequences from blacklist
	std::unique_lock<std::mutex> lockBlacklist( sequencesBlacklistMutex );
	deleteTimespan = max( minDistanceRepetition, recordingLength + CAudioSettings::GetRecordTimeLowerLimit() ); // the timespan starts with the detection of the sequence
	auto newEndIt = remove_if( begin( sequencesBlacklist ), end( sequencesBlacklist ), [=]( auto sequence ){
		return ( ( time - get<0>( sequence ) ) > microseconds( static_cast<long>( 2 * deleteTimespan * 1.0e6 ) ) );
	} );
	sequencesBlacklist.erase( newEndIt, sequencesBlacklist.end() );

	// suppress repetitions if they are too close to each other
	sequenceCode = sequenceData.GetCode();
	if ( ( get<1>( previousSequence ) == sequenceCode ) && ( ( time - get<0>( previousSequence ) ) < microseconds( static_cast<long>( minDistanceRepetition * 1.0e6 ) ) ) ) {
		sequencesBlacklist.push_back( make_tuple( time, sequenceData.GetCode() ) ); // entry to blacklist due to too close repetitions
		return;
	}

	// determine if the found sequence is allowed according to the user set whitelist - proceed only if the sequence is on the whitelist or if the whitelist is empty (i.e. any sequences are allowed)
	auto itFound = find( begin( sequencesWhitelist ), end( sequencesWhitelist ), sequenceCode );
	if ( ( itFound == end( sequencesWhitelist ) ) && ( !sequencesWhitelist.empty() ) ) {
		sequencesBlacklist.push_back( std::make_tuple( time, sequenceData.GetCode() ) ); // entry to blacklist due to a sequence not allowed to be stored
		return;
	}
	lockBlacklist.unlock();

	// user-defined processing
	onFoundSequenceCallback( sequenceData );

	// sending out the alarms via the gateway (state: directly after the detection)
	try {
		gateways.Send( sequenceData.GetCode(), sequenceData.GetStartTime(), Utilities::CMediaFile(), true );
	} catch ( std::logic_error e ) {
		status.code = NOT_IN_DB;
		OnStatusMessage( make_unique<CSendStatusMessage<CAlarmMessage> >( ptime( microsec_clock::universal_time() ), sequenceData.GetStartTime(), sequenceData.GetCode(), nullptr, status, 1, 0s ) );
	} catch ( std::range_error e ) {
		status.code = NO_MESSAGE;
		OnStatusMessage( make_unique<CSendStatusMessage<CAlarmMessage> >( ptime( microsec_clock::universal_time() ), sequenceData.GetStartTime(), sequenceData.GetCode(), nullptr, status, 1, 0s ) );
	}
}


/**	@brief		Receives all exceptions from std::threads that occur during running and require abortion
*	@param		error								Occured exception
*	@return 										None
*	@exception 										None
*	@remarks 										The method is a wrapper for compatibility with the boost::thread error handling
*/
void Middleware::CExecutionRuntime::OnRuntimeException( const std::exception_ptr& error )
{
	std::string errorString;

	try {
		std::rethrow_exception( error );
	} catch ( std::exception& e ) {
		errorString = e.what();
	}

	OnRuntimeError( errorString );
}


/**	@brief		Receives all exceptions from boost::threads that occur during running and require abortion 
*	@param		errorString							Description of the occured exception
*	@return 										None
*	@exception 										None
*	@remarks 										The function transfers the signal further to the caller
*/
void Middleware::CExecutionRuntime::OnRuntimeError(const std::string& errorString) {
	// signal the error to the caller
	runtimeErrorCallback( errorString );
}



/**	@brief		Receives all messages
*	@param		message								Emitted message
*	@return 										None
*	@exception 										None
*	@remarks 										The function transfers the signal further to the caller
*/
void Middleware::CExecutionRuntime::OnStatusMessage( std::unique_ptr<Utilities::Message::CStatusMessage> message )
{
	statusMessageCallback( std::move( message ) );
}
