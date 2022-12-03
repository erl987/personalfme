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

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Poco/JSON/Parser.h"
#include "DateTime.h"
#include "Time.h"
#include "Groupalarm2Gateway.h"
#include "Groupalarm2LoginData.h"
#include "Groupalarm2Message.h"


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

void main(int argc, char** argv)
{
	using namespace std;
	using namespace External::Groupalarm;

	if (argc != 2) {
		cout << "Required parameter: configuration file path" << endl;
		exit(-2);
	}

	auto config = readConfigFile(argv[1]);
	unsigned int organizationId = config.first;
	string apiToken = config.second;

	CGroupalarm2LoginData groupalarmLoginData;
	groupalarmLoginData.Set(organizationId, apiToken, "", 0, "", "");
	unique_ptr<External::CGatewayLoginData> loginData = make_unique<CGroupalarm2LoginData>(groupalarmLoginData);

	CGroupalarm2Message groupalarmMessage;
	groupalarmMessage.Set(false, {}, {}, { "B" }, {}, "Testalarm", "", 2);
	unique_ptr<External::CAlarmMessage> message = make_unique<CGroupalarm2Message>(groupalarmMessage);

	auto currUtcTimePoint = boost::posix_time::second_clock::universal_time();
	Utilities::CTime currTimeOfDay(
		static_cast<int>(currUtcTimePoint.time_of_day().hours()),
		static_cast<int>(currUtcTimePoint.time_of_day().minutes()),
		static_cast<int>(currUtcTimePoint.time_of_day().seconds())
	);
	Utilities::CDateTime alarmTime(
		static_cast<int>(currUtcTimePoint.date().day()),
		static_cast<int>(currUtcTimePoint.date().month()),
		static_cast<int>(currUtcTimePoint.date().year()),
		currTimeOfDay
	);

	std::vector<int> alarmCode{ 1, 2, 3, 4, 5 };
	bool isRealAlarm = true;

	unique_ptr<External::CAlarmGateway> gateway = make_unique<External::Groupalarm::CGroupalarm2Gateway>();

	try {
		gateway->Send(alarmCode, alarmTime, isRealAlarm, move(loginData), move(message));
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		exit(-3);
	}
}