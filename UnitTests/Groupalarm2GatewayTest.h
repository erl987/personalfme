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

#include <memory>
#include <sstream>
#include <boost/test/unit_test.hpp>
#include "EmailGateway.h"
#include "Groupalarm2Gateway.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace GroupalarmTest {
		namespace Groupalarm2GatewayTest {
			// Test section
			BOOST_AUTO_TEST_SUITE( Groupalarm2Gateway_test_suite, *label("default") );

			/**	@brief		Testing of the groupalarm gateway
			*/
			BOOST_AUTO_TEST_CASE( Groupalarm2Gateway_test_case )
			{
				using namespace std;
				unique_ptr< External::CAlarmGateway> gateway = make_unique<External::Groupalarm::CGroupalarm2Gateway>();

				// this test aims for detecting deadlocks that may occur from a broken implementation
				for ( int i = 0; i < 5; i++ ) {
					auto cloned = gateway->Clone();
				}
			}

			/**	@brief		Testing of the comparison operators
			*/
			BOOST_AUTO_TEST_CASE( comparison_test_case )
			{
				using namespace std;
				using namespace External;

				unique_ptr< External::CAlarmGateway> gateway1 = make_unique<Groupalarm::CGroupalarm2Gateway>();
				unique_ptr< External::CAlarmGateway> gateway2 = make_unique<Email::CEmailGateway>();

				// test valid comparisons
				BOOST_REQUIRE( gateway1 == gateway1 );
				BOOST_REQUIRE( !( gateway1 == gateway2 ) );
				BOOST_REQUIRE( ( gateway1 != gateway2 ) );
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
