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
#include "ExternalProgramLoginData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ExternalProgramTest {
		namespace ExternalProgramLoginDataTest {
			// test data
			const unsigned int numTrialsSet = 1;
			const float timeDistTrialSet = 0.0f;
			const unsigned int maxNumConnectionsSet = 10;

			// Test section
			BOOST_AUTO_TEST_SUITE( ExternalProgramLoginData_test_suite, *label("basic") );

			/**	@brief		Testing the constructors
			*/
			BOOST_AUTO_TEST_CASE( clone_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::ExternalProgram;

				unique_ptr<CGatewayLoginData> login = make_unique<ExternalProgram::CExternalProgramLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				unique_ptr<CGatewayLoginData> login2 = login->Clone();
				BOOST_REQUIRE( *login == *login2 );
			}


			/**	@brief		Testing the data handling
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::ExternalProgram;

				unsigned int numTrialsGet, maxNumConnectionsGet;
				float timeDistTrialGet;

				unique_ptr<CGatewayLoginData> login = make_unique<CExternalProgramLoginData>();
				BOOST_REQUIRE( !dynamic_cast<CExternalProgramLoginData*>( login.get() )->IsValid() );
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				login->GetConnectionTrialInfos( numTrialsGet, timeDistTrialGet, maxNumConnectionsGet );
				BOOST_REQUIRE( dynamic_cast<CExternalProgramLoginData*>( login.get() )->IsValid() );

				BOOST_REQUIRE( numTrialsSet == numTrialsGet );
				BOOST_REQUIRE( timeDistTrialSet == timeDistTrialGet );
				BOOST_REQUIRE( maxNumConnectionsSet == maxNumConnectionsGet );
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External;

				unique_ptr<CGatewayLoginData> login = make_unique<ExternalProgram::CExternalProgramLoginData>();
				login->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, maxNumConnectionsSet );
				unique_ptr<CGatewayLoginData> login2 = login->Clone();
				login2->SetConnectionTrialInfos( numTrialsSet, timeDistTrialSet, 5 );

				// test valid comparisons
				BOOST_REQUIRE( login == login );
				BOOST_REQUIRE( !( login == login2 ) );
				BOOST_REQUIRE( ( login != login2 ) );
				BOOST_REQUIRE( ExternalProgram::CExternalProgramLoginData() == ExternalProgram::CExternalProgramLoginData() );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
