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

#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSClientSession.h"

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


class Resources {
public:
	bool allUsers;
	std::map<std::string, unsigned int> labels;
	std::vector<std::string> scenarios;
	std::vector<std::string> units;
	std::vector<std::string> users;
};

class Message {
public:
	std::string messageText;
	std::string messageTemplate;
};

class Proxy {
public:
	std::string address;
	unsigned short port;
	std::string username;
	std::string password;
};

class AlarmConfig {
public:
	Resources resources;
	Message message;
	unsigned int closeEventInHours;
	Proxy proxy;
};


namespace External {
	class CGroupalarm2Gateway {
	public:
		NETWORKING_API CGroupalarm2Gateway(const unsigned int& organizationId, const std::string& apiToken);
		NETWORKING_API void sendAlarm(const std::array<unsigned int, 5> alarmCode, const std::string& alarmType, const boost::posix_time::ptime& alarmTimePoint, const AlarmConfig& alarmConfig, bool isTest);

	private:
		const std::string GROUPALARM_URI = "https://app.groupalarm.com/api/v1";

		const unsigned int organizationId;
		const std::string apiToken;

		void setProxy(Poco::Net::HTTPSClientSession& session, const Proxy& proxyConfig);
		bool hasJsonResponse(const Poco::Net::HTTPResponse& response, const std::string& responseBody);
		std::vector<unsigned int> parseResponseJson(const std::string& json, const std::vector<std::string>& entityNames, const std::string& subEndpoint);
		void raiseForStatus(const Poco::Net::HTTPResponse& response);
		Poco::JSON::Object getJsonPayload(const AlarmConfig& alarmConfig, const std::string& currIsoTime, const std::string& eventName);
		Poco::JSON::Object getAlarmResources(const AlarmConfig& alarmConfig);
		std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const std::string& organizationParam);
		std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint);
		std::vector<unsigned int> getIdsForUnits(const std::vector<std::string>& unitNames);
		std::vector<unsigned int> getIdsForLabels(const std::vector<std::string>& labelNames);
		std::vector<unsigned int> getIdsForUsers(const std::vector<std::string>& userNames);
		std::vector<unsigned int> getIdsForScenarios(const std::vector<std::string>& scenarioNames);
		unsigned int getAlarmTemplateId(const std::string& alarmTemplateName);
		template <class InputIt> static std::string join(InputIt first, InputIt last, const std::string& delim);
	};
}