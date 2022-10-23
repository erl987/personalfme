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
#include <string>


#if defined _WIN32 || defined __CYGWIN__
	#ifdef Middleware_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define Middleware_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define Middleware_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define Middleware_API __attribute__ ((visibility ("default")))
	#else
		#define Middleware_API
	#endif		
#endif

/*@{*/
/** \ingroup Middleware
*/
namespace Middleware {
	/** \ingroup Middleware
	*	Class defining basic audio setting parameters
	*/
	class CAudioSettings
	{
	public:
		Middleware_API CAudioSettings() {};
		Middleware_API ~CAudioSettings() {};
		/**	@brief		Name of parameter file containing all basic settings, including the file names of the other required settings files */
		Middleware_API static const std::string GetAudioSettingsFileName()
		{
				return "audioSettings.dat";
		}
		/**	@brief		Starting time of audio recording relative to the start of the sequence. Fix to the end of the second repetition of the sequence. */
		Middleware_API static const float GetRecordTimeLowerLimit()
		{
			return 1.9f;
		}
		/**	@brief		Time buffer for ensuring that data recorded for detecting the sequence can be stored (in seconds, always >= 0). Not required due to the chosen value of recordTimeLowerLimit */
		Middleware_API static const float GetRecordTimeBuffer()
		{
			return 10.0f;
		}
		/**	@brief		Sampling frequency for storing the audio files [Hz]. Note that due to integer-based downsampling the real sampling frequency will be different. */
		Middleware_API static const float GetStoringSamplingFreq()
		{
			return 14000.0f;
		}
	};
}
/*@}*/

