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
#pragma once

#include "FMEAudioInput.h"
#include "SeqDataComplete.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef AUDIOSP_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define AUDIOSP_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define AUDIOSP_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define AUDIOSP_API __attribute__ ((visibility ("default")))
	#else
		#define AUDIOSP_API
	#endif		
#endif

/*@{*/
/** \ingroup Core
*/
namespace Core {
/**	\ingroup Core
*	Class for getting FME sequences with full sequence information (tone frequencies, periods, tone lengths, ...) from data of the audio device in real time. This class is only intended for debugging.
*/
	class CFMEAudioInputDebug :
	public Core::CFMEAudioInput
	{
	public:
		AUDIOSP_API CFMEAudioInputDebug(void);
		AUDIOSP_API CFMEAudioInputDebug(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void ( const Utilities::CSeqDataComplete<float>& )> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams = nullptr);
		AUDIOSP_API ~CFMEAudioInputDebug(void);		
		AUDIOSP_API void Init(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void ( const Utilities::CSeqDataComplete<float>& )> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams = nullptr);
	protected:
		CFMEAudioInputDebug (const CFMEAudioInputDebug &); // prevent copying
    	CFMEAudioInputDebug & operator= (const CFMEAudioInputDebug &);
	};
}

/*@}*/