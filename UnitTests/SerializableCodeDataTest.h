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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "SerializableCodeData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SerializableCodeData		Unit test for the class CSerializableCodeData<T>
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SerializableCodeData
	*/
	namespace SerializableCodeData {
		/**	@brief		Generate test data
		*/
		void GenerateTestData(std::vector<int>& tones, std::vector<float>& toneLengths, std::vector<float>& tonePeriods, std::vector<float>& toneFreqs, std::vector<float>& absToneLevels)
		{
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				absToneLevels.push_back( 1.4e-5f + i * 1e-6f );
			}
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( SerializableCodeData_test_suite, *label("default") );

		/**	@brief		Testing of serialization of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableCodeData_serialization_test_case )
		{
			using namespace std;
			stringstream ss;
			vector<int> tones;
			vector<float> toneLengths, tonePeriods, toneFreqs, absToneLevels;
			Utilities::CSerializableCodeData<float> dataSet, dataGet;

			// generate test data
			GenerateTestData( tones, toneLengths, tonePeriods, toneFreqs, absToneLevels );
			dataSet.Set( tones, toneLengths, tonePeriods, toneFreqs, absToneLevels );

			// serialize
			const Utilities::CSerializableCodeData<float> constDataSet = dataSet; // workaround
			boost::archive::text_oarchive oa( ss ); 
			oa << constDataSet;

			// deserialize
			boost::archive::text_iarchive ia( ss ); 
			ia >> dataGet; 
			
			// check for identity
			BOOST_REQUIRE( dataSet == dataGet );
		}



		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableCodeData_set_get_test_case )
		{
			using namespace std;
			vector<int> tonesSet, tonesGet;
			vector<float> toneLengthsSet, toneLengthsGet, tonePeriodsSet, tonePeriodsGet, toneFreqsSet, toneFreqsGet, absToneLevelsSet, absToneLevelsGet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			
			// check direct getting
			Utilities::CSerializableCodeData<float> data( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
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



		/**	@brief		Testing of constructor from non-serializable class
		*/
		BOOST_AUTO_TEST_CASE( SerializableCodeData_non_serializable_ctor_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			vector<float> toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet;

			// generate test data
			GenerateTestData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );

			// perform test
			Utilities::CCodeData<float> setData( tonesSet, toneLengthsSet, tonePeriodsSet, toneFreqsSet, absToneLevelsSet );
			Utilities::CSerializableCodeData<float> serializableData( setData );
			Utilities::CCodeData<float> getData = serializableData;

			// check for identity
			BOOST_REQUIRE( setData == getData );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
