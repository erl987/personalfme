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
		#define NETWORKING_API __attribute__ ((dllexport))
#else
	// Microsoft Visual Studio
	#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <array>
#include <string>
#include "Groupalarm2Gateway.h"
#include "Groupalarm2GatewayImpl.h"


External::CGroupalarm2Gateway::CGroupalarm2Gateway(const unsigned int& organizationId, const std::string& apiToken)
	: privImpl(new CGroupalarm2GatewayImpl(organizationId, apiToken))
{
}

External::CGroupalarm2Gateway::~CGroupalarm2Gateway()
{
};


void External::CGroupalarm2Gateway::sendAlarm(const std::array<unsigned int, 5> alarmCode, const std::string& alarmType, const boost::posix_time::ptime& alarmTimePoint, const External::Groupalarm2::AlarmConfig& alarmConfig, bool isTest)
{
	privImpl->sendAlarm(alarmCode, alarmType, alarmTimePoint, alarmConfig, isTest);
}