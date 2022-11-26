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
#include <random>
#include <utility>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"


const std::string groupalarmUri = "https://app.groupalarm.com/api/v1";
const std::string alarmEndpoint = "/alarm";

const std::vector<std::string> units{ "B" };


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

std::string randomString(const unsigned int& length) {
	using namespace std;

	mt19937 generator{ random_device{}() };
	uniform_int_distribution<int> distribution{ 'a', 'z'};

	string randomStr(length, '\0');
	for (auto& dis : randomStr)
		dis = distribution(generator);

	return randomStr;
}

template <class T>
std::string join(const std::vector<T>& elements, const std::string& delim) {
	std::stringstream ss;
	for (int i = 0; i < elements.size(); i++) {
		ss << elements[i];
		if (i < elements.size() - 1) {
			ss << delim;
		}
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

		throw Poco::Exception("Did not find the following *" + subEndpoint + "* in the Groupalarm organization " + to_string(organizationId) + ": " + join(missingEntities, ", "));
	}

	return entityIds;
}

std::vector<unsigned int> getEntityIdsFromEndpoint(const std::vector<std::string>& entityNames, const unsigned int& organizationId, const std::string& apiToken, const std::string subEndpoint) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	URI uri(groupalarmUri + "/" + subEndpoint);
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	uri.addQueryParameter("organization", to_string(organizationId));
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

std::vector<unsigned int> getIdsForUnits(const std::vector<std::string>& unitNames, const unsigned int& organizationId, const std::string& apiToken) {
	return getEntityIdsFromEndpoint(unitNames, organizationId, apiToken, "units");
}

Poco::JSON::Object getAlarmResources(const std::string& apiToken, unsigned int organizationId) {
	Poco::JSON::Object alarmResources;
	alarmResources.set("allUsers", true);

	return alarmResources;
}

void sendAlarm(const unsigned int& organizationId, const std::string& apiToken) {
	using namespace std;
	using namespace boost::posix_time;
	using namespace Poco;
	using namespace Poco::Net;
	using namespace Poco::JSON;

	URI uri(groupalarmUri + alarmEndpoint);
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	string path(uri.getPathAndQuery());

	string currIsoTime = to_iso_extended_string(second_clock::universal_time()) + "Z";

	Object jsonPayload;
	jsonPayload.set("alarmResources", getAlarmResources(apiToken, organizationId));
	jsonPayload.set("message", "Testalarm");
	jsonPayload.set("organizationID", organizationId);
	jsonPayload.set("startTime", currIsoTime);
	jsonPayload.set("eventName", "Testalarm-" + randomString(4));

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

	try {
		cout << join(getIdsForUnits(units, organizationId, apiToken), ", ") << endl;
		sendAlarm(organizationId, apiToken);
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		exit(-3);
	}
}