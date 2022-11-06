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
#include <vector>
#include <tuple>
#include <deque>
#include <memory>
#include "FMEClient.h"
#include "SeqData.h"
#include "AudioInput.h"
#include "AudioPlugin.h"
#include "ExecutionRuntime.h"

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
	*	Class implementing the detector runtime environment
	*/
	class CExecutionDetectorRuntime : public CExecutionRuntime
	{
	public:
		Middleware_API CExecutionDetectorRuntime(const CSettingsParam& newParams, const boost::filesystem::path& appSettingsDir, const boost::filesystem::path& audioDir, const boost::filesystem::path& pluginDir, std::function<void(const Utilities::CSeqData&)> onFoundSequenceCallback, std::function < void( const Utilities::CSeqData&, const Utilities::CMediaFile& ) > onRecordedDataCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> messageFromDetectorCallback );
		Middleware_API virtual ~CExecutionDetectorRuntime(void);
		Middleware_API virtual void Run(void);
	private:
		Middleware_API virtual void OnProcessingSequence(const Utilities::CSeqData& sequence);
		Middleware_API virtual void OnRecordedData(const Utilities::CSeqData& sequence, std::vector<float> signalData, double samplingFreq);

		bool isInit;
		std::mutex audioInputMutex;
		std::unique_ptr<Core::CAudioInput> audioInput;	
		std::function < void( const Utilities::CSeqData&, const Utilities::CMediaFile& ) > onRecordedDataCallback;
		boost::filesystem::path audioDir;
		std::unique_ptr<Utilities::Plugins::AudioPlugin> audioPlugin;
	};

	Middleware_API void GetAvailableAudioDevices( std::vector<Core::Processing::CAudioDevice>& devices, Core::Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& appSettingsDir );
	Middleware_API bool IsDeviceAvailable( double& maxStandardSamplingFreq, const Core::Processing::CAudioDevice& device, const boost::filesystem::path& appSettingsDir );
}
/*@}*/
