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

#include <memory>
#include <boost/test/unit_test.hpp>
#include "DefaultValidity.h"
#include "SingleTimeValidity.h"



/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ValiditiesTest {
		namespace DefaultValidityTest {
			// Test section
			BOOST_AUTO_TEST_SUITE( DefaultValidity_test_suite );

			/**	@brief		Testing of the default validity exception
			*/
			BOOST_AUTO_TEST_CASE( DefaultValidity_test_case )
			{
				using namespace std;
				using namespace External;
				shared_ptr< Validities::CValidity > validity;

				// test default construction
				validity = Validities::DEFAULT_VALIDITY;
				BOOST_REQUIRE( validity->IsValid() );
				validity->Invalidate();
				BOOST_REQUIRE( validity->IsValid() );
				BOOST_REQUIRE( validity->GetValidityPeriods( 4, 2013 ).empty() );	// any date must deliver an empty set for the default validity
				BOOST_REQUIRE( validity->GetValidityPeriods( 4, 50 ).empty() );		// any date must deliver an empty set for the default validity
			}



			/**	@brief		Testing of the default validity exception comparisons
			*/
			BOOST_AUTO_TEST_CASE( DefaultValidity_comparison_test_case )
			{
				using namespace std;
				using namespace Utilities;
				using namespace External;
				using namespace External::Validities;
				shared_ptr< Validities::CValidity > validity1, validity2, validity3;

				validity1 = DEFAULT_VALIDITY;
				validity2 = DEFAULT_VALIDITY;
				validity3 = make_shared< CSingleTimeValidity >( CSingleTimeValidity( CDateTime( 5, 6, 2013, CTime( 20, 15, 30 ) ), CDateTime( 5, 6, 2013, CTime( 20, 35, 30 ) ) ) );

				// test valid comparisons
				BOOST_REQUIRE( *validity1 == *validity2 );
				BOOST_REQUIRE( !( *validity1 != *validity2 ) );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( *validity1 == *validity3 ) );
			}
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
