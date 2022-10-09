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
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include "SerializableSeqDataComplete.h"

using boost::unit_test::label;


/**	\defgroup	Utility				Unit tests for the Utility-library
*/

/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SerializableSeqDataComplete		Unit test for the class CSerializableSeqDataComplete<T>
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SerializableSeqDataComplete
	*/
	namespace SerializableSeqDataComplete {
		// Test section
		BOOST_AUTO_TEST_SUITE( SerializableSeqDataComplete_test_suite, *label("basic") );

		/**	@brief		Testing of serialization of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableSeqDataComplete_serialization_test_case )
		{
			using namespace std;
			stringstream ss;
			vector<int> tones;
			vector<float> toneLengths, tonePeriods, toneFreqs, absToneLevels;
			Utilities::CSerializableSeqDataComplete<float> dataSet, dataGet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				absToneLevels.push_back( 1.9e-5f + i * 1.0e-6f );
			}
			dataSet.Set( Utilities::CDateTime( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) ), Utilities::CCodeData<float>( tones, toneLengths, tonePeriods, toneFreqs, absToneLevels ), "Test string." );

			// serialize
			const Utilities::CSerializableSeqDataComplete<float> constDataSet = dataSet; // workaround
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
		BOOST_AUTO_TEST_CASE( SerializableSeqDataComplete_set_get_test_case )
		{
			using namespace std;
			vector<int> tones;
			vector<float> toneLengths, tonePeriods, toneFreqs, absToneLevels;
			std::string infoStringSet, infoStringGet;
			Utilities::CDateTime startTimeGet, startTimeSet;
			Utilities::CCodeData<float> codeDataGet, codeDataSet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				absToneLevels.push_back( 1.9e-5f + i * 1.0e-6f );
			}
			codeDataSet.Set( tones, toneLengths, tonePeriods, toneFreqs, absToneLevels );
			startTimeSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			infoStringSet = "Test string.";

			// check direct getting
			Utilities::CSerializableSeqDataComplete<float> data( startTimeSet, codeDataSet, infoStringSet );
			data.Get( startTimeGet, codeDataGet, infoStringGet );

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( codeDataSet == codeDataGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );

			// check indirect getting
			data.Set( startTimeSet, codeDataSet, infoStringSet );

			// check for identity
			BOOST_REQUIRE( data.GetStartTime() == startTimeSet );
			BOOST_REQUIRE( data.GetCodeData() == codeDataSet );
			BOOST_REQUIRE( data.GetInfoString() == infoStringSet );
		}



		/**	@brief		Testing of constructor from non-serializable class
		*/
		BOOST_AUTO_TEST_CASE( SerializableSeqDataComplete_non_serializable_ctor_test_case )
		{
			using namespace std;
			vector<int> tones;
			vector<float> toneLengths, tonePeriods, toneFreqs, absToneLevels;

			// generate test data
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
				toneLengths.push_back( 0.07f );
				tonePeriods.push_back( 0.07f );
				toneFreqs.push_back( 1300.0f + i );
				absToneLevels.push_back( 1.9e-5f + i * 1.0e-6f );
			}

			// check direct getting
			Utilities::CSeqDataComplete<float> dataSet( Utilities::CDateTime( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) ), Utilities::CCodeData<float>( tones, toneLengths, tonePeriods, toneFreqs, absToneLevels ), "Test string." );
			Utilities::CSerializableSeqDataComplete<float> serializableData( dataSet );
			Utilities::CSeqDataComplete<float> dataGet = serializableData;

			// check for identity
			BOOST_REQUIRE( dataSet == dataGet );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
