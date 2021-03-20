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
#include "GroupalarmLoginData.h"


/** @brief	Standard constructor.
*/
External::Groupalarm::CGroupalarmLoginData::CGroupalarmLoginData(void)
	: CGatewayLoginData(),
	  isServerSet( false )
{
}



/** @brief	Destructor.
*/
External::Groupalarm::CGroupalarmLoginData::~CGroupalarmLoginData(void)
{
}



/** @brief		Provides a clone of the current object
*	@return									Clone of the current object
*	@exception								None
*	@remarks								None
*/
std::unique_ptr< External::CGatewayLoginData > External::Groupalarm::CGroupalarmLoginData::Clone(void) const
{
	return std::make_unique<CGroupalarmLoginData>( *this );
}



/**	@brief		Getting the server settings for Groupalarm.de
*	@param		userName						User name for login on www.Groupalarm.de
*	@param		isHashedPassword				Flag stating if a hashed (true) or normal (false) password is used for login on www.Groupalarm.de
*	@param		password						Normal or hashed password for www.Groupalarm.de
*	@param		proxyServerName					Address (or IP-address) of the proxy server ("www.abcd.de"), omit it if no proxy server is used
*	@param		proxyServerPort					Port of the proxy server, omit it if the standard http-port (8080) is used
*	@return										None
*	@exception	std::runtime_error				Thrown if the class has not been completely set
*	@remarks									None
*/
void External::Groupalarm::CGroupalarmLoginData::GetServerInformation(std::string& userName, bool& isHashedPassword, std::string& password, std::string& proxyServerName, unsigned short& proxyServerPort) const
{
	// check if the gateway data has been completely set
	if ( !IsValid() ) {
		throw std::runtime_error( "The login data is not completely set." );
	}

	userName = CGroupalarmLoginData::userName;
	isHashedPassword = CGroupalarmLoginData::isHashedPassword;
	password = CGroupalarmLoginData::password;
	proxyServerName = CGroupalarmLoginData::proxyServerName;
	proxyServerPort = CGroupalarmLoginData::proxyServerPort;
}



/**	@brief		Setting the server settings for Groupalarm.de
*	@param		userName						User name for login on www.Groupalarm.de
*	@param		isHashedPassword				Flag stating if a hashed (true) or normal (false) password is used for login on www.Groupalarm.de
*	@param		password						Normal or hashed password for www.Groupalarm.de
*	@param		proxyServerName					Address (or IP-address) of the proxy server ("www.abcd.de"), omit it if no proxy server is used
*	@param		proxyServerPort					Port of the proxy server, omit it if the standard http-port (8080) is used
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::Groupalarm::CGroupalarmLoginData::SetServerInformation(const std::string& userName, const bool& isHashedPassword, const std::string& password, const std::string& proxyServerName, const unsigned short& proxyServerPort)
{
	CGroupalarmLoginData::userName = userName;
	CGroupalarmLoginData::isHashedPassword = isHashedPassword;
	CGroupalarmLoginData::password = password;
	CGroupalarmLoginData::proxyServerName = proxyServerName;
	CGroupalarmLoginData::proxyServerPort = proxyServerPort;
	isServerSet = true;
}


/**	@brief		Determines if the current login data is valid
*	@return										True if the login data is valid, false otherwise
*	@exception									None
*	@remarks									None
*/
bool External::Groupalarm::CGroupalarmLoginData::IsValid() const
{
	if ( !IsBaseClassValid() || !isServerSet ) {
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
bool External::Groupalarm::CGroupalarmLoginData::IsEqual( const CGatewayLoginData& rhs ) const
{
	std::string rhsUserName, rhsPassword, rhsProxyServerName;
	bool rhsIsHashedPassword;
	unsigned short rhsProxyServerPort;

	// compare the base objects
	if ( !CGatewayLoginData::IsEqual( rhs ) ) {
		return false;
	}

	try {
		const CGroupalarmLoginData& derivRhs = dynamic_cast< const CGroupalarmLoginData& >( rhs );

		// checks if both datasets are either empty or non-empty
		if ( ( IsValid() && !derivRhs.IsValid() ) || ( !IsValid() && derivRhs.IsValid() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( IsValid() && derivRhs.IsValid() ) {
			derivRhs.GetServerInformation( rhsUserName, rhsIsHashedPassword, rhsPassword, rhsProxyServerName, rhsProxyServerPort );
			if ( userName != rhsUserName ) {
				return false;
			}
			if ( isHashedPassword != rhsIsHashedPassword ) {
				return false;
			}
			if ( password != rhsPassword ) {
				return false;
			}
			if ( proxyServerName != rhsProxyServerName ) {
				return false;
			}
			if ( proxyServerPort != rhsProxyServerPort ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}
