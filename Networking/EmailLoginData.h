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
	namespace Email {
		/**		\ingroup Networking
		*		Possible types of connection safety
		*/
		enum ConnectionType { UNENCRYPTED_CONN, TLSSTART_CONN, TLS_SSL_CONN };

		/**		\ingroup Networking
		*		Possible types of authentification. The different types may include a number of methods that are tried out by the client when connecting to the server.
		*/
		enum AuthType { NO_AUTH, UNENCRYPTED_AUTH, ENCRYPTED_AUTH };

		/**		\ingroup Networking
		*		The default port will be chosen automatically according to connection and authentification type
		*/
		const unsigned short DEFAULT_PORT = 0;

		/** \ingroup Networking
		*	Class representing the login data for sending alarms via e-mail
		*/
		class CEmailLoginData : public CGatewayLoginData
		{
		public:
			NETWORKING_API CEmailLoginData(void);
			NETWORKING_API virtual ~CEmailLoginData(void);
			NETWORKING_API virtual std::unique_ptr< CGatewayLoginData > Clone(void) const override;
			NETWORKING_API virtual void SetServerInformation(const ConnectionType& connectionType, const std::string& hostName, const unsigned short& port = DEFAULT_PORT);
			NETWORKING_API virtual void SetLoginInformation(const std::string& senderAddress, const AuthType& authType, const std::string& userName = std::string(), const std::string& password = std::string());
			NETWORKING_API virtual void GetServerInformation(ConnectionType& connectionType, std::string& hostName, unsigned short& port) const;
			NETWORKING_API virtual void GetLoginInformation(std::string& senderAddress, AuthType& authType, std::string& userName, std::string& password) const;
			NETWORKING_API static unsigned short GetDefaultPort(const ConnectionType& connectionType, const AuthType& authType);
			NETWORKING_API virtual bool IsValid() const;
		protected:
			NETWORKING_API virtual bool IsEqual( const CGatewayLoginData& rhs ) const override;
		private:
			ConnectionType connectionType;
			std::string hostName;
			unsigned short port;
			std::string senderAddress;
			AuthType authType;
			std::string userName;
			std::string password;
			bool isServerSet;
			bool isLoginSet;
		};
	}
}
/*@}*/
