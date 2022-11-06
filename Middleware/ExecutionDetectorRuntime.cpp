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

#include <chrono>
#include <boost/filesystem.hpp>
#include "FileSettings.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"
#include "FMEAudioInput.h"
#include "AudioSettings.h"
#include "DetectorStatusMessage.h"
#include "GeneralStatusMessage.h"
#include "PluginLoader.h"
#include "ExecutionDetectorRuntime.h"



/**	@brief		Constructor
*	@param		newParams							Parameter for the detection operation
*	@param		appSettingsDir						Directory containing the basic general (audio, ...) settings files
*	@param		audioDir							Directory where the audio files with the recorded sequences should be stored
*	@param		pluginDir							Directory containing the audio plugins
*	@param		onFoundSequenceCallback				Function called if a sequence has been detected
*	@param		onRecordedDataCallback				Function called if audio sequence data has been completed
*	@param		runtimeErrorCallback				Function called if an error occurs during the execution of the detection
*	@param		messageFromDetectorCallback			Function called if the detector emits a message that should be logged by the caller
*	@exception 	std::runtime_error					Thrown if the setting of the configuration failed
*	@remarks 										This function is the only public function and controls the whole functionality of the class
*/
Middleware::CExecutionDetectorRuntime::CExecutionDetectorRuntime(const CSettingsParam& newParams, const boost::filesystem::path& appSettingsDir, const boost::filesystem::path& audioDir, const boost::filesystem::path& pluginDir, std::function<void(const Utilities::CSeqData&)> onFoundSequenceCallback, std::function < void( const Utilities::CSeqData&, const Utilities::CMediaFile& ) > onRecordedDataCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> messageFromDetectorCallback )
	: CExecutionRuntime( newParams, appSettingsDir, runtimeErrorCallback, messageFromDetectorCallback ),
	  isInit( false )
{
	using namespace std;
	using namespace Utilities::Message;
	using namespace Utilities::Plugins;
	using namespace boost::posix_time;

	Core::Processing::CAudioDevice device;
	bool isDefaultRecording, isPlayTone;
	string audioFormatID;
	float recordingLength, minDistanceRepetition;
	shared_ptr<Core::RecordingParam> recordingParams;
	map< string, std::unique_ptr<AudioPlugin> > plugins;

	// obtain settings
	CExecutionRuntime::GetParams().GetRecordingSettings( recordingLength, isDefaultRecording, audioFormatID );
	CExecutionRuntime::GetParams().GetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );

	CExecutionRuntime::onFoundSequenceCallback = onFoundSequenceCallback;
	CExecutionDetectorRuntime::onRecordedDataCallback = onRecordedDataCallback;
	CExecutionDetectorRuntime::audioDir = audioDir;

	// start audio device - it is assumed that the audio settings file and all connected settings files are given relative to the subfolder in the Windows application specific folder
	recordingParams = make_shared<Core::RecordingParam>();
	recordingParams->recordedCallback = bind( &CExecutionDetectorRuntime::OnRecordedData, this, placeholders::_1, placeholders::_2, placeholders::_3 );
	recordingParams->recordTimeBuffer = CAudioSettings::GetRecordTimeBuffer();
	recordingParams->recordTimeUpperLimit = recordingLength + CAudioSettings::GetRecordTimeLowerLimit();
	recordingParams->recordTimeLowerLimit = CAudioSettings::GetRecordTimeLowerLimit();
	recordingParams->reqStoringSamplingFreq = CAudioSettings::GetStoringSamplingFreq();

	try {
		auto refOnFoundSequence = bind( &CExecutionDetectorRuntime::OnProcessingSequence, this, placeholders::_1 );
		auto refOnRuntimeError = bind( &CExecutionDetectorRuntime::OnRuntimeError, this, placeholders::_1 );
		if ( recordingLength > 0 ) {
			audioInput = make_unique<Core::CFMEAudioInput>( device, absolute( CAudioSettings::GetAudioSettingsFileName(), appSettingsDir ).string(), refOnFoundSequence, refOnRuntimeError, recordingParams );
		} else {
			audioInput = make_unique<Core::CFMEAudioInput>( device, absolute( CAudioSettings::GetAudioSettingsFileName(), appSettingsDir ).string(), refOnFoundSequence, refOnRuntimeError );
		}
	} catch ( std::exception& e ) {
		throw std::runtime_error( "The chosen audio device is not available. Error message: " + std::string( e.what() ) );
	}

	// load all available plugins
	plugins = LoadPlugins<AudioPlugin>( pluginDir );
	try {
		audioPlugin = move( plugins.at( AudioPlugin::CompleteID( audioFormatID ) ) );
	} catch ( std::exception e ) {
		throw std::runtime_error( "For the chosen audio format \"" + audioFormatID + "\" no plugin is available." );
	}

	isInit = true;
}



/**	@brief		Starts the detection
*	@return 										None
*	@exception 	std::runtime_error					Thrown if the detection not successfully started in the constructor
*	@exception	std::logic_error					Thrown if the detection is already running within this object
*	@remarks 										Deleting the object while this function is running may lead to undefined behaviour
*/
void Middleware::CExecutionDetectorRuntime::Run(void)
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities::Message;
	int samplingFreq;
	ptime currTime;

	// check if the initialization was successfull
	if ( !isInit ) {
		throw std::runtime_error( "Der 5-Ton-Folgen-Detektor wurde nicht korrekt gestartet." );
	}

	// start audio processing
	std::lock_guard<std::mutex> lock( audioInputMutex ); 
	audioInput->StartAudioInput();

	// Log start of detection
	samplingFreq = static_cast<int>( dynamic_cast<Core::CFMEAudioInput*>( audioInput.get() )->GetAudioCaptureSamplingRate() );
	currTime = ptime( microsec_clock::universal_time() );
	CExecutionRuntime::OnStatusMessage( make_unique<CDetectorStatusMessage>( currTime, RUNNING, samplingFreq ) );
}



/**	@brief		Destructor
*	@remarks 										None
*/
Middleware::CExecutionDetectorRuntime::~CExecutionDetectorRuntime(void)
{
	using namespace boost::posix_time;
	using namespace Utilities::Message;
	ptime currTime;

	// stop audio processing (it will return when the audio processing threads have been finished)
	std::lock_guard<std::mutex> lockAudioInput( audioInputMutex ); 
	if ( audioInput->IsRunning() ) {
		audioInput->StopAudioInput();
	}

	// Log end of detection
	currTime = ptime( microsec_clock::universal_time() );
	CExecutionRuntime::OnStatusMessage( std::make_unique<CDetectorStatusMessage>( currTime, STOPPED ) );
}



/**	@brief		Callback function for finished recording of audio data after a sequence was found
*	@param		sequence							Containing the start time and the code of the sequence, for which the recorded data is transfered
*	@param		signalData							Audio signal data recorded
*	@param		samplingFreq						Sampling frequency of the recorded audio signal data
*	@return 										None
*	@exception 	std::runtime_error					Thrown if no audio signal data is available to be stored
*	@remarks 										None
*/
void Middleware::CExecutionDetectorRuntime::OnRecordedData( const Utilities::CSeqData& sequence, std::vector<float> signalData, double samplingFreq )
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities::Time;

	vector<int> code;
	stringstream codeStream;
	ptime utcTime, localTime;
	string fileName;
	stringstream ss;
	Utilities::CMediaFile audioFile;
	vector< tuple< boost::posix_time::ptime, vector<int> > > sequencesBlacklist;

	if ( signalData.empty() ) {
		throw std::runtime_error( "No audio data available for storage on file." );
	}

	// checking if data should be stored
	utcTime = CBoostStdTimeConverter::ConvertToBoostTime( sequence.GetStartTime() );
	code = sequence.GetCode();
	CExecutionRuntime::GetCurrentBlacklist( back_inserter( sequencesBlacklist ) );
	for ( auto blacklist : sequencesBlacklist ) {
		if ( ( get<1>( blacklist ) == code ) && ( get<0>( blacklist ) == utcTime ) ) {
			return; // this sequence is on the blacklist and is therefore ignored
		}
	}

	// generate file name including date and sequence number
	localTime = german_local_date_time( CBoostStdTimeConverter::ConvertToBoostTime( sequence.GetStartTime() ) ).local_time();

	fileName += to_simple_string( localTime.date() ) + "_";
	ss << setw( 2 ) << setfill( '0' ) << localTime.time_of_day().hours();
	ss << setw( 2 ) << setfill( '0' ) << localTime.time_of_day().minutes();
	ss << setw( 2 ) << setfill( '0' ) << localTime.time_of_day().seconds();
	ss << "_" << german_local_date_time( CBoostStdTimeConverter::ConvertToBoostTime( sequence.GetStartTime() ) ).zone_abbrev();
	fileName += ss.str() + "_";

	for ( auto digit : sequence.GetCode() ) {
		codeStream << digit;
	}
	fileName += codeStream.str();
	fileName += audioPlugin->GetExtension();

	// store the audio file
	audioFile = Utilities::CMediaFile( boost::filesystem::absolute( fileName, audioDir ), audioPlugin->GetMIMEtype() );
	audioPlugin->SetSamplingFreq( static_cast<int>( samplingFreq ) );
	audioPlugin->Save( audioFile.GetFilePath().string(), begin( signalData ), end( signalData ), true ); // the loudness is amplified to the maximum value

	// sending out the alarms via the gateway (state: after recording)
	try {
		gateways.Send( sequence.GetCode(), sequence.GetStartTime(), audioFile, false );
	} catch ( std::exception e ) {
		// exceptions can be ignored here, because they have already been signalled immediately after detection
	}

	// user-defined processing of the recorded sequence
	onRecordedDataCallback( sequence, audioFile );
}



/**	@brief		Callback function for processing found sequences
*	@param		sequence							Containing the start time and the code of the  new sequence
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void Middleware::CExecutionDetectorRuntime::OnProcessingSequence(const Utilities::CSeqData& sequence) {
	// common processing of the sequence
	CExecutionRuntime::BasicSequenceProcessing( sequence );
}



/**	@brief		Obtains the available input devices at the highest possible sampling rate
*	@param		devices							Container with all available input devices at the 'maxStandardSamplingFreq'
*	@param		stdDevice						Standard input device at the 'maxStandardSamplingFreq'
*	@param		maxStandardSamplingFreq			Highest possible sampling rate [Hz] (chosen out of the standard sampling rates of the detection algorithm)
*	@param		appSettingsDir					Directory containing the basic general (audio, ...) settings files
*	@return										None
*	@exception									None
*	@remarks									The highest possible sampling rate refers to the highest required sampling rate of the detection algorithm and may be lower than that of the sound device.
*												This method can be used before initializing the object and does not interfer with any running audio stream
*/
void Middleware::GetAvailableAudioDevices( std::vector<Core::Processing::CAudioDevice>& devices, Core::Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& appSettingsDir )
{
	std::unique_ptr<Core::CAudioInput> audioInput = std::make_unique<Core::CFMEAudioInput>();
	audioInput->GetAvailableAudioDevices( devices, stdDevice, maxStandardSamplingFreq, absolute( CAudioSettings::GetAudioSettingsFileName(), appSettingsDir ) );
}



/**	@brief		Determines if the given input device is available on the current system for one of the standard sampling rates of the detection algorithm
*	@param		maxStandardSamplingFreq			Will contain the highest possible sampling rate of the device [Hz] (chosen out of the standard sampling rates of the detection algorithm). If the device is not available, it will be set to 0 Hz.
*	@param		device							Input device to be checked
*	@param		appSettingsDir					Directory containing the basic general (audio, ...) settings files
*	@return										True if the device is available, false otherwise
*	@exception									None
*	@remarks									The highest possible sampling rate refers to the highest required sampling rate of the detection algorithm and may be lower than that of the sound device.
*												This method can be used before initializing the object and does not interfer with any running audio stream
*/
bool Middleware::IsDeviceAvailable( double& maxStandardSamplingFreq, const Core::Processing::CAudioDevice& device, const boost::filesystem::path& appSettingsDir )
{
	bool isAvailable;

	std::unique_ptr<Core::CAudioInput> audioInput = std::make_unique<Core::CFMEAudioInput>();
	isAvailable = audioInput->IsDeviceAvailable( maxStandardSamplingFreq, device, absolute( CAudioSettings::GetAudioSettingsFileName(), appSettingsDir ) );

	return isAvailable;
}