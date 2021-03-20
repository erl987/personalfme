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
*/
#pragma once

#include <vector>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "SerializableSeqData.h"


/*@{*/
/** \ingroup Networking
*/

namespace Networking {
	namespace Session {
		/** @param	baseHeaderID	Header string identifying the data format of the network transmission */
		const std::string baseHeaderID  = " FME/1.0\r\n";
		/**	@param	bufferSize		Size of buffer used in network transmission */
		const int bufferSize = 128;
		/**	@param	standardPort	Standard port used for the protocol */
		const unsigned short standardPort = 6351;

		/**	\ingroup Networking
		*	Predefinition of Session::CFMESessionManager class
		*/
		template <class T> class CFMESessionManager;

		/**	\ingroup Networking
		*	Class modelling the client-server network connection. It might be used as a client as well as a server.
		*/
		template <class T> class CFMESession : public std::enable_shared_from_this<CFMESession<T>>
		{
		public:
			virtual ~CFMESession(void);	
			CFMESession(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqData&) > foundCallback);
			void Init(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqData&) > foundCallback);
			boost::asio::ip::tcp::socket& Socket(void);
			void StartRead(void);
			void StartWrite(const Utilities::CSeqData& newSequence);
			void Stop(void);
		protected:
			void HandleReadHeader(const boost::system::error_code& error);
			void HandleReadData(const boost::system::error_code& error, size_t bytesTransferred);
			void HandleWrite(const boost::system::error_code& error);
			void PerformWriting(void);
			virtual void DecodeSequenceData(std::vector<char>& inboundData, size_t dataLength);
			virtual std::string EncodeSequenceData(void);

			std::string headerID;
			std::unique_ptr< boost::asio::ip::tcp::socket > socket;
			boost::asio::streambuf data;
			std::vector< char > inboundData;
			std::string outboundData;
			size_t dataLength;
			boost::array< char, bufferSize > buffer;
  			boost::signals2::signal< void ( const Utilities::CSeqData& ) > foundSequenceSignal;
			std::shared_ptr< CFMESessionManager<T> > sessionManager;
			Utilities::CSeqData sequenceData;
			bool isInit;
		};
	}
}



/** @brief		Constructor.
*	@param		ioService				Boost ASIO IO service for the network connection
*	@param		manager					Network manager for safe management of all connections
*	@param		foundCallback			Callback function called, when new data was received by an incoming connection.
*	@exception							None
*	@remarks							None
*/
template <class T> Networking::Session::CFMESession<T>::CFMESession(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqData&) > foundCallback)
	: isInit( false )
{
	Init( ioService, manager, foundCallback );
}



/** @brief		Initializer of the class.
*	@param		ioService				Boost ASIO IO service for the network connection
*	@param		manager					Network manager for safe management of all connections
*	@param		foundCallback			Callback function called, when new data was received by an incoming connection.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESession<T>::Init(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqData&) > foundCallback)
{
	socket.reset( new boost::asio::ip::tcp::socket(ioService) );
	sessionManager = manager;

	// connect callback function for received data
	if ( isInit ) {
		foundSequenceSignal.disconnect_all_slots();
	}
	foundSequenceSignal.connect( foundCallback );

	headerID = baseHeaderID;

	isInit = true;
}



/** @brief		Default destructor.
*/
template <class T> Networking::Session::CFMESession<T>::~CFMESession(void)
{
	// forcing a stop of the network connection
	Stop();
}



/** @brief		Returning the used Boost ASIO network socket
*	@return								Boost ASIO network socket used
*	@exception							None
*	@remarks							None
*/
template <class T> boost::asio::ip::tcp::socket& Networking::Session::CFMESession<T>::Socket(void)
{
	return *socket;
}



/** @brief		Stopping the network connection
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESession<T>::Stop(void)
{
	if ( socket->is_open() ) {
		// stop the network connection
		socket->close();
	}
}
	


/** @brief		Prepare receiving sequence data over the network from a client
*	@return								None
*	@exception	std::runtime_error		Thrown if the class was not fully initialized before calling this function
*	@remarks							The network operations will not be started before io_service::run() is executed in the calling function
*/
template <class T> void Networking::Session::CFMESession<T>::StartRead(void)
{
	if ( !isInit ) {
		throw std::runtime_error("The object was not initialized before use!");
	}

	// Nagle algorithm is not required due to the small transmitted data size
	socket->set_option( boost::asio::ip::tcp::no_delay( true ) ); 

	// read header (some part of the data section might also be read due to implementation of async_read_until)
	boost::asio::async_read_until( *socket, data, headerID, boost::bind( &CFMESession<T>::HandleReadHeader, this, boost::asio::placeholders::error ) );
}



/** @brief		Processing the header section data
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@remarks							This is a handler for Boost ASIO asynchronous operations. The session finishes itself in case of an error.
*/
template <class T> void Networking::Session::CFMESession<T>::HandleReadHeader(const boost::system::error_code& error)
{
	using namespace std;
	
	string dataLengthString, uri, shortHeaderID;
	char sp1, cr, lf, bufferVal;

	if ( !error ) {
		// read header
		istream inStream( &data );
		inStream.unsetf( ios_base::skipws );
		inStream >> dataLengthString >> sp1 >> uri >> cr >> lf;
		
		shortHeaderID = headerID;
		shortHeaderID.erase( remove_if( shortHeaderID.begin(), shortHeaderID.end(), ::isspace ), shortHeaderID.end() );
		if ( uri.compare( shortHeaderID ) != 0 ) {
			// header ID is not valid
			sessionManager->Stop( this->shared_from_this() );
			return;		
		}
		
		try {
			dataLength = boost::lexical_cast<int>( dataLengthString );
		} catch (...) {
			// header is not valid
			sessionManager->Stop( this->shared_from_this() );
			return;
		}

		// obtain all data already read by async_read_until and store it in the inbound buffer
		stringstream stringStream;
		while ( inStream.good() ) {
			bufferVal = inStream.get();
			if ( inStream.good() ) {
				stringStream.put( bufferVal );
			}
		}
		string inString = stringStream.str();
		inboundData.insert( inboundData.end(), inString.begin(), inString.end() );
		
		// read missing data (with another buffer)
		socket->async_read_some( boost::asio::buffer( buffer ), boost::bind( &CFMESession::HandleReadData, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred ) );
	} else {
		// abort network connection
		sessionManager->Stop( this->shared_from_this() );
	}
}



/** @brief		Processing the data section
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@param		bytesTransferred		Numbr of bytes transferred in the last receiving operation
*	@return								None
*	@remarks							This is a handler for Boost ASIO asynchronous operations. The session finishes itself in case after successfull receiving of the data or an error.
*/
template <class T> void Networking::Session::CFMESession<T>::HandleReadData(const boost::system::error_code& error, size_t bytesTransferred)
{
	// add received data to inbound buffer
	inboundData.insert( inboundData.end(), buffer.begin(), buffer.begin() + bytesTransferred );

	if ( bytesTransferred > 0 ) {
		// further data might be available from the network client
		socket->async_read_some( boost::asio::buffer( buffer ), boost::bind( &CFMESession<T>::HandleReadData, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
	} else {
		if ( inboundData.size() > 0 ) {
			// decode sequence data and finally signal the information
			try {
				if ( inboundData.size() != dataLength ) {
					throw std::length_error( "Received data has wrong length." );
				}
				DecodeSequenceData( inboundData, dataLength );
			} catch (...) {
				// data is not valid
				sessionManager->Stop( this->shared_from_this() );
				return;
			}
		}

		// data transmission is completed, finish regularly the connection
		sessionManager->Stop( this->shared_from_this() );
	}
}



/** @brief		Deserialize a received data sequence (based on CFMEAudioInput)
*	@param		inboundData				Buffer containing the serialized data
*	@param		dataLength				Size of data buffer
*	@return								None
*	@remarks							The data is stored in the class member CFMESession<T>::sequenceData
*/
template <class T> void Networking::Session::CFMESession<T>::DecodeSequenceData(std::vector<char>& inboundData, size_t dataLength)
{
	Utilities::CSerializableSeqData serializableSequenceData;
	// decode sequence data
	std::string archive_data( &inboundData[0], dataLength );
	std::istringstream archive_stream( archive_data );
	boost::archive::text_iarchive archive( archive_stream );
	archive >> serializableSequenceData;
	sequenceData = serializableSequenceData;

	// signal new received sequence
	foundSequenceSignal( sequenceData );
}



/** @brief		Prepare sending sequence data over the network to the server
*	@param		newSequence				Object containing the data of the sequence (from CFMEAudioInput)
*	@return								None
*	@exception	std::runtime_error		Thrown if the class was not fully initialized before calling this function
*	@remarks							The network operations will not be started before io_service::run() is executed in the calling function
*/
template <class T> void Networking::Session::CFMESession<T>::StartWrite(const Utilities::CSeqData& newSequence)
{
	if ( !isInit ) {
		throw std::runtime_error("The object was not initialized before use!");
	}
	
	sequenceData = newSequence;

	// Prepare sending of data
	PerformWriting();
}



/** @brief		Final preparation of sequence data sending to the server
*	@return								None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESession<T>::PerformWriting(void)
{
	using namespace std;
	using namespace boost::asio;

	std::string dataStream;
	
	// encode sequence data
	dataStream = EncodeSequenceData();

	// generate header section
	outboundData = boost::lexical_cast<string>( dataStream.length() );
	outboundData += headerID;

	// add sequence data section
	outboundData += dataStream;
	
	// Nagle algorithm is not required here as the data is so small
	socket->set_option( ip::tcp::no_delay( true ) ); 
	
	// transfer the header section together with the data section to the server on the already established connection	
	boost::asio::async_write( *socket, boost::asio::buffer( outboundData ), boost::asio::transfer_all(), boost::bind( &CFMESession::HandleWrite, this, boost::asio::placeholders::error ) );
}



/** @brief		Serialize the data sequence (based on CFMEAudioInput)
*	@return								Serialized data string
*	@remarks							The data from the class member CFMESession<T>::sequenceData is serialized
*/
template <class T> std::string Networking::Session::CFMESession<T>::EncodeSequenceData()
{
	using namespace std;

	const Utilities::CSerializableSeqData constData( sequenceData ); // workaround for proper function of Boost Serialize
	
	// encode data sequence
	ostringstream archive_stream;
	boost::archive::text_oarchive archive( archive_stream );

	archive << constData;

	return archive_stream.str();
}



/** @brief		Handling after sending the sequence data to the server
*	@param		error					Error code from Boost ASIO library for the asynchronous connect operation
*	@return								None
*	@remarks							This is a handler for Boost ASIO asynchronous operations. The session finishes itself here.
*/
template <class T> void Networking::Session::CFMESession<T>::HandleWrite(const boost::system::error_code& error)
{
	if ( !error ) {
		// initiate graceful connection closure
		boost::system::error_code ignored_ec;
		socket->shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}

	// close current session as its job is completed
	sessionManager->Stop( this->shared_from_this() );

	if ( error ) {
		// abort in case of error
		throw std::runtime_error( error.message() );
	}
}
