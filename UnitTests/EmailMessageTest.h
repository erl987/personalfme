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
#include "EmailMessage.h"
#include "EmailGateway.h"
#include "Groupalarm2Message.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace EmailTest {
		namespace EmailMessageTest {
			const std::string siteIDset = "siteID";
			const std::string alarmIDset = "alarmID";
			const std::vector< std::pair<std::string, std::string> > recipientsSet { { "recipient1", "recipient@test.de" } };
			const std::string alarmTextSet = "alarmText";
			const bool requiredStateSet = false;


			// Test section
			BOOST_AUTO_TEST_SUITE( EmailMessage_test_suite, *label("default") );

			/**	@brief		Testing of the constructors
			*/
			BOOST_AUTO_TEST_CASE( constructor_test_case )
			{
				using namespace std;
				using namespace External::Email;

				string siteIDget1, alarmIDget1, alarmTextGet1;
				vector< pair<string, string> > recipientsGet1;
				bool requiredStateGet1;
				CEmailMessage message1, message2;
				unique_ptr<External::CAlarmMessage> message3, message4;

				// test empty construction
				message1 = CEmailMessage();
				BOOST_REQUIRE( message1.IsEmpty() );
				BOOST_CHECK_THROW( message1.Get( siteIDget1, alarmIDget1, recipientsGet1, alarmTextGet1, requiredStateGet1 ), std::domain_error );

				// test normal construction
				message2 = CEmailMessage( siteIDset, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
				BOOST_REQUIRE( !message2.IsEmpty() );
				
				// test cloning
				message3 = make_unique<CEmailMessage>( siteIDset, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
				BOOST_REQUIRE( message2 == *message3 );
				message4 = message3->Clone();
				BOOST_REQUIRE( *message3 == *message4 );
			}



			/**	@brief		Testing of the Set / Get functions
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External::Email;

				string siteIDget, alarmIDget, alarmTextGet;
				vector< pair<string, string> > recipientsGet;
				bool requiredStateGet;
				CEmailMessage message1, message2;

				const string siteIDset2 = "siteID2";
				const string alarmIDset2 = "alarmID2";

				// test resetting
				message1 = CEmailMessage( siteIDset, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
				BOOST_REQUIRE( !message1.IsEmpty() );
				message1.Get( siteIDget, alarmIDget, recipientsGet, alarmTextGet, requiredStateGet );
				BOOST_REQUIRE( siteIDset == siteIDget );
				BOOST_REQUIRE( alarmIDset == alarmIDget );
				BOOST_REQUIRE( recipientsSet == recipientsGet );
				BOOST_REQUIRE( alarmTextSet == alarmTextGet );
				BOOST_REQUIRE( requiredStateSet == requiredStateGet );

				BOOST_REQUIRE( message1.GetGatewayType() == typeid( CEmailGateway ) );

				message2.Set( siteIDset, alarmIDset, recipientsSet, alarmTextSet ); // in this case the required state is set implicitly to true
				message2.Get( siteIDget, alarmIDget, recipientsGet, alarmTextGet, requiredStateGet );
				BOOST_REQUIRE( requiredStateGet == true );

				message2.SetIDs( siteIDset2, alarmIDset2 );
				message2.Get( siteIDget, alarmIDget, recipientsGet, alarmTextGet, requiredStateGet );
				BOOST_REQUIRE( siteIDget == siteIDset2 );
				BOOST_REQUIRE( alarmIDget == alarmIDset2 );

				// all checks for invalid inputs are tested in other functions
			}



			/**	@brief		Testing of the management of empty objects
			*/
			BOOST_AUTO_TEST_CASE( empty_test_case )
			{
				using namespace std;
				using namespace External::Email;

				string siteIDget, alarmIDget, alarmTextGet;
				vector< pair<string, string> > recipientsGet;
				bool requiredStateGet;
				CEmailMessage message1;

				message1 = CEmailMessage( siteIDset, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
				BOOST_REQUIRE( !message1.IsEmpty() );

				// set empty
				message1.SetEmpty();
				BOOST_REQUIRE( message1.IsEmpty() );
				BOOST_CHECK_THROW( message1.Get( siteIDget, alarmIDget, recipientsGet, alarmTextGet, requiredStateGet ), std::domain_error );

				// reset non-empty
				message1.Set( siteIDset, alarmIDset, recipientsSet, alarmTextSet );
				BOOST_REQUIRE( !message1.IsEmpty() );
				BOOST_CHECK_NO_THROW( message1.Get( siteIDget, alarmIDget, recipientsGet, alarmTextGet, requiredStateGet ) );
			}



			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;
				using namespace External::Email;
				CEmailMessage message1, message2;
				CGroupalarm2Message message3;

				message1 = CEmailMessage( siteIDset, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
				message2 = CEmailMessage( siteIDset, "alarmID2", recipientsSet, alarmTextSet, requiredStateSet );

				// test valid comparisons
				BOOST_REQUIRE( message1 == message1 );
				BOOST_REQUIRE( !( message1 == message2 ) );
				BOOST_REQUIRE( message1 != message2 );
				BOOST_REQUIRE( CEmailMessage() == CEmailMessage() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( ( message1 != message3 ) );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
