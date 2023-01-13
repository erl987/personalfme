/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

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
/**	\defgroup	AudioSPTest	Test program for continuous real-time detection of 5-tone-FME-sequences with the sound card.
*/

/*@{*/
/** \ingroup AudioSPTest
*/
#if defined(_MSC_VER)
	#include "stdafx.h"
#endif
#include <csignal>
#include <iostream>
#include <fstream>
#include <locale>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "FMEAudioInputDebug.h"
#include "FMEClientDebug.h"
#include "BoostStdTimeConverter.h"
#include "SeqData.h"
#include "SeqDataComplete.h"
#include "PluginLoader.h"
#include "AudioPlugin.h"

using namespace std;
using namespace boost::filesystem;

std::mutex sequenceDataMutex;
std::condition_variable_any newFoundSequenceCondition;
bool isTerminateThread = false;
bool isNewFoundSequence = false;
deque< Utilities::CSeqDataComplete<float> > detectionData;
Networking::CFMEClientDebug client;

/**	@param finishDetectionMutex			Mutex securing the finish detection condition */
std::mutex finishDetectionMutex;

/**	@param finishDetectionCondition		Condition for controlling the user abort event during code detection */
std::condition_variable_any finishDetectionCondition;

/**	@param isFinish						Flag stating if the finish program condition is true */
bool isFinish = false;

/**	@param audioSettingsFileName		File name of audio device settings file. It is assumed that all underlying parameter files are located relative to this path. */
string audioSettingsFileName = "../audioSettings.dat";

/**	@param pluginPath					Path of the audio format plugins */
#if defined (_WIN32)
	#ifdef NDEBUG
		boost::filesystem::path pluginPath = "../Release";
	#else
		boost::filesystem::path pluginPath = "../Debug";
	#endif
#elif defined (__linux)
	// Linux
	boost::filesystem::path pluginPath = ".";
#endif

/**	@param pluginID						Type of the audio format plugin */
std::string pluginID = "WAV";

/**	@param outputFileName				Name of the output file containing the detailed information on the sequences for debugging (*.csv) */
string outputFileName = "outputFile.csv";

/**	@param	hostName					Name of server ("192.168.178.25", "localhost", ...)*/
string hostName = "localhost";

/**	@param	port						Port of server (standard: 6351)*/
unsigned short port = 6351;

/**	@param	recordTimeBuffer			Time buffer for ensuring that data recorded for detecting the sequence can be stored (in seconds, always > 0). It must always be of a larger absolute value than recordTimeLowerLimit. */
float recordTimeBuffer = 1.6f;

/**	@param	recordTimeUpperLimit		Stopping time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence). */
float recordTimeUpperLimit = 1.9f;

/**	@param	recordTimeLowerLimit		Starting time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence). */
float recordTimeLowerLimit = -0.6f;

/**	@param	audioPlugin					Audio format plugin */
std::unique_ptr<Utilities::Plugins::AudioPlugin> audioPlugin;


/**	\ingroup AudioSPTest
*	Output formatter for printing commas as decimal separators
*/
struct comma_facet : public std::numpunct<char>
{
protected:
	char_type do_decimal_point() const { return ','; } 
	string do_grouping() const { return ""; }
};



/**	@brief		Callback function for found sequences
*	@param		newSequence				Information on the new sequence: ( start time of sequence (DD, MM, YYYY, HH, MM, SS), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing further information on the sequence (not used for FME-sequences) )
*	@return 							None
*	@exception 							None
*	@remarks 							The function is connected to the Core-library
*/
void OnFoundSequence(const Utilities::CSeqDataComplete<float>& newSequence)
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	std::vector<int> code;
	std::vector<float> toneLengths, tonePeriods, toneFreqs;
	ptime time;
	
	// import time and date information
	time = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( newSequence.GetStartTime() );
	
	// output to screen
	cout << time << "\t";
	code = newSequence.GetCodeData().GetTones();
	for (size_t i=0; i < code.size(); i++) {
		cout << code[i];
	}
	cout << endl;

	// start sending sequence information to network clients
	{
		lock_guard<mutex> lock( sequenceDataMutex );
		detectionData.push_back( newSequence );
		isNewFoundSequence = true;
	}
	newFoundSequenceCondition.notify_all();
	
	// store all results on file
	std::ofstream out( outputFileName, ios::app );
	locale l(locale(""), new comma_facet());
	out.imbue( l );

	out << setprecision (10) << ( ( time.time_of_day().total_milliseconds() ) / 1000.0 ) << ";"; // time
	out << "0;"; // double detection flag (out of use)
	toneLengths = newSequence.GetCodeData().GetToneLengths();
	tonePeriods = newSequence.GetCodeData().GetTonePeriods();
	toneFreqs = newSequence.GetCodeData().GetToneFrequencies();
	for (size_t i=0; i < code.size(); i++) {
		out << code[i] << ";";
		out << toneLengths[i] << ";";
		out << tonePeriods[i] << ";";
		out << toneFreqs[i] << ";";
	}
	out << "1" << endl; // successful detection flag (always on, because all stored sequences have been detected successfully)
	out.close();
}



/**	@brief		Notifies the main thread of the user event CRTL + C
*	@param		signal								Number of signal
*	@return 										None
*	@exception 										None
*	@remarks 										This function is connected to C-signals
*/
void AbortDetectionHandler(int signal) 
{
	// signal user request for finishing detection
	{
		lock_guard<mutex> lock( finishDetectionMutex );
		isFinish = true;
	}
	finishDetectionCondition.notify_all();
}



/**	@brief		Notifies the main thread of a runtime error in the detection thread
*	@param		errorString							Error string containing the detailed exception information from the failed thread
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void OnRuntimeError(const std::string& errorString)
{
	cout << "An error occured during the detection." << endl;

	// request for finishing detection
	{
		lock_guard<mutex> lock( finishDetectionMutex );
		isFinish = true;
	}
	finishDetectionCondition.notify_all();
}



/**	@brief		Thread sending new detected sequences to the server
*	@return 										None
*	@exception 										None
*	@remarks 										The network connection has to be initiated before starting the thread
*/
void NetworkingThread(void)
{
	deque< Utilities::CSeqDataComplete<float> > localDetectionData;

	// process audio data until interruption is requested
	while ( true ) {
		unique_lock<mutex> lock( sequenceDataMutex );
		localDetectionData.insert( localDetectionData.end(), detectionData.begin(), detectionData.end() );
		detectionData.clear();

		if ( localDetectionData.size() > 0 ) {
			lock.unlock();
			client.Send( localDetectionData.front() );
			localDetectionData.pop_front();

			// start asynchronous transmission to the server - in case of failure, the data will be lost
			if ( !( client.Run() ) ) {
				cout << "Data transmission to server " + hostName + ":" + boost::lexical_cast<string>( port ) + " failed." << endl;
			}
		} else {
			// wait for new sequence detection
			newFoundSequenceCondition.wait( lock, []() { return isNewFoundSequence; } );
			isNewFoundSequence = false;
			if ( isTerminateThread ) {
				break;
			}
		}
	}
}



/**	@brief		Callback function for recorded sequences
*	@param		sequence				Containing the start time and the code of the sequence, for which the recorded data is transfered
*	@param		signalData				Recorded audio signal data
*	@param		samplingFreq			Sampling frequency of the recorded audio signal data
*	@return 							None
*	@exception 							None
*	@remarks 							The function is connected to the Core-library
*/
void OnRecordedData(const Utilities::CSeqData& sequence, std::vector<float> signalData, double samplingFreq)
{
	string recordedFileName;
	vector<int> code;
	boost::posix_time::time_duration time;
	stringstream ss;

	cout << "Storing on file ..." << endl;

	// determine file name
	time = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( sequence.GetStartTime() ).time_of_day();
	recordedFileName = "../" + boost::lexical_cast<string>( time.total_milliseconds() / 1000.0f ) + "_";
	code = sequence.GetCode();
	for (size_t i=0; i < code.size(); i++) {
		recordedFileName += boost::lexical_cast<string>( code[i] );
	}
	recordedFileName += audioPlugin->GetExtension();

	// store audio file
	audioPlugin->SetSamplingFreq( static_cast<int>( samplingFreq ) );
	audioPlugin->Save( recordedFileName, begin( signalData ), end( signalData ), false ); // the loudness is not changed
}



/**	@brief		Main program
*	@param		argc								Number of arguments
*	@param		argv								Not used
*	@return 										By default 0
*	@exception 										None
*	@remarks 										None
*/
int main(int argc, char *argv[])
{
	unique_ptr<Core::CAudioInput> audioInput;
	shared_ptr<Core::RecordingParam> recordingParams;
	std::map< std::string, std::unique_ptr<Utilities::Plugins::AudioPlugin> > plugins;

	// reset sequence output file (this file of identical format as the files generated by the project "Test")
	std::ofstream out( outputFileName );
	out << "startTime;doubleDetect;tone1;length1;period1;frequency1;tone2;length2;period2;frequency2;tone3;length3;period3;frequency3;tone4;length4;period4;frequency4;tone5;length5;period5;frequency5;successfullDetec" << endl;
	out.close();

	// set up network connection
	cout << "Setting up network connection ...";
	try {
		client.Init( hostName, port );
		cout << " network connection successfully initalized." << endl;
	} catch (...) {
		cout << " connection to server " + hostName + ":" + boost::lexical_cast<string>( port ) + " failed." << endl;
		return -1;
	}
	thread networkThread( NetworkingThread );

	// load the audio format plugins
	plugins = Utilities::Plugins::LoadPlugins<Utilities::Plugins::AudioPlugin>( pluginPath );
	audioPlugin = move( plugins[ Utilities::Plugins::AudioPlugin::CompleteID( pluginID ) ] );

	// set audio device
	recordingParams.reset( new Core::RecordingParam );
	recordingParams->recordedCallback = OnRecordedData;
	recordingParams->recordTimeBuffer = recordTimeBuffer;
	recordingParams->recordTimeUpperLimit = recordTimeUpperLimit;
	recordingParams->recordTimeLowerLimit = recordTimeLowerLimit;
	recordingParams->reqStoringSamplingFreq = Core::RECORDING_MAX_SAMPLING_FREQ;

	try {
		audioInput.reset( new Core::CFMEAudioInputDebug( Core::Processing::CAudioDevice( Core::IN_DEVICE ), audioSettingsFileName, OnFoundSequence, OnRuntimeError, recordingParams ) );
	} catch (...) {
		cout << "Chosen audio device is not ready!" << endl << endl;
		return -1;
	}

	// take over control for CTRL + C
	cout << endl << "Use CRTL + C for stopping detection ..." << endl;
	signal( SIGINT, &AbortDetectionHandler );
		
	// start audio processing
	audioInput->StartAudioInput();

	// disabling the main thread until the user requests end of detection
	{
		unique_lock<mutex> lock( finishDetectionMutex );
		finishDetectionCondition.wait( lock, []() { return isFinish; } );
	}

	// give back control for CTRL + C
	signal( SIGINT, SIG_DFL );

	// stop audio processing
	audioInput->StopAudioInput();

	// finish network connection
	{
		lock_guard<mutex> lock( sequenceDataMutex );
		isTerminateThread = true;
		isNewFoundSequence = true;
	}
	newFoundSequenceCondition.notify_all();
	networkThread.join();
	
	return 0;
}

/*@}*/