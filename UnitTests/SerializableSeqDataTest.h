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

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include "SerializableSeqData.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	SerializableSeqData			Unit test for the class CSerializableSeqData
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup SerializableSeqData
	*/
	namespace SerializableSeqData {
		// Test section
		BOOST_AUTO_TEST_SUITE( SerializableSeqData_test_suite, *label("default") );

		/**	@brief		Testing of serialization of the class
		*/
		BOOST_AUTO_TEST_CASE( SerializableSeqData_serialization_test_case )
		{
			using namespace std;
			stringstream ss;
			vector<int> tones;
			Utilities::CSerializableSeqData dataSet, dataGet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tones.push_back( i );
			}
			dataSet.Set( Utilities::CDateTime( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) ), tones, "Test string." );

			// serialize
			const Utilities::CSerializableSeqData constDataSet = dataSet; // workaround
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
		BOOST_AUTO_TEST_CASE( SerializableSeqData_set_get_test_case )
		{
			using namespace std;
			vector<int> tonesSet, tonesGet;
			string infoStringSet, infoStringGet;
			Utilities::CDateTime startTimeGet, startTimeSet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tonesSet.push_back( i );
			}
			infoStringSet = "Test string.";
			startTimeSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			
			// check direct getting
			Utilities::CSerializableSeqData data( startTimeSet, tonesSet, infoStringSet );
			data.Get( startTimeGet, tonesGet, infoStringGet );

			// check for identity
			BOOST_REQUIRE( startTimeSet == startTimeGet );
			BOOST_REQUIRE( tonesSet == tonesGet );
			BOOST_REQUIRE( infoStringSet == infoStringGet );

			// check indirect getting
			data.Set( startTimeSet, tonesSet, infoStringSet );

			// check for identity
			BOOST_REQUIRE( data.GetStartTime() == startTimeSet );
			BOOST_REQUIRE( data.GetCode() == tonesSet );
			BOOST_REQUIRE( data.GetInfoString() == infoStringSet );
		}



		/**	@brief		Testing of constructor from non-serializable class
		*/
		BOOST_AUTO_TEST_CASE( SerializableSeqData_non_serializable_ctor_test_case )
		{
			using namespace std;
			vector<int> tonesSet;
			string infoStringSet;
			Utilities::CDateTime startTimeSet;

			// generate test data
			for (int i=0; i < 5; i++) {
				tonesSet.push_back( i );
			}
			infoStringSet = "Test string.";
			startTimeSet.Set( 10, 6, 2012, Utilities::CTime( 11, 07, 20, 205 ) );
			
			// check direct getting
			Utilities::CSeqData setData( startTimeSet, tonesSet, infoStringSet );
			Utilities::CSerializableSeqData serializableData( setData );
			Utilities::CSeqData getData = serializableData;

			// check for identity
			BOOST_REQUIRE( setData == getData );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
