/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)

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

This file contains code under the following copyright:
	Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
	Distributed under the Boost Software License, Version 1.0. (See accompanying
	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
*/
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#if defined _WIN32
	#include "stdafx.h"
#endif
#include <iostream>
#include <istream>
#include <ostream>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "SimpleHTTPClient.h"


/** @brief		Constructor.
*	@param		server					Address of the HTTP-server (for example: "www.boost.org")
*	@param		path					Path of the document to be loaded (for example: "/LICENSE_1_0.txt")
*	@param		proxyServer				Address of a proxy server used for the connection (omit it if no proxy server is used)
*	@param		proxyPort				Port of the proxy server (omit if it is port 8080)
*	@exception							None
*	@remarks							This is a HTTP 1.0-protocol client. IPv4- as well as IPv6-addresses might be used
*/
Networking::HTTP::CSimpleHTTPClient::CSimpleHTTPClient(std::string server, std::string path, std::string proxyServer, unsigned int proxyPort)
	: ioService(),
	  resolver(ioService),
	  socket(ioService)
{
	using boost::asio::ip::tcp;
	std::string port = "http";

	// generate the HTTP-request
	std::ostream requestStream( &request );
	requestStream << "GET ";
	
	// handle using a proxy server
	if (!proxyServer.empty()) {
		requestStream << "http://" << server;
	}
	requestStream << path << " HTTP/1.0\r\n";
	requestStream << "Host: " << server << "\r\n";
	requestStream << "Accept: */*\r\n";
	requestStream << "Connection: close\r\n\r\n";

	// handle using a proxy server
	if ( !proxyServer.empty() ) {
		server = proxyServer;
		if ( proxyPort != 0 ) {
			port = boost::lexical_cast<std::string>( proxyPort );
		}
		else {
			port = boost::lexical_cast<std::string>( 8080 ); // default port for http-proxies
		}
	}

	// resolve server name with DNS
	tcp::resolver::query query( server, port );
	resolver.async_resolve( query, std::bind( &CSimpleHTTPClient::HandleResolve, this, std::placeholders::_1, std::placeholders::_2) );
}



/** @brief		Executing the client request.
*	@return								None
*	@exception	std::domain_error		Thrown if the DNS resolution was not successfull (indicating a missing internet connection)
*	@exception	std::runtime_error		Thrown in case of any other connection or setting error
*	@remarks							The request is only performed during calling this function. It will return after finishing all work. It might safely be called several times.
*/
void Networking::HTTP::CSimpleHTTPClient::Run(void)
{
	// reset the network service in order to allow later recalls
	ioService.reset();

	// start HTTP-client request
	ioService.run();
}



/** @brief		Returning the information obtained from the server
*	@param		header					Containing the full HTTP-response header sent by the server
*	@param		information				Containing the data sent by the HTTP-server
*	@return								None
*	@exception							None
*	@remarks							This function will only give useful results after calling CSimpleHTTPClient::Run()
*/
void Networking::HTTP::CSimpleHTTPClient::GetData(std::string& header, std::string& information)
{
	header = CSimpleHTTPClient::header;
	information = CSimpleHTTPClient::information;
}



/** @brief		Handler for resolving a address with a DNS-server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@param		endpointIt				Iterator the next TCP endpoint (relevant if the connection operation failed) - the different endpoints for example consider IPv4 and IPv6 addresses
*	@return								None
*	@exception	std::domain_error		Thrown if the DNS resolution was not successfull (indicating a missing internet connection)
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleResolve(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
	using boost::asio::ip::tcp;
		
	if ( !error ) {
		// connect to the HTTP-server
		tcp::endpoint endpoint = *endpointIt;
		socket.async_connect( endpoint, std::bind( &CSimpleHTTPClient::HandleConnect, this, std::placeholders::_1, ++endpointIt ) );
	} else {
		throw std::domain_error( error.message() );
	}
}



/** @brief		Handler for connecting to the HTTP-server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@param		endpointIt				Iterator the next TCP endpoint (relevant if the connection operation failed) - the different endpoints for example consider IPv4 and IPv6 addresses
*	@return								None
*	@exception	std::domain_error		Thrown if the server connection failed and all possible endpoints had been tested (indicating a missing internet connection)
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleConnect(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
	using boost::asio::ip::tcp;
		
	if ( !error ) {
		// successfull connection
		boost::asio::async_write( socket, request, std::bind( &CSimpleHTTPClient::HandleWriteRequest, this, std::placeholders::_1) );
	} else if ( endpointIt != tcp::resolver::iterator() ) {
		// retry due to invalid endpoint
		socket.close();
		tcp::endpoint endpoint = *endpointIt;
		socket.async_connect( endpoint, std::bind( &CSimpleHTTPClient::HandleConnect, this, std::placeholders::_1, ++endpointIt ) );
	} else {
		throw std::domain_error( error.message() );
	}
}



/** @brief		Handler for writing the HTTP-request
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if the HTTP-request could not be transmitted
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleWriteRequest(const boost::system::error_code& error)
{
	if ( !error ) {
		// read HTTP status response
		boost::asio::async_read_until( socket, response, "\r\n", std::bind( &CSimpleHTTPClient::HandleReadStatusLine, this, std::placeholders::_1) );
	} else {
		throw std::runtime_error( error.message() );
	}
}



/** @brief		Handler for receiving the HTTP-status line from the server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if for some reason the status line was not valid or could not be received
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleReadStatusLine(const boost::system::error_code& error)
{
	using namespace std;
	
	if ( !error ) {
		// check the response
		istream response_stream( &response );
		string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		string status_message;
		getline( response_stream, status_message );
		if ( ( !response_stream ) || ( http_version.substr(0, 5) != "HTTP/" ) ) {
			throw std::runtime_error( "Invalid response" );
			return;
		}

		if (status_code != 200) {
			throw runtime_error( string( "Response returned with status code " + boost::lexical_cast<string>( status_code ) ) );
			return;
		}
		
		// read the response headers
		boost::asio::async_read_until( socket, response, "\r\n\r\n", std::bind( &CSimpleHTTPClient::HandleReadHeaders, this, std::placeholders::_1) );
	} else {
		throw std::runtime_error( error.message() );
	}
}



/** @brief		Handler for reading the HTTP-header
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if the reading the header was not successfull
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleReadHeaders(const boost::system::error_code& error) {
	using namespace std;
	string headerLine;
	stringstream strStream;

	if ( !error ) {
		// process the response headers
		istream responseStream( &response );
		while ( ( getline( responseStream, headerLine ) ) && ( headerLine != "\r" ) ) {
			header += headerLine + "\n";
		}

		// write any data content already transferred
		if (response.size() > 0) {
			strStream << &response;
			information += strStream.str();
		}

		// start reading remaining data content until EOF
		boost::asio::async_read( socket, response, boost::asio::transfer_at_least(1), std::bind( &CSimpleHTTPClient::HandleReadContent, this, std::placeholders::_1 ) );
	} else {
		throw std::runtime_error( error.message() );
	}
}



/** @brief		Handler for reading the data information from the server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if reading the data was not successfull
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::HTTP::CSimpleHTTPClient::HandleReadContent(const boost::system::error_code& error) {
	std::stringstream strStream;

	if (!error) {
		// write all of the data that has been read so far
		strStream << &response;
		information += strStream.str();

		// continue reading remaining data until EOF
		boost::asio::async_read( socket, response, boost::asio::transfer_at_least(1), std::bind( &CSimpleHTTPClient::HandleReadContent, this, std::placeholders::_1) );
	} else if (error != boost::asio::error::eof) {
		throw std::runtime_error( error.message() );
	}
}



/** @brief		URL-encoding for use in a HTTP-URL
*	@param		originalString			Standard string to be URL-encoded
*	@return								URL-encoded string
*	@exception							None
*	@remarks							This function is implemented for ASCII-characters only
*/
std::string Networking::HTTP::CSimpleHTTPClient::URLEncoded(const std::string originalString)
{
	using namespace std;
	string buf;
	ostringstream oss;

    // all unreserved characters possible in a URL
    const string unreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    string encoded;
    for( size_t i=0; i < originalString.length(); i++) {
        if (unreserved.find_first_of( originalString[i] ) != string::npos ) {
            // unreserved character
			encoded.push_back( originalString[i] );
        } else {
			// encoding to hexadecimal character
			encoded.append( "%" );
			string stringChar = originalString.substr( i, 1 );
			oss.str("");
			oss.clear();
			int hValInt = static_cast<char>( stringChar[0] ) & 0xFF; // ensure always two digits
			oss << hex << setfill('0') << setw(2) << hValInt; 
			buf = oss.str(); 
            encoded.append( buf );
        }
    }
    return encoded;
}
