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
#include "SeqData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SeqData			Unit test for the class CSeqData
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SeqData
	*/
	namespace SeqData {
		// Test section
		BOOST_AUTO_TEST_SUITE( SeqData_test_suite, *label("basic") );

		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( SeqData_set_get_test_case )
		{
			using namespace std;
			vector<int> tonesSet, tonesGet;
			string infoStringSet, infoStringGet;
			Utilities::CDateTime startTimeGet, startTimeSet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tonesSet.push_back( i );
			}
			startTimeSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			infoStringSet = "Test string.";
			
			// check direct getting
			Utilities::CSeqData data( startTimeSet, tonesSet, infoStringSet );
			data.Get( startTimeGet, tonesGet, infoStringGet );

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( tonesSet == tonesGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );

			// check indirect getting
			data.Set( startTimeSet, tonesSet, infoStringSet );
			startTimeGet = data.GetStartTime();
			tonesGet = data.GetCode();
			infoStringGet = data.GetInfoString();

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( tonesSet == tonesGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/