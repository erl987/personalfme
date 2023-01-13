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
#include "DateTime.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	DateTime			Unit test for the class CDateTime
*/
namespace Utilitites {
	/*@{*/
	/** \ingroup DateTime
	*/
	namespace DateTime {
		const int daySet = 10;
		const int monthSet = 6;
		const int yearSet = 2012;
		const Utilities::CTime timeOfDaySet( 11, 7, 20, 205 );

		// Test section
		BOOST_AUTO_TEST_SUITE( DateTime_test_suite, *label("default") );

		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( DateTime_set_get_test_case )
		{
			int dayGet, monthGet, yearGet;
			Utilities::CTime timeOfDayGet;

			// check direct getting
			Utilities::CDateTime data( daySet, monthSet, yearSet, timeOfDaySet );
			data.Get( dayGet, monthGet, yearGet, timeOfDayGet );
			BOOST_REQUIRE( data.IsValid() );

			// check for identity
			BOOST_REQUIRE( dayGet == daySet );
			BOOST_REQUIRE( monthGet == monthSet );
			BOOST_REQUIRE( yearGet == yearSet );
			BOOST_REQUIRE( timeOfDayGet == timeOfDaySet );

			// check indirect getting
			data.Set( daySet, monthSet, yearSet, timeOfDaySet );
			BOOST_REQUIRE( data.IsValid() );
			BOOST_REQUIRE( data.Day() == daySet );
			BOOST_REQUIRE( data.Month() == monthSet );
			BOOST_REQUIRE( data.Year() == yearSet );
			BOOST_REQUIRE( data.TimeOfDay() == timeOfDaySet );

			// check usage for dates
			Utilities::CDateTime data2( daySet, monthSet, yearSet );
			BOOST_CHECK_NO_THROW( data2.Get( dayGet, monthGet, yearGet, timeOfDayGet ) );
			BOOST_CHECK_NO_THROW( data2.TimeOfDay() );
			BOOST_REQUIRE( timeOfDayGet == Utilities::CTime( 0, 0, 0 ) );
		}


		/**	@brief		Testing of the behaviour with invalid inputs
		*/
		BOOST_AUTO_TEST_CASE( DateTime_error_test_case )
		{
			BOOST_CHECK_THROW( Utilities::CDateTime data( 0, monthSet, yearSet, timeOfDaySet ), std::out_of_range );
			BOOST_CHECK_THROW( Utilities::CDateTime data( 29, 2, 2015, timeOfDaySet ), std::out_of_range );
			BOOST_CHECK_THROW( Utilities::CDateTime data( 29, 13, 2015, timeOfDaySet ), std::out_of_range );
			BOOST_CHECK_THROW( Utilities::CDateTime data( 29, 11, -10, timeOfDaySet ), std::out_of_range );

			BOOST_CHECK_THROW( Utilities::CDateTime( daySet, monthSet, yearSet, Utilities::CTime() ), std::out_of_range );
		}


		/**	@brief		Testing of invalid times
		*/
		BOOST_AUTO_TEST_CASE( DateTime_invalidate_test_case )
		{
			Utilities::CDateTime data( daySet, monthSet, yearSet, timeOfDaySet );
			data.Invalidate();
			BOOST_REQUIRE( !data.IsValid() );
			BOOST_CHECK_THROW( data.Year(), std::runtime_error );

			data.Set( daySet, monthSet, yearSet, timeOfDaySet );
			BOOST_REQUIRE( data.IsValid() );
		}


		/**	@brief		Testing of equality comparison operators
		*/
		BOOST_AUTO_TEST_CASE( DateTime_equality_comparison_test_case )
		{
			using namespace Utilities;

			CDateTime data1( daySet, monthSet, yearSet, timeOfDaySet );
			CDateTime data2( daySet, monthSet + 1, yearSet, timeOfDaySet );

			BOOST_REQUIRE( data1 == data1 );
			BOOST_REQUIRE( !( data1 == data2 ) );
			BOOST_REQUIRE( data1 != data2 );			
			BOOST_REQUIRE( !( data1 == data2 ) );

			BOOST_REQUIRE( CDateTime() == CDateTime() );
			BOOST_REQUIRE( data1 != CDateTime() );
		}


		/**	@brief		Testing of inequality comparison operators
		*/
		BOOST_AUTO_TEST_CASE( DateTime_inequality_comparison_test_case )
		{
			using namespace Utilities;

			CDateTime data1( daySet, monthSet, yearSet, timeOfDaySet );
			CDateTime data2( daySet, monthSet + 1, yearSet, timeOfDaySet );

			BOOST_REQUIRE( data2 > data1 );
			BOOST_REQUIRE( data2 >= data1 );
			BOOST_REQUIRE( data1 < data2 );
			BOOST_REQUIRE( data1 <= data2 );

			BOOST_REQUIRE( data1 >= data1 );
			BOOST_REQUIRE( data2 <= data2 );

			BOOST_REQUIRE( !( data1 > data2 ) );
			BOOST_REQUIRE( !( data1 >= data2 ) );
			BOOST_REQUIRE( !( data2 < data1 ) );
			BOOST_REQUIRE( !( data2 <= data1 ) );

			BOOST_REQUIRE( !( CDateTime() > CDateTime() ) );
			BOOST_REQUIRE( !( CDateTime() < CDateTime() ) );
			BOOST_REQUIRE( !( CDateTime() >= CDateTime() ) );
			BOOST_REQUIRE( !( CDateTime() <= CDateTime() ) );

			BOOST_REQUIRE( !( data1 > CDateTime() ) );
			BOOST_REQUIRE( !( data1 < CDateTime() ) );
			BOOST_REQUIRE( !( data1 >= CDateTime() ) );
			BOOST_REQUIRE( !( data1 <= CDateTime() ) );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/