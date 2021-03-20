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

#include "FMEAudioInput.h"
#include "privImplementation.h"
#include "FME.h"



/**	@brief	Default constructor.
*/
Core::CFMEAudioInput::CFMEAudioInput(void)
	: CAudioInput()
{
}



/** @brief		Constructor.
*				Initializes FFTW and Portaudio for FME code analysis.
*	@param		device					Input device. The index can be obtained from CAudioInput::GetAudioDevices. For default device set it with an empty name.
*	@param		audioSettingsFileName	File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback			Function, which is called in case of found sequences (Utilities::CSeqData arguments: (DD, MM, YYYY, HH, MM, SS, millisec), code digits, additional text information (empty for FME codes))
*	@param		runtimeErrorCallback	Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@param		recordingParams			Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@exception							See CSearchForCode::Init
*	@remark								None
*/
Core::CFMEAudioInput::CFMEAudioInput(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
	: CAudioInput()
{
	// initialize audio detection system
	Init( device, audioSettingsFileName, foundCallback, runtimeErrorCallback, recordingParams );
}



/**	@brief	Default destructor.
*/
Core::CFMEAudioInput::~CFMEAudioInput(void)
{
}



/** @brief		Initializes code searcher and Portaudio for FME code analysis.
*	@param		device											Input device. It can be obtained from CAudioInput::GetAudioDevices.For default device set it with an empty name.
*	@param		audioSettingsFileName							File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback									Function, which is called in case of found sequences (Utilities::CSeqData arguments: (DD, MM, YYYY, HH, MM, SS, millisec), code digits, additional text information (empty for FME codes))
*	@param		runtimeErrorCallback							Function, which is called in case of a runtime error during execution. The execution will not stop automatically! 
*	@param		recordingParams									Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@return														None
*	@exception	std::logic_error								Thrown if the input device is not ready
*	@exception													See CSearchForCode::Init
*	@remark														None
*/
void Core::CFMEAudioInput::Init(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
{
	double samplingFreqProcessing, samplingFreqInput;
	std::string parameterFileName, specializedParameterFileName;

	CAudioInput::InitBase( device, audioSettingsFileName, foundCallback, runtimeErrorCallback, recordingParams );

	// initialize analysis thread for FME code analysis
	privHandle->GetBasicInformation( samplingFreqProcessing, samplingFreqInput, parameterFileName, specializedParameterFileName );
	privHandle->ResetSearcher( std::shared_ptr< Core::General::CSearch<float> >( new Core::FME::CFME<float>( samplingFreqProcessing, parameterFileName, specializedParameterFileName, runtimeErrorCallback ) ) );
}



/** @brief		Returns the length of a FME sequence according to TR-BOS FME.
*	@return														Length of a FME sequence according to TR-BOS FME
*	@exception													None
*	@remark														None
*/
int Core::CFMEAudioInput::GetSequenceLength(void)
{
	return LENGTH_SEQUENCE;
}



/** @brief		Returns the sampling frequency used for audio capture
*	@return														Sampling frequency for audio capture [Hz]
*	@exception													None
*	@remark														None
*/
double Core::CFMEAudioInput::GetAudioCaptureSamplingRate()
{
	double samplingFreqProcessing, samplingFreqInput;
	std::string parameterFileName, specializedParameterFileName;

	privHandle->GetBasicInformation( samplingFreqProcessing, samplingFreqInput, parameterFileName, specializedParameterFileName );

	return samplingFreqInput;
}