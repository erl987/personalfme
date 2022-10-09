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
/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BoostStdTimeConverter.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	BoostStdTimeConverter		Unit test for the class CBoostStdTimeConverter
*/

namespace Utilitites {
	namespace Time {
		/*@{*/
		/** \ingroup BoostStdTimeConverter
		*/
		namespace BoostStdTimeConverter {
			// Test section
			BOOST_AUTO_TEST_SUITE( BoostStdTimeConverter_test_suite, *label("basic") );

			/**	@brief		Testing of time conversion
			*/
			BOOST_AUTO_TEST_CASE( BoostStdTimeConverter_test_case )
			{
				using namespace boost::posix_time;
				using namespace boost::gregorian;
				using namespace Utilities;
				CDateTime convertedTime;
				ptime backConvertedTime;

				// convert to standard format
				ptime originalTime( date( 2016, Jan, 10 ), hours( 1 ) + minutes( 31 ) + seconds( 35 ) );
				convertedTime = Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( originalTime );

				// convert to Boost DateTime format
				backConvertedTime = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( convertedTime );

				BOOST_REQUIRE( backConvertedTime == originalTime );

				// check invalid times
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( ptime( not_a_date_time ) ) == CDateTime() );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( CDateTime() ).is_not_a_date_time() );
			}



			/**	@brief		Testing of checks for possible existence of a certain day in a year
			*/
			BOOST_AUTO_TEST_CASE( BoostStdTimeConverter_day_can_exist_test_case )
			{
				using namespace Utilities::Time;

				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 5, 12 ) == true );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 5, 13 ) == false );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( -5, 11 ) == false );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 4, 0 ) == false );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 29, 2 ) == true );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 30, 2 ) == false );
				BOOST_REQUIRE( Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist( 31, 4 ) == false );
			}



			/**	@brief		Testing of week number conversion
			*/
			BOOST_AUTO_TEST_CASE( BoostStdTimeConverter_Get_Boost_Week_Num_test_case )
			{
				using namespace Utilities::Time;
				using namespace boost::gregorian;

				BOOST_REQUIRE( CBoostStdTimeConverter::GetBoostWeekNum( SECOND ) == nth_day_of_the_week_in_month::second );
				// wrong parameters are intrinsically not possible
			}



			/**	@brief		Testing of month number conversion
			*/
			BOOST_AUTO_TEST_CASE( BoostStdTimeConverter_Get_Boost_Month_Num_test_case )
			{
				using namespace Utilities::Time;
				using namespace boost::gregorian;

				BOOST_REQUIRE( CBoostStdTimeConverter::GetBoostMonthNum( 5 ) == May );
				
				// check wrong inputs
				BOOST_CHECK_THROW( CBoostStdTimeConverter::GetBoostMonthNum( -1 ), std::out_of_range );
				BOOST_CHECK_THROW( CBoostStdTimeConverter::GetBoostMonthNum( 13 ), std::out_of_range );
			}



			/**	@brief		Testing of week day conversion
			*/
			BOOST_AUTO_TEST_CASE( BoostStdTimeConverter_Get_Boost_Week_Day_test_case )
			{
				using namespace Utilities::Time;
				using namespace boost::gregorian;

				BOOST_REQUIRE( CBoostStdTimeConverter::GetBoostWeekDay( TUESDAY ) == Tuesday );
				// wrong parameters are intrinsically not possible
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/