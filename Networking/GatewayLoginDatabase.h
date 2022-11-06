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

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <typeindex>
#include "AlarmGateway.h"
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
	using LoginDatasetType = std::pair< std::type_index, std::unique_ptr<External::CGatewayLoginData> >;
	using GatewayDatasetType = std::pair< std::type_index, std::unique_ptr<External::CAlarmGateway> >;
	using LoginDBType = std::map< std::type_index, std::unique_ptr<CGatewayLoginData> >;
	using GatewayDBType = std::map< std::type_index, std::unique_ptr<CAlarmGateway> >;

	/** \ingroup Networking
	*	Class representing the database for the gateway login informations
	*/
	class CGatewayLoginDatabase
	{
	public:
		NETWORKING_API CGatewayLoginDatabase();
		NETWORKING_API virtual ~CGatewayLoginDatabase();
		NETWORKING_API CGatewayLoginDatabase( const CGatewayLoginDatabase& src );
		NETWORKING_API CGatewayLoginDatabase& operator=( const CGatewayLoginDatabase& rhs );
		NETWORKING_API virtual void Add( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> loginData );
		NETWORKING_API virtual void Replace( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> loginData );
		NETWORKING_API virtual void Remove( const std::type_index& gatewayType );
		NETWORKING_API void Clear();
		NETWORKING_API int Size() const;
		NETWORKING_API virtual std::unique_ptr<CGatewayLoginData> Search( const std::type_index& gatewayType ) const;
		NETWORKING_API virtual std::vector<LoginDatasetType> GetAllEntries() const;
		NETWORKING_API virtual std::vector<GatewayDatasetType> GetAllGateways() const;
		NETWORKING_API friend bool operator==( const CGatewayLoginDatabase& lhs, const CGatewayLoginDatabase& rhs );
		NETWORKING_API friend bool operator!=( const CGatewayLoginDatabase& lhs, const CGatewayLoginDatabase& rhs );
	protected:
		void CopyFrom( const CGatewayLoginDatabase& src );
	private:
		LoginDBType loginDatabase;
		GatewayDBType gatewayDatabase;
	};

	NETWORKING_API bool operator==( const CGatewayLoginDatabase& lhs, const CGatewayLoginDatabase& rhs );
	NETWORKING_API bool operator!=( const CGatewayLoginDatabase& lhs, const CGatewayLoginDatabase& rhs );
}
/*@}*/
