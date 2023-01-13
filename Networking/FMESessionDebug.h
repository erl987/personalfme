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

#include <typeinfo>
#include "FMESession.h"
#include "SerializableSeqDataComplete.h"


/*@{*/
/** \ingroup Networking
*/

namespace Networking {
	namespace Session {
		/** @param		Header string identifying the debug data format of the network transmission */
		const std::string debugHeaderID = " FMEDebug/1.0\r\n";

		/**	\ingroup Networking
		*	Class modelling the client-server network connection. It might be used as a client as well as a server. This class corresponds to the full sequence information (tone frequencies, periods, tone lengths, ...) of CFMEAudioInputDebug and is only intended for debug purposes.
		*/
		template <class T> class CFMESessionDebug : public CFMESession<T>
		{
		public:
			virtual ~CFMESessionDebug(void){};
			CFMESessionDebug(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqDataComplete<T>&) > foundCallbackDebug);
			void Init(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqDataComplete<T>&) > foundCallbackDebug);
			void StartWrite(const Utilities::CSeqDataComplete<T>& newSequence);
		protected:
			virtual void DecodeSequenceData(std::vector<char>& inboundData, size_t dataLength);
			virtual std::string EncodeSequenceData(void);
			static void dummyBaseSession(const Utilities::CSeqData& data){};

			boost::signals2::signal< void (const Utilities::CSeqDataComplete<T>&) > foundSequenceSignalDebug;
			Utilities::CSeqDataComplete<T> sequenceDataDebug;
			bool isFromConstructor;
		};
	}
}



/** @brief		Constructor.
*	@param		ioService				Boost ASIO IO service for the network connection
*	@param		manager					Network manager for safe management of all connections
*	@param		foundCallbackDebug		Callback function called, when new data was received by an incoming connection. This function handles the full sequence data from CFMEAudioInputDebug.
*	@exception							None
*	@remarks							None
*/
template <class T> Networking::Session::CFMESessionDebug<T>::CFMESessionDebug(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqDataComplete<T>&) > foundCallbackDebug)
	: CFMESession<T>( ioService, manager, CFMESessionDebug<T>::dummyBaseSession )
{
	isFromConstructor = true;
	Init( ioService, manager, foundCallbackDebug );
	isFromConstructor = false;
}



/** @brief		Initializer of the class.
*	@param		ioService				Boost ASIO IO service for the network connection
*	@param		manager					Network manager for safe management of all connections
*	@param		foundCallbackDebug		Callback function called, when new data was received by an incoming connection. This function handles the full sequence data from CFMEAudioInputDebug.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESessionDebug<T>::Init(boost::asio::io_service& ioService, std::shared_ptr<CFMESessionManager<T>> manager, std::function< void(const Utilities::CSeqDataComplete<T>&) > foundCallbackDebug)
{
	// this ensures that the base class CFMESession::Init class is only called once
	if ( !isFromConstructor ) {
		CFMESession<T>::Init( ioService, manager, dummyBaseSession );
	}

	this->headerID = debugHeaderID;

	// connect callback function for received data
	foundSequenceSignalDebug.connect( foundCallbackDebug );
}



/** @brief		Prepare sending sequence data over the network to the server
*	@param		newSequence				Object containing the data of the sequence (from CFMEAudioInputDebug)
*	@return								None
*	@exception	std::runtime_error		Thrown if the class was not fully initialized before calling this function
*	@remarks							The network operations will not be started before io_service::run() is executed in the calling function
*/
template <class T> void Networking::Session::CFMESessionDebug<T>::StartWrite(const Utilities::CSeqDataComplete<T>& newSequence)
{
	if ( !this->isInit ) {
		throw std::runtime_error("The object was not initialized before use!");
	}

	sequenceDataDebug = newSequence;

	this->PerformWriting();
}



/** @brief		Serialize the data sequence (based on CFMEAudioInputDebug)
*	@return								Serialized data string
*	@remarks							The data from the class member CFMESession<T>::sequenceDataDebug is serialized
*/
template <class T> std::string Networking::Session::CFMESessionDebug<T>::EncodeSequenceData(void)
{
	using namespace std;

	const Utilities::CSerializableSeqDataComplete<T> constData( sequenceDataDebug ); // workaround for proper function of Boost Serialize
	
	// encode data sequence
	ostringstream archive_stream;
	boost::archive::text_oarchive archive( archive_stream );
	archive << constData;

	return archive_stream.str();
}



/** @brief		Deserialize a received data sequence (based on CFMEAudioInputDebug)
*	@param		inboundData				Buffer containing the serialized data
*	@param		dataLength				Size of data buffer
*	@return								None
*	@remarks							The data is stored in the class member CFMESession<T>::sequenceDataDebug
*/
template <class T> void Networking::Session::CFMESessionDebug<T>::DecodeSequenceData(std::vector<char>& inboundData, size_t dataLength)
{
	Utilities::CSerializableSeqDataComplete<T> serializableSequenceData;

	// decode sequence data
	std::string archive_data( &inboundData[0], dataLength );
	std::istringstream archive_stream( archive_data );
	boost::archive::text_iarchive archive( archive_stream );
	archive >> serializableSequenceData;
	sequenceDataDebug = serializableSequenceData;

	// signal new received sequence
	foundSequenceSignalDebug( sequenceDataDebug );
}