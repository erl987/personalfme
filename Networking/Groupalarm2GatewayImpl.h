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
#include "Groupalarm2LoginData.h"
#include "Groupalarm2Gateway.h"


class External::CGroupalarm2Gateway::CGroupalarm2GatewayImpl {
public:
	CGroupalarm2GatewayImpl(const unsigned int& organizationId, const std::string& apiToken);
	virtual ~CGroupalarm2GatewayImpl() {};
	void sendAlarm(const std::array<unsigned int, 5> alarmCode, const std::string& alarmType, const boost::posix_time::ptime& alarmTimePoint, const External::Groupalarm2::AlarmConfig& alarmConfig, bool isTest);

private:
	const std::string GROUPALARM_URI = "https://app.groupalarm.com/api/v1";

	const unsigned int organizationId;
	const std::string apiToken;

	void setProxy(Poco::Net::HTTPSClientSession& session, const External::Groupalarm2::Proxy& proxyConfig);
	bool hasJsonResponse(const Poco::Net::HTTPResponse& response, const std::string& responseBody);
	std::vector<unsigned int> parseResponseJson(const std::string& json, const std::vector<std::string>& entityNames, const std::string& subEndpoint);
	void raiseForStatus(const Poco::Net::HTTPResponse& response);
	Poco::JSON::Object getJsonPayload(const External::Groupalarm2::AlarmConfig& alarmConfig, const std::string& currIsoTime, const std::string& eventName);
	Poco::JSON::Object getAlarmResources(const External::Groupalarm2::AlarmConfig& alarmConfig);
	std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const std::string& organizationParam);
	std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint);
	std::vector<unsigned int> getIdsForUnits(const std::vector<std::string>& unitNames);
	std::vector<unsigned int> getIdsForLabels(const std::vector<std::string>& labelNames);
	std::vector<unsigned int> getIdsForUsers(const std::vector<std::string>& userNames);
	std::vector<unsigned int> getIdsForScenarios(const std::vector<std::string>& scenarioNames);
	unsigned int getAlarmTemplateId(const std::string& alarmTemplateName);
	template <class InputIt> static std::string join(InputIt first, InputIt last, const std::string& delim);
};