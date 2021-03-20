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

#include <string>
#include <memory>
#include <functional>
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
	*	Server for receiving FME sequences over the network for a client. This class corresponds to the output data of CFMEAudioInput
	*/
	class CFMEServer
	{
	public:
		NETWORKING_API CFMEServer(void);
		NETWORKING_API CFMEServer(std::string address, unsigned short port, std::function< void(const Utilities::CSeqData&) > foundCallback);
		NETWORKING_API ~CFMEServer(void);
		NETWORKING_API void Init(std::string address, unsigned short port, std::function< void(const Utilities::CSeqData&) > foundCallback);
		NETWORKING_API void Run(void);
		NETWORKING_API void Stop(void);
		NETWORKING_API bool IsRunning(void);
		NETWORKING_API static unsigned short GetStandardPort(void);
	protected:
		class FMEServerPrivImplementation;
		std::shared_ptr<FMEServerPrivImplementation> privHandle;
		CFMEServer (const CFMEServer &); // prevent copying
    	CFMEServer & operator= (const CFMEServer &);
	};
}


/*@}*/