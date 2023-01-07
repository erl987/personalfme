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
#include "InfoalarmMessageDecorator.h"
#include "EmailMessage.h"
#include "Groupalarm2LoginData.h"
#include "Groupalarm2Message.h"
#include "Groupalarm2Gateway.h"


/**	\ingroup Networking
*	Private implementation class for the e-mail gateway
*/
class External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl {
public:
	CGroupalarm2GatewayImpl();
	virtual ~CGroupalarm2GatewayImpl() {};
	void Send(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile);

private:
	const std::string GROUPALARM_URI = "https://app.groupalarm.com/api/v1";

	void SetProxy(Poco::Net::HTTPSClientSession& session, const CGroupalarm2LoginData& loginData);
	std::string ExtractErrorInfo(const Poco::Net::HTTPResponse& response, const std::string& responseBody);
	std::map<std::string, unsigned int> ParseResponseJson(const std::string& json, const std::string& entry);
	std::map<std::pair<std::string, std::string>, unsigned int> ParseResponseJsonForUsers(const std::string& json);
	std::vector<unsigned int> GetIdsForEntities(std::map<std::string, unsigned int> entityIdMap, const std::vector<std::string>& entityNames, const std::string& subEndpoint, unsigned int organizationId);
	std::vector<unsigned int> GetIdsForUsers(std::map<std::pair<std::string, std::string>, unsigned int> userIdMap, const std::vector<std::pair<std::string, std::string>>& userNames, unsigned int organizationId);
	void RaiseForStatus(const Poco::Net::HTTPResponse& response, const std::string& responseBody);
	Poco::JSON::Object CreateAlarmJson(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<External::CAlarmMessage> message, const CGroupalarm2LoginData& loginData);
	Poco::JSON::Object GetJsonPayload(const CGroupalarm2Message& message, const std::string& currIsoTime, const std::string& eventName, const std::string& otherMessagesInfo, const CGroupalarm2LoginData& loginData);
	Poco::JSON::Object GetAlarmResources(const CGroupalarm2Message& message, const CGroupalarm2LoginData& loginData);
	std::string GetJsonFromEndpoint(const std::string& subEndpoint, const CGroupalarm2LoginData& loginData, const std::string& organizationParam);
	std::vector<unsigned int> GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const CGroupalarm2LoginData& loginData);
	std::vector<unsigned int> GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const CGroupalarm2LoginData& loginData, const std::string& organizationParam);
	std::vector<unsigned int> GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const CGroupalarm2LoginData& loginData, const std::string& organizationParam, const std::string& entry);
	std::vector<unsigned int> GetIdsForUnits(const std::vector<std::string>& unitNames, const CGroupalarm2LoginData& loginData);
	std::vector<unsigned int> GetIdsForLabels(const std::vector<std::string>& labelNames, const CGroupalarm2LoginData& loginData);
	std::vector<unsigned int> GetIdsForUsers(const std::vector<std::pair<std::string, std::string>>& userNames, const CGroupalarm2LoginData& loginData);
	std::vector<unsigned int> GetIdsForScenarios(const std::vector<std::string>& scenarioNames, const CGroupalarm2LoginData& loginData);
	unsigned int GetIdForAlarmTemplate(const std::string& alarmTemplateName, const CGroupalarm2LoginData& loginData);
	unsigned int GetIdForMessageTemplate(const std::string& messageTemplateName, const CGroupalarm2LoginData& loginData);
	std::string CreateOtherMessagesInfo(const External::Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage);
	std::string GetAlarmReceiverID(const External::Email::CEmailMessage& message);
	void PerformSend(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile);
};
/*@}*/