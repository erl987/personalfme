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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <algorithm>
#include "GatewayLoginDatabase.h"


/** @brief	Standard constructor.
*/
External::CGatewayLoginDatabase::CGatewayLoginDatabase(void)
{
}



/** @brief	Destructor.
*/
External::CGatewayLoginDatabase::~CGatewayLoginDatabase(void)
{
}



/** @brief	Copy constructor.
*	@param	src									Source object
*/
External::CGatewayLoginDatabase::CGatewayLoginDatabase(const CGatewayLoginDatabase& src)
{
	CopyFrom( src );
}



/** @brief	Copy assigment constructor.
*	@param	rhs									Right-hand side object
*/
External::CGatewayLoginDatabase& External::CGatewayLoginDatabase::operator=(const CGatewayLoginDatabase& rhs)
{
	if ( this == &rhs ) {
		return *this;
	}
	Clear();
	CopyFrom( rhs );

	return *this;
}



/** @brief	Helper method for deep copying of the object
*	@param	src									Right-hand side object to be copied into the present object
*	@return										None
*	@exception									None
*	@remarks									This method assumes that the data of the present object is empty
*/
void External::CGatewayLoginDatabase::CopyFrom(const CGatewayLoginDatabase& src)
{
	std::vector<LoginDatasetType> srcLoginDatabase;
	std::vector<GatewayDatasetType> srcGatewayDatabase;

	srcLoginDatabase = src.GetAllEntries();
	for (auto it = begin( srcLoginDatabase ); it != end( srcLoginDatabase ); it++) {
		loginDatabase[ it->first ] = it->second->Clone();
	}

	srcGatewayDatabase = src.GetAllGateways();
	for ( auto it = begin( srcGatewayDatabase ); it != end( srcGatewayDatabase ); it++ ) {
		gatewayDatabase[ it->first ] = it->second->Clone();
	}
}



/** @brief	Clears the database
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::CGatewayLoginDatabase::Clear( void )
{
	loginDatabase.clear();
	gatewayDatabase.clear();
}



/** @brief	Obtains the size of the database
*	@return										Size of the database
*	@exception									None
*	@remarks									None
*/
int External::CGatewayLoginDatabase::Size( void ) const
{
	return loginDatabase.size();
}



/**	@brief		Adds an entry to the database.
*	@param		gateway							Unique pointer to a gateway instance (for example: External::Email::CEmailGateway ).
*	@param		loginData						Base class pointer to the login data corresponding to the gateway type. The pointer needs to be moved to the method.
*	@return										None
*	@exception	std::runtime_error				Thrown if either the 'gateway' or the 'loginData' pointer are empty
*	@remarks									An existing entry is overwritten
*/
void External::CGatewayLoginDatabase::Add( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> loginData)
{
	if ( ( gateway == nullptr ) || ( loginData == nullptr ) ) {
		throw std::runtime_error( "Either the gateway or the loginData pointer are empty." );
	}

	auto type = std::type_index( typeid( *gateway ) );
	gatewayDatabase[ type ] = std::move( gateway );
	loginDatabase[ type ] = std::move( loginData );
}


/**	@brief		Replaces an entry in the database.
*	@param		gateway							Unique pointer to a gateway instance (for example: External::Email::CEmaiGateway ).
*	@param		loginData						Base class pointer to the login data corresponding to the gateway type. The pointer needs to be moved to the method.
*	@return										None
*	@exception	std::runtime_error				Thrown if either the 'gateway' or the 'loginData' pointer are empty or if the gateway type is not present in the database
*	@remarks									None
*/
void External::CGatewayLoginDatabase::Replace( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> loginData )
{
	if ( ( gateway == nullptr ) || ( loginData == nullptr ) ) {
		throw std::runtime_error( "Either the gateway or the loginData pointer are empty." );
	}

	if ( loginDatabase.find( typeid( *gateway ) ) == loginDatabase.end() ) {
		throw std::runtime_error( "The gateway type is not existing in the database." );
	}
	
	Add( std::move( gateway ), std::move( loginData ) );
}


/**	@brief		Removes an entry from the database.
*	@param		gatewayType						Type index of the gateway to be removed. Use for example: typeid( External::Email::CEmailMessage ).
*	@return										None
*	@exception	std::runtime_error				Thrown if the gateway type is not existing in the database
*	@remarks									None
*/
void External::CGatewayLoginDatabase::Remove( const std::type_index& gatewayType )
{
	if ( loginDatabase.find( gatewayType ) == loginDatabase.end() ) {
		throw std::runtime_error( "The gateway type is not existing in the database." );
	}

	loginDatabase.erase( gatewayType );
}



/**	@brief		Obtains an entry from the database.
*	@param		gatewayType						Type index of the gateway to be found. Use for example: typeid( External::Email::CEmailMessage ).
*	@return										Base class smart pointer containing the found gateway login data
*	@exception	std::runtime_error				Thrown if the gateway type is not existing in the database
*	@remarks									None
*/
std::unique_ptr< External::CGatewayLoginData > External::CGatewayLoginDatabase::Search( const std::type_index& gatewayType ) const
{
	if ( loginDatabase.find( gatewayType ) == end( loginDatabase ) ) {
		throw std::runtime_error( "The gateway type is not existing in the database." );
	}

	return loginDatabase.at( gatewayType )->Clone();
}



/**	@brief		Obtains the complete login database.
*	@return										Complete login database in a std::vector
*	@exception									None
*	@remarks									None
*/
std::vector< External::LoginDatasetType > External::CGatewayLoginDatabase::GetAllEntries() const
{
	std::vector<LoginDatasetType> loginList;

	loginList.reserve( loginDatabase.size() );
	for (auto it = begin( loginDatabase ); it != end( loginDatabase ); it++) {
		loginList.push_back( LoginDatasetType( it->first, it->second->Clone() ) );
	}

	return loginList;
}



/**	@brief		Obtains the all gateways present in the database
*	@return										All gateways present in the database in a std::vector
*	@exception									None
*	@remarks									None
*/
std::vector<External::GatewayDatasetType> External::CGatewayLoginDatabase::GetAllGateways() const
{
	std::vector<GatewayDatasetType> gatewayList;

	gatewayList.reserve( gatewayDatabase.size() );
	for ( auto it = begin( gatewayDatabase ); it != end( gatewayDatabase ); it++ ) {
		gatewayList.push_back( GatewayDatasetType( it->first, it->second->Clone() ) );
	}

	return gatewayList;
}


/**	@brief		Equality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::operator==( const External::CGatewayLoginDatabase& lhs, const External::CGatewayLoginDatabase& rhs ) {
	// compare the login data
	auto lhsLoginData = lhs.GetAllEntries();
	auto rhsLoginData = rhs.GetAllEntries();

	if ( lhsLoginData.size() != rhsLoginData.size() ) {
		return false;
	}
	for ( size_t i = 0; i < lhsLoginData.size(); i++ ) {
		if ( lhsLoginData[i].first != rhsLoginData[i].first ) {
			return false;
		}
		if ( *( lhsLoginData[i].second ) != *( rhsLoginData[i].second ) ) {
			return false;
		}
	}

	// compare the gateway data
	auto lhsGatewayData = lhs.GetAllGateways();
	auto rhsGatewayData = rhs.GetAllGateways();

	if ( lhsGatewayData.size() != rhsGatewayData.size() ) {
		return false;
	}
	for ( size_t i = 0; i < lhsGatewayData.size(); i++ ) {
		if ( lhsGatewayData[i].first != rhsGatewayData[i].first ) {
			return false;
		}
		if ( *( lhsGatewayData[i].second ) != *( rhsGatewayData[i].second ) ) {
			return false;
		}
	}

	return true;
}



/**	@brief		Unequality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are not identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::operator!=( const External::CGatewayLoginDatabase& lhs, const External::CGatewayLoginDatabase& rhs ) {
	return !( lhs == rhs );
}
