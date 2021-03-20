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
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <deque>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#ifdef _WIN32
	#include "libalglib/alglibversion.h"
#endif
#include "PortaudioWrapper.h"
#include "SeqDataComplete.h"
#include "DataProcessing.h"
#include "privImplementation.h"



/**	@brief		Obtain all possible audio sampling frequencies of the audio capture device
*	@param		device							Audio capture device. The available devices can be obtained from CPrivImplementation::GetAudioDevices. For default device set it to an empty name.
*	@param		numChannels						Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param		standardSamplingFreqs			Vector container storing all sampling frequencies to be tested [Hz]
*	@return 									Vector container storing all valid sampling frequencies of the audio capture device [Hz]
*	@exception 									None
*	@remarks 									None
*/
std::vector<double> Core::CAudioInput::CPrivImplementation::GetPossibleSamplingFreqs(const Processing::CAudioDevice& device, const int& numChannels, const std::vector<double>& standardSamplingFreqs)
{
	Processing::CAudioDevice defaultInputDevice;
	std::vector<Processing::CAudioDevice> availableInputDevices;
	std::vector<double> possibleSamplingFreqs;
	Core::Processing::CPortaudio<float> portaudio;

	// checks if the required device is available for the given sampling rates
	for ( auto samplingFreq : standardSamplingFreqs ) {
		if ( portaudio.IsDeviceAvailable( device, samplingFreq, numChannels ) ) {
			possibleSamplingFreqs.push_back(samplingFreq );	
		}	
	}

	return possibleSamplingFreqs;
}



/**	@brief		Load audio settings
*	@param		audioSettingsFileName			Name of data file with an absolute path(*.dat)
*	@param		params							Parameters for audio settings
*	@return 									None
*	@exception 	std::ios_base::failure			Thrown if parameter file cannot be read
*	@remarks 									None
*/
void Core::CAudioInput::CPrivImplementation::LoadParameters(std::string audioSettingsFileName, Core::CAudioInputParam &params)
{
	// deserialize parameter data from file
	std::ifstream ifs( audioSettingsFileName );
	boost::archive::text_iarchive ia( ifs );
	
	// read parameters
	if ( !ifs.eof() ) {
		ia >> params;
	} else {
		throw std::ios_base::failure("Parameter file cannot be read.");
	}

	ifs.close();
}



/**	@brief		Main thread controlling the whole audio capture processing and the data passing to the central data analysis class (CSearch)
*	@return										None
*	@exception	std::runtime_error				Thrown if the class was not initialized before starting the thread
*	@remarks									None
*/
void Core::CAudioInput::CPrivImplementation::MainThread(void)
{
	using namespace std;
	using namespace boost::posix_time;

	vector< boost::posix_time::ptime > inputTime, processInputTime, recordInputTime;
	vector<float> inputSignal, processInputSignal, recordInputSignal;
	deque< Utilities::CSeqDataComplete<float> > newFoundSequences;

	try {	
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		// record audio data until interruption is requested
		while ( !( boost::this_thread::interruption_requested() ) ) {
			// get new audio signal data
			inputTime.clear();
			inputSignal.clear();
			dataReader.GetSignalData( back_inserter( inputTime ), std::back_inserter( inputSignal ) );

			if ( !inputSignal.empty() ) {
				processInputTime.clear();
				processInputSignal.clear();
				recordInputTime.clear();
				recordInputSignal.clear();
				if ( isRecording ) {
					fullDownsampler.PerformDownsampling( inputTime.begin(), inputTime.end(), inputSignal.begin(), back_inserter( processInputTime ), back_inserter( processInputSignal ), back_inserter( recordInputTime ), back_inserter( recordInputSignal ) );
				} else {
					simpleDownsampler.Processing( inputTime.begin(), inputTime.end(), inputSignal.begin(), back_inserter( processInputTime ), back_inserter( processInputSignal ) );
				}
				
				// send new signal data to the analysis thread
				searchCode->PutSignalData( processInputTime.begin(), processInputTime.end(), processInputSignal.begin(), processInputSignal.end() );

				// send new signal data to the data preserving thread (required for possible later storage of audio data connected to a found sequence)
				if ( isRecording ) {
					dataPreserver.PutSignalData( recordInputTime.begin(), recordInputTime.end(), recordInputSignal.begin() );
				}
			}

			// obtain all new found sequences
			newFoundSequences = searchCode->GetSequencesDebug();

			if( !newFoundSequences.empty() ) {
				// initiate notification of all connected caller functions in case of new found sequences
				sequencePasser->PutSequences( newFoundSequences.begin(), newFoundSequences.end() );

				// initiate storage of audio data connected to a found sequence
				dataPreserver.PutSequences( newFoundSequences.begin(), newFoundSequences.end() );
			}

			// delay thread
			std::this_thread::sleep_for( std::chrono::microseconds( static_cast<long long>( mainThreadCycleTime * 1e6 ) ) );
		}
	} catch (const std::exception& e) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Main thread: " + string( e.what() ) );
	}
}



/**	@brief		Setting the names of the parameter files
*	@param		parameterFileName				File name of general code analysis settings file. The directory is given relative to the current parameter file.
*	@param		specializedParameterFileName	File name of FME code analysis settings file. The directory is given relative to the current parameter file.
*	@return 									None
*	@exception 	std::runtime_errorr				Thrown if the object is in use and the parameters cannot be changed
*	@remarks 									None
*/
void Core::CAudioInput::CPrivImplementation::SetFileNames(const std::string& parameterFileName, const std::string& specializedParameterFileName)
{
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		CPrivImplementation::parameterFileName = parameterFileName;
		CPrivImplementation::specializedParameterFileName = specializedParameterFileName;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}
}



/**	@brief		Setting the class parameters
*	@param		device							Input device. It can be obtained from CAudioInput::GetAudioDevices.For default device set it to an empty name.
*	@param		audioSettingsFileName			File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback					Function, which is called in case of found sequences
*	@param		runtimeErrorCallback			Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@param		recordingParams					Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@return 									None
*	@exception	std::runtime_error				Thrown if the main thread is running and it is attempted to call CPrivImplementation::Init(). Call CPrivImplementation::StopAudioInput to stop the thread before calling this function.
*	@remarks 									This function can be called repeatedly for resetting the class parameters
*/
void Core::CAudioInput::CPrivImplementation::SetParameters(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
{
	using namespace std;
	int downsamplingFactorProc, downsamplingFactorRec;
	double transWidthProc, transWidthRec, cutoffFreqProc, cutoffFreqRec;
	float mainThreadCycleTime;
	vector<float> simpleFilterParams;

	if ( ( threadMain != nullptr ) && ( threadMain->joinable() ) ) {
		throw std::runtime_error( "The parameters cannot be reset when the main thread is running." );
	}

	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( !lock.owns_lock() ) {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );	
	}

	if ( recordingParams != nullptr ) {
		CPrivImplementation::isRecording = true;
		
		// obtain the parameters for running sound device for audio capture
		GetAudioReaderParams( device, audioSettingsFileName, CPrivImplementation::samplingFreqInput, downsamplingFactorProc, cutoffFreqProc, downsamplingFactorRec, cutoffFreqRec, recordingParams->reqStoringSamplingFreq );
		
		// initialize recording of signal data after detection of a sequence
		dataPreserver.SetParameters( CPrivImplementation::samplingFreqInput / downsamplingFactorRec, recordingParams->recordTimeLowerLimit, recordingParams->recordTimeUpperLimit, recordingParams->recordTimeBuffer, recordingParams->recordedCallback, runtimeErrorCallback );
	} else {
		// obtain the parameters for running sound device for audio capture 
		GetAudioReaderParams( device, audioSettingsFileName, CPrivImplementation::samplingFreqInput, downsamplingFactorProc, cutoffFreqProc, downsamplingFactorRec, cutoffFreqRec );
	}

	CPrivImplementation::samplingFreqProcessing = CPrivImplementation::samplingFreqInput / downsamplingFactorProc;

	// initialize downsampling filtering
	GetRelevantAudioSettings( audioSettingsFileName, transWidthProc, transWidthRec, mainThreadCycleTime );
	CPrivImplementation::mainThreadCycleTime = mainThreadCycleTime;
	if ( isRecording ) {
		fullDownsampler.SetParameters( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, CPrivImplementation::samplingFreqInput );
	} else {
		Processing::Filter::CFIRfilter<float>::DesignLowPassFilter( static_cast<float>( transWidthProc ), static_cast<float>( cutoffFreqProc ), static_cast<float>( CPrivImplementation::samplingFreqInput ), back_inserter( simpleFilterParams ) );
		simpleDownsampler.SetParams( simpleFilterParams.begin(), simpleFilterParams.end(), downsamplingFactorProc, 1, 1e-7f ); // reduced accuracy limit is required due to datatype float
	}

	// initialize passing of detected sequences to the callback functions
	sequencePasser.reset( new Audio::CSequencePasser<float>( foundCallback, runtimeErrorCallback ) );

	// initialize audio capture - however the audio capture thread is not started here
	InitializeAudioReader( device, audioSettingsFileName, runtimeErrorCallback, samplingFreqInput );

	// initialize signaling of errors in the main thread
	runtimeErrorSignal.disconnect_all_slots();
	runtimeErrorSignal.connect( runtimeErrorCallback );

	isInit = true;
}
		


/**	@brief		Obtaining the best parameters for the processing of the audio capture, signal analysis and recording streams
*	@param		possibleSamplingFreqs			Vector container storing all possible sampling frequencies for the current audio capture device
*	@param		maxRequiredProcFreq				Maximum frequency required for useful process data stream [Hz]. This value is typically corresponding to the maximum tone frequency possible.
*	@param		requestedRecSamplingFreq		The sampling frequency requested by the user for recording the audio signal after a detection [Hz]. It is identical to zero, if no recording is required.
*	@param		samplingFreqInput				Best sampling frequency for audio capture [Hz]
*	@param		downsamplingFactorProc			Best downsampling factor for audio processing (signal analysis). This value corresponds to the sampling frequency.
*	@param		cutoffFreqProc					Cutoff frequency of the downsampling filter for the processing output data [Hz]. A value of 1 corresponds to the Nyquist frequency.
*	@param		downsamplingFactorRec			Best downsampling factor for audio recording. This value corresponds to the sampling frequency and is identical to one if recording is not used.
*	@param		cutoffFreqRec					Cutoff frequency of the downsampling filter for the recording output data [Hz]. A value of 1 corresponds to the Nyquist frequency.
*	@return 									None
*	@exception 	std::logic_error				Thrown if the input device is not available or the required sampling frequencies of the detection algorithm are not available
*	@remarks 									This function delivers automatically the best parameter combination possible considering performance (by multi-step downsampling). The downsampling factor for data processing can always be divided by the downsampling factor for recording, if the maximum frequency required for data processing is smaller than that for data recording.
*/
void Core::CAudioInput::CPrivImplementation::GetBestWorkingParameters(const std::vector<double>& possibleSamplingFreqs, const double& maxRequiredProcFreq, const double& requestedRecSamplingFreq, double& samplingFreqInput, int& downsamplingFactorProc, double& cutoffFreqProc, int& downsamplingFactorRec, double& cutoffFreqRec)
{
	// determine the largest possible sampling frequency for audio capture
	if ( !possibleSamplingFreqs.empty() ) {
		samplingFreqInput = possibleSamplingFreqs.back();
	} else {
		// input device is not available
		throw std::logic_error( "Input device is not ready." );
	}

	// determine the best downsampling factor for the processing data
	downsamplingFactorProc = static_cast<int>( floor( samplingFreqInput / ( 2 * maxRequiredProcFreq ) ) );
	if ( downsamplingFactorProc < 1 ) {
		downsamplingFactorProc = 1;
	}

	// adjust the downsampling factor if it is a prime number (except 2) in order to make it divisable
	if ( ( Processing::CDataProcessing<float>::IsPrimeNumber( downsamplingFactorProc ) ) && ( downsamplingFactorProc != 2 ) ) {
		downsamplingFactorProc--;
	}

	// determine the best downsampling factor for the recording data
	if ( ( requestedRecSamplingFreq == RECORDING_MAX_SAMPLING_FREQ ) || ( requestedRecSamplingFreq == NO_RECORDING ) ) {
		// the recording option is not used or the maximum sampling frequency is required
		downsamplingFactorRec = 1;
	} else {
		downsamplingFactorRec = static_cast<int>( ceil( samplingFreqInput / requestedRecSamplingFreq ) );
	}

	// adjust the downsampling factor as long no division of processing and recording prime number is possible
	if ( downsamplingFactorProc >= downsamplingFactorRec ) {
		while ( ( downsamplingFactorProc % downsamplingFactorRec ) != 0 ) {
			downsamplingFactorRec--;
			if ( downsamplingFactorRec <= 1 ) {
				break;
			}
		}
	}

	// determine cutoff frequencies of the filters
	cutoffFreqProc = maxRequiredProcFreq;
	cutoffFreqRec = samplingFreqInput / 2.0 / downsamplingFactorRec;
}



/**	@brief		Obtains the available input devices at the highest possible sampling rate
*	@param		devices							Container with all available input devices at the 'maxStandardSamplingFreq'
*	@param		stdDevice						Standard input device at the 'maxStandardSamplingFreq'
*	@param		maxStandardSamplingFreq			Highest possible sampling rate [Hz] (chosen out of the standard sampling rates of the detection algorithm)
*	@param		audioSettingsFile				Filename and path of the audio device settings file
*	@return										None
*	@exception									None
*	@remarks									The highest possible sampling rate refers to the highest required sampling rate of the detection algorithm and may be lower than that of the sound device.
*												This method can be used before initializing the object and does not interfer with any running audio stream
*/
void  Core::CAudioInput::CPrivImplementation::GetAvailableAudioDevices( std::vector<Processing::CAudioDevice>& devices, Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& audioSettingsFile )
{
	using namespace std;

	vector<Processing::CAudioDevice> currDevices;
	Processing::CAudioDevice currStdDevice;
	map< double, vector<Processing::CAudioDevice> > devicesMap;
	map< double, Processing::CAudioDevice > stdDeviceMap;
	string parameterFileName, specializedParameterFileName;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	double sampleLength, maxRequiredProcFreq, transWidthProc, transWidthRec;
	float mainThreadCycleTime;
	vector<double> standardSamplingFreqs;
	CAudioInputParam params;

	// load the required audio settings from file
	LoadParameters( audioSettingsFile.string(), params );
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );

	// obtain the available devices for all standard sampling frequencies
	for ( auto currStandardSamplingFreq : standardSamplingFreqs ) {
		currDevices.clear();
		dataReader.GetAudioDevices( currDevices, currStdDevice, currStandardSamplingFreq, numChannels ); // the method is independent of any running streams and works with the default initialized object of CPortaudio

		if ( !currDevices.empty() ) {
			devicesMap[currStandardSamplingFreq] = currDevices;
			stdDeviceMap[currStandardSamplingFreq] = currStdDevice;
		}
	}

	// determine the available devices at the highest possible standard sampling frequency
	if ( !devicesMap.empty() ) {
		devices = devicesMap.rbegin()->second;
		stdDevice = stdDeviceMap.rbegin()->second;
		maxStandardSamplingFreq = devicesMap.rbegin()->first;
	} else {
		devices.clear();
		stdDevice = Processing::CAudioDevice();
		maxStandardSamplingFreq = 0;
	}
}



/**	@brief		Determines if the given input device is available on the current system for one of the standard sampling rates of the detection algorithm
*	@param		maxStandardSamplingFreq			Will contain the highest possible sampling rate of the device [Hz] (chosen out of the standard sampling rates of the detection algorithm). If the device is not available, it will be set to 0 Hz.
*	@param		device							Input device to be checked
*	@param		audioSettingsFile				Filename and path of the audio device settings file
*	@return										True if the device is available, false otherwise
*	@exception									None
*	@remarks									The highest possible sampling rate refers to the highest required sampling rate of the detection algorithm and may be lower than that of the sound device.
*												This method can be used before initializing the object and does not interfer with any running audio stream
*/
bool Core::CAudioInput::CPrivImplementation::IsDeviceAvailable( double& maxStandardSamplingFreq, const Processing::CAudioDevice& device, const boost::filesystem::path& audioSettingsFile ) const
{
	using namespace std;

	bool currIsAvailable;
	set<double> availabilitySet;
	string parameterFileName, specializedParameterFileName;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	double sampleLength, maxRequiredProcFreq, transWidthProc, transWidthRec;
	float mainThreadCycleTime;
	vector<double> standardSamplingFreqs;
	CAudioInputParam params;

	// load the required audio settings from file
	LoadParameters( audioSettingsFile.string(), params );
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );

	// obtain the available devices for all standard sampling frequencies)
	for ( auto currStandardSamplingFreq : standardSamplingFreqs ) {
		currIsAvailable = dataReader.IsDeviceAvailable( device, currStandardSamplingFreq, numChannels ); // the method is independent of any running streams and works with the default initialized object of CPortaudio
		if ( currIsAvailable ) {
			availabilitySet.insert( currStandardSamplingFreq );
		}
	}

	// determine the highest possible standard sampling frequency
	if ( !availabilitySet.empty() ) {
		maxStandardSamplingFreq = *( availabilitySet.rbegin() );
		return true;
	} else {
		maxStandardSamplingFreq = 0.0;
		return false;
	}
}



/**	@brief		Set the input device
*	@param		device							Chosen input device. It can be obtained from CAudioInput::GetAudioDevices. For default device set it to an empty name.
*	@return										None
*	@exception	std::runtime_error				Thrown if this function is called before starting properly the audio devices or if the object is in use and the parameters cannot be changed
*	@remarks									Any call before calling CAudioInput::Init or the constructor will not be successfull.
*/
void  Core::CAudioInput::CPrivImplementation::SetAudioDevice(const Processing::CAudioDevice& device)
{
	if ( !isInit ) {
		throw std::runtime_error( "The object was not initialized before use!" );
	}

	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( !lock.owns_lock() ) {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );	
	}

	dataReader.SetAudioDevice( device );
}



/**	@brief		Start reading and processing of audio input
*	@return										None
*	@exception	std::logic_error				Thrown if the detection is already running. Stop it before any restart trial.
*	@remarks									This function starts the complete processing of the audio device input for code sequences.
*												The processing can be stopped by calling CAudioInput::StopAudioInput.
*/
void Core::CAudioInput::CPrivImplementation::StartAudioInput(void)
{
	// start the main thread
	boost::lock_guard< boost::mutex > lock( mainThreadMutex );
	if ( threadMain != nullptr ) {
		throw std::logic_error( "The detection is already running." );
	}
	threadMain = std::unique_ptr<boost::thread>( new boost::thread( std::bind( &CAudioInput::CPrivImplementation::MainThread, this ) ) );
	
	// start the audio capturing thread
	dataReader.StartReading();
}



/**	@brief		Stop reading and processing of audio input
*	@return										None
*	@exception									See boost::thread
*	@remarks									This function stops the audio processing started by CAudioInput::StartAudioInput. If it is called concurrently to StartAudioInput() the thread may not be stopped.
*/
void Core::CAudioInput::CPrivImplementation::StopAudioInput(void)
{
	// stop the audio capturing thread
	dataReader.StopReading();

	// stop the main thread
	boost::lock_guard< boost::mutex > lock( mainThreadMutex );
	if ( threadMain != nullptr ) {
		// stop all running threads
		threadMain->interrupt();
	
		// wait until all threads have stopped
		threadMain->join();

		threadMain.reset();
	}
}



/**	@brief		Checks if the processing of audio input is running
*	@return										True if the audio input processing is running, false otherwise
*	@exception									None
*	@remarks									None
*/
bool Core::CAudioInput::CPrivImplementation::IsRunning(void)
{
	boost::lock_guard< boost::mutex > lock( mainThreadMutex );
	if ( threadMain != nullptr ) {
		return true;
	} else {
		return false;
	}
}



/**	@brief		Get important class parameters
*	@param		samplingFreqProcessing			Sampling frequency for audio processing [Hz]
*	@param		samplingFreqInput				Sampling frequency for audio recording [Hz]
*	@param		parameterFileName				File name of general code analysis settings file. The directory is given relative to the current parameter file.
*	@param		specializedParameterFileName	File name of FME code analysis settings file. The directory is given relative to the current parameter file.
*	@return										None
*	@exception									None
*	@remarks									None
*/
void Core::CAudioInput::CPrivImplementation::GetBasicInformation(double& samplingFreqProcessing, double& samplingFreqInput, std::string& parameterFileName, std::string& specializedParameterFileName) const
{
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
	samplingFreqProcessing = CPrivImplementation::samplingFreqProcessing;
	samplingFreqInput = CPrivImplementation::samplingFreqInput;
	parameterFileName = CPrivImplementation::parameterFileName;
	specializedParameterFileName = CPrivImplementation::specializedParameterFileName;
}



/**	@brief		Resetting the audio signal analyzer
*	@param		newSearcher						Smart pointer to the new signal analysis object
*	@return										None
*	@exception 	std::runtime_errorr				Thrown if the object is in use and the parameters cannot be changed
*	@remarks									None
*/
void Core::CAudioInput::CPrivImplementation::ResetSearcher(std::shared_ptr< Core::General::CSearch<float> > newSearcher)
{
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( !lock.owns_lock() ) {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );	
	}

	searchCode = newSearcher;
}



/**	@brief		Resetting the sequence passer
*	@param		newSequencePasser				Smart pointer to the new sequence passer object
*	@return										None
*	@exception 	std::runtime_errorr				Thrown if the object is in use and the parameters cannot be changed
*	@remarks									None
*/
void Core::CAudioInput::CPrivImplementation::ResetSequencePasser(std::shared_ptr< Core::Audio::CSequencePasser<float> > newSequencePasser)
{
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( !lock.owns_lock() ) {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );	
	}

	sequencePasser = newSequencePasser;
}



/**	@brief		Setting the audio capture class returning the basic parameters
*	@param		device							Chosen input device.It can be obtained from CAudioInput::GetAudioDevices. For default device set it to an empty name.			
*	@param		audioSettingsFileName			File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.			
*	@param		runtimeErrorCallback			Function, which is called in case of a runtime error during execution. The execution will not stop automatically!			
*	@param		samplingFreqInput				Sampling frequency for audio capture [Hz]
*	@return										None
*	@exception									None
*	@remarks									This function alone is not thread-safe
*/
void Core::CAudioInput::CPrivImplementation::InitializeAudioReader(const Processing::CAudioDevice& device, const std::string& audioSettingsFileName, std::function<void(const std::string&)> runtimeErrorCallback, const double& samplingFreqInput)
{
	using namespace std;

	unsigned long samplesPerBuf;
	std::string parameterFileName, specializedParameterFileName;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	double sampleLength, maxRequiredProcFreq, transWidthProc, transWidthRec;
	float mainThreadCycleTime;
	vector<double> standardSamplingFreqs;
	CAudioInputParam params;

	// load audio settings
	LoadParameters( audioSettingsFileName, params );
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );

	// set the sound device for audio capture with the determined best parameters
	samplesPerBuf = static_cast<unsigned long>( samplingFreqInput * sampleLength );
	dataReader.SetParameters( device, samplingFreqInput, samplesPerBuf, numChannels, channel, maxMissedAttempts, maxLengthInputQueue, runtimeErrorCallback );
}



/**	@brief		Obtaining all basic parameters for setting the audio capture class
*	@param		device							Chosen input device. It can be obtained from CAudioInput::GetAudioDevices. For default device set it to an empty name.		
*	@param		audioSettingsFileName			File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.				
*	@param		samplingFreqInput				Sampling frequency for audio capture [Hz]
*	@param		downsamplingFactorProc			Downsampling factor for audio processing (signal analysis). This value corresponds to the sampling frequency.
*	@param		cutoffFreqProc					Cutoff frequency of the downsampling filter for the processing output data [Hz]. A value of 1 corresponds to the Nyquist frequency.
*	@param		downsamplingFactorRec			Downsampling factor for audio recording. This value corresponds to the sampling frequency and is identical to one if recording is not used.
*	@param		cutoffFreqRec					Cutoff frequency of the downsampling filter for the recording output data [Hz]. A value of 1 corresponds to the Nyquist frequency.
*	@param		requestedRecSamplingFreq		The sampling frequency requested by the user for recording the audio signal after a detection [Hz]. It is identical to zero, if no recording is required.
*	@return										None
*	@exception									None
*	@remarks									This function alone is not thread-safe
*/
void Core::CAudioInput::CPrivImplementation::GetAudioReaderParams(const Processing::CAudioDevice& device, const std::string& audioSettingsFileName, double& samplingFreqInput, int& downsamplingFactorProc, double& cutoffFreqProc, int& downsamplingFactorRec, double& cutoffFreqRec, const double& requestedRecSamplingFreq)
{
	using namespace std;
	using namespace boost::filesystem;

	std::string parameterFileName, specializedParameterFileName;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	double sampleLength, maxRequiredProcFreq, transWidthProc, transWidthRec;
	float mainThreadCycleTime;
	vector<double> standardSamplingFreqs, possibleSamplingFreqs;
	CAudioInputParam params;
	path dataPathName;

	// load audio settings
	LoadParameters( audioSettingsFileName, params );
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );

	// adjust filenames, it is assumed that the standard path of the underlying parameter files is identical to that of the audio settings file
	dataPathName = absolute( path( audioSettingsFileName ).parent_path() );
	CPrivImplementation::parameterFileName = absolute( parameterFileName, dataPathName ).string();
	CPrivImplementation::specializedParameterFileName = absolute( specializedParameterFileName, dataPathName ).string();

	// find the possible sampling frequency of the audio device
	possibleSamplingFreqs = GetPossibleSamplingFreqs( device, numChannels, standardSamplingFreqs );

	// determine the best sampling frequencies for all processes and the corresponding required downsampling factors
	GetBestWorkingParameters( possibleSamplingFreqs, maxRequiredProcFreq, requestedRecSamplingFreq, samplingFreqInput, downsamplingFactorProc, cutoffFreqProc, downsamplingFactorRec, cutoffFreqRec );
}



/**	@brief		Obtain the most relevant information for audio processing from the audio settings file
*	@param		audioSettingsFileName			File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		transWidthProc					Transition width of the audio processing filter [Hz]
*	@param		transWidthRec					Transition width of the audio recording filter [Hz]
*	@param		mainThreadCycleTime				Time between two cycles of the main thread controlling the full audio signal evaluation [s]
*	@return										None
*	@exception									None
*	@remarks									None
*/
void Core::CAudioInput::CPrivImplementation::GetRelevantAudioSettings(const std::string& audioSettingsFileName, double& transWidthProc, double& transWidthRec, float& mainThreadCycleTime)
{
	double sampleLength, maxRequiredProcFreq;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	std::string parameterFileName, specializedParameterFileName;
	std::vector<double> standardSamplingFreqs;
	CAudioInputParam params;

	// load audio settings
	LoadParameters( audioSettingsFileName, params );
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );
}



/**	@brief		Returns the version information of the Portaudio-library
*	@param		versionString					Version information
*	@param		buildNumber						Build number of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::CPrivImplementation::GetPortaudioVersion(std::string& versionString, int& buildNumber, std::string& licenseText)
{
	versionString = Pa_GetVersionText();
	buildNumber = Pa_GetVersion();
	licenseText = "Copyright(c) 1999-2011 Ross Bencina and Phil Burk";
}



/**	@brief		Returns the version information of the Alglib-library
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::CPrivImplementation::GetAlglibVersion(std::string& versionString, std::string& dateString, std::string& licenseText)
{
	#ifdef _WIN32
		alglib::GetVersionInfo( versionString, dateString, licenseText );
	#else
		versionString = std::string();
		dateString = std::string();
		licenseText = std::string();
	#endif
}
