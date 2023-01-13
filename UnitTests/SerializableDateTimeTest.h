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
#include "SerializableDateTime.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SerializableDateTime			Unit test for the class CSerializableDateTime
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SerializableDateTime
	*/
	namespace SerializableDateTime {
		// Test section
		BOOST_AUTO_TEST_SUITE( SerializableDateTime_test_suite, *label("default") );

		/**	@brief		Testing of serialization of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableDateTime_serialization_test_case )
		{
			using namespace std;
			stringstream ss, ss2;
			Utilities::CSerializableDateTime dataSet, dataGet, emptyDataSet, emptyDataGet;

			// generate test data
			dataSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );

			// serialize
			const Utilities::CSerializableDateTime constDataSet = dataSet; // workaround
			boost::archive::text_oarchive oa( ss ); 
			oa << constDataSet;

			// deserialize
			boost::archive::text_iarchive ia( ss ); 
			ia >> dataGet; 
			
			// check for identity
			BOOST_REQUIRE( dataSet == dataGet );

			// empty serialization
			const Utilities::CSerializableDateTime constEmptyDataSet = emptyDataSet; // workaround
			boost::archive::text_oarchive oa2( ss2 ); 
			oa2 << constEmptyDataSet;

			// deserialize
			boost::archive::text_iarchive ia2( ss2 ); 
			ia2 >> emptyDataGet; 
			
			// check for identity
			BOOST_REQUIRE( emptyDataSet == emptyDataGet );
		}


		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableDateTime_set_get_test_case )
		{
			int dayGet, monthGet, yearGet;
			Utilities::CTime timeOfDayGet;
			int daySet = 10;
			int monthSet = 6;
			int yearSet = 2012;
			Utilities::CTime timeOfDaySet( 11, 07, 20, 205 );

			// check direct getting
			Utilities::CSerializableDateTime time( daySet, monthSet, yearSet, timeOfDaySet );
			time.Get( dayGet, monthGet, yearGet, timeOfDayGet );
			
			// check for identity
			BOOST_REQUIRE( time.IsValid() );
			BOOST_REQUIRE( daySet == dayGet );
			BOOST_REQUIRE( monthSet == monthGet );
			BOOST_REQUIRE( yearSet == yearGet );
			BOOST_REQUIRE( timeOfDaySet == timeOfDayGet );

			// check indirect getting
			time.Set( daySet, monthSet, yearSet, timeOfDaySet );

			// check for identity
			BOOST_REQUIRE( time.IsValid() );
			BOOST_REQUIRE( time.Day() == daySet );
			BOOST_REQUIRE( time.Month() == monthSet );
			BOOST_REQUIRE( time.TimeOfDay() == timeOfDaySet );
		}



		/**	@brief		Testing of constructor from non-serializable class
		*/
		BOOST_AUTO_TEST_CASE( SerializableDateTime_non_serializable_ctor_test_case )
		{
			// generate test data
			Utilities::CDateTime timeSet( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			Utilities::CSerializableDateTime serializableTime = timeSet;
			Utilities::CDateTime timeGet = serializableTime;

			// check for identity
			BOOST_REQUIRE( timeSet == timeGet );

			// check with invalid time
			Utilities::CDateTime timeSet2;
			Utilities::CSerializableDateTime serializableTime2 = timeSet2;
			Utilities::CDateTime timeGet2 = serializableTime2;

			// check for identity
			BOOST_REQUIRE( !timeGet2.IsValid() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
