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

#include <boost/test/unit_test.hpp>
#include "EmailLoginData.h"
#include "EmailGateway.h"
#include "GroupalarmLoginData.h"
#include "GroupalarmGateway.h"
#include "GatewayLoginDatabase.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace GatewayLoginDatabaseTest {
		std::vector< std::unique_ptr<External::CGatewayLoginData> > GetTestLogins()
		{
			using namespace std;
			using namespace External;

			vector< unique_ptr<CGatewayLoginData> > logins;
			unique_ptr<CGatewayLoginData> emailLoginData = make_unique<Email::CEmailLoginData>();
			unique_ptr<CGatewayLoginData> groupalarmLoginData = make_unique<Groupalarm::CGroupalarmLoginData>();

			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetServerInformation( Email::TLS_SSL_CONN, "mail.gmx.net" );
			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetLoginInformation( "ralf.rettig@gmx.de", Email::UNENCRYPTED_AUTH, "ralf.rettig@gmx.de", "" );
			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetConnectionTrialInfos( 10, 30.0, 1 );
			logins.push_back( move( emailLoginData ) );

			dynamic_cast< Groupalarm::CGroupalarmLoginData* >( groupalarmLoginData.get() )->SetServerInformation( "user", false, "pass" );
			dynamic_cast< Groupalarm::CGroupalarmLoginData* >( groupalarmLoginData.get() )->SetConnectionTrialInfos( 10, 30.0, 1 );
			logins.push_back( move( groupalarmLoginData ) );

			return logins;
		}


		External::CGatewayLoginDatabase GetTestLoginDB()
		{
			using namespace std;
			using namespace External;

			CGatewayLoginDatabase loginDatabase;
			vector< unique_ptr<CGatewayLoginData> > logins;

			logins = GetTestLogins();
			loginDatabase.Add( make_unique<Email::CEmailGateway>(), move( logins[0] ) );
			loginDatabase.Add( make_unique<Groupalarm::CGroupalarmGateway>(), move( logins[1] ) );

			return loginDatabase;
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( GatewayLoginDatabase_test_suite, *label("basic") );

		/**	@brief		Testing the constructors
		*/
		BOOST_AUTO_TEST_CASE( constructor_test_case )
		{
			External::CGatewayLoginDatabase loginDatabase;

			loginDatabase = GetTestLoginDB();									// testing the assignment operator
			External::CGatewayLoginDatabase loginDatabase2( loginDatabase );	// testing the copy constructor
			BOOST_REQUIRE( loginDatabase == GetTestLoginDB() );
			BOOST_REQUIRE( loginDatabase2 == loginDatabase );
		}



		/**	@brief		Testing the database handling
		*/
		BOOST_AUTO_TEST_CASE( database_operations_test_case )
		{
			using namespace std;
			using namespace External;

			CGatewayLoginDatabase loginDatabase;
			unique_ptr<CGatewayLoginData> emailLoginData = make_unique<Email::CEmailLoginData>();

			BOOST_CHECK_NO_THROW( loginDatabase.Add( make_unique<Email::CEmailGateway>(), emailLoginData->Clone() ) );
			BOOST_REQUIRE( loginDatabase.Size() == 1 );
			BOOST_CHECK_NO_THROW( loginDatabase.Replace( make_unique<Email::CEmailGateway>(), emailLoginData->Clone() ) );
			BOOST_REQUIRE( loginDatabase.Size() == 1 );
			BOOST_CHECK_NO_THROW( loginDatabase.Remove( typeid( Email::CEmailGateway ) ) );
			BOOST_REQUIRE( loginDatabase.Size() == 0 );

			BOOST_CHECK_NO_THROW( loginDatabase.Add( make_unique<Email::CEmailGateway>(), emailLoginData->Clone() ) );
			loginDatabase.Clear();
			BOOST_REQUIRE( loginDatabase.Size() == 0 );

			// check wrong inputs
			BOOST_REQUIRE_THROW( loginDatabase.Add( nullptr, move( emailLoginData ) ), std::runtime_error );
			BOOST_REQUIRE_THROW( loginDatabase.Add( make_unique<Email::CEmailGateway>(), nullptr ), std::runtime_error );
			BOOST_REQUIRE_THROW( loginDatabase.Add( nullptr, nullptr ), std::runtime_error );

			BOOST_REQUIRE_THROW( loginDatabase.Replace( nullptr, move( emailLoginData ) ), std::runtime_error );
			BOOST_REQUIRE_THROW( loginDatabase.Replace( make_unique<Email::CEmailGateway>(), nullptr ), std::runtime_error );
			BOOST_REQUIRE_THROW( loginDatabase.Replace( nullptr, nullptr ), std::runtime_error );

			BOOST_REQUIRE_THROW( loginDatabase.Remove( typeid( Groupalarm::CGroupalarmGateway ) ), std::runtime_error ); // this gateway is not present in the database and needs to cause an exception
		}



		/**	@brief		Testing the operations obtaining the database entries
		*/
		BOOST_AUTO_TEST_CASE( entry_operations_test_case )
		{
			using namespace std;
			using namespace External;
			unique_ptr<CGatewayLoginData> loginDataGet;
			vector<LoginDatasetType> loginDatasetGet;
			vector<GatewayDatasetType> gatewaysGet;
			vector< unique_ptr<CGatewayLoginData> > origLogins;

			origLogins = GetTestLogins();

			CGatewayLoginDatabase loginDatabase = GetTestLoginDB();
			loginDatasetGet = loginDatabase.GetAllEntries();
			BOOST_REQUIRE( loginDatasetGet.size() == 2 );
			auto emailPosIt = find_if( begin( loginDatasetGet ), end( loginDatasetGet ), []( const auto& val ) { return ( val.first == typeid( Email::CEmailGateway ) ); } );
			BOOST_REQUIRE( emailPosIt != end( loginDatasetGet ) );
			BOOST_REQUIRE( *( emailPosIt->second ) == *( origLogins[0] ) );
			auto groupalarmPosIt = find_if( begin( loginDatasetGet ), end( loginDatasetGet ), []( const auto& val ) { return ( val.first == typeid( Groupalarm::CGroupalarmGateway ) ); } );
			BOOST_REQUIRE( groupalarmPosIt != end( loginDatasetGet ) );
			BOOST_REQUIRE( *( groupalarmPosIt->second ) == *( origLogins[1] ) );

			gatewaysGet = loginDatabase.GetAllGateways();
			BOOST_REQUIRE( gatewaysGet.size() == 2 );
			auto emailPosIt2 = find_if( begin( gatewaysGet ), end( gatewaysGet ), []( const auto& val ) { return( val.first == typeid( Email::CEmailGateway ) ); } );
			BOOST_REQUIRE( emailPosIt2 != end( gatewaysGet ) );
			BOOST_REQUIRE( *( emailPosIt2->second ) == *( make_unique<Email::CEmailGateway>() ) );
			auto groupalarmPosIt2 = find_if( begin( gatewaysGet ), end( gatewaysGet ), []( const auto& val ) { return( val.first == typeid( Groupalarm::CGroupalarmGateway ) ); } );
			BOOST_REQUIRE( groupalarmPosIt2 != end( gatewaysGet ) );
			BOOST_REQUIRE( *( groupalarmPosIt2->second ) == *( make_unique<Groupalarm::CGroupalarmGateway>() ) );

			loginDataGet = loginDatabase.Search( typeid( Email::CEmailGateway ) );
			BOOST_REQUIRE( *loginDataGet == *( origLogins[0] ) );
		}


		/**	@brief		Testing of the comparison operators
		*/
		BOOST_AUTO_TEST_CASE( comparison_test_case )
		{
			using namespace std;
			using namespace External;

			CGatewayLoginDatabase loginDatabase = GetTestLoginDB();
			CGatewayLoginDatabase loginDatabase2 = GetTestLoginDB();
			loginDatabase2.Remove( typeid( Email::CEmailGateway ) );

			// test valid comparisons
			BOOST_REQUIRE( loginDatabase == loginDatabase );
			BOOST_REQUIRE( !( loginDatabase == loginDatabase2 ) );
			BOOST_REQUIRE( ( loginDatabase != loginDatabase2 ) );
			BOOST_REQUIRE( CGatewayLoginDatabase() == CGatewayLoginDatabase() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
