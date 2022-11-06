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
/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "EmailMessage.h"
#include "SendStatusMessage.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup StatusMessage
	*/
	namespace Messages {
		namespace SendStatusMessage {
			// Test section
			BOOST_AUTO_TEST_SUITE( SendStatusMessage_test_suite, *label("basic") );

			/**	@brief		Testing of setting and getting functions of the class
			*/
			BOOST_AUTO_TEST_CASE( SendStatusMessage_set_get_test_case )
			{
				using namespace std;
				using namespace boost::posix_time;
				using namespace Utilities;
				using namespace Utilities::Message;
				using namespace External;

				ptime messageTimeSet, messageTimeGet;
				CDateTime sequenceTimeSet, sequenceTimeGet, sequenceTimeGet2;
				MessageType typeGet;
				vector<int> codeGet, codeGet2;
				unsigned int numTrialsSet, numTrialsGet, numTrialsGet2;
				SendStatus sendStatusSet, sendStatusGet, sendStatusGet2;
				chrono::seconds timeDistTrialsSet, timeDistTrialsGet, timeDistTrialsGet2;
				std::unique_ptr<CAlarmMessage> alarmMessageSet, alarmMessageGet, alarmMessageGet2;

				vector< pair<string, string> > recipients = { { "Name1", "name1@test.de" }, { "Name2", "name2@test.de" } };
				vector<int> codeSet = { 1, 2, 3, 4, 5 };
				sequenceTimeSet = CDateTime( 15, 11, 2015, Utilities::CTime( 19, 21, 13, 234 ) );
				sendStatusSet.code = SUCCESS;
				sendStatusSet.text = "test";
				numTrialsSet = 10;
				timeDistTrialsSet = 30s;
				messageTimeSet = ptime( microsec_clock::universal_time() );
				alarmMessageSet = make_unique<Email::CEmailMessage>( "siteID", "alarmID", recipients, "alarmText", true );

				CSendStatusMessage<CAlarmMessage> message( messageTimeSet, sequenceTimeSet, codeSet, alarmMessageSet->Clone(), sendStatusSet, numTrialsSet, timeDistTrialsSet );
				message.GetAlarmMessage( alarmMessageGet );
				message.GetSequenceInfo( sequenceTimeGet, codeGet );
				message.GetStatusInfo( sendStatusGet, numTrialsGet, timeDistTrialsGet );
				messageTimeGet = message.GetTimestamp();
				typeGet = message.GetType();

				BOOST_REQUIRE( *alarmMessageGet == *alarmMessageSet );
				BOOST_REQUIRE( sequenceTimeGet == sequenceTimeSet );
				BOOST_REQUIRE( codeGet == codeSet );
				BOOST_REQUIRE( sendStatusGet.code == sendStatusSet.code );
				BOOST_REQUIRE( sendStatusGet.text == sendStatusSet.text );
				BOOST_REQUIRE( numTrialsGet == numTrialsSet );
				BOOST_REQUIRE( timeDistTrialsGet == timeDistTrialsSet );
				BOOST_REQUIRE( messageTimeGet == messageTimeSet );
				BOOST_REQUIRE( typeGet == MESSAGE_SUCCESS );

				message.GetMessageContent( sequenceTimeGet2, codeGet2, alarmMessageGet2, sendStatusGet2, numTrialsGet2, timeDistTrialsGet2 );

				BOOST_REQUIRE( *alarmMessageGet2 == *alarmMessageSet );
				BOOST_REQUIRE( sequenceTimeGet2 == sequenceTimeSet );
				BOOST_REQUIRE( codeGet2 == codeSet );
				BOOST_REQUIRE( sendStatusGet2.code == sendStatusSet.code );
				BOOST_REQUIRE( sendStatusGet2.text == sendStatusSet.text );
				BOOST_REQUIRE( numTrialsGet2 == numTrialsSet );
				BOOST_REQUIRE( timeDistTrialsGet2 == timeDistTrialsSet );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/