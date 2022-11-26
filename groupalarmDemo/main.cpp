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
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSClientSession.h"

int main() {
	using namespace std;
	using namespace Poco;
	using namespace Poco::Net;

	std::string hostname = "personalfme.de";
	unsigned short port = 443;


	try {
		HTTPSClientSession session(hostname, port);

		HTTPRequest request(HTTPRequest::HTTP_GET, hostname, HTTPMessage::HTTP_1_1);

		session.sendRequest(request);
		HTTPResponse response;

		istream& is = session.receiveResponse(response);
		cout << response.getStatus() << " " << response.getReason() << endl;
		cout << "Success" << endl;
		StreamCopier::copyStream(is, cout);
	}
	catch (const Poco::Exception& e) {
		cout << "Error: " << e.displayText() << endl;
		return -1;
	}

	return 0;
}