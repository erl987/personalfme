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
#include "GeneralStatusMessage.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	StatusMessage		Unit test for a status message
*/
namespace Utilitites {
	/*@{*/
	/** \ingroup StatusMessage
	*/
	namespace Messages {
		namespace GeneralStatusMessage {
			// Test section
			BOOST_AUTO_TEST_SUITE( GeneralStatusMessage_test_suite, *label("basic") );

			/**	@brief		Testing of setting and getting functions of the class
			*/
			BOOST_AUTO_TEST_CASE( GeneralStatusMessage_set_get_test_case )
			{
				using namespace boost::posix_time;
				using namespace Utilities::Message;

				ptime timeSet, timeGet;
				std::string textSet, textGet;
				MessageType typeSet, typeGet;

				textSet = "test";
				typeSet = MESSAGE_SUCCESS;

				timeSet = ptime( microsec_clock::universal_time() );

				CGeneralStatusMessage message( typeSet, timeSet, textSet );
				message.GetMessageText( textGet );
				timeGet = message.GetTimestamp();
				typeGet = message.GetType();

				BOOST_REQUIRE( textGet == textSet );
				BOOST_REQUIRE( timeGet == timeSet );
				BOOST_REQUIRE( typeGet == typeSet );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/