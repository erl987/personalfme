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

#include <memory>
#include <vector>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Groupalarm2LoginData.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef NETWORKING_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define NETWORKING_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define NETWORKING_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define NETWORKING_API __attribute__ ((visibility ("default")))
	#else
		#define NETWORKING_API
	#endif		
#endif


namespace External {
	class CGroupalarm2Gateway {
	public:
		NETWORKING_API CGroupalarm2Gateway(const unsigned int& organizationId, const std::string& apiToken);
		NETWORKING_API virtual ~CGroupalarm2Gateway();
		NETWORKING_API void sendAlarm(const std::array<unsigned int, 5> alarmCode, const std::string& alarmType, const boost::posix_time::ptime& alarmTimePoint, const External::Groupalarm2::AlarmConfig& alarmConfig, bool isTest);
	private:
		class CGroupalarm2GatewayImpl;
		std::unique_ptr<CGroupalarm2GatewayImpl> privImpl;
	};
}