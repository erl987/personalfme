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
/**	\defgroup	Core	Core library for digital signal processing of the audio signal.
*/
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <boost/filesystem.hpp>
#include "AudioDevice.h"
#include "SeqData.h"
#include "publicAudioSPDefinitions.h"

#if defined _WIN32
	#define AE_CPU AE_INTEL // enabling SSE2 for the Alglib-Library
#endif

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
	*	Structure for storing the information required to record audio signal data corresponding to a found sequence (for example voice information after an alarm sequence)
	*/
	struct RecordingParam {
		/**	@param		recordedCallback		Callback function called when recording signal data after a sequence is finished (CSeqData: time of the start of the sequence and code of the sequence, deque<float>: recorded audio signal data, float: sampling frequency [Hz] */
		std::function< void(const Utilities::CSeqData&, std::vector<float>, double) > recordedCallback;
		/**	@param		recordTimeBuffer		Time buffer for ensuring that data recorded for detecting the sequence can be stored (in seconds, always > 0). It must always be of a larger absolute value than recordTimeLowerLimit. */
		float recordTimeBuffer;
		/**	@param		recordTimeUpperLimit	Stopping time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence). */
		float recordTimeUpperLimit;
		/**	@param		recordTimeLowerLimit	Starting time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence). */
		float recordTimeLowerLimit;
		/**	@param		reqStoringSamplingFreq	Requested sampling frequency for storing in the audio data in the audio files (in Hz). Usually the real sampling frequency is different to some extent due to integer-based downsampling. If the value is zero, no downsampling will be performed. */
		double reqStoringSamplingFreq;
	};

	/**	\ingroup Core
	*	Base class for acquiring audio data from the audio input device and sending them to the signal processing classes. It is used by the concrete derived classes, for example for FME sequence analysis.
	*/
	class CAudioInput
	{
	public:
		AUDIOSP_API CAudioInput(void);
		AUDIOSP_API virtual ~CAudioInput(void);
		AUDIOSP_API virtual void Init( Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams = nullptr ) = 0;
		AUDIOSP_API virtual void InitBase( Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams = nullptr );
		AUDIOSP_API void StartAudioInput(void);
		AUDIOSP_API void StopAudioInput(void);
		AUDIOSP_API bool IsRunning(void);
		AUDIOSP_API void GetAvailableAudioDevices( std::vector<Processing::CAudioDevice>& devices, Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& audioSettingsFile ) const;
		AUDIOSP_API bool IsDeviceAvailable( double& maxStandardSamplingFreq, const Processing::CAudioDevice& device, const boost::filesystem::path& audioSettingsFile ) const;
		AUDIOSP_API void SetAudioDevice( const Processing::CAudioDevice& device );
		AUDIOSP_API static void SaveParameters( std::string audioSettingsFileName, double sampleLength, int numChannels, int maxLengthInputQueue, int maxMissedAttempts, int channel, std::string parameterFileName, std::string specializedParameterFileName, double maxRequiredProcFreq, double transWidthProc, double transWidthRec, float mainThreadCycleTime, std::vector<double> standardSamplingFreqs );
		AUDIOSP_API static void GetAudioSPVersion( std::string& versionString, std::string& dateString, std::string& licenseText );
		AUDIOSP_API static void GetPortaudioVersion( std::string& versionString, int& buildNumber, std::string& licenseText );
		AUDIOSP_API static void GetAlglibVersion( std::string& versionString, std::string& dateString, std::string& licenseText );
	protected:
		class CPrivImplementation;
		std::unique_ptr<CPrivImplementation> privHandle;
		CAudioInput (const CAudioInput &); // prevent copying
    	CAudioInput & operator= (const CAudioInput &);
	};
}

/*@}*/