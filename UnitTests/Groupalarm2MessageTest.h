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

#include <vector>
#include <map>
#include <string>
#include <boost/test/unit_test.hpp>
#include "EmailMessage.h"
#include "Groupalarm2Message.h"
#include "Groupalarm2Gateway.h"
#include "AlarmMessage.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace GroupalarmTest {
	namespace Groupalarm2MessageTest {
		const bool allUsersSet = false;
		const std::map<std::string, unsigned int> labelsSet = { {"label1", 3}, {"label2", 4} };
		const std::vector<std::string> unitsSet = {"unit1", "unit2"};
		const std::vector<std::string> usersSet = {"user1", "user2"};
		const std::vector<std::string> scenariosSet = {"scenario1", "scenario2"};
		const std::string messageTextSet = "a message";
		const std::string messageTemplateSet = "";
		const double eventOpenPeriodInHoursSet = 2.13;

			// Test section
			BOOST_AUTO_TEST_SUITE( GroupalarmMessage_test_suite, *label("basic") );

			/**	@brief		Testing of the CGroupalarmMessage constructors
			*/
			BOOST_AUTO_TEST_CASE( Constructor_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarm2Message groupalarm1;

				// test empty construction
				groupalarm1 = CGroupalarm2Message();
				BOOST_REQUIRE( groupalarm1.IsEmpty() );
				BOOST_CHECK_THROW(groupalarm1.GetEventOpenPeriodInHours(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetLabels(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetMessageTemplate(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetMessageText(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetScenarios(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetUnits(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.GetUsers(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.HasMessageText(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.ToAllUsers(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.ToLabels(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.ToScenarios(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.ToUnits(), std::domain_error);
				BOOST_CHECK_THROW(groupalarm1.ToUsers(), std::domain_error);

				// test normal construction
				groupalarm1 = CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );

				BOOST_REQUIRE(groupalarm1.GetEventOpenPeriodInHours() == eventOpenPeriodInHoursSet);
				BOOST_REQUIRE(groupalarm1.GetLabels() == labelsSet);
				BOOST_REQUIRE(groupalarm1.GetMessageTemplate() == messageTemplateSet);
				BOOST_REQUIRE(groupalarm1.GetMessageText() == messageTextSet);
				BOOST_REQUIRE(groupalarm1.GetScenarios() == scenariosSet);
				BOOST_REQUIRE(groupalarm1.GetUnits() == unitsSet);
				BOOST_REQUIRE(groupalarm1.GetUsers() == usersSet);
				BOOST_REQUIRE(groupalarm1.HasMessageText() == true);
				BOOST_REQUIRE(groupalarm1.ToAllUsers() == false);
				BOOST_REQUIRE(groupalarm1.ToLabels() == true);
				BOOST_REQUIRE(groupalarm1.ToScenarios() == true);
				BOOST_REQUIRE(groupalarm1.ToUnits() == true);
				BOOST_REQUIRE(groupalarm1.ToUsers() == true);
			}


			/**	@brief		Testing of the Set / Get functions
			*/
			BOOST_AUTO_TEST_CASE(SetGet_test_case)
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarm2Message groupalarm1, groupalarm2;

				// standard cases are already covered by the constructor tests
				// test message templates
				groupalarm1 = CGroupalarm2Message();
				groupalarm1.Set(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, "", "templateId", eventOpenPeriodInHoursSet);
				BOOST_REQUIRE(!groupalarm1.IsEmpty());
				BOOST_REQUIRE(groupalarm1.HasMessageText() == false);
				BOOST_REQUIRE(groupalarm1.GetMessageTemplate() == "templateId");

				// test full alarm
				groupalarm2 = CGroupalarm2Message();
				groupalarm2.Set(true, {}, {}, {}, {}, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				BOOST_REQUIRE(!groupalarm2.IsEmpty());
				BOOST_REQUIRE(groupalarm2.ToAllUsers() == true);

				BOOST_REQUIRE( groupalarm1.GetGatewayType() == typeid( CGroupalarm2Gateway ) );
			}


			/**	@brief		Testing of invalid inputs
			*/
			BOOST_AUTO_TEST_CASE(Error_test_case)
			{
				using namespace std;
				using namespace External::Groupalarm;

				// check full alarm
				BOOST_CHECK_NO_THROW(CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet));
				BOOST_CHECK_THROW(CGroupalarm2Message(true, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet), Exception::Groupalarm2FullAlarmInconsistent);

				// check that either message text or template defined
				BOOST_CHECK_THROW(CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, "a text", "aTemplateId", eventOpenPeriodInHoursSet), Exception::Groupalarm2MessageContentInconsistent);
				BOOST_CHECK_THROW(CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, "", "", eventOpenPeriodInHoursSet), Exception::Groupalarm2MessageContentInconsistent);
				
				// check the validity of the event open period
				BOOST_CHECK_THROW(CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, -1.0), Exception::Groupalarm2EventOpenPeriodOutOfRange);
			}

			
			/**	@brief		Testing of the management of empty objects
			*/
			BOOST_AUTO_TEST_CASE(Empty_test_case)
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarm2Message groupalarm1;

				groupalarm1 = CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				BOOST_REQUIRE(!groupalarm1.IsEmpty());

				// set empty
				groupalarm1.SetEmpty();
				BOOST_REQUIRE(groupalarm1.IsEmpty());
				BOOST_CHECK_THROW( groupalarm1.GetLabels(), std::domain_error);

				// reset non-empty
				groupalarm1.Set(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );
				BOOST_CHECK_NO_THROW( groupalarm1.GetLabels());
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE(GroupalarmMessage_comparison_test_case)
			{
				using namespace std;
				using namespace External::Groupalarm;
				using namespace External::Email;
				CGroupalarm2Message groupalarm1, groupalarm2, groupalarm3;
				CEmailMessage email1;

				groupalarm1 = CGroupalarm2Message(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				groupalarm2 = CGroupalarm2Message(true, {}, {}, {}, {}, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				groupalarm3 = CGroupalarm2Message();

				vector< pair<string, string> > recipients = { { "test", "test" } };
				email1 = CEmailMessage( "test", "test", recipients, "test", true );

				// test valid comparisons
				BOOST_REQUIRE( groupalarm1 == groupalarm1 );
				BOOST_REQUIRE( !( groupalarm1 == groupalarm2 ) );
				BOOST_REQUIRE( ( groupalarm1 != groupalarm2 ) );
				BOOST_REQUIRE( CGroupalarm2Message() == CGroupalarm2Message() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( groupalarm1 == groupalarm3 ) );
				BOOST_REQUIRE( !( groupalarm1 == email1 ) );

				// test smaller operator
				BOOST_REQUIRE(!(groupalarm1 < groupalarm2));
			}

			/**	@brief		Testing of the event open period evaluations
			*/
			BOOST_AUTO_TEST_CASE(GroupalarmMessage_get_event_open_period_test_case)
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarm2Message groupalarm1(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, eventOpenPeriodInHoursSet);
				BOOST_REQUIRE(groupalarm1.GetEventOpenPeriodHour() == 2);
				BOOST_REQUIRE(groupalarm1.GetEventOpenPeriodMinute() == 7);
				BOOST_REQUIRE(groupalarm1.GetEventOpenPeriodSecond() == 47);

				CGroupalarm2Message groupalarm2(allUsersSet, labelsSet, scenariosSet, unitsSet, usersSet, messageTextSet, messageTemplateSet, 0);
				BOOST_REQUIRE(groupalarm2.GetEventOpenPeriodHour() == 0);
				BOOST_REQUIRE(groupalarm2.GetEventOpenPeriodMinute() == 0);
				BOOST_REQUIRE(groupalarm2.GetEventOpenPeriodSecond() == 0);
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
