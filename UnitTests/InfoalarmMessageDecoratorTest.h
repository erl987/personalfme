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
#include <boost/test/included/unit_test.hpp>
#include "EmailMessage.h"
#include "EmailGateway.h"
#include "InfoalarmMessageDecorator.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace InfoalarmTest {
		namespace InfoalarmMessageDecoratorTest {
			const std::string siteIDset1 = "siteID";
			const std::string siteIDset2 = "siteID2";
			const std::string alarmIDset = "alarmID";
			const std::vector< std::pair<std::string, std::string> > recipientsSet { { "recipient1", "recipient@test.de" } };
			const std::string alarmTextSet = "alarmText";
			const bool requiredStateSet = false;
			const std::unique_ptr<External::Email::CEmailMessage> emailMessage1 = std::make_unique<External::Email::CEmailMessage>( siteIDset1, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
			const std::unique_ptr<External::Email::CEmailMessage> emailMessage2 = std::make_unique<External::Email::CEmailMessage>( siteIDset2, alarmIDset, recipientsSet, alarmTextSet, requiredStateSet );
			const std::vector< std::shared_ptr<External::CAlarmMessage> > otherMessagesSet = { emailMessage1->Clone(), emailMessage2->Clone() };


			// Test section
			BOOST_AUTO_TEST_SUITE( InfoalarmMessageDecorator_test_suite, *label("basic") );

			/**	@brief		Testing of the constructors
			*/
			BOOST_AUTO_TEST_CASE( constructor_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Email;
				using namespace External::Infoalarm;

				CInfoalarmMessageDecorator message1, message2;

				// test empty construction - also tests the assignment operator
				message1 = CInfoalarmMessageDecorator();
				BOOST_REQUIRE( message1.IsEmpty() );

				// test normal construction - also tests the assignment operator
				message2 = CInfoalarmMessageDecorator( emailMessage1->Clone() );
				BOOST_REQUIRE( !message2.IsEmpty() );
				BOOST_REQUIRE( message2.RequiredState() == requiredStateSet );
				BOOST_CHECK_THROW( CInfoalarmMessageDecorator( nullptr ), std::runtime_error );

				// test copy construction
				message2.SetOtherMessages( otherMessagesSet );
				CInfoalarmMessageDecorator message3( message2 );
				BOOST_REQUIRE( message2 == message3 );

				CInfoalarmMessageDecorator message4( message1 );
				BOOST_REQUIRE( message4.IsEmpty() );
				
				// test cloning
				auto message5 = CInfoalarmMessageDecorator().Clone();
				BOOST_REQUIRE( message5->IsEmpty() );

				auto message6 = message3.Clone();
				BOOST_REQUIRE( *message6 == message3 );
			}


			/**	@brief		Testing of the Set / Get functions
			*/
			BOOST_AUTO_TEST_CASE( set_get_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Email;
				using namespace External::Infoalarm;

				CInfoalarmMessageDecorator messageSet;
				unique_ptr<CAlarmMessage> messageGet;
				vector< shared_ptr<CAlarmMessage> > otherMessagesGet;

				// test resetting
				messageSet = CInfoalarmMessageDecorator( emailMessage1->Clone() );
				BOOST_REQUIRE( !messageSet.IsEmpty() );

				messageSet.SetContainedMessage( emailMessage2->Clone() );
				messageGet = messageSet.GetContainedMessage();
				BOOST_REQUIRE( *messageGet == *emailMessage2 );
				BOOST_REQUIRE( messageSet.GetGatewayType() == typeid( CEmailGateway ) );

				messageSet.SetOtherMessages( otherMessagesSet );
				otherMessagesGet = messageSet.GetOtherMessages();
				BOOST_REQUIRE( otherMessagesGet != otherMessagesSet ); // the pointers must be cloned
				BOOST_REQUIRE(
					equal( begin( otherMessagesGet ), end( otherMessagesGet ), begin( otherMessagesSet ), end( otherMessagesSet ), []( auto val1, auto val2 ) { return ( *val1 == *val2 ); } )
				);

				// testing invalid input parameters
				BOOST_CHECK_THROW( messageSet.SetContainedMessage( nullptr ), std::runtime_error );
			}


			/**	@brief		Testing of the management of empty objects
			*/
			BOOST_AUTO_TEST_CASE( empty_test_case )
			{
				using namespace External::Infoalarm;

				auto message = CInfoalarmMessageDecorator( emailMessage1->Clone() );
				BOOST_REQUIRE( !message.IsEmpty() );

				// set empty
				message.SetEmpty();
				BOOST_REQUIRE( message.IsEmpty() );
				BOOST_CHECK_THROW( message.GetContainedMessage(), std::domain_error );
				BOOST_CHECK_THROW( message.GetOtherMessages(), std::domain_error );
				BOOST_CHECK_THROW( message.GetGatewayType(), std::domain_error );

				// reset non-empty
				message.SetContainedMessage( emailMessage2->Clone() );
				BOOST_REQUIRE( !message.IsEmpty() );
				BOOST_CHECK_NO_THROW( message.GetContainedMessage() );
			}


			/**	@brief		Testing of getting the gateway type
			*/
			BOOST_AUTO_TEST_CASE( get_gateway_type_test_case )
			{
				using namespace External::Infoalarm;

				auto message = CInfoalarmMessageDecorator( emailMessage1->Clone() );

				// the gateway type of the infoalarm is always that of the contained message
				BOOST_REQUIRE( message.GetGatewayType() == typeid( External::Email::CEmailGateway ) );

				BOOST_CHECK_THROW( CInfoalarmMessageDecorator().GetGatewayType(), std::domain_error );
			}


			/**	@brief		Testing of the equality comparison operators
			*/
			BOOST_AUTO_TEST_CASE( equality_comparison_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Infoalarm;
				using namespace External::Email;
				CInfoalarmMessageDecorator message1, message2;
				CEmailMessage message3;

				message1 = CInfoalarmMessageDecorator( emailMessage1->Clone() );
				message1.SetOtherMessages( otherMessagesSet );
				message2 = CInfoalarmMessageDecorator( emailMessage2->Clone() );
				message2.SetOtherMessages( otherMessagesSet );

				// test valid comparisons
				BOOST_REQUIRE( message1 == message1 );
				BOOST_REQUIRE( !( message1 == message2 ) );
				BOOST_REQUIRE( message1 != message2 );
				BOOST_REQUIRE( CInfoalarmMessageDecorator() == CInfoalarmMessageDecorator() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( message1 != message3 );
			}


			/**	@brief		Testing of the inequality comparison operators
			*/
			BOOST_AUTO_TEST_CASE( inequality_comparison_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Infoalarm;
				using namespace External::Email;
				CInfoalarmMessageDecorator message1, message2;
				CEmailMessage message3;

				message1 = CInfoalarmMessageDecorator( emailMessage1->Clone() );
				message1.SetOtherMessages( otherMessagesSet );
				message2 = CInfoalarmMessageDecorator( emailMessage2->Clone() );
				message2.SetOtherMessages( otherMessagesSet );

				// test valid comparisons
				BOOST_REQUIRE( !( message1 < message1 ) );
				BOOST_REQUIRE( !( message2 < message1 ) );
				BOOST_REQUIRE( message1 < message2 );
				BOOST_REQUIRE( !( CInfoalarmMessageDecorator() < CInfoalarmMessageDecorator() ) );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( message1 < message3 );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
