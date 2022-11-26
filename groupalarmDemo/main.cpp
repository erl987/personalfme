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
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"


const std::string groupalarmUri = "https://app.groupalarm.com/api/v1";
const std::string unitsEndpoint = "/units";
const std::string alarmEndpoint = "/alarm";


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

std::string getUnits(const unsigned int& organizationId, const std::string& apiToken) {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;
	using namespace Poco::JSON;

	URI uri(groupalarmUri + unitsEndpoint);
	HTTPSClientSession session(uri.getHost(), uri.getPort());
	uri.addQueryParameter("organization", to_string(organizationId));
	string path(uri.getPathAndQuery());

	HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
	request.setContentType("application/json");
	request.add("API-Token", apiToken);
	session.sendRequest(request);

	HTTPResponse response;
	istream& stream = session.receiveResponse(response);
	cout << response.getStatus() << " " << response.getReason() << endl;
	string json(istreambuf_iterator<char>(stream), {});

	Parser parser;
	auto result = parser.parse(json);
	Array::Ptr pObject = result.extract<Array::Ptr>();

	const auto& firstEntry = pObject->get(0).extract<Object::Ptr>();
	return firstEntry->getValue<string>("description");
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

	Object alarmResources;
	alarmResources.set("allUsers", true);

	Object jsonPayload;
	jsonPayload.set("alarmResources", alarmResources);
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
	istream& stream = session.receiveResponse(response);
	cout << response.getStatus() << " " << response.getReason() << endl << endl;
	string json(istreambuf_iterator<char>(stream), {});
	cout << json << endl;
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
		cout << getUnits(organizationId, apiToken) << endl;
		sendAlarm(organizationId, apiToken);
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		exit(-3);
	}
}