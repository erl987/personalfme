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
#include "ExternalProgramMessage.h"
#include "ExternalProgramGateway.h"
#include "Groupalarm2Message.h"

using boost::unit_test::label;

/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ExternalProgramTest {
		namespace ExternalProgramMessageTest {
			const std::string commandSet = "python.exe";
			const std::string programArgumentsSet = "\"arg 1\" \"$CODE\"";


			// Test section
			BOOST_AUTO_TEST_SUITE( ExternalProgramMessage_test_suite, *label("basic") );

			/**	@brief		Testing of the constructors
			*/
			BOOST_AUTO_TEST_CASE( constructor_test_case )
			{
				using namespace std;
				using namespace External::ExternalProgram;

				string commandGet, programArgumentsGet;
				CExternalProgramMessage message1, message2;
				unique_ptr<External::CAlarmMessage> message3, message4;

				// test empty construction
				message1 = CExternalProgramMessage();
				BOOST_REQUIRE( message1.IsEmpty() );
				BOOST_CHECK_THROW( message1.GetCommand(), std::domain_error );
				BOOST_CHECK_THROW( message1.GetProgramArguments(), std::domain_error );

				// test normal construction
				message2 = CExternalProgramMessage( commandSet, programArgumentsSet );
				BOOST_REQUIRE( !message2.IsEmpty() );
				
				// test cloning
				message3 = make_unique<CExternalProgramMessage>( commandSet, programArgumentsSet );
				BOOST_REQUIRE( message2 == *message3 );
				message4 = message3->Clone();
				BOOST_REQUIRE( *message3 == *message4 );
			}



			/**	@brief		Testing of the Set / Get functions
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External::ExternalProgram;

				string commandGet, programArgumentsGet;
				CExternalProgramMessage message1;

				// test resetting
				message1 = CExternalProgramMessage( commandSet, programArgumentsSet );
				BOOST_REQUIRE( !message1.IsEmpty() );
				commandGet = message1.GetCommand();
				programArgumentsGet = message1.GetProgramArguments();
				BOOST_REQUIRE( commandSet == commandGet );
				BOOST_REQUIRE( programArgumentsSet == programArgumentsGet );

				BOOST_REQUIRE( message1.GetGatewayType() == typeid( CExternalProgramGateway ) );

				// all checks for invalid inputs are tested in other functions
			}



			/**	@brief		Testing of the management of empty objects
			*/
			BOOST_AUTO_TEST_CASE( empty_test_case )
			{
				using namespace std;
				using namespace External::ExternalProgram;

				string commandGet, programArgumentsGet;
				CExternalProgramMessage message1;

				message1 = CExternalProgramMessage( commandSet, programArgumentsSet );
				BOOST_REQUIRE( !message1.IsEmpty() );

				// set empty
				message1.SetEmpty();
				BOOST_REQUIRE( message1.IsEmpty() );
				BOOST_CHECK_THROW( message1.GetCommand(), std::domain_error );
				BOOST_CHECK_THROW( message1.GetProgramArguments(), std::domain_error );

				// reset non-empty
				message1.Set( commandSet, programArgumentsSet );
				BOOST_REQUIRE( !message1.IsEmpty() );
				BOOST_CHECK_NO_THROW( message1.GetCommand() );
				BOOST_CHECK_NO_THROW( message1.GetProgramArguments() );
			}



			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;
				using namespace External::ExternalProgram;
				CExternalProgramMessage message1, message2;
				CGroupalarm2Message message3;

				message1 = CExternalProgramMessage( commandSet, programArgumentsSet );
				message2 = CExternalProgramMessage( "program.exe", programArgumentsSet );

				// test valid comparisons
				BOOST_REQUIRE( message1 == message1 );
				BOOST_REQUIRE( !( message1 == message2 ) );
				BOOST_REQUIRE( message1 != message2 );
				BOOST_REQUIRE( CExternalProgramMessage() == CExternalProgramMessage() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( ( message1 != message3 ) );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
