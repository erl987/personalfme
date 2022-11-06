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

#include <string>
#include <boost/asio.hpp>

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


/*@{*/
/** \ingroup Networking
*/
namespace Networking {
	namespace HTTP {
		/**	\ingroup Networking
		*	Simple HTTP-client. Only the GET-request in HTTP1.0-protocol is implemented.
		*/
		class CSimpleHTTPClient
		{
		public:
			NETWORKING_API CSimpleHTTPClient(std::string server, std::string path, std::string proxyServer = std::string(), unsigned int proxyPort = 0);
			NETWORKING_API ~CSimpleHTTPClient(void) {};
			NETWORKING_API void Run(void);
			NETWORKING_API void GetData(std::string& header, std::string& information);
			NETWORKING_API static std::string URLEncoded(const std::string originalString);
		private:
			void HandleResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
			void HandleConnect(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
			void HandleWriteRequest(const boost::system::error_code& error);
			void HandleReadStatusLine(const boost::system::error_code& error);	
			void HandleReadHeaders(const boost::system::error_code& error);
			void HandleReadContent(const boost::system::error_code& error);
			CSimpleHTTPClient (const CSimpleHTTPClient &); // prevent copying
    		CSimpleHTTPClient & operator= (const CSimpleHTTPClient &);
	
			boost::asio::io_service ioService;
			boost::asio::ip::tcp::resolver resolver;
			boost::asio::ip::tcp::socket socket;
			boost::asio::streambuf request;
			boost::asio::streambuf response;
			std::string header;
			std::string information;
		};
	}
}

/*@}*/
