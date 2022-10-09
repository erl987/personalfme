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
/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "DetectorStatusMessage.h"

using boost:unit_test::label;


/*@{*/
/** \ingroup Utility
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup StatusMessage
	*/
	namespace Messages {
		namespace DetectorStatusMessage {
			// Test section
			BOOST_AUTO_TEST_SUITE( DetectorStatusMessage_test_suite, *label("basic") );

			/**	@brief		Testing of setting and getting functions of the class
			*/
			BOOST_AUTO_TEST_CASE( DetectorStatusMessage_set_get_test_case )
			{
				using namespace boost::posix_time;
				using namespace Utilities::Message;

				ptime timeSet, timeGet;
				int samplingFreqSet, samplingFreqGet;
				DetectorStatusCode statusSet, statusGet;
				MessageType typeGet;

				statusSet = RUNNING;
				samplingFreqSet = 96000;

				timeSet = ptime( microsec_clock::universal_time() );

				CDetectorStatusMessage message( timeSet, statusSet, samplingFreqSet );
				message.GetMessageContent( statusGet, samplingFreqGet );
				timeGet = message.GetTimestamp();
				typeGet = message.GetType();

				BOOST_REQUIRE( statusSet == statusGet );
				BOOST_REQUIRE( timeGet == timeSet );
				BOOST_REQUIRE( typeGet == MESSAGE_SUCCESS );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/