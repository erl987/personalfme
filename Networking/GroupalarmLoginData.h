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
#include "GatewayLoginData.h"

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
namespace External {
	namespace Groupalarm {
		/** \ingroup Networking
		*	Class representing the login data for sending alarms via the gateway www.groupalarm.de
		*/
		class CGroupalarmLoginData : public CGatewayLoginData
		{
		public:
			NETWORKING_API CGroupalarmLoginData(void);
			NETWORKING_API virtual ~CGroupalarmLoginData(void);
			NETWORKING_API virtual std::unique_ptr< CGatewayLoginData > Clone(void) const override;
			NETWORKING_API virtual void SetServerInformation(const std::string& userName, const bool& isHashedPassword, const std::string& password, const std::string& proxyServerName = std::string(), const unsigned short& proxyServerPort = 0);
			NETWORKING_API virtual void GetServerInformation(std::string& userName, bool& isHashedPassword, std::string& password, std::string& proxyServerName, unsigned short& proxyServerPort) const;
			NETWORKING_API virtual bool IsValid() const;
		protected:
			NETWORKING_API virtual bool IsEqual( const CGatewayLoginData& rhs ) const override;
		private:
			std::string userName;
			bool isHashedPassword;
			std::string password;
			std::string proxyServerName;
			unsigned short proxyServerPort;
			bool isServerSet;
		};
	}
}
/*@}*/
