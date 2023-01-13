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
#pragma once

#include <vector>
#include <deque>
#include <string>
#include <boost/filesystem.hpp>
#include "AudioDevice.h"
#include "AlarmMessagesDatabase.h"
#include "GatewayLoginDatabase.h"

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
	*	Class representing user parameters for real-time FME-sequence detection
	*/
	class CSettingsParam
	{
	public:
		Middleware_API CSettingsParam(void);
		Middleware_API ~CSettingsParam(void);
		Middleware_API void GetFromXML( const boost::filesystem::path& xmlFile, const boost::filesystem::path& schemaDir, const boost::filesystem::path& pluginDir );
		Middleware_API void GenerateXML( const boost::filesystem::path& xmlFile, const boost::filesystem::path& schemaDir );
		Middleware_API void Set(const Core::Processing::CAudioDevice& device, const float& minDistanceRepetition, const bool& isPlayTone, const External::CGatewayLoginDatabase& loginDatabase, const External::CAlarmMessageDatabase& alarmDatabase, const float& recordingLength, const bool& isDefaultRecording, const std::string& audioFormatID );
		Middleware_API void Get(Core::Processing::CAudioDevice& device, float& minDistanceRepetition, bool& isPlayTone, External::CGatewayLoginDatabase& loginDatabase, External::CAlarmMessageDatabase& alarmDatabase, float& recordingLength, bool& isDefaultRecording, std::string& audioFormatID ) const;
		Middleware_API void SetFunctionalitySettings(const Core::Processing::CAudioDevice& device, const float& minDistanceRepetition, const bool& isPlayTone);
		Middleware_API void GetFunctionalitySettings(Core::Processing::CAudioDevice& device, float& minDistanceRepetition, bool& isPlayTone) const;
		Middleware_API std::vector< std::vector<int> > GetWhitelist() const;
		Middleware_API void SetGatewaySettings(const External::CGatewayLoginDatabase& loginDatabase, const External::CAlarmMessageDatabase& alarmDatabase);
		Middleware_API void GetGatewaySettings(External::CGatewayLoginDatabase& loginDatabase, External::CAlarmMessageDatabase& alarmDatabase) const;
		Middleware_API void SetRecordingSettings( const float& recordingLength, const bool& isDefaultRecording, const std::string& audioFormatID );
		Middleware_API void GetRecordingSettings( float& recordingLength, bool& isDefaultRecording, std::string& audioFormatID ) const;
		Middleware_API bool IsValid() const;
		Middleware_API friend bool operator==( const CSettingsParam& lhs, const CSettingsParam& rhs );
		Middleware_API friend bool operator!=( const CSettingsParam& lhs, const CSettingsParam& rhs );
	protected:
		Middleware_API void UpdateWhitelist();
	private:
		Core::Processing::CAudioDevice device;
		float minDistanceRepetition;
		bool isPlayTone;
		External::CGatewayLoginDatabase loginDatabase;
		External::CAlarmMessageDatabase alarmDatabase;
		float recordingLength;
		bool isDefaultRecording;
		std::string audioFormatID;

		std::vector< std::vector<int> > whitelist; // this is an automatically generated variable and needs no storage
		bool isInitFunctionality;
		bool isInitRecording;
	};

	Middleware_API bool operator==( const CSettingsParam& lhs, const CSettingsParam& rhs );
	Middleware_API bool operator!=( const CSettingsParam& lhs, const CSettingsParam& rhs );
}
/*@}*/
