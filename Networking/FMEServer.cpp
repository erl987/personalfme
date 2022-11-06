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
#include <boost/lexical_cast.hpp>
#include "FMEServer.h"
#include "FMEServerPrivImplementation.h"


/**	@brief	Default constructor.
*/
Networking::CFMEServer::CFMEServer(void)
	: privHandle( new FMEServerPrivImplementation )
{
}



/** @brief		Constructor.
*	@param		address					Address of the server. Examples: "localhost", "192.168.233.23"
*	@param		port					Number of server port
*	@param		foundCallback			Name of the callback, which is called if the server received a new sequence. The new data is passed to this function.
*	@exception							See CFMEServer::Init
*	@remarks							IPv4- as well as IPv6-addresses might be used
*/
Networking::CFMEServer::CFMEServer(std::string address, unsigned short port, std::function< void(const Utilities::CSeqData&) > foundCallback)
	: privHandle( new FMEServerPrivImplementation )
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
void Networking::CFMEServer::Init(std::string address, unsigned short port, std::function< void(const Utilities::CSeqData&) > foundCallback)
{
	using boost::asio::ip::tcp;
	
	// initial settings
	privHandle->foundCallback = foundCallback;
	privHandle->ResetConnection();
	try {
		// prepare server for listening in the network
		tcp::resolver resolver( privHandle->ioService );
		tcp::resolver::query query( address, boost::lexical_cast<std::string>( port ) );
		tcp::endpoint endpoint = *resolver.resolve( query );
		if ( privHandle->acceptor.is_open() ) {
			privHandle->acceptor.close();
		}
		privHandle->acceptor.open( endpoint.protocol() );
		privHandle->acceptor.set_option( tcp::acceptor::reuse_address( true ) );
		privHandle->acceptor.bind( endpoint );
		privHandle->acceptor.listen();
	} catch ( std::exception& e ) {
		// DNS error during address resolving
		throw std::runtime_error( e.what() );
	}

	// wait for next connection requests for any client
	privHandle->acceptor.async_accept( privHandle->newConnection->Socket(), boost::bind( &FMEServerPrivImplementation::HandleAccept, privHandle.get(), boost::asio::placeholders::error) );

	privHandle->isInit = true;
}



/**	@brief	Default constructor.
*/
Networking::CFMEServer::FMEServerPrivImplementation::FMEServerPrivImplementation(void)
	: ioService(),
	  acceptor( ioService ),
	  sessionManager( new Session::CFMESessionManager<float>() ),
	  newConnection( new Session::CFMESession<float>( ioService, sessionManager, foundCallback ) ),
	  isRunning( false )
{
	isInit = false;
}



/**	@brief	Default destructor.
*/
Networking::CFMEServer::~CFMEServer(void)
{
}



/** @brief		Handler for the request of a client
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@exception							None
*	@remarks							This a handler for Boost ASIO asynchronous operations.
*/
void Networking::CFMEServer::FMEServerPrivImplementation::HandleAccept(const boost::system::error_code& error)
{
	if ( !error ) {
		// start a new client-server connection
		sessionManager->StartRead( newConnection );
		
		// prepare the next connection (used in future)
		ResetConnection();

		// wait for next connection request
		acceptor.async_accept( newConnection->Socket(), boost::bind(&FMEServerPrivImplementation::HandleAccept, this, boost::asio::placeholders::error) );
	}
}



/** @brief		Preparing a new client-server connection.
*	@return								None
*	@exception							None
*	@remarks							The connection is not established, but only prepared
*/
void Networking::CFMEServer::FMEServerPrivImplementation::ResetConnection(void)
{
	newConnection.reset( new Session::CFMESession<float>( ioService, sessionManager, foundCallback ) );
}



/** @brief		Executing the network connection.
*	@return								None
*	@exception	std::runtime_error		Thrown if neither the full constructor nor CFMEServer::Init() were used for initalizing the function or if the server aborted with an error
*	@remarks							The network communication is only performed during calling this function. It will return after finishing all work.
*/
void Networking::CFMEServer::Run(void)
{
	std::string errorMessage;

	{
		std::lock_guard<std::mutex> isRunningLock( privHandle->isRunningMutex );
		if ( privHandle->isRunning ) {
			throw std::runtime_error( "The server is already running" );
		}
		if ( !( privHandle->isInit ) ) {
			throw std::runtime_error( "The object was not initialized before use!" );
		}

		privHandle->isRunning = true;
	}

	// start network processing - it will return when the service has finished execution
	try {
		privHandle->ioService.run();
	} catch ( std::exception& e ) {
		errorMessage = e.what();
	}

	{
		std::lock_guard<std::mutex> lock(privHandle->isRunningMutex);
		privHandle->isRunning = false;

		// resetting all networking objects
		privHandle->acceptor.cancel();
		privHandle->acceptor.close();
		privHandle->sessionManager->StopAll();

		// reset the network service in order to allow later recalls
		privHandle->ioService.reset();

		// handle potential errors that occured during the server processing
		if ( !errorMessage.empty() ) {
			throw std::runtime_error( errorMessage );
		}
	}
}



/** @brief		Stopping the server.
*	@return								None
*	@exception							None
*	@remarks							This function should be called for stopping the all server with smoothly finishing all client connections.
										The function returns immediately without waiting for the server to finish. It also finishes not yet running servers in the future.
*/
void Networking::CFMEServer::Stop(void)
{
	// stopping all server connections
	privHandle->ioService.stop();
}



/** @brief		Returns if the server is running or not.
*	@return								True if the server is running, otherwise false
*	@exception							None
*	@remarks							None
*/
bool Networking::CFMEServer::IsRunning(void)
{
	std::lock_guard<std::mutex> isRunningLock( privHandle->isRunningMutex );
	if ( privHandle->isRunning ) {
		return true;
	} else {
		return false;
	}
}



/**	@brief	Default destructor.
*/
Networking::CFMEServer::FMEServerPrivImplementation::~FMEServerPrivImplementation(void)
{
	// smoothly finishing the client connection
	if ( acceptor.is_open() ) {
		acceptor.close();
	}
}



/** @brief		Obtain standard port for the protocol.
*	@return								Standard port
*	@exception							None
*	@remarks							None
*/
unsigned short Networking::CFMEServer::GetStandardPort(void)
{
	return Session::standardPort;
}