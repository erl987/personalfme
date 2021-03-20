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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <stdexcept>
#include "EmailLoginData.h"


/** @brief	Standard constructor.
*/
External::Email::CEmailLoginData::CEmailLoginData(void)
	: CGatewayLoginData(),
	  isServerSet( false ),
	  isLoginSet( false )
{
}



/** @brief	Destructor.
*/
External::Email::CEmailLoginData::~CEmailLoginData(void)
{
}



/** @brief		Provides a clone of the current object
*	@return									Clone of the current object
*	@exception								None
*	@remarks								None
*/
std::unique_ptr< External::CGatewayLoginData > External::Email::CEmailLoginData::Clone(void) const
{
	return std::make_unique<CEmailLoginData>( *this );
}



/** @brief		Obtains the default port for the given combination of connection and authentification type
*	@param		connectionType				Type of the connection
*	@param		authType					Type of the user authentification
*	@return									Default port for the given combination
*	@exception								None
*	@remarks								None
*/
unsigned short External::Email::CEmailLoginData::GetDefaultPort(const ConnectionType& connectionType, const AuthType& authType)
{
	unsigned short port;

	if ( connectionType == TLS_SSL_CONN ) {
		port = 465;
	} else {
		if ( authType == NO_AUTH ) {
			port = 25;
		} else {
			port = 587;
		}
	}

	return port;
}



/**	@brief		Getting the e-mail server settings
*	@param		connectionType					Type of the connection. Currently supported are plain unencryted SMTP (NO_ENCRYPTION) and TLSSTART.
*	@param		hostName						Host name (or IP-address) of the server
*	@param		port							Port of the server. If omitted, the default port for the chosen connection and authentification type is chosen.
*	@return										None
*	@exception	std::runtime_error				Thrown if the class has not been completely set
*	@remarks									None
*/
void External::Email::CEmailLoginData::GetServerInformation(ConnectionType& connectionType, std::string& hostName, unsigned short& port) const
{
	// check if the gateway data has been completely set
	if ( !IsValid() ) {
		throw std::runtime_error( "The login data is not completely set." );
	}
	
	connectionType = CEmailLoginData::connectionType;
	hostName = CEmailLoginData::hostName;
	port = CEmailLoginData::port;
}



/**	@brief		Getting the e-mail server login settings
*	@param		senderAddress					E-Mail address of the sender
*	@param		authType						Type of the user authentification. UNENCRYPTED_AUTH supports SMTP AUTH_LOGIN and AUTH_PLAIN and ENCRYPTED_AUTH supports AUTH_CRAM_MD5 and AUTH_CRAM_SHA1.
*	@param		userName						User name (ignored if the authentification type is NO_AUTH, may be omitted in this case)
*	@param		password						Password (ignored if the authentification type is NO_AUTH, may be omitted in this case)
*	@return										None
*	@exception	std::runtime_error				Thrown if the class has not been completely set
*	@remarks									None
*/
void External::Email::CEmailLoginData::GetLoginInformation(std::string& senderAddress, AuthType& authType, std::string& userName, std::string& password) const
{
	// check if the gateway data has been completely set
	if ( !IsValid() ) {
		throw std::runtime_error( "The login data is not completely set." );
	}

	senderAddress = CEmailLoginData::senderAddress;
	authType = CEmailLoginData::authType;
	userName = CEmailLoginData::userName;
	password = CEmailLoginData::password;
}



/**	@brief		Setting the e-mail server settings
*	@param		connectionType					Type of the connection. Currently supported are plain unencryted SMTP (NO_ENCRYPTION) and TLSSTART.
*	@param		hostName						Host name (or IP-address) of the server
*	@param		port							Port of the server. If omitted, the default port for the chosen connection and authentification type is chosen.
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::Email::CEmailLoginData::SetServerInformation(const ConnectionType& connectionType, const std::string& hostName, const unsigned short& port)
{
	CEmailLoginData::connectionType = connectionType;
	CEmailLoginData::hostName = hostName;
	CEmailLoginData::port = port;
	isServerSet = true;
}



/**	@brief		Setting the e-mail server login settings
*	@param		senderAddress					E-Mail address of the sender. It may have the following format: "First Last <first.last@test.de>" or "first.last@test.de"
*	@param		authType						Type of the user authentification. UNENCRYPTED_AUTH supports SMTP AUTH_LOGIN and AUTH_PLAIN and ENCRYPTED_AUTH supports AUTH_CRAM_MD5 and AUTH_CRAM_SHA1.
*	@param		userName						User name (ignored if the authentification type is NO_AUTH, may be omitted in this case)
*	@param		password						Password (ignored if the authentification type is NO_AUTH, may be omitted in this case)
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::Email::CEmailLoginData::SetLoginInformation(const std::string& senderAddress, const AuthType& authType, const std::string& userName, const std::string& password)
{
	CEmailLoginData::senderAddress = senderAddress;
	CEmailLoginData::authType = authType;
	CEmailLoginData::userName = userName;
	CEmailLoginData::password = password;
	isLoginSet = true;
}



/**	@brief		Determines if the current login data is valid
*	@return										True if the login data is valid, false otherwise
*	@exception									None
*	@remarks									None
*/
bool External::Email::CEmailLoginData::IsValid() const
{
	if ( !IsBaseClassValid() || !isLoginSet || !isServerSet ) {
		return false;
	}

	return true;
}



/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::Email::CEmailLoginData::IsEqual( const CGatewayLoginData& rhs ) const
{
	std::string rhsSenderAddress, rhsUserName, rhsPassword, rhsHostName;
	AuthType rhsAuthType;
	ConnectionType rhsConnectionType;
	unsigned short rhsPort;

	// compare the base objects
	if ( !CGatewayLoginData::IsEqual( rhs ) ) {
		return false;
	}

	try {
		auto derivRhs = dynamic_cast< const CEmailLoginData& >( rhs );

		// checks if both datasets are either empty or non-empty
		if ( ( IsValid() && !derivRhs.IsValid() ) || ( !IsValid() && derivRhs.IsValid() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( IsValid() && derivRhs.IsValid() ) {
			derivRhs.GetLoginInformation( rhsSenderAddress, rhsAuthType, rhsUserName, rhsPassword );
			if ( senderAddress != rhsSenderAddress ) {
				return false;
			}
			if ( authType != rhsAuthType) {
				return false;
			}
			if ( userName != rhsUserName) {
				return false;
			}
			if ( password != rhsPassword ) {
				return false;
			}

			derivRhs.GetServerInformation( rhsConnectionType, rhsHostName, rhsPort );
			if ( connectionType != rhsConnectionType ) {
				return false;
			}
			if ( hostName != rhsHostName ) {
				return false;
			}
			if ( port != rhsPort ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}
