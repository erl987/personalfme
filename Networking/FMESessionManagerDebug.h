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
#include "SeqDataComplete.h"
#include "FMESessionDebug.h"
#include "FMESessionManager.h"

/*@{*/
/** \ingroup Networking
*/

namespace Networking {
	namespace Session {
		/**	\ingroup Networking
		*	Manages open connections so that they may be cleanly stopped when the server needs to shut down. This class corresponds to the full sequence information (tone frequencies, periods, tone lengths, ...) of CFMEAudioInputDebug and is only intended for debug purposes.
		*/
		// 
		template <class T> class CFMESessionManagerDebug : public CFMESessionManager<T>
		{
		public:
			virtual ~CFMESessionManagerDebug(void){};
			void StartWrite(std::shared_ptr<CFMESession<T>> newConnection, const Utilities::CSeqDataComplete<T>& newSequence);
		};
	}
}
/*@}*/



/**	@brief		Starting a new connection for transmitting data to the server
*	@param		newConnection			Smart pointer holding the client-server connection. Derived classes of CFMESession might be used as well.
*	@param		newSequence				Object storing the information of the sequence to be transmitted (from CFMEAudioInputDebug containing all information on tone frequencies, lengths, periods, ...)
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void Networking::Session::CFMESessionManagerDebug<T>::StartWrite(std::shared_ptr<CFMESession<T>> newConnection, const Utilities::CSeqDataComplete<T>& newSequence)
{
	// add connection
	this->connections.insert( newConnection );

	// start sending of sequence data
	std::dynamic_pointer_cast< CFMESessionDebug<T> >( newConnection )->StartWrite( newSequence );
}