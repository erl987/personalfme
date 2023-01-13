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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include <fstream>
#pragma warning(push)
#pragma warning(disable:4244) // Conversion: possible loss of data.
	#include <boost/archive/text_oarchive.hpp>
	#include <boost/archive/text_iarchive.hpp>
#pragma warning(pop)
#include "VersionInfo.h"
#include "AudioInputParam.h"
#include "privImplementation.h"



/**	@brief	Default constructor
*/
Core::CAudioInput::CAudioInput(void)
	: privHandle( new CPrivImplementation )
{
}



/**	@brief	Destructor
*/
Core::CAudioInput::~CAudioInput(void)
{
}



/** @brief		Initializes code searcher and Portaudio
*	@param		device							Input audio device. The available devices can be obtained from CAudioInput::GetAudioDevices.For default device set the name of the device empty.
*	@param		audioSettingsFileName			File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback					Function, which is called in case of found sequences (Utilities::CSeqData: (DD, MM, YYYY, HH, MM, SS, millisec), code digits, additional text information)
*	@param		runtimeErrorCallback			Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@param		recordingParams					Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@return										None
*	@exception	std::logic_error				Thrown if the input device is not ready
*	@exception	std::length_error				Thrown if the parameters for recording (recordTimeBuffer, recordTimeLowerLimit or recordTimeUpperLimit) are not consistent
*	@remarks									This is an virtual base class and must be called from its concrete derived classes, for example for analyzing FME sequences.
*/
void Core::CAudioInput::InitBase(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
{	
	privHandle->SetParameters( device, audioSettingsFileName, foundCallback, runtimeErrorCallback, recordingParams );
}



/**	@brief		Start reading and processing of audio input
*	@return										None
*	@exception									None
*	@remarks									This function starts the complete processing of the audio device input for code sequences.
*												The processing can be stopped by calling CAudioInput::StopAudioInput.
*/
void Core::CAudioInput::StartAudioInput(void)
{
	privHandle->StartAudioInput();
}



/**	@brief		Stop reading and processing of audio input
*	@return										None
*	@exception									None
*	@remarks									This function stops the audio processing started by CAudioInput::StartAudioInput
*/
void Core::CAudioInput::StopAudioInput(void)
{
	privHandle->StopAudioInput();
}



/**	@brief		Checks if the processing of audio input is running
*	@return										True if the audio input processing is running, false otherwise
*	@exception									None
*	@remarks									None
*/
bool Core::CAudioInput::IsRunning(void)
{
	return privHandle->IsRunning();
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
void Core::CAudioInput::GetAvailableAudioDevices( std::vector<Processing::CAudioDevice>& devices, Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& audioSettingsFile ) const
{
	privHandle->GetAvailableAudioDevices( devices, stdDevice, maxStandardSamplingFreq, audioSettingsFile );
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
bool Core::CAudioInput::IsDeviceAvailable( double& maxStandardSamplingFreq, const Processing::CAudioDevice& device, const boost::filesystem::path& audioSettingsFile ) const
{
	return privHandle->IsDeviceAvailable( maxStandardSamplingFreq, device, audioSettingsFile );
}



/**	@brief		Set the input device
*	@param		device							Chosen input device. The available devices can be obtained from CAudioInput::GetAudioDevices. For default device set the name of the device empty.
*	@return										None
*	@exception	std::runtime_error				Thrown if this function is called before starting properly the audio devices.
*	@remarks									Any call before calling CAudioInput::Init or the constructor will not be successfull.
*/
void Core::CAudioInput::SetAudioDevice(const Processing::CAudioDevice& device)
{
	privHandle->SetAudioDevice( device );
}



/**	@brief		Save audio settings
*	@param		audioSettingsFileName			Name of data file with an absolute path (*.dat)
*	@param		sampleLength					Length of timestep for sample acquisition [s]
*	@param		numChannels						Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param		maxLengthInputQueue				Maximum senseful input queue length
*	@param		maxMissedAttempts				Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param		channel							Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'
*	@param		parameterFileName				File name of general code analysis settings file
*	@param		specializedParameterFileName	File name of FME code analysis settings file
*	@param		maxRequiredProcFreq				Maximum frequency required for useful process data stream [Hz]. This value is typically corresponding to the maximum tone frequency possible.
*	@param		transWidthProc					Transition width of the audio processing filter [Hz]
*	@param		transWidthRec					Transition width of the audio recording filter [Hz]
*	@param		mainThreadCycleTime				Time between two cycles of the main thread controlling the full audio signal evaluation [s]
*	@param		standardSamplingFreqs			Vector container storing all sampling frequencies that might be used for audio processing [Hz]
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::SaveParameters(std::string audioSettingsFileName, double sampleLength, int numChannels, int maxLengthInputQueue, int maxMissedAttempts, int channel, std::string parameterFileName, std::string specializedParameterFileName, double maxRequiredProcFreq, double transWidthProc, double transWidthRec, float mainThreadCycleTime, std::vector<double> standardSamplingFreqs)
{
	CAudioInputParam params;

	// generate serialization object
	params.Set( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );

	// initialize serialization
	std::ofstream ofs( audioSettingsFileName );
	boost::archive::text_oarchive oa( ofs );

	// serialize parameter object
	const CAudioInputParam constParams = params; // workaround for correct storing of non-pointer
	oa << constParams;

	ofs.close();
}



/**	@brief		Returns the version information of the Core-library
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::GetAudioSPVersion(std::string& versionString, std::string& dateString, std::string& licenseText)
{
	Utilities::CVersionInfo::GetSoftwareVersion( versionString, dateString, licenseText );
}



/**	@brief		Returns the version information of the Portaudio-library
*	@param		versionString					Version number information
*	@param		buildNumber						Build number of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::GetPortaudioVersion(std::string& versionString, int& buildNumber, std::string& licenseText)
{
	CPrivImplementation::GetPortaudioVersion( versionString, buildNumber, licenseText );
}



/**	@brief		Returns the version information of the Alglib-library
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Core::CAudioInput::GetAlglibVersion(std::string& versionString, std::string& dateString, std::string& licenseText)
{
	CPrivImplementation::GetAlglibVersion( versionString, dateString, licenseText );
}