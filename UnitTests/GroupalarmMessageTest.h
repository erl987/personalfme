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
#include "GroupalarmMessage.h"
#include "GroupalarmGateway.h"
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
	namespace GroupalarmMessageTest {
			const std::string alarmPhoneNumSet1 = "0151123456";
			const bool isFreeTextSet1 = true;
			const std::string messageTextSet1 = "test";
			const bool isLoadFeedbackSet1 = true;
			const bool isFlashSMSSet1 = false;
			const bool isNoFaxSet1 = true;

			const std::vector<int> alarmListsSet2 = { 12345, 23456 };
			const std::vector<int> alarmGroupsSet2 = { 3456, 4567, 56789 };
			const std::string alarmStringSet2 = "-12345-23456+3456+4567+56789";
			const bool isFreeTextSet2 = false;
			const std::string messageTextSet2 = "52342";
			const bool isLoadFeedbackSet2 = false;
			const bool isFlashSMSSet2 = true;
			const bool isNoFaxSet2 = false;

			const std::vector<int> alarmListsSet3 = { 1234, 1234 };			// containing repeated codes
			const std::vector<int> alarmGroupsSet3 = { 3456, 3456, 987 };	// containing repeated codes

			const std::vector<int> alarmListsSet4 = { 1234, 0, 98645 };		// containing "full alarm"


			// Test section
			BOOST_AUTO_TEST_SUITE( GroupalarmMessage_test_suite, *label("basic") );

			/**	@brief		Testing of the CGroupalarmMessage constructors
			*/
			BOOST_AUTO_TEST_CASE( Constructor_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarmMessage groupalarm1, groupalarm2;
				bool isFreeTextGet1, isLoadFeedbackGet1, isFlashSMSGet1, isNoFaxGet1, isCodeGet1;	
				bool isFreeTextGet2, isLoadFeedbackGet2, isFlashSMSGet2, isNoFaxGet2;
				string messageTextGet1, alarmStringGet1, alarmPhoneNumGet1;
				string messageTextGet2, alarmStringGet2, alarmPhoneNumGet2;
				vector<int> alarmListsGet1, alarmGroupsGet1;
				vector<int> alarmListsGet2, alarmGroupsGet2;

				// test empty construction
				groupalarm1 = CGroupalarmMessage();
				BOOST_REQUIRE( groupalarm1.IsEmpty() );
				BOOST_CHECK_THROW( groupalarm1.GetAlarmString( alarmStringGet1 ), std::domain_error );
				BOOST_CHECK_THROW( groupalarm1.GetAlarmData( alarmPhoneNumGet1, alarmListsGet1, alarmGroupsGet1 ), std::domain_error );
				BOOST_CHECK_THROW( groupalarm1.GetAlarmMessage( messageTextGet1 ), std::domain_error );
				BOOST_CHECK_THROW( groupalarm1.GetSettings( isLoadFeedbackGet1, isFlashSMSGet1, isNoFaxGet1 ), std::domain_error );

				// test normal construction
				groupalarm1 = CGroupalarmMessage( alarmPhoneNumSet1, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );
				isCodeGet1 = groupalarm1.GetAlarmString( alarmStringGet1 );
				groupalarm1.GetAlarmData( alarmPhoneNumGet1, alarmListsGet1, alarmGroupsGet1 );
				isFreeTextGet1 = groupalarm1.GetAlarmMessage( messageTextGet1 );
				groupalarm1.GetSettings( isLoadFeedbackGet1, isFlashSMSGet1, isNoFaxGet1 );

				BOOST_REQUIRE( !isCodeGet1 );
				BOOST_REQUIRE( alarmPhoneNumSet1 == alarmPhoneNumGet1 );
				BOOST_REQUIRE( isFreeTextSet1 == isFreeTextGet1 );
				BOOST_REQUIRE( messageTextSet1 == messageTextGet1 );
				BOOST_REQUIRE( isLoadFeedbackSet1 == isLoadFeedbackGet1 );
				BOOST_REQUIRE( isFlashSMSSet1 == isFlashSMSGet1 );
				BOOST_REQUIRE( isNoFaxSet1 == isNoFaxGet1 );
				BOOST_REQUIRE( alarmListsGet1.empty() );
				BOOST_REQUIRE( alarmGroupsGet1.empty() );
				BOOST_REQUIRE( alarmStringGet1 == alarmPhoneNumSet1 );

				groupalarm2 = CGroupalarmMessage( alarmListsSet2, alarmGroupsSet2, isFreeTextSet2, messageTextSet2, isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 );
				BOOST_REQUIRE( !groupalarm2.IsEmpty() );
				groupalarm2.GetAlarmString( alarmStringGet2 );
				groupalarm2.GetAlarmData( alarmPhoneNumGet2, alarmListsGet2, alarmGroupsGet2 );
				isFreeTextGet2 = groupalarm2.GetAlarmMessage( messageTextGet2 );
				groupalarm2.GetSettings( isLoadFeedbackGet2, isFlashSMSGet2, isNoFaxGet2 );

				BOOST_REQUIRE( alarmListsSet2 == alarmListsGet2 );
				BOOST_REQUIRE( alarmGroupsSet2 == alarmGroupsGet2 );
				BOOST_REQUIRE( isFreeTextSet2 == isFreeTextGet2 );
				BOOST_REQUIRE( messageTextSet2 == messageTextGet2 );
				BOOST_REQUIRE( isLoadFeedbackSet2 == isLoadFeedbackGet2 );
				BOOST_REQUIRE( isFlashSMSSet2 == isFlashSMSGet2 );
				BOOST_REQUIRE( isNoFaxSet2 == isNoFaxGet2 );
				BOOST_REQUIRE( alarmStringSet2 == alarmStringGet2 );
				BOOST_REQUIRE( alarmPhoneNumGet2.empty() );
			}


			/**	@brief		Testing of the Set / Get functions
			*/
			BOOST_AUTO_TEST_CASE( SetGet_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarmMessage groupalarm1, groupalarm2;
				bool isCodeGet2;
				string  alarmStringGet2, alarmPhoneNumGet1, alarmPhoneNumGet2;
				vector<int> alarmListsGet1, alarmListsGet2, alarmGroupsGet1, alarmGroupsGet2;

				// standard cases are already covered by the constructor tests
				// test handling of repeated identical codes
				groupalarm1 = CGroupalarmMessage();
				groupalarm1.Set( alarmListsSet3, alarmGroupsSet3, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );
				groupalarm1.GetAlarmData( alarmPhoneNumGet1, alarmListsGet1, alarmGroupsGet1 );
				BOOST_REQUIRE( alarmListsGet1.size() == 1 );
				BOOST_REQUIRE( alarmListsGet1.front() == alarmListsSet3.front() );
				BOOST_REQUIRE( alarmGroupsGet1.size() == 2 );
				BOOST_REQUIRE( alarmGroupsGet1.front() == alarmGroupsSet3.front() );
				BOOST_REQUIRE( alarmGroupsGet1.back() == alarmGroupsSet3.back() );

				// test handling of occurring "full alarm"
				groupalarm2 = CGroupalarmMessage( alarmListsSet4, alarmGroupsSet3, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				isCodeGet2 = groupalarm2.GetAlarmString( alarmStringGet2 );
				groupalarm2.GetAlarmData( alarmPhoneNumGet2, alarmListsGet2, alarmGroupsGet2 );
				BOOST_REQUIRE( isCodeGet2 );
				BOOST_REQUIRE( alarmListsGet2.size() == 1 );
				BOOST_REQUIRE( alarmListsGet2.front() == 0 );
				BOOST_REQUIRE( alarmGroupsGet2.empty() );
				BOOST_REQUIRE( alarmStringGet2 == "0" );

				BOOST_REQUIRE( groupalarm1.GetGatewayType() == typeid( CGroupalarmGateway ) );
			}


			/**	@brief		Testing of invalid inputs
			*/
			BOOST_AUTO_TEST_CASE( Error_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;

				// check testing of the maximum SMS-length
				string testText1 = "This is a test";
				string testText2 = "";
				string testText3( 161, 't' );

				BOOST_CHECK_NO_THROW( CGroupalarmMessage( alarmListsSet2, alarmGroupsSet2, true, testText1, isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ) );
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( alarmListsSet2, alarmGroupsSet2, true, testText2, isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ) );
				BOOST_CHECK_THROW( CGroupalarmMessage( alarmListsSet2, alarmGroupsSet2, true, testText3, isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ), Exception::GroupalarmFreeTextLength );

				// check for wrong message format
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( alarmPhoneNumSet1, false, "2", isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ) );
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( alarmPhoneNumSet1, false, "25", isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ) );
				BOOST_CHECK_THROW( CGroupalarmMessage( alarmPhoneNumSet1, false, "+2", isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ), Exception::GroupalarmTextCodeIncorrect );
				BOOST_CHECK_THROW( CGroupalarmMessage( alarmPhoneNumSet1, false, "hallo", isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ), Exception::GroupalarmTextCodeIncorrect );
				BOOST_CHECK_THROW( CGroupalarmMessage( alarmPhoneNumSet1, false, "", isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 ), Exception::GroupalarmTextCodeIncorrect );

				// check for wrong phone number format
				BOOST_CHECK_THROW( CGroupalarmMessage( "", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmPhoneNumberIncorrect );
				BOOST_CHECK_THROW( CGroupalarmMessage( "test", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmPhoneNumberIncorrect );
				BOOST_CHECK_THROW( CGroupalarmMessage( "0171/111111", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmPhoneNumberIncorrect );
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( "(0171)111111", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ) );
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( "+453 171 1111-11", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ) );
				BOOST_CHECK_NO_THROW( CGroupalarmMessage( "171 1111-11", isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ) );

				// check wrong list code formats
				BOOST_CHECK_THROW( CGroupalarmMessage( { -5, 3 }, { 12, 23 }, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmListCodeIncorrect );
				BOOST_CHECK_THROW( CGroupalarmMessage( { 6 }, { -9 }, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmListCodeIncorrect );

				// check for completely empty list codes
				BOOST_CHECK_THROW( CGroupalarmMessage( {}, {}, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 ), Exception::GroupalarmListCodeIncorrect );
			}

			
			/**	@brief		Testing of the management of empty objects
			*/
			BOOST_AUTO_TEST_CASE( Empty_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;

				CGroupalarmMessage groupalarm1;
				string alarmStringGet1;

				groupalarm1 = CGroupalarmMessage( alarmPhoneNumSet1, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );

				// set empty
				groupalarm1.SetEmpty();
				BOOST_REQUIRE( groupalarm1.IsEmpty() );
				BOOST_CHECK_THROW( groupalarm1.GetAlarmString( alarmStringGet1 ), std::domain_error );

				// reset non-empty
				groupalarm1.Set( alarmPhoneNumSet1, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				BOOST_REQUIRE( !groupalarm1.IsEmpty() );
				BOOST_CHECK_NO_THROW( groupalarm1.GetAlarmString( alarmStringGet1 ) );
			}


			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( GroupalarmMessage_comparison_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;
				using namespace External::Email;
				CGroupalarmMessage groupalarm1, groupalarm2, groupalarm3;
				CEmailMessage email1;

				groupalarm1 = CGroupalarmMessage( alarmPhoneNumSet1, isFreeTextSet1, messageTextSet1, isLoadFeedbackSet1, isFlashSMSSet1, isNoFaxSet1 );
				groupalarm2 = CGroupalarmMessage( alarmListsSet2, alarmGroupsSet2, isFreeTextSet2, messageTextSet2, isLoadFeedbackSet2, isFlashSMSSet2, isNoFaxSet2 );
				groupalarm3 = CGroupalarmMessage();

				vector< pair<string, string> > recipients = { { "test", "test" } };
				email1 = CEmailMessage( "test", "test", recipients, "test", true );

				// test valid comparisons
				BOOST_REQUIRE( groupalarm1 == groupalarm1 );
				BOOST_REQUIRE( !( groupalarm1 == groupalarm2 ) );
				BOOST_REQUIRE( ( groupalarm1 != groupalarm2 ) );
				BOOST_REQUIRE( CGroupalarmMessage() == CGroupalarmMessage() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( groupalarm1 == groupalarm3 ) );
				BOOST_REQUIRE( !( groupalarm1 == email1 ) );
			}


			/** @brief	Testing of the SMS-length validation
			*/
			BOOST_AUTO_TEST_CASE( GetSMSWithValidLength_test_case )
			{
				using namespace std;
				using namespace External::Groupalarm;
				string gotText, gotText2, gotText3;

				string tooLongText( 200, 'a' );
				gotText = CGroupalarmMessage::GetSMSMessageWithValidLength( tooLongText );
				BOOST_REQUIRE( gotText == tooLongText.substr( 0, 160 ) );

				string validText( 160, 'a' );
				gotText2 = CGroupalarmMessage::GetSMSMessageWithValidLength( validText );
				BOOST_REQUIRE( gotText2 == validText );

				string emptyText;
				gotText3 = CGroupalarmMessage::GetSMSMessageWithValidLength( emptyText );
				BOOST_REQUIRE( gotText3 == emptyText );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
