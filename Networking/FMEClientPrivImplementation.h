/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

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

#include <boost/asio.hpp>
#include "FMESession.h"
#include "FMESessionManager.h"
#include "FMEClient.h"

/*@{*/
/** \ingroup Networking
*/

/**	\ingroup Networking
*	Pimple idiom for hiding the private implementation details of CFMEClient
*/
class Networking::CFMEClient::FMEClientPrivImplementation
{
public:
	FMEClientPrivImplementation(void);
	virtual ~FMEClientPrivImplementation(void){};
	void PerformSending(void);
	void HandleConnect(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt);
	virtual void HandleDirectConnect(const boost::system::error_code& error);
	virtual void ResetConnection(void);

	boost::asio::io_service ioService;
	boost::asio::ip::tcp::endpoint hostEndpoint;
	boost::asio::ip::tcp::resolver::iterator hostEndpointIt;
	boost::asio::ip::tcp::resolver::iterator originalHostEndpointIt;
	std::shared_ptr< Session::CFMESessionManager<float> > sessionManager;
	Utilities::CSeqData sequence;
	std::shared_ptr< Session::CFMESession<float> > newConnection;
	std::string serverAddress;
	unsigned short port;
	bool isValidEndpoint;
	bool isInit;
};

/*@}*/
