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

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "GroupalarmLoginData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace GroupalarmTest {
		namespace GroupalarmLoginDataTest {
			// test data
			const unsigned int numTrialsSet = 10;
			const float timeDistTrialSet = 30.0f;
			const unsigned int maxNumConnectionsSet = 1;
			const std::string userNameSet = "user";
			const bool isHashedPasswordSet = false;
			const std::string passwordSet = "pass";
			const std::string proxyServerNameSet = "proxy.ov.wan.thw.de";
			const unsigned short proxyServerPortSet = 8080;

			// Test section
			BOOST_AUTO_TEST_SUITE( GroupalarmLoginData_test_suite, *label("basic") );

			/**	@brief		Testing the constructors
			*/
			BOOST_AUTO_TEST_CASE( clone_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Groupalarm;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarmLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<CGroupalarmLoginData*>( login.get() )->SetServerInformation( userNameSet, isHashedPasswordSet, passwordSet, proxyServerNameSet, proxyServerPortSet );
				unique_ptr<CGatewayLoginData> login2 = login->Clone();
				BOOST_REQUIRE( *login == *login2 );
			}


			/**	@brief		Testing the data handling
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Groupalarm;

				unsigned int numTrialsGet, maxNumConnectionsGet;
				float timeDistTrialGet;
				string userNameGet, passwordGet, proxyServerNameGet;
				bool isHashedPasswordGet;
				unsigned short proxyServerPortGet;

				unique_ptr<CGatewayLoginData> login = make_unique<CGroupalarmLoginData>();
				BOOST_REQUIRE( !dynamic_cast<CGroupalarmLoginData*>( login.get() )->IsValid() );
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				login->GetConnectionTrialInfos( numTrialsGet, timeDistTrialGet, maxNumConnectionsGet );
				BOOST_REQUIRE( !dynamic_cast<CGroupalarmLoginData*>( login.get() )->IsValid() );

				dynamic_cast<CGroupalarmLoginData*>( login.get() )->SetServerInformation( userNameSet, isHashedPasswordSet, passwordSet, proxyServerNameSet, proxyServerPortSet );
				dynamic_cast<CGroupalarmLoginData*>( login.get() )->GetServerInformation( userNameGet, isHashedPasswordGet, passwordGet, proxyServerNameGet, proxyServerPortGet );
				BOOST_REQUIRE( dynamic_cast<CGroupalarmLoginData*>( login.get() )->IsValid() );

				BOOST_REQUIRE( numTrialsSet == numTrialsGet );
				BOOST_REQUIRE( timeDistTrialSet == timeDistTrialGet );
				BOOST_REQUIRE( maxNumConnectionsSet == maxNumConnectionsGet );
				BOOST_REQUIRE( userNameSet == userNameGet );
				BOOST_REQUIRE( isHashedPasswordSet == isHashedPasswordGet );
				BOOST_REQUIRE( passwordSet == passwordGet );
				BOOST_REQUIRE( proxyServerNameSet == proxyServerNameGet );
				BOOST_REQUIRE( proxyServerPortSet == proxyServerPortGet );
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarmLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Groupalarm::CGroupalarmLoginData*>( login.get() )->SetServerInformation( userNameSet, isHashedPasswordSet, passwordSet );
				unique_ptr<CGatewayLoginData> login2 = make_unique<Groupalarm::CGroupalarmLoginData>();
				login2->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Groupalarm::CGroupalarmLoginData*>( login2.get() )->SetServerInformation( userNameSet, isHashedPasswordSet, "pass2" );

				// test valid comparisons
				BOOST_REQUIRE( login == login );
				BOOST_REQUIRE( !( login == login2 ) );
				BOOST_REQUIRE( ( login != login2 ) );
				BOOST_REQUIRE( Groupalarm::CGroupalarmLoginData() == Groupalarm::CGroupalarmLoginData() );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
