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
/**	\defgroup	Networking	Library for transmission of found sequences to other computers in a network.
*/
#pragma once

#include <string>
#include <memory>
#include "SeqData.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef NETWORKING_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define NETWORKING_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define NETWORKING_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define NETWORKING_API __attribute__ ((visibility ("default")))
	#else
		#define NETWORKING_API
	#endif		
#endif

/*@{*/
/** \ingroup Networking
*/

namespace Networking {
	/**	\ingroup Networking
	*	Client for sending FME sequences over the network to a server. This class corresponds to the output data of CFMEAudioInput
	*/
	class CFMEClient
	{
	public:
		NETWORKING_API CFMEClient(void);
		NETWORKING_API CFMEClient(std::string serverAddress, unsigned short port);
		NETWORKING_API ~CFMEClient(void);			
		NETWORKING_API void Init(std::string serverAddress, unsigned short port);
		NETWORKING_API bool Run(void);
		NETWORKING_API void Send(const Utilities::CSeqData& newSequence);
		NETWORKING_API static unsigned short GetStandardPort(void);
	protected:
		class FMEClientPrivImplementation;
		std::shared_ptr<FMEClientPrivImplementation> privHandle;
		CFMEClient (const CFMEClient &); // prevent copying
    	CFMEClient & operator= (const CFMEClient &);
	};
}

/*@}*/