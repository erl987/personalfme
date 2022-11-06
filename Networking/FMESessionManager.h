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
#include <set>
#include <algorithm>
#include <memory>
#include "FMESession.h"
#include "SeqData.h"


/*@{*/
/** \ingroup Networking
*/

namespace Networking {
	namespace Session {
		/**	\ingroup Networking
		*	Manages open connections so that they may be cleanly stopped when the server needs to shut down.
		*/
		template <class T> class CFMESessionManager
		{
		public:
			virtual ~CFMESessionManager(void){};
			void StartWrite(std::shared_ptr<CFMESession<T>> newConnection, const Utilities::CSeqData& newSequence);
			void StartRead(std::shared_ptr<CFMESession<T>> newConnection);
			void Stop(std::shared_ptr<CFMESession<T>> connection);
			void StopAll(void);
		protected:
			std::set< std::shared_ptr<CFMESession<T>> > connections;
		};
	}
}
/*@}*/



/**	@brief		Starting a new connection for transmitting data to the server
*	@param		newConnection			Smart pointer holding the client-server connection. Derived classes of CFMESession might be used as well.
*	@param		newSequence				Object storing the information of the sequence to be transmitted
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESessionManager<T>::StartWrite(std::shared_ptr<CFMESession<T>> newConnection, const Utilities::CSeqData& newSequence)
{
	// add connection
	connections.insert( newConnection );

	// start sending of sequence data
	newConnection->StartWrite( newSequence );
}



/**	@brief		Starting a new connection for receiving data from a client
*	@param		newConnection			Smart pointer holding the client-server connection. Derived classes of CFMESession might be used as well.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESessionManager<T>::StartRead(std::shared_ptr<CFMESession<T>> newConnection)
{
	// add connection
	connections.insert( newConnection );

	// start receiving of sequence data
	newConnection->StartRead();
}



/**	@brief		Smoothly stopping a certain client-server connection
*	@param		connection				Smart pointer holding the client-server connection to be stopped. Derived classes of CFMESession might be used as well.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESessionManager<T>::Stop(std::shared_ptr<CFMESession<T>> connection)
{
	// remove the connection
	connections.erase( connection );

	// stop the network connection
	connection->Stop();
}



/**	@brief		Smoothly stopping all client-server connections
*	@return								None
*	@exception							None
*	@remarks							This function can be used for stopping the client or server
*/
template <class T> void Networking::Session::CFMESessionManager<T>::StopAll()
{
	// stop all connections
	for ( auto it = connections.begin(); it != connections.end(); it++ ) {
		(*it)->Stop();
	}

	// remove all connections
	connections.clear();
}
