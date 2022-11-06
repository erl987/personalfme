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
#include "CodeData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	CodeData		Unit test for the class CCodeData<T>
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup CodeData
	*/
	namespace CodeData {
		/**	@brief		Generate test data
		*/
		void GenerateTestData(std::vector<int>& tones, std::vector<float>& toneLengths, std::vector<float>& tonePeriods, std::vector<float>& toneFreqs, std::vector<float>& absToneLevels )
		{
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				absToneLevels.push_back( 0.95f + i / 10 );
			}
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( CodeData_test_suite, *label("basic") );

		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( CodeData_set_get_test_case )
		{
			using namespace std;
			vector<int> tonesSet, tonesGet;
			vector<float> toneLengthsSet, toneLengthsGet, tonePeriodsSet, tonePeriodsGet, toneFreqsSet, toneFreqsGet, absToneLevelsSet, absToneLevelsGet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check direct getting
			Utilities::CCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			data.Get( tonesGet, toneLengthsGet, tonePeriodsGet, toneFreqsGet, absToneLevelsGet );

			// check for identity
			BOOST_REQUIRE( tonesGet == tonesSet );
			BOOST_REQUIRE( toneLengthsGet == toneLengthsSet );
			BOOST_REQUIRE( tonePeriodsGet == tonePeriodsSet );
			BOOST_REQUIRE( toneFreqsGet == toneFreqsSet );
			BOOST_REQUIRE( absToneLevelsGet == absToneLevelsSet );

			// check indirect getting
			data.Set( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );

			// check for identity
			BOOST_REQUIRE( data.GetTones() == tonesSet );
			BOOST_REQUIRE( data.GetToneLengths() == toneLengthsSet );
			BOOST_REQUIRE( data.GetTonePeriods() == tonePeriodsSet );
			BOOST_REQUIRE( data.GetToneFrequencies() == toneFreqsSet );
			BOOST_REQUIRE( data.GetAbsToneLevels() == absToneLevelsSet );
		}



		/**	@brief		Testing of GetLength function of the class
		*/
		BOOST_AUTO_TEST_CASE( CodeData_GetLength_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			vector<float> toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check
			Utilities::CCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			BOOST_REQUIRE( data.GetLength() == tonesSet.size() );
		}



		/**	@brief		Testing of AddOneTone function of the class
		*/
		BOOST_AUTO_TEST_CASE( CodeData_AddOneTone_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			vector<float> toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check
			Utilities::CCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			tonesSet.push_back( 8 );
			toneLengthsSet.push_back( 0.05f );
			tonePeriodsSet.push_back( 0.05f );
			toneFreqsSet.push_back( 1200.0f );
			absToneLevelsSet.push_back( 0.93f );
			data.AddOneTone( tonesSet.back(), toneLengthsSet.back(), tonePeriodsSet.back(), toneFreqsSet.back(), absToneLevelsSet.back() );
			BOOST_REQUIRE( data.GetTones() == tonesSet );
			BOOST_REQUIRE( data.GetToneLengths() == toneLengthsSet );
			BOOST_REQUIRE( data.GetTonePeriods() == tonePeriodsSet );
			BOOST_REQUIRE( data.GetToneFrequencies() == toneFreqsSet );
			BOOST_REQUIRE( data.GetAbsToneLevels() == absToneLevelsSet );
		}



		/**	@brief		Testing of SetTones function of the class
		*/
		BOOST_AUTO_TEST_CASE( CodeData_SetTones_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			vector<float> toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check
			Utilities::CCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			tonesSet.assign( 5, 1 );
			data.SetTones( tonesSet );
			BOOST_REQUIRE( data.GetTones() == tonesSet );	
		}



		/**	@brief		Testing of Clear function of the class
		*/
		BOOST_AUTO_TEST_CASE( CodeData_Clear_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			vector<float> toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check
			Utilities::CCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			data.Clear();
			BOOST_REQUIRE( data.GetLength() == 0 );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/