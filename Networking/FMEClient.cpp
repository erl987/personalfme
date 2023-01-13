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
#include <boost/lexical_cast.hpp>
#include "FMEClient.h"
#include "FMEClientPrivImplementation.h"


namespace Networking {
	/** @brief		Dummy function for replacing the found sequence callback function not required in the client mode.
	*	@param		data					Object storing the data for a found sequence
	*	@return								None
	*	@exception							None
	*	@remarks							None
	*/
	void dummy(const Utilities::CSeqData& data){};
}



/**	@brief	Default constructor.
*/
Networking::CFMEClient::CFMEClient()
	: privHandle( new FMEClientPrivImplementation )
{
}



/** @brief		Constructor.
*	@param		serverAddress			Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@exception							See CFMEClient::Init
*	@remarks							IPv4- as well as IPv6-addresses might be used
*/
Networking::CFMEClient::CFMEClient(std::string serverAddress, unsigned short port)
	: privHandle( new FMEClientPrivImplementation )
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
void Networking::CFMEClient::Init(std::string serverAddress, unsigned short port)
{
	using boost::asio::ip::tcp;

	privHandle->serverAddress = serverAddress;
	privHandle->port = port;
	privHandle->isValidEndpoint = false;
		
	// resolving server address with DNS
	try {
		tcp::resolver resolver( privHandle->ioService );
		tcp::resolver::query query( serverAddress, boost::lexical_cast<std::string>( port ) );
		privHandle->hostEndpointIt = resolver.resolve( query );
		privHandle->originalHostEndpointIt = privHandle->hostEndpointIt;
	} catch ( std::exception& e ) {
		// server address could not be resolved
		throw std::runtime_error( e.what() );
	}

	privHandle->isInit = true;
}



/** @brief		Preparing a new client-server connection.
*	@return								None
*	@exception							None
*	@remarks							The connection is not established, but only prepared
*/
void Networking::CFMEClient::FMEClientPrivImplementation::ResetConnection(void)
{
	newConnection.reset( new Session::CFMESession<float>( ioService, sessionManager, dummy ) );
}



/**	@brief	Default constructor.
*/
Networking::CFMEClient::FMEClientPrivImplementation::FMEClientPrivImplementation(void)
	: ioService(),
	  sessionManager( new Session::CFMESessionManager<float>() ),
	  newConnection( new Session::CFMESession<float>( ioService, sessionManager, dummy ) )
{
	isInit = false;
}



/**	@brief	Default destructor.
*/
Networking::CFMEClient::~CFMEClient(void)
{
}



/** @brief		Executing the network connection.
*	@return								True in case of successfull data transmission and false in case of any failure
*	@exception	std::runtime_error		Thrown if neither the full constructor nor CFMEClient::Init() were used for initalizing the function
*	@remarks							The network communication is only performed during calling this function. It will return after finishing all work. It might safely be called several times.
*/
bool Networking::CFMEClient::Run(void)
{
	bool isSuccessfull = true;

	if ( !( privHandle->isInit ) ) {
		throw std::runtime_error("The object was not initialized before use!");
	}

	try {
		// reset the network service
		privHandle->ioService.reset();

		// start asynchronous networking
		privHandle->ioService.run();
	} catch (...) {
		// reset for further trials
		privHandle->newConnection->Socket().close();
		privHandle->hostEndpointIt = privHandle->originalHostEndpointIt;
		privHandle->isValidEndpoint = false;
		isSuccessfull = false;
	}

	return isSuccessfull;
}



/** @brief		Preparing the transmission of the data of the sequence to the server.
*	@param		newSequence				Object containing the sequence data (from CFMEAudioInput)
*	@return								None
*	@exception							None
*	@remarks							The data will not be sent to the server before calling CFMEClient::Run()
*/
void Networking::CFMEClient::Send(const Utilities::CSeqData& newSequence)
{
	// obtain data
	privHandle->sequence = newSequence;
	
	// prepare data transmission
	privHandle->PerformSending();
}



/** @brief		Prepare the actual sending of sequence data to the server.
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Networking::CFMEClient::FMEClientPrivImplementation::PerformSending(void)
{
	using namespace boost::asio::placeholders;

	if ( !( isValidEndpoint ) ) {
		// no valid server endpoint was found up to now
		hostEndpoint = *( hostEndpointIt );
		newConnection->Socket().async_connect( hostEndpoint, boost::bind(&FMEClientPrivImplementation::HandleConnect, this, error, ++( hostEndpointIt ) ) );
	} else {
		// valid server endpoint is known
		newConnection->Socket().async_connect( hostEndpoint, boost::bind(&FMEClientPrivImplementation::HandleDirectConnect, this, error) );
	}
}



/** @brief		Handler for connection to the server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@param		endpointIt				Iterator the next TCP endpoint (relevant if the connection operation failed) - the different endpoints for example consider IPv4 and IPv6 addresses
*	@return								None
*	@exception	std::runtime_error		Thrown if the connect operation finally failed
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::CFMEClient::FMEClientPrivImplementation::HandleConnect(const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator endpointIt)
{
	if ( !(error) ) {
		// connection successfull
		HandleDirectConnect( error );
	} else if (endpointIt != boost::asio::ip::tcp::resolver::iterator() ) {
		// The connection failed. Try the next endpoint in the list.
		newConnection->Socket().close();
		hostEndpoint = *endpointIt;
		newConnection->Socket().async_connect( hostEndpoint, boost::bind(&FMEClientPrivImplementation::HandleConnect, this, boost::asio::placeholders::error, ++endpointIt) );
	} else {
		// connection could not be established
		throw std::runtime_error( error.message() );
    }
}



/** @brief		Starting data transmission to the server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception	std::runtime_error		Thrown if the connect operation finally failed
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::CFMEClient::FMEClientPrivImplementation::HandleDirectConnect(const boost::system::error_code& error)
{
	if ( !(error) ) {
		// connection to the server is standing
		isValidEndpoint = true;

		// start managing connection and sending data to the server
		sessionManager->StartWrite( newConnection, sequence );

		// prepare the new connection which will be used for the transmission of the sequence
		ResetConnection();	
	} else {
		// connection could not be established
		throw std::runtime_error( error.message() );
    }
}



/** @brief		Obtain standard port for the protocol.
*	@return								Standard port
*	@exception							None
*	@remarks							None
*/
unsigned short Networking::CFMEClient::GetStandardPort(void)
{
	return Session::standardPort;
}