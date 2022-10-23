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
/**	\defgroup	PersonalFME		Console program for continuous real-time detection of 5-tone-FME-sequences with the sound card.
*/

/*@{*/
/** \ingroup PersonalFME
*/
#ifdef _MSC_VER
	#include "stdafx.h"
#endif
#ifdef __linux
	#include <sys/stat.h>
	#include <csignal>
	#include <curses.h>
#endif
#include <vector>
#include <string>
#include <thread>
#include <sstream>
#include <boost/filesystem.hpp>
#include "AudioSettings.h"
#include "BasicFunctionality.h"
#include "Dir.h"
#include "VersionInfo.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"
#include "Logger.h"
#include "sync_cout.h"
#include "GeneralStatusMessage.h"
#include "SettingsParam.h"
#include "ExecutionDetectorRuntime.h"

using namespace std;


/**	@param	ControlHandlerType			Type of the Crtl + C handler */
enum ControlHandlerType { STANDARD_HANDLER, RUNTIME_HANDLER };
/**	@param	finishMutex					Mutex for the runtime finish condition */
std::mutex finishMutex;
/**	@param	finishCondition				Finish condition for the runtime */
std::condition_variable_any finishCondition;
/**	@param	isFinish					Flag stating if the execution should be finished */
bool isFinish = false;
/**	@param	isPlayTone					Flag determining if a tone is played in case of detected alarm */
bool isPlayTone;
/**	@param	logFileName					Name of the log file for storing all operational information. It is located in the user app directory */
const std::string logFileName = Utilities::CVersionInfo::SoftwareName() + "_log.txt";
/**	@param	isGenerateNewBasicSettings	Flag determining if a new basic settings file is generated (not required for normal operation -> false) */
bool isGenerateNewBasicSettings = false;

#ifdef __linux
	/**	@param	old_SIGINT_action		Action for the Crtl+C signal before starting the program (backuped for later restorage) */
	struct sigaction old_SIGINT_action;
	/**	@param	old_SIGTERM_action		Action for the SIGTERM signal before starting the program (backuped for later restorage) */
	struct sigaction old_SIGTERM_action;
#endif


/**	@brief		Stores the audio parameter settings file
*	@param		audioSettingsFileName				File name of the settings file using an absolute path
*	@return 										None
*	@exception 										None
*	@remarks 										This function is not required for normal operation and uses fixed values built into the code.
*/
void SetAudioSettingsFile(std::string audioSettingsFileName)
{
	double sampleLength = 0.05;								// Length of sample acquired in one timestep [s]
	int numChannels = 1;									// Number of channels required for the audio input (usually one is sufficient)
	int maxLengthInputQueue = 100000000;					// Maximum senseful input queue length
	int maxMissedAttempts = 10;								// Number of synchronization attempts in audio acquisition before synchronization is forced
	int channel = 1;										// Channel number for reading the audio input (first channel is 1)
	string parameterFileName = "params.dat";				// File name of general code analysis settings file
	string specializedParameterFileName = "fmeParams.dat";	// File name of FME code analysis settings file
	double maxRequiredProcFreq = 3350.0;					// Maximum frequency required for useful process data stream [Hz]. It is the frequency where the main leaf of the downsampling filter has the minimum attenuation of the stop band (i.e. that of first side leaf) and differs from the highest tone frequency due to the sharpness of the filter.
	double transWidthProc = 350;							// Transition width for the processing downsampling filter [Hz]. Small values reduce performance.
	double transWidthRec = 1000;							// Transition width for the audio recording filter [Hz]. Small values reduce performance.
	float mainThreadCycleTime = 0.01f;						// Time between two cycles of the main thread controlling the full audio signal evaluation [s]
	std::vector<double> standardSamplingFreqs = { 11025.0, 22050.0, 44100.0, 48000.0, 88200.0, 96000.0 }; // sampling frequencies that might be used for audio processing, the alogorithm will always choose the largest one possible [Hz]

	Core::CAudioInput::SaveParameters( audioSettingsFileName, sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );
}



#ifdef _WIN32
	/**	@brief		Aborts the program in case of CTRL + C
	*	@param		ctrl_type							Type of user event
	*	@return 										Flag stating if installation of the handler was successfull
	*	@exception 										None
	*	@remarks 										This function is connected to the WIN API
	*/
	BOOL WINAPI RuntimeAbortHandler( DWORD ctrl_type )
	{
		// notify that the abort of the processing is required
		{
			std::unique_lock< std::mutex > lock( finishMutex );
			isFinish = true;
		}
		finishCondition.notify_all();

		return TRUE;
	}
#elif __linux
	/**	@brief		Aborts the program in case of CTRL + C
	*	@param		signal								Type of user event
	*	@return 										None
	*	@exception 										None
	*	@remarks 										This function is connected to Linux POSIX
	*/
	void RuntimeAbortHandler( int signal ) {
		// notify that the abort of the processing is required
		{
			std::unique_lock< std::mutex > lock( finishMutex );
			isFinish = true;
		}
		finishCondition.notify_all();
	}
#endif



/**	@brief		Receives all exceptions that occur during running detection or server mode and require abortion 
*	@param		errorString							Description of the occured exception
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void OnRuntimeError(const std::string& errorString)
{
	using namespace boost::posix_time;
	using namespace Utilities::Message;
	ptime currTime;

	// log the error message
	currTime = ptime( microsec_clock::universal_time() );
	try {
		Logger::CLogger::Instance().Log( std::make_unique<CGeneralStatusMessage>( MESSAGE_ERROR, currTime, u8"Während des Betriebs ist ein Fehler aufgetreten: " + errorString ) );
	} catch ( std::exception e ) {
		sync_cout::Inst() << u8"FEHLER: Schreiben in die Log-Datei nicht erfolgreich." << endl;
	}
	sync_cout::Inst() << u8"FEHLER: " << errorString << u8". Betrieb wurde abgebrochen." << std::endl;

	// trigger stop of the current processing mode
	#ifdef _WIN32
		GenerateConsoleCtrlEvent( CTRL_C_EVENT, 0 );
	#elif __linux
		raise( SIGINT );
	#endif
}



/**	@brief		Receives all detected sequences
*	@param		sequence							Containing the start time and the code of the sequence, for which the recorded data is transfered
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void OnFoundSequence(const Utilities::CSeqData& sequence)
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities::Message;
	using namespace Utilities::Time;

	ptime time;
	vector<int> sequenceCode;
	stringstream ss, timeStream;

	try {
		time = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( sequence.GetStartTime() );
	} catch (std::out_of_range e) {
		// set to "not a date / time"
		time = ptime();
	}

	sequenceCode = sequence.GetCode();

	// output to screen
	timeStream << german_local_date_time( time );
	sync_cout::Inst() << timeStream.str() << "   ";
	ss.clear();
	for_each( sequenceCode.begin(), sequenceCode.end(), [&](int val){ ss << val; } );
	sync_cout::Inst() << ss.str() << endl;

	// logging in file
	try {
		Logger::CLogger::Instance().Log( std::make_unique<CGeneralStatusMessage>( MESSAGE_SUCCESS, time, u8"Alarm für Schleife " + ss.str() + u8" empfangen." ) );
	} catch ( std::exception e ) {
		sync_cout::Inst() << u8"FEHLER: Schreiben in die Log-Datei nicht erfolgreich." << endl;
	}

	// play tone if required
	if ( isPlayTone ) {
		#ifdef _WIN32
			Beep( 2500, 500 );
		#elif __linux
			beep();
		#endif
	}
}
	


/**	@brief		Callback function for recorded sequences
*	@param		sequence						Containing the start time and the code of the sequence, for which the recorded data is transfered
*	@param		audioFile						Stored audio file with the recorded data
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void OnRecordedData( const Utilities::CSeqData& sequence, const Utilities::CMediaFile& audioFile )
{
	// everything required is handled in the middleware module
}



/**	@brief		Callback function for messages emitted by the detector
*	@param		message								Emitted message
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void OnMessage(std::unique_ptr<Utilities::Message::CStatusMessage> message)
{
	std::string messageText;

	try {
		messageText = Logger::CLogger::Instance().Log( std::move( message ) );
	} catch ( std::exception e ) {
		sync_cout::Inst() << u8"FEHLER: Schreiben in die Log-Datei nicht erfolgreich." << endl;
	}

	// output on the screen
	sync_cout::Inst() << messageText << endl;
}



/**	@brief		Resets the CTRL + C handler
*	@param		type								Type of the handler to be set
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void ResetControlHandler(const ControlHandlerType& type)
{
	switch ( type ) {
	case STANDARD_HANDLER:
		// restores normal processing of the Ctrl + C input
		#ifdef _WIN32
			SetConsoleCtrlHandler( NULL, FALSE );
		#elif __linux
			sigaction( SIGINT, &old_SIGINT_action, NULL );
			sigaction( SIGTERM, &old_SIGTERM_action, NULL );
		#endif
		break;
	case RUNTIME_HANDLER:
		#ifdef _WIN32
			SetConsoleCtrlHandler( RuntimeAbortHandler, TRUE );
		#elif __linux
			struct sigaction new_action;

			new_action.sa_handler = RuntimeAbortHandler;
			new_action.sa_flags = 0;

			// reset the Crtl + C handling (saving the previous signal setting for later restorage)
			sigaction( SIGINT, NULL, &old_SIGINT_action );
			sigaction( SIGINT, &new_action, NULL );

			// reset the SIGTERM handling (saving the previous signal setting for later restorage)
			sigaction( SIGTERM, NULL, &old_SIGTERM_action );
			sigaction( SIGTERM, &new_action, NULL );
		#endif
		break;
	}
}



/**	@brief		Disable the main thread and resume execution when notified
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void PerformRunning(void)
{
	ResetControlHandler( RUNTIME_HANDLER );

	// lock the main thread until resuming execution is notified
	std::unique_lock< std::mutex > lockFinishDetection( finishMutex );
	finishCondition.wait( lockFinishDetection, []() { return isFinish; } );

	ResetControlHandler( STANDARD_HANDLER );
}




/**	@brief		Resets the Windows console font to one containing non-standard characters and changes the codepage to UTF-8
*	@return 										None
*	@exception 										None
*	@remarks 										This function has no effect on non-Windows systems
*/
void SetWindowsConsoleCodePage()
{
#ifdef _WIN32
	CONSOLE_FONT_INFOEX cfi;

	// change the font to one handling special characters
	cfi.cbSize = sizeof cfi;
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 16;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	wcscpy_s( cfi.FaceName, L"Consolas" );
	SetCurrentConsoleFontEx( GetStdHandle( STD_OUTPUT_HANDLE ), FALSE, &cfi );

	// set the code page to UTF-8
	SetConsoleCP( CP_UTF8 );
	SetConsoleOutputCP( CP_UTF8 );
#endif
}



/**	@brief		Forks the program into a linux daemon
*	@return 										None
*	@exception 	std::runtime_error					Thrown if starting the daemon failed
*	@remarks 										This function has no effect on non-Linux systems. After execution the parent process has stopped and a background child process is running.
*/
void Daemonize()
{
#ifdef __linux	
	pid_t pid, sid;

	// fork the parent process
	pid = fork();

	if ( pid < 0 ) {
		throw std::runtime_error( "Starten des Daemons fehlgeschlagen." );
	}

	// close the parent process
	if ( pid > 0 ) {
		exit( EXIT_SUCCESS );
	}

	// from here on this is the background child process
	// change the file mask (not to inherit the permissions from the parent process, however not used by C++ standard library functions)
	umask( 0 );

	// create a new signature ID for the child
	sid = setsid();
	if ( sid < 0 ) {
		throw std::runtime_error( "Starten des Daemons fehlgeschlagen." );
	}

	// change working directory to root (changing to a directory that cannot be unmounted)
	if ( ( chdir( "/" ) ) < 0 ) {
		throw std::runtime_error( "Starten des Daemons fehlgeschlagen." );
	}

	// close standard file descriptors
	close( STDIN_FILENO );
	close( STDOUT_FILENO );
	close( STDERR_FILENO );
#endif
}




/**	@brief		Main program
*	@param		argc								Number of arguments
*	@param		argv								Command line parameters
*	@return 										Zero in case of success, non-zero in case of any error
*	@exception 										None
*	@remarks 										None
*/
int main(int argc, char *argv[])
{
	using namespace boost::filesystem;
	using namespace boost::posix_time;
	using namespace Utilities::Message;

	Core::Processing::CAudioDevice device;
	bool doDaemonize;
	float minDistanceRepetition;
	path configFile;
	string versionString, dateString, licenseString;
	vector<string> commandLineArgs;
	Middleware::CSettingsParam params;
	Middleware::CDir directories;

	string softwareName = Utilities::CVersionInfo::SoftwareName();
	TypeOfChoice choice = NOT_VALID;

	try {
		// obtain standard folders
		directories = Middleware::CDir( Utilities::CVersionInfo::SoftwareName() );
		Logger::CLogger::Instance( absolute( logFileName, directories.GetLogDir() ) );

		// generate the basic settings file if required - not required for normal operation
		if ( isGenerateNewBasicSettings ) {
			SetAudioSettingsFile( Middleware::CAudioSettings::GetAudioSettingsFileName() );
		}

		SetWindowsConsoleCodePage();

		// process the command line options
		for ( int argumentID = 1; argumentID < argc; argumentID++ ) {
			commandLineArgs.push_back( argv[argumentID] );
		}
		choice = CBasicFunctionality::ProcessCommandLineArguments( commandLineArgs, configFile, doDaemonize );
		if ( !configFile.empty() ) {
			configFile = absolute( configFile, directories.GetUserSettingsDir() );
		}

		// convert the program into a daemon if required
		if ( doDaemonize ) {
			Daemonize();
		}
		
		// perform the chosen task
		sync_cout::Inst() << CBasicFunctionality::GetBasicVersionInformation();
		Core::CAudioInput::GetAudioSPVersion( versionString, dateString, licenseString );
		sync_cout::Inst() << endl;

		switch ( choice ) {
			case DETECTION:
			{
				sync_cout::Inst() << u8"Starte die Fünftonfolgen-Detektion ..." << endl << endl;
				Logger::CLogger::Instance().Log( std::make_unique<CGeneralStatusMessage>( MESSAGE_SUCCESS, ptime( microsec_clock::universal_time() ), string( Utilities::CVersionInfo::SoftwareName() + " " + versionString + u8" gestartet, Konfigurationsdatei: " + configFile.string() + "." ) ) );	
				params = CBasicFunctionality::ValidateXMLConfigFile( configFile );

				std::unique_ptr< Middleware::CExecutionRuntime > runtime; // will be automatically destroyed (stopping all processing) in any situation when leaving the try-block
				params.GetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );
				runtime.reset( new Middleware::CExecutionDetectorRuntime( params, directories.GetAppSettingsDir(), directories.GetAudioDir(), directories.GetPluginDir(), OnFoundSequence, OnRecordedData, OnRuntimeError, OnMessage ) );
				runtime->Run();

				// lock the main thread until resuming excecution is notified
				PerformRunning();

				break;
			}
			case TEST:
			{
				try {
					CBasicFunctionality::ValidateXMLConfigFile( configFile );
				} catch ( std::exception& e ) {
					// as this is only a test, no error logging is required
					sync_cout::Inst() << u8"Warnung: " << endl << e.what() << endl << endl;
				}
				break;
			}
			case VERSION_INFO:
			{
				sync_cout::Inst() << CBasicFunctionality::GetCompleteVersionInformation();
				break;
			}
			case AUDIO_INFO:			
			{
				sync_cout::Inst() << u8"Suche die vorhandenen Audiogeräte ..." << endl << endl;
				sync_cout::Inst() << CBasicFunctionality::GetAudioInputDeviceList();
				break;
			}
			case HELP:
			{
				sync_cout::Inst() << CBasicFunctionality::GetHelpText();
				break;
			}
			case PRINT_WORKING_DIR:
			{
				sync_cout::Inst() << u8"Konfigurationsverzeichnis: " << directories.GetUserSettingsDir();
				break;
			}
		}
		sync_cout::Inst() << endl;
	} catch(std::exception& e) {
		// handling of fatal errors if the program cannot be continued
		if ( choice == DETECTION ) {
			try {
				Logger::CLogger::Instance( absolute( logFileName, directories.GetLogDir() ) ).Log( std::make_unique<CGeneralStatusMessage>( MESSAGE_ERROR, ptime( microsec_clock::universal_time() ), e.what() ) );
			} catch ( std::exception& e ) {
				sync_cout::Inst() << u8"Fehler: " << endl << e.what() << endl << endl;
			}
		}
		sync_cout::Inst() << u8"Fehler: " << endl << e.what() << endl << endl;
		return -1;
	}

	return 0;
}
/*@}*/