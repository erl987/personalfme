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

#include <map>


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


namespace External {
	namespace Groupalarm2 {
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
	}
}
