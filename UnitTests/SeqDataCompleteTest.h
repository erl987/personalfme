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
/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "SeqDataComplete.h"

using boost::unit_test::label;


/**	\defgroup	Utility				Unit tests for the Utility-library
*/

/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SeqDataComplete		Unit test for the class CCSeqDataComplete<T>
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SeqDataComplete
	*/
	namespace SeqDataComplete {
		// Test section
		BOOST_AUTO_TEST_SUITE( SeqDataComplete_test_suite, *label("basic") );

		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( SeqDataComplete_set_get_test_case )
		{
			using namespace std;
			string infoStringSet, infoStringGet;
			vector<int> tones;
			vector<float> toneLengths, tonePeriods, toneFreqs, toneLevels;
			Utilities::CDateTime startTimeGet, startTimeSet;
			Utilities::CCodeData<float> codeGet, codeSet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				toneLevels.push_back( 1.2e-5f);
			}
			startTimeSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			codeSet.Set( tones, toneLengths, tonePeriods, toneFreqs, toneLevels );
			infoStringSet = "info string";
			
			// check direct getting
			Utilities::CSeqDataComplete<float> data( startTimeSet, codeSet, infoStringSet );
			data.Get( startTimeGet, codeGet, infoStringGet );

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( codeSet == codeGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );

			// check indirect getting
			data.Set( startTimeSet, codeSet, infoStringSet );
			startTimeGet = data.GetStartTime();
			codeGet = data.GetCodeData();
			infoStringGet = data.GetInfoString();

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( codeSet == codeGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/