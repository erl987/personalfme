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
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include "FMEAudioInputDebug.h"
#include "SequencePasserDebug.h"
#include "privImplementation.h"



/** @brief		Standard constructor.	
*	@remarks							None
*/
Core::CFMEAudioInputDebug::CFMEAudioInputDebug(void)
	: CFMEAudioInput()
{
}



/** @brief		Initializes code searcher and Portaudio.
*	@param		device											Input device. It can be obtained from CAudioInput::GetAudioDevices.For default device set set it to an empty name.
*	@param		audioSettingsFileName							File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback									Function, which is called in case of found sequences. It will transfer debug information: ( start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing additional information (empty for FME codes) )
*	@param		runtimeErrorCallback							Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@param		recordingParams									Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@return														None
*	@exception	std::logic_error								Thrown if the input device is not ready
*	@exception													See CSearchForCode::Init
*	@remarks													This function is intended for debugging purposes, when detailed information on the found sequences is required (tone frequencies, periods, ...)
*/
Core::CFMEAudioInputDebug::CFMEAudioInputDebug(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void ( const Utilities::CSeqDataComplete<float>& )> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
	: CFMEAudioInput()
{
	Init( device, audioSettingsFileName, foundCallback, runtimeErrorCallback, recordingParams );
}



/** @brief		Standard destructor.	
*	@remarks							None
*/
Core::CFMEAudioInputDebug::~CFMEAudioInputDebug(void)
{
}



/** @brief		Initializes code searcher and Portaudio.
*	@param		device											Input device. It can be obtained from CAudioInput::GetAudioDevices.For default device set it to an empty name.
*	@param		audioSettingsFileName							File name of audio device settings file, it must be given as an absolute path. It is assumed that all underlying parameter files are located relative to this path.
*	@param		foundCallback									Function, which is called in case of found sequences. It will transfer debug information: ( start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing additional information (empty for FME codes) )
*	@param		runtimeErrorCallback							Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@param		recordingParams									Structure containing all the information required for recording audio signal data connected to a sequence. This can for example be used for recording voice information after a sequence. It can be omitted.
*	@return														None
*	@exception	std::logic_error								Thrown if the input device is not ready
*	@exception													See CSearchForCode::Init
*	@remarks													This function is intended for debugging purposes, when detailed information on the found sequences is required (tone frequencies, periods, ...)
*/
void Core::CFMEAudioInputDebug::Init(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void ( const Utilities::CSeqDataComplete<float>& )> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams)
{
	std::function< void(const Utilities::CSeqData&) > dummy;
	CFMEAudioInput::Init( device, audioSettingsFileName, dummy, runtimeErrorCallback, recordingParams );
	privHandle->ResetSequencePasser( std::shared_ptr< Audio::CSequencePasser<float> >( new Audio::CSequencePasserDebug<float>( foundCallback, runtimeErrorCallback ) ) );
}
