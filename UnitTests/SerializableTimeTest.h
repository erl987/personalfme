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
/*@{*/
/** \ingroup UnitTests
*/

#include <sstream>
#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "SerializableTime.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SerializableTime			Unit test for the class CSerializableTime
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SerializableTime
	*/
	namespace SerializableStartTime {
		// Test section
		BOOST_AUTO_TEST_SUITE( SerializableTime_test_suite, *label("default") );

		/**	@brief		Testing of serialization of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableTime_serialization_test_case )
		{
			std::stringstream ss, ss2;
			Utilities::CSerializableTime timeSet, timeGet, emptyTimeSet, emptyTimeGet;

			// generate test data
			timeSet.Set( 11, 07, 20, 205 );
			
			// serialize
			const Utilities::CSerializableTime constTimeSet = timeSet; // workaround
			boost::archive::text_oarchive oa( ss );
			oa << constTimeSet;

			// deserialize
			boost::archive::text_iarchive ia( ss );
			ia >> timeGet;

			// check for identity
			BOOST_REQUIRE( timeSet == timeGet );

			// empty serialization
			const Utilities::CSerializableTime constEmptyTimeSet = emptyTimeSet; // workaround
			boost::archive::text_oarchive oa2( ss2 );
			oa2 << constEmptyTimeSet;

			// deserialize
			boost::archive::text_iarchive ia2( ss2 );
			ia2 >> emptyTimeGet;

			// check for identity
			BOOST_REQUIRE( emptyTimeSet == emptyTimeGet );
		}


		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableTime_set_get_test_case )
		{
			int hourGet, minuteGet, secondGet, millisecGet;
			int hourSet = 15;
			int minuteSet = 9;
			int secondSet = 48;
			int millisecSet = 123;

			// check direct getting
			Utilities::CSerializableTime time( hourSet, minuteSet, secondSet, millisecSet );
			time.Get( hourGet, minuteGet, secondGet, millisecGet );

			// check for identity
			BOOST_REQUIRE( time.IsValid() );
			BOOST_REQUIRE( hourSet == hourGet );
			BOOST_REQUIRE( minuteSet == minuteGet );
			BOOST_REQUIRE( secondSet == secondGet );
			BOOST_REQUIRE( millisecSet == millisecGet );
			
			// check indirect getting
			time.Set( hourSet, minuteSet, secondSet, millisecSet );

			// check for identity
			BOOST_REQUIRE( time.IsValid() );
			BOOST_REQUIRE( hourSet == hourGet );
			BOOST_REQUIRE( minuteSet == minuteGet );
			BOOST_REQUIRE( secondSet == secondGet );
			BOOST_REQUIRE( millisecSet == millisecGet );
		}



		/**	@brief		Testing of constructor from non-serializable class
		*/
		BOOST_AUTO_TEST_CASE( SerializableTime_non_serializable_ctor_test_case )
		{
			using namespace Utilities;

			CTime timeSet, timeGet, timeSet2, timeGet2;

			// generate test data for construction (wich is an assignment)
			timeSet.Set( 11, 07, 20, 205 );
			CSerializableTime serializableTime = timeSet;
			timeGet = serializableTime;
			BOOST_REQUIRE( timeSet == timeGet );

			// check with invalid time
			CSerializableTime serializableTime2 = timeSet2;
			timeGet2 = serializableTime2;
			BOOST_REQUIRE( !timeGet2.IsValid() );

			// check assignment
			serializableTime2 = timeSet;
			BOOST_REQUIRE( serializableTime2 == timeSet );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
