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
#include "CTime.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	Time			Unit test for the class CTime
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup Time
	*/
	namespace Time {
		const int hourSet1 = 19;
		const int minuteSet1 = 21;
		const int secondSet1 = 51;
		const int millisecSet1 = 951;

		// Test section
		BOOST_AUTO_TEST_SUITE( Time_test_suite, *label("basic") );

		/**	@brief		Testing of setting and getting functions of the class
		*/
		BOOST_AUTO_TEST_CASE( Time_set_get_test_case )
		{
			int hourGet, minuteGet, secondGet, millisecGet;
			Utilities::CTime timeSet1, timeSet2;

			// constructor test
			BOOST_REQUIRE( !timeSet1.IsValid() );
			timeSet1 = Utilities::CTime( hourSet1, minuteSet1, secondSet1, millisecSet1 );
			BOOST_REQUIRE( timeSet1.IsValid() );

			timeSet1.Get( hourGet, minuteGet, secondGet, millisecGet );
			BOOST_REQUIRE( hourGet == hourSet1 );
			BOOST_REQUIRE( minuteGet == minuteSet1 );
			BOOST_REQUIRE( secondGet == secondSet1 );
			BOOST_REQUIRE( millisecGet == millisecSet1 );

			BOOST_REQUIRE( timeSet1.Hour() == hourSet1 );
			BOOST_REQUIRE( timeSet1.Minute() == minuteSet1 );
			BOOST_REQUIRE( timeSet1.Second() == secondSet1 );
			BOOST_REQUIRE( timeSet1.Millisec() == millisecSet1 );

			// invalidate test
			timeSet1.Invalidate();
			BOOST_REQUIRE( !timeSet1.IsValid() );
			BOOST_CHECK_THROW( timeSet1.Hour(), std::runtime_error );

			// set test
			timeSet2.Set( hourSet1, minuteSet1, secondSet1, millisecSet1 );
			BOOST_REQUIRE( timeSet2.Hour() == hourSet1 );
			BOOST_REQUIRE( timeSet2.Minute() == minuteSet1 );
			BOOST_REQUIRE( timeSet2.Second() == secondSet1 );
			BOOST_REQUIRE( timeSet2.Millisec() == millisecSet1 );
		}



		/**	@brief		Testing of invalid times
		*/
		BOOST_AUTO_TEST_CASE( error_test_case )
		{
			Utilities::CTime timeSet;

			BOOST_CHECK_THROW( timeSet.Set( 24, minuteSet1, secondSet1, millisecSet1 ), std::out_of_range );
			BOOST_CHECK_THROW( timeSet.Set( hourSet1, 60, secondSet1, millisecSet1 ), std::out_of_range );
			BOOST_CHECK_THROW( timeSet.Set( hourSet1, minuteSet1, 60, millisecSet1 ), std::out_of_range );
			BOOST_CHECK_THROW( timeSet.Set( hourSet1, minuteSet1, secondSet1, 1000 ), std::out_of_range );
		}



		/**	@brief		Testing of equality comparison operators
		*/
		BOOST_AUTO_TEST_CASE( Time_equality_comparison_test_case )
		{
			using namespace Utilities;

			CTime time1( hourSet1, minuteSet1, secondSet1, millisecSet1 );
			CTime time2( hourSet1 + 2, minuteSet1, secondSet1, millisecSet1 );

			BOOST_REQUIRE( time1 == time1 );
			BOOST_REQUIRE( !( time1 == time2 ) );
			BOOST_REQUIRE( time1 != time2 );			
			BOOST_REQUIRE( !( time1 == time2 ) );

			BOOST_REQUIRE( CTime() == CTime() );
			BOOST_REQUIRE( time1 != CTime() );
		}


		/**	@brief		Testing of inequality comparison operators
		*/
		BOOST_AUTO_TEST_CASE( Time_inequality_comparison_test_case )
		{
			using namespace Utilities;

			CTime time1( hourSet1, minuteSet1, secondSet1, millisecSet1 );
			CTime time2( hourSet1 + 2, minuteSet1, secondSet1, millisecSet1 );

			BOOST_REQUIRE( time2 > time1 );
			BOOST_REQUIRE( time2 >= time1 );
			BOOST_REQUIRE( time1 < time2 );
			BOOST_REQUIRE( time1 <= time2 );

			BOOST_REQUIRE( time1 >= time1 );
			BOOST_REQUIRE( time2 <= time2 );

			BOOST_REQUIRE( !( time1 > time2 ) );
			BOOST_REQUIRE( !( time1 >= time2 ) );
			BOOST_REQUIRE( !( time2 < time1 ) );
			BOOST_REQUIRE( !( time2 <= time1 ) );

			BOOST_REQUIRE( !( CTime() > CTime() ) );
			BOOST_REQUIRE( !( CTime() < CTime() ) );
			BOOST_REQUIRE( !( CTime() >= CTime() ) );
			BOOST_REQUIRE( !( CTime() <= CTime() ) );
			
			BOOST_REQUIRE( !( time1 > CTime() ) );
			BOOST_REQUIRE( !( time1 < CTime() ) );
			BOOST_REQUIRE( !( time1 >= CTime() ) );
			BOOST_REQUIRE( !( time1 <= CTime() ) );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/