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
#include <boost/date_time/posix_time/posix_time.hpp>
#include "german_local_date_time.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	GermanLocalDateTime		Unit test for the class german_local_date_time
*/

namespace Utilitites {
	namespace Time {
		using namespace std;
		using namespace boost::posix_time;
		using namespace boost::gregorian;

		/*@{*/
		/** \ingroup GermanLocalDateTime
		*/
		namespace GermanLocalDateTime {
			// Test section
			BOOST_AUTO_TEST_SUITE( GermanLocalDateTime_test_suite, *label("basic") );

			/**	@brief		Testing of formatted time output for first wintertime of the year
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_first_wintertime_test_case )
			{
				ostringstream ss;

				ptime testTime( date( 2015, Jan, 10 ), hours( 1 ) + minutes( 9 ) + seconds( 5 ) );		// UTC
				Utilities::Time::german_local_date_time timeFormatter( testTime );

				// test output
				ss << timeFormatter;
				BOOST_REQUIRE( ss.str() == "Samstag, 10.01.2015 02:09:05" );
			}



			/**	@brief		Testing of formatted time output for second wintertime of the year
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_second_wintertime_test_case )
			{
				ostringstream ss;

				ptime testTime( date( 2015, Nov, 19 ), hours( 11 ) + minutes( 3 ) + seconds( 41 ) );	// UTC
				Utilities::Time::german_local_date_time timeFormatter( testTime );

				// test output
				ss << timeFormatter;
				BOOST_REQUIRE( ss.str() == "Donnerstag, 19.11.2015 12:03:41" );
			}



			/**	@brief		Testing of formatted time output for daylight saving time of the year
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_daylight_saving_test_case )
			{
				ostringstream ss;

				ptime testTime( date( 2014, Jul, 15 ), hours( 18 ) + minutes( 31 ) + seconds( 2 ) );	// UTC
				Utilities::Time::german_local_date_time timeFormatter( testTime );

				// test output
				ss << timeFormatter;
				BOOST_REQUIRE( ss.str() == "Dienstag, 15.07.2014 20:31:02" );
			}



			/**	@brief		Testing of formatted time output for first time-shift of the year
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_first_time_shift_test_case )
			{
				ostringstream ss1, ss2;

				// before time-shift
				ptime testTime1( date( 2012, Mar, 25 ), hours( 0 ) + minutes( 5 ) + seconds( 21 ) );	// UTC
				Utilities::Time::german_local_date_time timeFormatter1( testTime1 );
				ss1 << timeFormatter1;
				BOOST_REQUIRE( ss1.str() == "Sonntag, 25.03.2012 01:05:21" );

				// after time-shift
				ptime testTime2( date( 2012, Mar, 25 ), hours( 1 ) + minutes( 5 ) + seconds( 21 ) );	// UTC
				Utilities::Time::german_local_date_time timeFormatter2( testTime2 );
				ss2 << timeFormatter2;
				BOOST_REQUIRE( ss2.str() == "Sonntag, 25.03.2012 03:05:21" );
			}



			/**	@brief		Testing of formatted time output for second time-shift of the year
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_second_time_shift_test_case )
			{
				ostringstream ss1, ss2;

				// before time-shift
				ptime testTime1( date( 2016, Oct, 30 ), hours( 0 ) + minutes( 4 ) + seconds( 5 ) );		// UTC
				Utilities::Time::german_local_date_time timeFormatter1( testTime1 );
				ss1 << timeFormatter1;
				BOOST_REQUIRE( ss1.str() == "Sonntag, 30.10.2016 02:04:05 A" );

				// after time-shift
				ptime testTime2( date( 2016, Oct, 30 ), hours( 1 ) + minutes( 4 ) + seconds( 5 ) );		// UTC
				Utilities::Time::german_local_date_time timeFormatter2( testTime2 );
				ss2 << timeFormatter2;
				BOOST_REQUIRE( ss2.str() == "Sonntag, 30.10.2016 02:04:05 B" );
			}



			/**	@brief		Testing of construction from UTC-time
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_test_utc_constructor_test_case )
			{
				ptime testTime1( date( 2016, Oct, 30 ), hours( 0 ) + minutes( 4 ) + seconds( 5 ) );		// UTC
				Utilities::Time::german_local_date_time germanTime1( testTime1 );
				BOOST_REQUIRE( germanTime1.utc_time() == testTime1 );
				BOOST_REQUIRE( germanTime1.local_time() == ( testTime1 + hours(2) ) );

				ptime testTime2( date( 2020, Feb, 29 ), hours( 13 ) + minutes( 2 ) + seconds( 51 ) );	// UTC
				Utilities::Time::german_local_date_time germanTime2( testTime2 );
				BOOST_REQUIRE( germanTime2.utc_time() == testTime2 );
				BOOST_REQUIRE( germanTime2.local_time() == ( testTime2 + hours(1) ) );
			}



			/**	@brief		Testing of construction from local time
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_test_local_time_constructor_test_case )
			{
				date testDate1( 2016, Oct, 30 );
				time_duration localTime1 = hours( 2 ) + minutes( 4 ) + seconds( 5 );
				Utilities::Time::german_local_date_time germanTime1( testDate1, localTime1 );
				BOOST_REQUIRE( germanTime1.date() == testDate1 );
				BOOST_REQUIRE( germanTime1.local_time().time_of_day() == localTime1 );
				BOOST_REQUIRE( germanTime1.utc_time().time_of_day() == ( localTime1 - hours(2) ) );		// as the local time is ambiguous, daylight saving time must be chosen

				date testDate2( 2021, Jul, 29 );
				time_duration localTime2 = hours( 12 ) + minutes( 23 ) + seconds( 1 );
				Utilities::Time::german_local_date_time germanTime2( testDate2, localTime2 );
				BOOST_REQUIRE( germanTime2.date() == testDate2 );
				BOOST_REQUIRE( germanTime2.local_time().time_of_day() == localTime2 );
			}



			/**	@brief		Testing of empty construction
			*/
			BOOST_AUTO_TEST_CASE( GermanLocalDateTime_default_constructor_test_case )
			{
				Utilities::Time::german_local_date_time germanTime1;
				BOOST_REQUIRE( germanTime1.local_time().is_not_a_date_time() );
			}
			
			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/