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

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace EmailTest {
		namespace EmailLoginDataTest {
			// test data
			const unsigned int numTrialsSet = 10;
			const float timeDistTrialSet = 30.0f;
			const unsigned int maxNumConnectionsSet = 1;
			const External::Email::ConnectionType connectionTypeSet = External::Email::TLS_SSL_CONN;
			const std::string hostNameSet = "mail.gmx.net";
			const unsigned short portSet = 465;
			const std::string senderAddressSet = "ralf.rettig@gmx.de";
			const External::Email::AuthType authTypeSet = External::Email::UNENCRYPTED_AUTH;
			const std::string userNameSet = "user";
			const std::string passwordSet = "password";

			// Test section
			BOOST_AUTO_TEST_SUITE( EmailLoginData_test_suite, *label("basic") );

			/**	@brief		Testing the constructors
			*/
			BOOST_AUTO_TEST_CASE( clone_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Email;

				unique_ptr<CGatewayLoginData> login = make_unique<Email::CEmailLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Email::CEmailLoginData*>( login.get() )->SetServerInformation( connectionTypeSet, hostNameSet, portSet );
				dynamic_cast<Email::CEmailLoginData*>( login.get() )->SetLoginInformation( senderAddressSet, authTypeSet, userNameSet, passwordSet );
				unique_ptr<CGatewayLoginData> login2 = login->Clone();
				BOOST_REQUIRE( *login == *login2 );
			}


			/**	@brief		Testing the data handling
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Email;

				unsigned int numTrialsGet, maxNumConnectionsGet;
				float timeDistTrialGet;
				ConnectionType connectionTypeGet;
				AuthType authTypeGet;
				string hostNameGet, senderAddressGet, userNameGet, passwordGet;
				unsigned short portGet;

				unique_ptr<CGatewayLoginData> login = make_unique<CEmailLoginData>();
				BOOST_REQUIRE( !dynamic_cast<CEmailLoginData*>( login.get() )->IsValid() );
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				login->GetConnectionTrialInfos( numTrialsGet, timeDistTrialGet, maxNumConnectionsGet );
				BOOST_REQUIRE( !dynamic_cast<CEmailLoginData*>( login.get() )->IsValid() );

				dynamic_cast<CEmailLoginData*>( login.get() )->SetServerInformation( connectionTypeSet, hostNameSet, portSet );
				BOOST_REQUIRE( !dynamic_cast<CEmailLoginData*>( login.get() )->IsValid() );
				dynamic_cast<CEmailLoginData*>( login.get() )->SetLoginInformation( senderAddressSet, authTypeSet, userNameSet, passwordSet );

				dynamic_cast<CEmailLoginData*>( login.get() )->GetServerInformation( connectionTypeGet, hostNameGet, portGet );
				dynamic_cast<CEmailLoginData*>( login.get() )->GetLoginInformation( senderAddressGet, authTypeGet, userNameGet, passwordGet );
				BOOST_REQUIRE( dynamic_cast<CEmailLoginData*>( login.get() )->IsValid() );

				BOOST_REQUIRE( numTrialsSet == numTrialsGet );
				BOOST_REQUIRE( timeDistTrialSet == timeDistTrialGet );
				BOOST_REQUIRE( maxNumConnectionsSet == maxNumConnectionsGet );
				BOOST_REQUIRE( connectionTypeSet == connectionTypeGet );
				BOOST_REQUIRE( hostNameSet == hostNameGet );
				BOOST_REQUIRE( portSet == portGet );
				BOOST_REQUIRE( senderAddressSet == senderAddressGet );
				BOOST_REQUIRE( authTypeSet == authTypeGet );
				BOOST_REQUIRE( userNameSet == userNameGet );
				BOOST_REQUIRE( passwordSet == passwordGet );

				BOOST_REQUIRE( CEmailLoginData::GetDefaultPort( connectionTypeSet, authTypeSet ) == 465 );
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Email::CEmailLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Email::CEmailLoginData*>( login.get() )->SetServerInformation( connectionTypeSet, hostNameSet, portSet );
				dynamic_cast<Email::CEmailLoginData*>( login.get() )->SetLoginInformation( senderAddressSet, authTypeSet, userNameSet, passwordSet );
				unique_ptr<CGatewayLoginData> login2 = login->Clone();
				dynamic_cast<Email::CEmailLoginData*>( login2.get() )->SetLoginInformation( senderAddressSet, authTypeSet, userNameSet, "password2" );

				// test valid comparisons
				BOOST_REQUIRE( login == login );
				BOOST_REQUIRE( !( login == login2 ) );
				BOOST_REQUIRE( ( login != login2 ) );
				BOOST_REQUIRE( Email::CEmailLoginData() == Email::CEmailLoginData() );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
