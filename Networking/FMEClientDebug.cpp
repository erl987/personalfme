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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#if defined _WIN32
	#include "stdafx.h"
#endif
#include "FMEClientDebug.h"
#include "FMEClientPrivImplDebug.h"
#include "FMESessionDebug.h"
#include "FMESessionManagerDebug.h"


namespace Networking {
	/** @brief		Dummy function for replacing the found sequence callback function not required in the client mode.
	*	@param		data					Object storing the (full) data for a found sequence
	*	@return								None
	*	@exception							None
	*	@remarks							None
	*/
	void dummyBase(const Utilities::CSeqDataComplete<float>& data){};
}



/** @brief		Standard constructor.	
*	@remarks							None
*/
Networking::CFMEClientDebug::CFMEClientDebug(void)
	: CFMEClient()
{
}



/** @brief		Constructor.
*	@param		serverAddress			Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@exception							See CFMEClientDebug::Init
*	@remarks							IPv4- as well as IPv6-addresses might be used
*/
Networking::CFMEClientDebug::CFMEClientDebug(std::string serverAddress, unsigned short port)
	: CFMEClient()
{
	Init( serverAddress, port );
}



/** @brief		Initializes the class.
*	@param		serverAddress			Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@return								None
*	@exception	std::runtime_error		Thrown if the server for the given address and port is not available
*	@remarks							IPv4- as well as IPv6-addresses might be used. This function can be called for complete and safe reinitialization of the class.
*/
void Networking::CFMEClientDebug::Init(std::string serverAddress, unsigned short port)
{
	// adjust pimple idiom for use with derived class
	privHandle.reset( new FMEClientPrivImplDebug );

	// initialize base class
	CFMEClient::Init( serverAddress, port );

	privHandle->sessionManager.reset( new Session::CFMESessionManagerDebug<float>() );
	privHandle->ResetConnection();
}



/** @brief		Preparing the transmission of the data of the sequence to the server.
*	@param		newSequence				Object containing the sequence data (from CFMEAudioInputDebug containing all information on tone frequencies, lengths, periods, ...)
*	@return								None
*	@exception							None
*	@remarks							The data will not be sent to the server before calling CFMEClientDebug::Run()
*/
void Networking::CFMEClientDebug::Send(const Utilities::CSeqDataComplete<float>& newSequence)
{
	// obtain data
	std::dynamic_pointer_cast< FMEClientPrivImplDebug >( privHandle )->sequenceDebug = newSequence;
	
	privHandle->PerformSending();
}



/** @brief		Preparing a new client-server connection for debug sequence information.
*	@return								None
*	@exception							None
*	@remarks							The connection is not established, but only prepared
*/
void Networking::CFMEClientDebug::FMEClientPrivImplDebug::ResetConnection(void)
{
	// connection based on derived class
	newConnection.reset( new Session::CFMESessionDebug<float>( ioService, sessionManager, dummyBase ) );
}



/** @brief		Starting data transmission to the server for debug sequence information.
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if the connect operation finally failed
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::CFMEClientDebug::FMEClientPrivImplDebug::HandleDirectConnect(const boost::system::error_code& error)
{
	if ( !(error) ) {
		// connection to server is standing
		isValidEndpoint = true;

		// start managing connection and sending data to the server
		std::dynamic_pointer_cast< Session::CFMESessionManagerDebug<float> >( sessionManager )->StartWrite( newConnection, sequenceDebug );

		// prepare the new connection which will be used for the transmission of the sequence
		ResetConnection();	
	} else {
		// connection could not be established
		throw std::runtime_error( error.message() );
    }
}
