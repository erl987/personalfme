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

#include <iostream>
#include <fstream>
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/JSON/Parser.h"


const std::string uriStr = "https://app.groupalarm.com/api/v1/units";


std::pair<std::string, std::string> readConfigFile(const std::string& configFilePath)
{
	using namespace std;
	using namespace Poco::JSON;

	string organizationId, apiToken;

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

		organizationId = pObject->getValue<string>("organization_id");
		apiToken = pObject->getValue<string>("api_token");
	}
	catch (Poco::Exception e) {
		std::cout << "Error reading config file: " << e.displayText() << std::endl;
	}

	return pair<string, string>(organizationId, apiToken);
}

int main(int argc, char** argv)
{
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;
	using namespace Poco::JSON;

	if (argc != 2) {
		std::cout << "Required parameter: configuration file path" << std::endl;
		return -2;
	}

	auto config = readConfigFile(argv[1]);
	string organizationId = config.first;
	string apiToken = config.second;

	try {
		URI uri(uriStr);
		HTTPSClientSession session(uri.getHost(), uri.getPort());

		uri.addQueryParameter("organization", organizationId);

		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		request.setContentType("application/json");
		request.add("API-Token", apiToken);

		session.sendRequest(request);
		HTTPResponse response;

		istream& stream = session.receiveResponse(response);
		cout << response.getStatus() << " " << response.getReason() << endl;
		std::string json(std::istreambuf_iterator<char>(stream), {});

		Parser parser;
		auto result = parser.parse(json);
		Array::Ptr pObject = result.extract<Array::Ptr>();

		auto description = pObject->get(0).extract<Object::Ptr>();
		cout << description->getValue<std::string>("description") << endl;;
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		return -1;
	}

	return 0;
}