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
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"
#include "Groupalarm2Message.h"
#include "Groupalarm2GatewayImpl.h"


template <class InputIt>
static std::string External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::Join(InputIt first, InputIt last, const std::string& delim) {
	std::stringstream ss;
	size_t length = std::distance(first, last);

	int i = 0;
	for (auto it = first; it != last; ++it) {
		ss << *it;
		if (i < length - 1) {
			ss << delim;
		}
		i++;
	}

	return ss.str();
}

bool External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::HasJsonResponse(const Poco::Net::HTTPResponse& response, const std::string& responseBody) {
	using namespace std;
	using namespace Poco::JSON;

	bool hasJsonResponse;

	if (response.has("Content-Type") && response.get("Content-Type").find("application/json") != string::npos) {
		hasJsonResponse = true;

		Parser parser;
		auto jsonVar = parser.parse(responseBody);
		if (!jsonVar.isArray()) {
			const auto& jsonObj = jsonVar.extract<Object::Ptr>();

			if (jsonObj->has("success") && jsonObj->has("error") && !jsonObj->getValue<bool>("success")) {
				string message = jsonObj->getValue<string>("message");
				string details = jsonObj->getValue<string>("error");
				throw Poco::Exception("Information vom Server Groupalarm.com: " + message + ", Details : " + details);
			}
		}
	}
	else {
		hasJsonResponse = false;
	}

	return hasJsonResponse;
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::ParseResponseJson(const std::string& json, const std::vector<std::string>& entityNames, const std::string& subEndpoint, unsigned int organizationId) {
	using namespace std;
	using namespace Poco::JSON;

	Parser parser;
	auto result = parser.parse(json);
	Array::Ptr jsonObj = result.extract<Array::Ptr>();

	map<string, unsigned int> entityIdMap;
	for (int i = 0; i < jsonObj->size(); i++) {
		const auto& entry = jsonObj->get(i).extract<Object::Ptr>();
		entityIdMap[entry->getValue<string>("name")] = entry->getValue<unsigned int>("id");
	}

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

		throw Poco::Exception("Did not find the following *" + subEndpoint + "* in the Groupalarm organization " + to_string(organizationId) + ": " + Join(missingEntities.cbegin(), missingEntities.cend(), ", "));
	}

	return entityIds;
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::RaiseForStatus(const Poco::Net::HTTPResponse& response) {
	if (response.getStatus() >= 400 && response.getStatus() < 500) {
		throw Poco::Exception("Client error, status code " + response.getStatus(), ": " + response.getReason());
	}
	else if (response.getStatus() >= 500 && response.getStatus() < 600) {
		throw Poco::Exception("Server error, status code " + response.getStatus(), ": " + response.getReason());
	}
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, unsigned int organizationId, const std::string& apiToken, const std::string& organizationParam) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	URI uri(GROUPALARM_URI + "/" + subEndpoint);
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	uri.addQueryParameter(organizationParam, to_string(organizationId));
	string path(uri.getPathAndQuery());

	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.add("API-Token", apiToken);
	session.sendRequest(request);

	HTTPResponse response;
	istream& responseStream = session.receiveResponse(response);
	string responseBody(istreambuf_iterator<char>(responseStream), {});

	HasJsonResponse(response, responseBody);
	RaiseForStatus(response);

	return ParseResponseJson(responseBody, entityNames, subEndpoint, organizationId);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const std::string& subEndpoint, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint(entityNames, subEndpoint, organizationId, apiToken, "organization");
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForUnits(const std::vector<std::string>& unitNames, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint(unitNames, "units", organizationId, apiToken);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForLabels(const std::vector<std::string>& labelNames, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint(labelNames, "labels", organizationId, apiToken);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForUsers(const std::vector<std::string>& userNames, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint(userNames, "users", organizationId, apiToken);
}

std::vector<unsigned int> External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetIdsForScenarios(const std::vector<std::string>& scenarioNames, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint(scenarioNames, "scenarios", organizationId, apiToken);
}

unsigned int External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetAlarmTemplateId(const std::string& alarmTemplateName, unsigned int organizationId, const std::string& apiToken) {
	return GetEntityIdsFromEndpoint({ alarmTemplateName }, "alarms/templates", organizationId, apiToken, "organization_id")[0];
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetAlarmResources(std::unique_ptr<External::CAlarmMessage> message, unsigned int organizationId, const std::string& apiToken) {
	using namespace std;

	auto groupalarmMessage = dynamic_cast<const CGroupalarm2Message&>(*message);

	Poco::JSON::Object alarmResources;

	if (groupalarmMessage.ToAllUsers()) {
		alarmResources.set("allUsers", true);
	}
	else if (groupalarmMessage.ToLabels()) {
		vector<string> labelNames;
		for (const auto& entry: groupalarmMessage.GetLabels()) {
			labelNames.push_back(entry.first);
		}

		auto labelIds = GetIdsForLabels(labelNames, organizationId, apiToken);

		vector<Poco::JSON::Object> labelsArray;
		unsigned int index = 0;
		for (const auto& entry: groupalarmMessage.GetLabels()) {
			Poco::JSON::Object labelObject;
			labelObject.set("amount", entry.second);
			labelObject.set("labelID", labelIds[index]);
			labelsArray.push_back(labelObject);
			index++;
		}
		alarmResources.set("labels", labelsArray);
	}
	else if (groupalarmMessage.ToUnits()) {
		auto unitIds = GetIdsForUnits(groupalarmMessage.GetUnits(), organizationId, apiToken);
		alarmResources.set("units", unitIds);
	}
	else if (groupalarmMessage.ToUsers()) {
		auto userIds = GetIdsForUsers(groupalarmMessage.GetUsers(), organizationId, apiToken);
		alarmResources.set("users", userIds);
	}
	else if (groupalarmMessage.ToScenarios()) {
		auto scenarioIds = GetIdsForScenarios(groupalarmMessage.GetScenarios(), organizationId, apiToken);
		alarmResources.set("scenarios", scenarioIds);
	}
	else {
		throw Poco::Exception("Incorrect alarm configuration: no alarm resources");
	}

	return alarmResources;
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::SetProxy(Poco::Net::HTTPSClientSession& session, std::unique_ptr<CGatewayLoginData> loginData) {
	auto groupalarmLoginData = dynamic_cast<const CGroupalarm2LoginData&>(*loginData);

	if (groupalarmLoginData.IsWithProxy()) {
		session.setProxyHost(groupalarmLoginData.GetProxyAddress());
		session.setProxyPort(groupalarmLoginData.GetProxyPort());

		if (groupalarmLoginData.IsWithProxyWithUserNameAndPassword()) {
			session.setProxyUsername(groupalarmLoginData.GetProxyUserName());
			session.setProxyPassword(groupalarmLoginData.GetProxyPassword());
		}
		else if (groupalarmLoginData.IsWithProxyWithUserNameOnly()) {
			session.setProxyUsername(groupalarmLoginData.GetProxyUserName());
		}
	}
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::GetJsonPayload(std::unique_ptr<External::CAlarmMessage> message, const std::string& currIsoTime, const std::string& eventName, unsigned int organizationId, const std::string& apiToken) {
	using namespace boost::posix_time;

	Poco::JSON::Object jsonPayload;
	auto groupalarmMessage = dynamic_cast<const CGroupalarm2Message&>(*message);

	if (groupalarmMessage.GetEventActivePeriodInHours() > 0)
	{
		jsonPayload.set("scheduledEndTime", to_iso_extended_string(second_clock::universal_time() + hours(groupalarmMessage.GetEventActivePeriodInHours())) + "Z");
	}

	if (groupalarmMessage.HasMessageText())
	{
		jsonPayload.set("message", groupalarmMessage.GetMessageText());
	}
	else
	{
		jsonPayload.set("alarmTemplateID", GetAlarmTemplateId(groupalarmMessage.GetMessageTemplate(), organizationId, apiToken));
	}

	jsonPayload.set("alarmResources", GetAlarmResources(move(message), organizationId, apiToken));
	jsonPayload.set("organizationID", organizationId);
	jsonPayload.set("startTime", currIsoTime);
	jsonPayload.set("eventName", eventName);

	return jsonPayload;
}

Poco::JSON::Object External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::CreateAlarmJson(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<External::CAlarmMessage> message, unsigned int organizationId, const std::string& apiToken) {
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities::Time;
	
	string alarmType;
	if (isRealAlarm) {
		alarmType = "Einsatzalarmierung";
	}
	else {
		alarmType = "Probealarm";
	}

	stringstream ss;
	ss << "[Funkmelderalarm] Schleife " << Join(code.cbegin(), code.cend(), "") << " " << german_local_date_time(CBoostStdTimeConverter::ConvertToBoostTime(alarmTime)) << " (" + alarmType << ")";
	string eventName = ss.str();

	string currIsoTime = to_iso_extended_string(second_clock::universal_time()) + "Z";
	return GetJsonPayload(move(message), currIsoTime, eventName, organizationId, apiToken);
}

External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::CGroupalarm2GatewayImpl()
{
}

void External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2GatewayImpl::Send(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	bool isValidation = false;
	auto groupalarmLoginData = dynamic_cast<const CGroupalarm2LoginData&>(*loginData);
	unsigned int organizationId = groupalarmLoginData.GetOrganizationId();
	string apiToken = groupalarmLoginData.GetApiToken();

	Poco::JSON::Object jsonPayload = CreateAlarmJson(code, alarmTime, isRealAlarm, move(message), organizationId, apiToken);

	URI uri(GROUPALARM_URI + "/alarm");
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	SetProxy(session, move(loginData));

	string path(uri.getPathAndQuery());
	if (isValidation) {
		path += "/preview";
	}

	HTTPRequest request(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.add("API-Token", apiToken);

	stringstream ss;
	jsonPayload.stringify(ss);
	request.setContentLength(ss.str().size());
	ostream& outStream = session.sendRequest(request);
	jsonPayload.stringify(outStream);

	HTTPResponse response;
	istream& responseStream = session.receiveResponse(response);
	string responseBody(istreambuf_iterator<char>(responseStream), {});

	HasJsonResponse(response, responseBody);
	RaiseForStatus(response);
}