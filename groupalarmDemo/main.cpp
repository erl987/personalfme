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
		#define Groupalarm_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define Groupalarm_API __declspec(dllexport)
	#endif
#endif

#include <fstream>
#include <iostream>
#include <utility>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"


const std::string groupalarmUri = "https://app.groupalarm.com/api/v1";

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

class AlarmConfig {
public:
	Resources resources;
	Message message;
	unsigned int closeEventInHours;
};

std::map<std::array<unsigned int, 5>, AlarmConfig> getGroupalarmConfig() {
	std::map<std::array<unsigned int, 5>, AlarmConfig> alarmConfigs{};
	alarmConfigs[{1, 2, 3, 4, 5}] = AlarmConfig{ Resources{false, {}, {}, {"B"}, {}}, Message{"Testlarm", ""}, 2};

	return alarmConfigs;
}

std::array<unsigned int, 5> alarmCode{ 1, 2, 3, 4, 5 };
std::string alarmType = "Einsatzalarmierung";
bool doEmitAlarm = true;


std::pair<unsigned int, std::string> readConfigFile(const std::string& configFilePath)
{
	using namespace std;
	using namespace Poco::JSON;

	unsigned int organizationId;
	string apiToken;

	try {
		cout << "Reading config file: " << configFilePath << endl;

		ifstream file(configFilePath);
		string json;
		if (file) {
			ostringstream ss;
			ss << file.rdbuf();
			json = ss.str();
		}

		Parser parser;
		auto result = parser.parse(json);
		Object::Ptr pObject = result.extract<Object::Ptr>();

		organizationId = pObject->getValue<unsigned int>("organization_id");
		apiToken = pObject->getValue<string>("api_token");
	}
	catch (Poco::Exception e) {
		cout << "Error reading config file: " << e.displayText() << endl;
		exit(-2);
	}

	return pair<unsigned int, string>(organizationId, apiToken);
}

template <class InputIt>
std::string join(InputIt first, InputIt last, const std::string& delim) {
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


bool hasJsonResponse(const Poco::Net::HTTPResponse& response, const std::string& responseBody) {
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

std::vector<unsigned int> parseResponseJson(const std::string& json, const std::vector<std::string>& entityNames, unsigned int organizationId, const std::string& subEndpoint) {
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

		throw Poco::Exception("Did not find the following *" + subEndpoint + "* in the Groupalarm organization " + to_string(organizationId) + ": " + join(missingEntities.cbegin(), missingEntities.cend(), ", "));
	}

	return entityIds;
}

std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const unsigned int& organizationId, const std::string& apiToken, const std::string& subEndpoint, const std::string& organizationParam) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	URI uri(groupalarmUri + "/" + subEndpoint);
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

	hasJsonResponse(response, responseBody);
	cout << response.getStatus() << " " << response.getReason() << endl; // TODO: should be like "raise_for_status()"

	return parseResponseJson(responseBody, entityNames, organizationId, subEndpoint);
}

std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const unsigned int& organizationId, const std::string& apiToken, const std::string& subEndpoint) {
	return getEntityIdsFromEndpoint(entityNames, organizationId, apiToken, subEndpoint, "organization");
}

std::vector<unsigned int> getIdsForUnits(const std::vector<std::string>& unitNames, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint(unitNames, organizationId, apiToken, "units");
}

std::vector<unsigned int> getIdsForLabels(const std::vector<std::string>& labelNames, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint(labelNames, organizationId, apiToken, "labels");
}

std::vector<unsigned int> getIdsForUsers(const std::vector<std::string>& userNames, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint(userNames, organizationId, apiToken, "users");
}

std::vector<unsigned int> getIdsForScenarios(const std::vector<std::string>& scenarioNames, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint(scenarioNames, organizationId, apiToken, "scenarios");
}

unsigned int getAlarmTemplateId(const std::string& alarmTemplateName, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint({alarmTemplateName}, organizationId, apiToken, "alarms/templates", "organization_id")[0];
}

Poco::JSON::Object getAlarmResources(const AlarmConfig& alarmConfig, const std::string& apiToken, unsigned int organizationId) {
	using namespace std;

	Poco::JSON::Object alarmResources;

	if (alarmConfig.resources.allUsers) {
		alarmResources.set("allUsers", true);
	}
	else if (!alarmConfig.resources.labels.empty()) {
		vector<string> labelNames;
		for (const auto& entry : alarmConfig.resources.labels) {
			labelNames.push_back(entry.first);
		}

		auto labelIds = getIdsForLabels(labelNames, organizationId, apiToken);
		
		vector<Poco::JSON::Object> labelsArray;
		unsigned int index = 0;
		for (const auto& entry : alarmConfig.resources.labels) {
			Poco::JSON::Object labelObject;
			labelObject.set("amount", entry.second);
			labelObject.set("labelID", labelIds[index]);
			labelsArray.push_back(labelObject);
			index++;
		}
		alarmResources.set("labels", labelsArray);
	}
	else if (!alarmConfig.resources.units.empty()) {
		auto unitIds = getIdsForUnits(alarmConfig.resources.units, organizationId, apiToken);
		alarmResources.set("units", unitIds);
	}
	else if (!alarmConfig.resources.users.empty()) {
		auto userIds = getIdsForUsers(alarmConfig.resources.users, organizationId, apiToken);
		alarmResources.set("users", userIds);
	}
	else if (!alarmConfig.resources.scenarios.empty()) {
		auto scenarioIds = getIdsForScenarios(alarmConfig.resources.scenarios, organizationId, apiToken);
		alarmResources.set("users", scenarioIds);
	}
	else {
		throw Poco::Exception("Incorrect alarm configuration: no alarm resources");
	}

	return alarmResources;
}

void sendAlarm(const std::array<unsigned int, 5> alarmCode, const std::string& alarmType, const boost::posix_time::ptime& alarmTimePoint, const AlarmConfig& alarmConfig, const unsigned int& organizationId, const std::string& apiToken, bool doEmitAlarm) {
	using namespace std;
	using namespace boost::posix_time;
	using namespace Poco;
	using namespace Poco::Net;
	using namespace Poco::JSON;

	URI uri(groupalarmUri + "/alarm");
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	string path(uri.getPathAndQuery());
	if (!doEmitAlarm)
	{
		path += "/preview";
	}

	string currIsoTime = to_iso_extended_string(second_clock::universal_time()) + "Z";
	string alarmTimePointStr = to_iso_extended_string(alarmTimePoint);
	string eventName = "[Funkmelderalarm] Schleife " + join(alarmCode.cbegin(), alarmCode.cend(), "") + " " + alarmTimePointStr + " (" + alarmType + ")";

	Object jsonPayload;
	jsonPayload.set("alarmResources", getAlarmResources(alarmConfig, apiToken, organizationId));
	jsonPayload.set("organizationID", organizationId);
	jsonPayload.set("startTime", currIsoTime);
	jsonPayload.set("eventName", eventName);

	if (alarmConfig.closeEventInHours > 0)
	{
		jsonPayload.set("scheduledEndTime", to_iso_extended_string(second_clock::universal_time() + hours(alarmConfig.closeEventInHours)) + "Z");
	}

	if (!alarmConfig.message.messageText.empty())
	{
		jsonPayload.set("message", alarmConfig.message.messageText);
	}
	else
	{
		jsonPayload.set("alarmTemplateID", getAlarmTemplateId(alarmConfig.message.messageTemplate, organizationId, apiToken));
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

	hasJsonResponse(response, responseBody);
	cout << response.getStatus() << " " << response.getReason() << endl << endl;
}

void main(int argc, char** argv)
{
	using namespace std;

	if (argc != 2) {
		cout << "Required parameter: configuration file path" << endl;
		exit(-2);
	}

	auto config = readConfigFile(argv[1]);
	unsigned int organizationId = config.first;
	string apiToken = config.second;
	auto alarmTimePoint = boost::posix_time::second_clock::local_time();

	try {
		sendAlarm(alarmCode, alarmType, alarmTimePoint, getGroupalarmConfig()[alarmCode], organizationId, apiToken, doEmitAlarm);
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		exit(-3);
	}
}