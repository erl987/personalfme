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
#include <fstream>
#include <iostream>
#include <utility>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/NetException.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"
#include "StringUtilities.h"
#include "Groupalarm2Message.h"
#include "ExternalProgramMessage.h"
#include "Groupalarm2GatewayImpl.h"


std::string External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::ExtractErrorInfo(const Poco::Net::HTTPResponse& response, const std::string& responseBody) {
	using namespace std;
	using namespace Poco::JSON;

	string errorInfo = "";

	if (response.getContentType() != "text/html") {
		Parser parser;
		auto jsonVar = parser.parse(responseBody);
		if (!jsonVar.isArray()) {
			const auto& jsonObj = jsonVar.extract<Object::Ptr>();

			if (jsonObj->has("success") && jsonObj->has("error") && !jsonObj->getValue<bool>("success")) {
				string message = jsonObj->getValue<string>("message");
				string details = jsonObj->getValue<string>("error");
				errorInfo = "Fehler von Groupalarm.com: " + message + ", Details: " + details;
			}
		}
	}
	
	return errorInfo;
}

std::map<std::string, unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::ParseResponseJson(const std::string& json, const std::string& entry) {
	using namespace std;
	using namespace Poco::JSON;

	Array::Ptr jsonObj;

	Parser parser;
	auto result = parser.parse(json);

	if (entry.empty()) {
		jsonObj = result.extract<Array::Ptr>();
	} else {
		jsonObj = result.extract<Object::Ptr>()->getArray(entry);
	}

	map<string, unsigned int> entityIdMap;
	for (int i = 0; i < jsonObj->size(); i++) {
		const auto& entry = jsonObj->get(i).extract<Object::Ptr>();
		entityIdMap[entry->getValue<string>("name")] = entry->getValue<unsigned int>("id");
	}

	return entityIdMap;
}

std::map<std::pair<std::string, std::string>, unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::ParseResponseJsonForUsers(const std::string& json) {
	using namespace std;
	using namespace Poco::JSON;

	Parser parser;
	auto result = parser.parse(json);
	Array::Ptr jsonObj = result.extract<Array::Ptr>();

	map<pair<string, string>, unsigned int> userIdMap;
	for (int i = 0; i < jsonObj->size(); i++) {
		const auto& entry = jsonObj->get(i).extract<Object::Ptr>();
		string firstName = entry->getValue<string>("name");
		string lastName = entry->getValue<string>("surname");
		userIdMap[make_pair<>(firstName, lastName)] = entry->getValue<unsigned int>("id");
	}

	return userIdMap;
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForEntities(std::map<std::string, unsigned int> entityIdMap, const std::vector<std::string>& entityNames, const std::string& subEndpoint, unsigned int organizationId) {
	using namespace std;
	using namespace Utilities;
	
	vector<string> foundEntityNames;
	vector<unsigned int> entityIds;

	for (const string& entityName : entityNames) {
		if (entityIdMap.count(entityName) > 0) {
			entityIds.push_back(entityIdMap[entityName]);
			foundEntityNames.push_back(entityName);
		}
	}

	if (entityIds.size() != entityNames.size()) {
		vector<string> missingEntities;
		set_difference(entityNames.begin(), entityNames.end(), foundEntityNames.begin(), foundEntityNames.end(), inserter(missingEntities, missingEntities.begin()));

		string errorMessage = "Did not find the following *" + subEndpoint + "* in the Groupalarm organization " + to_string(organizationId) + ": " + CStringUtilities().Join(missingEntities.cbegin(), missingEntities.cend(), ", ");
		throw Exception::GroupalarmClientError(errorMessage);
	}

	return entityIds;
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForUsers(std::map<std::pair<std::string, std::string>, unsigned int> userIdMap, const std::vector<std::pair<std::string, std::string>>& userNames, unsigned int organizationId) {
	using namespace std;
	using namespace Utilities;

	vector<string> foundUserNames;
	vector<unsigned int> userIds;

	for (const auto& userName : userNames) {
		if (userIdMap.count(userName) > 0) {
			userIds.push_back(userIdMap[userName]);
			foundUserNames.push_back(userName.first + " " + userName.second);
		}
	}

	if (userIds.size() != userNames.size()) {
		vector<string> allUserNames;
		for (const auto& user : userNames) {
			allUserNames.push_back(user.first + " " + user.second);
		}

		vector<string> missingUsers;
		set_difference(allUserNames.begin(), allUserNames.end(), foundUserNames.begin(), foundUserNames.end(), inserter(missingUsers, missingUsers.begin()));

		string errorMessage = "Did not find the following users in the Groupalarm organization " + to_string(organizationId) + ": " + CStringUtilities().Join(missingUsers.cbegin(), missingUsers.cend(), ", ");
		throw Exception::GroupalarmClientError(errorMessage);
	}

	return userIds;
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::RaiseForStatus(const Poco::Net::HTTPResponse& response, const std::string& responseBody) {
	std::string errorInfo = ExtractErrorInfo(response, responseBody);
	std::string errorMessage = "status code " + std::to_string(response.getStatus()) + ": " + response.getReason();
	if (!errorInfo.empty()) {
		errorMessage += ", " + errorInfo;
	}

	if (response.getStatus() >= 300 && response.getStatus() < 500) {
		throw Exception::GroupalarmClientError("Client error, " + errorMessage);
	}
	else if (response.getStatus() >= 500 && response.getStatus() < 600) {
		throw Exception::GroupalarmServerError("Server error, " + errorMessage);
	}
}

std::string External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetJsonFromEndpoint(const std::string& subEndpoint, const CGroupalarm2LoginData& loginData, const std::string& organizationParam) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	URI uri(GROUPALARM_URI + "/" + subEndpoint);
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	SetProxy(session, loginData);
	uri.addQueryParameter(organizationParam, to_string(loginData.GetOrganizationId()));
	if (subEndpoint != "scenarios") { // note: not supported for scenarios
		uri.addQueryParameter("all", "true");
	}
	string path(uri.getPathAndQuery());

	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.add("API-Token", loginData.GetApiToken());
	session.sendRequest(request);

	HTTPResponse response;
	istream& responseStream = session.receiveResponse(response);
	string jsonResponseBody(istreambuf_iterator<char>(responseStream), {});

	RaiseForStatus(response, jsonResponseBody);

	return jsonResponseBody;
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const CGroupalarm2LoginData& loginData) {
	std::string json = GetJsonFromEndpoint(subEndpoint, loginData, "organization");
	std::string entry = "";
	if (subEndpoint != "scenarios") { // note: not supported for scenarios
		entry = subEndpoint;
	}
	auto entityIdMap = ParseResponseJson(json, entry);
	return GetIdsForEntities(entityIdMap, entityNames, subEndpoint, loginData.GetOrganizationId());
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, const CGroupalarm2LoginData& loginData, const std::string& organizationParam, const std::string& entry) {
	std::string json = GetJsonFromEndpoint(subEndpoint, loginData, organizationParam);
	auto entityIdMap = ParseResponseJson(json, entry);
	return GetIdsForEntities(entityIdMap, entityNames, subEndpoint, loginData.GetOrganizationId());
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForUnits(const std::vector<std::string>& unitNames, const CGroupalarm2LoginData& loginData) {
	return GetEntityIdsFromEndpoint(unitNames, "units", loginData);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForLabels(const std::vector<std::string>& labelNames, const CGroupalarm2LoginData& loginData) {
	return GetEntityIdsFromEndpoint(labelNames, "labels", loginData);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForUsers(const std::vector<std::pair<std::string, std::string>>& userNames, const CGroupalarm2LoginData& loginData) {
	std::string json = GetJsonFromEndpoint("users", loginData, "organization");
	auto userIdMap = ParseResponseJsonForUsers(json);
	return GetIdsForUsers(userIdMap, userNames, loginData.GetOrganizationId());
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForScenarios(const std::vector<std::string>& scenarioNames, const CGroupalarm2LoginData& loginData) {
	return GetEntityIdsFromEndpoint(scenarioNames, "scenarios", loginData);
}

unsigned int External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdForMessageTemplate(const std::string& messageTemplateName, const CGroupalarm2LoginData& loginData) {
	return GetEntityIdsFromEndpoint({ messageTemplateName }, "alarms/templates", loginData, "organization_id", "templates")[0];
}

unsigned int External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdForAlarmTemplate(const std::string& alarmTemplateName, const CGroupalarm2LoginData& loginData) {
	return GetEntityIdsFromEndpoint({ alarmTemplateName }, "alarms/resource-templates", loginData, "organization_id", "templates")[0];
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetAlarmResources(const CGroupalarm2Message& message, const CGroupalarm2LoginData& loginData) {
	using namespace std;

	Poco::JSON::Object alarmResources;

	if (message.ToAllUsers()) {
		alarmResources.set("allUsers", true);
	}
	else {
		if (message.ToLabels()) {
			vector<string> labelNames;
			for (const auto& entry : message.GetLabels()) {
				labelNames.push_back(entry.first);
			}

			auto labelIds = GetIdsForLabels(labelNames, loginData);

			vector<Poco::JSON::Object> labelsArray;
			unsigned int index = 0;
			for (const auto& entry : message.GetLabels()) {
				Poco::JSON::Object labelObject;
				labelObject.set("amount", entry.second);
				labelObject.set("labelID", labelIds[index]);
				labelsArray.push_back(labelObject);
				index++;
			}
			alarmResources.set("labels", labelsArray);
		}

		if (message.ToUnits()) {
			auto unitIds = GetIdsForUnits(message.GetUnits(), loginData);
			alarmResources.set("units", unitIds);
		}

		if (message.ToUsers()) {
			auto userIds = GetIdsForUsers(message.GetUsers(), loginData);
			alarmResources.set("users", userIds);
		}

		if (message.ToScenarios()) {
			auto scenarioIds = GetIdsForScenarios(message.GetScenarios(), loginData);
			alarmResources.set("scenarios", scenarioIds);
		}
	}

	return alarmResources;
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::SetProxy(Poco::Net::HTTPSClientSession& session, const CGroupalarm2LoginData& loginData) {
	if (loginData.IsWithProxy()) {
		session.setProxyHost(loginData.GetProxyAddress());
		session.setProxyPort(loginData.GetProxyPort());

		if (loginData.IsWithProxyWithUserNameAndPassword()) {
			session.setProxyUsername(loginData.GetProxyUserName());
			session.setProxyPassword(loginData.GetProxyPassword());
		}
		else if (loginData.IsWithProxyWithUserNameOnly()) {
			session.setProxyUsername(loginData.GetProxyUserName());
		}
	}
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetJsonPayload(const CGroupalarm2Message& message, const std::string& currIsoTime, const std::string& eventName, const std::string& otherMessagesInfo, const CGroupalarm2LoginData& loginData) {
	using namespace boost::posix_time;

	Poco::JSON::Object jsonPayload;

	if (message.GetEventOpenPeriodInHours() > 0) {
		jsonPayload.set("scheduledEndTime", to_iso_extended_string(second_clock::universal_time() + hours(message.GetEventOpenPeriodHour()) + minutes(message.GetEventOpenPeriodMinute()) + seconds(message.GetEventOpenPeriodSecond()) ) + "Z");
	}

	if (message.ToAlarmTemplate()) {
		jsonPayload.set("alarmResourceTemplateID", GetIdForAlarmTemplate(message.GetAlarmTemplate(), loginData));
	} else {
		jsonPayload.set("alarmResources", GetAlarmResources(message, loginData));

		if (message.HasMessageText()) {
			std::string messageText = message.GetMessageText();
			if (!otherMessagesInfo.empty()) {
				messageText += u8"\n\n" + otherMessagesInfo;
			}

			jsonPayload.set("message", messageText);
		}
		else {
			// the additional infoalarm information is ignored if a message template is used
			jsonPayload.set("alarmTemplateID", GetIdForMessageTemplate(message.GetMessageTemplate(), loginData));
		}

	}
	jsonPayload.set("organizationID", loginData.GetOrganizationId());
	jsonPayload.set("startTime", currIsoTime);
	jsonPayload.set("eventName", eventName);

	return jsonPayload;
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::CreateAlarmJson(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<External::CAlarmMessage> message, const CGroupalarm2LoginData& loginData) {
	using namespace std;
	using namespace Utilities;
	using namespace boost::posix_time;
	using namespace Utilities::Time;
	using namespace External::Infoalarm;
	
	CGroupalarm2Message groupalarmMessage;
	string otherMessagesInfo = "";
	bool isInfoAlarm;

	if (typeid(*message) == typeid(CInfoalarmMessageDecorator)) {
		const auto& infoalarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>(*message);
		otherMessagesInfo = CreateOtherMessagesInfo(infoalarmMessage);
		groupalarmMessage = dynamic_cast<const CGroupalarm2Message&>(*infoalarmMessage.GetContainedMessage());
		isInfoAlarm = true;
	}
	else {
		groupalarmMessage = dynamic_cast<const CGroupalarm2Message&>(*message);
		isInfoAlarm = false;
	}

	string alarmType;

	if (isInfoAlarm) {
		alarmType = "Infoalarm";
	} else {
		if (isRealAlarm) {
			alarmType = "Einsatzalarmierung";
		}
		else {
			alarmType = "Probealarm";
		}
	}

	stringstream ss;
	ss << "[Funkmelderalarm] Schleife " << CStringUtilities().Join(code.cbegin(), code.cend(), "") << " " << german_local_date_time(CBoostStdTimeConverter::ConvertToBoostTime(alarmTime)) << " (" + alarmType << ")";
	string eventName = ss.str();

	string currIsoTime = to_iso_extended_string(second_clock::universal_time()) + "Z";
	return GetJsonPayload(groupalarmMessage, currIsoTime, eventName, otherMessagesInfo, loginData);
}

External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::CGroupalarm2GatewayImpl()
{
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::PerformSend(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	bool isValidation = false;
	auto groupalarmLoginData = dynamic_cast<const CGroupalarm2LoginData&>(*loginData->Clone());

	Poco::JSON::Object jsonPayload = CreateAlarmJson(code, alarmTime, isRealAlarm, move(message), groupalarmLoginData);

	URI uri(GROUPALARM_URI + "/alarm");
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	SetProxy(session, groupalarmLoginData);

	string path(uri.getPathAndQuery());
	if (isValidation) {
		path += "/preview";
	}

	HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.add("API-Token", groupalarmLoginData.GetApiToken());

	stringstream ss;
	jsonPayload.stringify(ss);
	request.setContentLength(ss.str().size());
	ostream& outStream = session.sendRequest(request);
	jsonPayload.stringify(outStream);

	HTTPResponse response;
	istream& responseStream = session.receiveResponse(response);
	string responseBody(istreambuf_iterator<char>(responseStream), {});

	RaiseForStatus(response, responseBody);
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::Send(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	try {
		PerformSend(code, alarmTime, isRealAlarm, move(loginData), move(message), audioFile);
	}
	catch (const Poco::Net::HostNotFoundException& e) {
		// retry makes sense
		throw std::domain_error(e.displayText());
	}
	catch (const Poco::Exception& e) {
		// retry does not make sense
		throw std::logic_error(e.displayText());
	}
}

/** @brief		Provides a string containing a summary of the alarms corresponding to the infoalarm
*   @param		infoalarmMessage				Infoalarm message
*   @return										Summary string containing the alarms corresponding to the infoalarm
*   @exception									None
*   @remarks									None
*/
std::string External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::CreateOtherMessagesInfo(const External::Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage)
{
	using namespace std;
	using namespace Email;
	using namespace Groupalarm;
	using namespace ExternalProgram;

	unsigned int emailCounter = 0;
	unsigned int typeCounter = 0;
	set<string> otherMessageTypes;
	string receiverIDs;
	vector< shared_ptr<External::CAlarmMessage> > otherMessages;
	string otherMessagesInfo;

	otherMessages = infoalarmMessage.GetOtherMessages();

	for (const auto& message : otherMessages) {
		// infoalarms are not included
		if (typeid(*message) == typeid(CEmailMessage)) {
			otherMessageTypes.insert(u8"E-Mail");
			if (emailCounter > 0) {
				receiverIDs += " / ";
			}
			receiverIDs += GetAlarmReceiverID(dynamic_cast<const Email::CEmailMessage&>(*message));
			emailCounter++;
		}
		else if (typeid(*message) == typeid(CGroupalarm2Message)) {
			otherMessageTypes.insert(u8"Groupalarm");
		}
		else if (typeid(*message) == typeid(CExternalProgramMessage)) {
			otherMessageTypes.insert(u8"Externes Programm");
		}
	}

	// add information on the receiver ids (only available for e-mail messages)
	if (!receiverIDs.empty()) {
		otherMessagesInfo += u8"für: " + receiverIDs;
	}

	// add information on all message types that were used
	if (!otherMessageTypes.empty()) {
		if (!receiverIDs.empty()) {
			otherMessagesInfo += u8". ";
		}
		otherMessagesInfo += u8"Ausgelöst: ";
		for (auto type : otherMessageTypes) {
			if (typeCounter > 0) {
				otherMessagesInfo += u8", ";
			}
			otherMessagesInfo += type;
			typeCounter++;
		}
	}

	return otherMessagesInfo;
}


/**	@brief		Provides a string containing the receiver ID of an alarm e-mail message
*	@param		message							E-mail alarm message
*	@return										String containing the receiver ID of the e-mail message
*	@exception									None
*	@remarks									None
*/
std::string External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetAlarmReceiverID(const External::Email::CEmailMessage& message)
{
	using namespace std;

	bool isWithAlarmMessage;
	string siteID, alarmID, alarmText, alarmReceiverID;
	vector< pair<string, string> > recipients;

	message.Get(siteID, alarmID, recipients, alarmText, isWithAlarmMessage);
	alarmReceiverID = siteID + ", " + alarmID;

	return alarmReceiverID;
}