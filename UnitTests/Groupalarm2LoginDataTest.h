/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

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
#include "Groupalarm2LoginData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace GroupalarmTest {
		namespace Groupalarm2LoginDataTest {
			// test data
			const unsigned int numTrialsSet = 10;
			const float timeDistTrialSet = 30.0f;
			const unsigned int maxNumConnectionsSet = 1;
			const unsigned int organizationIdSet = 12345;
			const std::string apiTokenSet = "aToken123";
			const std::string proxyAddressSet = "proxy.company.org";
			const unsigned short proxyPortSet = 8080;
			const std::string proxyUserNameSet = "user";
			const std::string proxyPasswordSet = "passwd";


			// Test section
			BOOST_AUTO_TEST_SUITE( Groupalarm2LoginData_test_suite, *label("default") );

			/**	@brief		Testing the constructors
			*/
			BOOST_AUTO_TEST_CASE( clone_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Groupalarm;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<CGroupalarm2LoginData*>(login.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
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
				unsigned int organizationIdGet;
				string apiTokenGet, proxyAddressGet, proxyUserNameGet, proxyPasswordGet;
				unsigned short proxyPortGet;

				unique_ptr<CGatewayLoginData> login = make_unique<CGroupalarm2LoginData>();
				BOOST_REQUIRE( !dynamic_cast<CGroupalarm2LoginData*>( login.get() )->IsValid() );
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				login->GetConnectionTrialInfos( numTrialsGet, timeDistTrialGet, maxNumConnectionsGet );
				BOOST_REQUIRE( !dynamic_cast<CGroupalarm2LoginData*>( login.get() )->IsValid() );

				dynamic_cast<CGroupalarm2LoginData*>(login.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
				dynamic_cast<CGroupalarm2LoginData*>(login.get())->Get(organizationIdGet, apiTokenGet, proxyAddressGet, proxyPortGet, proxyUserNameGet, proxyPasswordGet);
				BOOST_REQUIRE( dynamic_cast<CGroupalarm2LoginData*>( login.get() )->IsValid() );

				BOOST_REQUIRE(numTrialsSet == numTrialsGet);
				BOOST_REQUIRE(timeDistTrialSet == timeDistTrialGet);
				BOOST_REQUIRE(maxNumConnectionsSet == maxNumConnectionsGet);
				BOOST_REQUIRE(organizationIdSet == organizationIdGet);
				BOOST_REQUIRE(apiTokenSet == apiTokenGet);
				BOOST_REQUIRE(proxyAddressSet == proxyAddressGet);
				BOOST_REQUIRE(proxyPortSet == proxyPortGet);
				BOOST_REQUIRE(proxyUserNameSet == proxyUserNameGet);
				BOOST_REQUIRE(proxyPasswordSet == proxyPasswordGet);
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>( login.get() )->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
				unique_ptr<CGatewayLoginData> login2 = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login2->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, "pass2");

				// test valid comparisons
				BOOST_REQUIRE( login == login );
				BOOST_REQUIRE( !( login == login2 ) );
				BOOST_REQUIRE( ( login != login2 ) );
				BOOST_REQUIRE( Groupalarm::CGroupalarm2LoginData() == Groupalarm::CGroupalarm2LoginData() );
			}

			/**	@brief		Testing of the IsWithProxy()
			*/
			BOOST_AUTO_TEST_CASE(is_with_proxy_test_case)
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login->SetConnectionTrialInfos(numTrialsSet, timeDistTrialSet, maxNumConnectionsSet);
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
				bool isWithProxy = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->IsWithProxy();
				BOOST_REQUIRE(isWithProxy == true);

				unique_ptr<CGatewayLoginData> login2 = make_unique<Groupalarm::CGroupalarm2LoginData>();
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->Set(organizationIdSet, apiTokenSet, "", 0, "", "");
				isWithProxy = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->IsWithProxy();
				BOOST_REQUIRE(isWithProxy == false);
			}

			/**	@brief		Testing of the IsWithProxyWithUserNameAndPassword()
			*/
			BOOST_AUTO_TEST_CASE(is_with_proxy_with_user_name_and_password_test_case)
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login->SetConnectionTrialInfos(numTrialsSet, timeDistTrialSet, maxNumConnectionsSet);
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
				bool isWithUserNameAndPassword = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->IsWithProxyWithUserNameAndPassword();
				BOOST_REQUIRE(isWithUserNameAndPassword == true);

				unique_ptr<CGatewayLoginData> login2 = make_unique<Groupalarm::CGroupalarm2LoginData>();
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, "");
				isWithUserNameAndPassword = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->IsWithProxyWithUserNameAndPassword();
				BOOST_REQUIRE(isWithUserNameAndPassword == false);

				unique_ptr<CGatewayLoginData> login3 = make_unique<Groupalarm::CGroupalarm2LoginData>();
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login3.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, "", "");
				isWithUserNameAndPassword = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login3.get())->IsWithProxyWithUserNameAndPassword();
				BOOST_REQUIRE(isWithUserNameAndPassword == false);
			}

			/**	@brief		Testing of the IsWithProxyWithUserNameOnly()
			*/
			BOOST_AUTO_TEST_CASE(is_with_proxy_with_user_name_only_test_case)
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<Groupalarm::CGroupalarm2LoginData>();
				login->SetConnectionTrialInfos(numTrialsSet, timeDistTrialSet, maxNumConnectionsSet);
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, proxyPasswordSet);
				bool isWithUserNameOnly = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login.get())->IsWithProxyWithUserNameOnly();
				BOOST_REQUIRE(isWithUserNameOnly == false);

				unique_ptr<CGatewayLoginData> login2 = make_unique<Groupalarm::CGroupalarm2LoginData>();
				dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->Set(organizationIdSet, apiTokenSet, proxyAddressSet, proxyPortSet, proxyUserNameSet, "");
				isWithUserNameOnly = dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(login2.get())->IsWithProxyWithUserNameOnly();
				BOOST_REQUIRE(isWithUserNameOnly == true);
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
