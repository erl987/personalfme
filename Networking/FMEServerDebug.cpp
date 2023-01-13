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
#include "FMESessionDebug.h"
#include "FMESessionManagerDebug.h"
#include "FMEServerDebug.h"
#include "FMEServerPrivImplDebug.h"


namespace Networking {
	/** @brief		Dummy function for replacing the found sequence callback function of the base class of the server.
	*	@param		data					Object storing the data for a found sequence
	*	@return								None
	*	@exception							None
	*	@remarks							None
	*/
	void dummyBaseServer(const Utilities::CSeqData& data){};
}



/** @brief		Standard constructor.	
*	@remarks							None
*/
Networking::CFMEServerDebug::CFMEServerDebug(void)
	: CFMEServer()
{
}



/** @brief		Constructor.
*	@param		address					Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@param		foundCallback			Name of the callback, which is called if the server received a new sequence. The new data is passed to this function.
*	@exception							See CFMEClientDebug::Init
*	@remarks							IPv4- as well as IPv6-addresses might be used
*/
Networking::CFMEServerDebug::CFMEServerDebug(std::string address, unsigned short port, std::function< void(const Utilities::CSeqDataComplete<float>&) > foundCallback)
	: CFMEServer()
{
	Init( address, port, foundCallback );
}



/** @brief		Initializes the class.
*	@param		address					Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@param		foundCallback			Name of the callback, which is called if the server received a new sequence. The new data is passed to this function.
*	@exception							Thrown if the preparation of the server in the network was failing
*	@remarks							IPv4- as well as IPv6-addresses might be used
*/
void Networking::CFMEServerDebug::Init(std::string address, unsigned short port, std::function< void(const Utilities::CSeqDataComplete<float>&) > foundCallback)
{
	// prepare pimpl idiom for derived class
	privHandle.reset( new FMEServerPrivImplDebug( foundCallback ) );

	privHandle->sessionManager.reset( new Session::CFMESessionManagerDebug<float>() );

	// initialize base class
	CFMEServer::Init( address, port, dummyBaseServer );
}



/** @brief		Constructor.
*	@param		foundCallback			Name of the callback, which is called if the server received a new sequence. This is a callback function for the full sequence information (i.e. tone frequencies, lengths, periods, ...).
*	@exception							None
*	@remarks							None
*/
Networking::CFMEServerDebug::FMEServerPrivImplDebug::FMEServerPrivImplDebug(std::function< void(const Utilities::CSeqDataComplete<float>&) > foundCallback)
{
	FMEServerPrivImplDebug::foundCallbackDebug = foundCallback;
}



/** @brief		Standard destructor.	
*	@remarks							None
*/
Networking::CFMEServerDebug::~CFMEServerDebug(void)
{
}



/** @brief		Preparing a new client-server connection for debug sequence information.
*	@return								None
*	@exception							None
*	@remarks							The connection is not established, but only prepared
*/
void Networking::CFMEServerDebug::FMEServerPrivImplDebug::ResetConnection(void)
{
	newConnection.reset( new Session::CFMESessionDebug<float>( ioService, sessionManager, foundCallbackDebug ) );
}