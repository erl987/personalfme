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

#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "DefaultValidity.h"
#include "SingleTimeValidity.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"


/*@{*/
/** \ingroup UnitTests
*/

namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ValiditiesTest {
		namespace SingleTimeValidityTest {
			const Utilities::CDateTime beginTimeSet1 = Utilities::CDateTime( 7, 4, 2013, Utilities::CTime( 15, 20, 30 ) );
			const Utilities::CDateTime endTimeSet1 = Utilities::CDateTime( 7, 4, 2013, Utilities::CTime( 15, 40, 30 ) );

			const Utilities::CDateTime beginTimeSet2 = Utilities::CDateTime( 9, 10, 2021, Utilities::CTime( 18, 23, 50 ) );
			const Utilities::CDateTime endTimeSet2 = Utilities::CDateTime( 13, 12, 2021, Utilities::CTime( 19, 31, 10 ) );

			const Utilities::CDateTime beginTimeSet3 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 02, 10, 00 ) ); // time shift at 02:00am local time on this day
			const Utilities::CDateTime endTimeSet3 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 03, 10, 00 ) );

			const Utilities::CDateTime beginTimeSet4 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 01, 10, 00 ) ); 
			const Utilities::CDateTime endTimeSet4 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 02, 10, 00 ) ); // time shift at 02:00am local time on this day

			const Utilities::CDateTime beginTimeSet5 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 02, 20, 00 ) );
			const Utilities::CDateTime endTimeSet5 = Utilities::CDateTime( 27, 3, 2016, Utilities::CTime( 02, 40, 00 ) ); // time shift at 02:00am local time on this day

			const Utilities::CDateTime beginTimeSet6 = Utilities::CDateTime( 30, 10, 2016, Utilities::CTime( 02, 20, 00 ) ); // time shift at 02:00am local time on this day
			const Utilities::CDateTime endTimeSet6 = Utilities::CDateTime( 30, 10, 2016, Utilities::CTime( 03, 10, 00 ) );

			const Utilities::CDateTime beginTimeSet7 = Utilities::CDateTime( 01, 07, 2016, Utilities::CTime( 00, 05, 00 ) ); // time shift at 02:00am local time on this day
			const Utilities::CDateTime endTimeSet7 = Utilities::CDateTime( 01, 07, 2016, Utilities::CTime( 06, 00, 00 ) );

			// Test section
			BOOST_AUTO_TEST_SUITE( SingleTimeValidity_test_suite, *label("basic") );

			/**	@brief		Testing of the construction of the single time validity exception
			*/
			BOOST_AUTO_TEST_CASE( SingleTimeValidity_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Validities;
				using namespace Utilities::Time;
				auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;

				shared_ptr< Validities::CValidity > validity, validity2;
				Utilities::CDateTime beginTimeGet1, endTimeGet1;
				vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validPeriods;

				// test default construction
				validity = make_shared< CSingleTimeValidity >( beginTimeSet1, endTimeSet1 );
				dynamic_pointer_cast<CSingleTimeValidity>( validity )->GetValidity( beginTimeGet1, endTimeGet1 );
				BOOST_REQUIRE( beginTimeSet1 == beginTimeGet1 );
				BOOST_REQUIRE( endTimeSet1 == endTimeGet1 );

				// check invalid inputs
				BOOST_CHECK_THROW( validity->GetValidityPeriods( beginTimeSet1.Month(), 50 ), std::out_of_range );
				BOOST_CHECK_THROW( validity->GetValidityPeriods( beginTimeSet1.Month(), 120000 ), std::out_of_range );
				BOOST_CHECK_THROW( validity->GetValidityPeriods( 13, 2020 ), std::out_of_range );
				BOOST_CHECK_THROW( validity->GetValidityPeriods( 0, 1990 ), std::out_of_range );

				BOOST_CHECK_THROW( dynamic_pointer_cast<CSingleTimeValidity>( validity )->SetValidity( endTimeSet1, beginTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( CSingleTimeValidity( Utilities::CDateTime(), Utilities::CDateTime() ), std::out_of_range );
	
				// test setting empty
				validity2 = make_shared<CSingleTimeValidity>( beginTimeSet1, endTimeSet1 );
				dynamic_pointer_cast<CSingleTimeValidity>( validity2 )->Invalidate();

				BOOST_CHECK_THROW( dynamic_pointer_cast<CSingleTimeValidity>( validity2 )->GetValidity( beginTimeGet1, endTimeGet1 ), std::runtime_error );
				BOOST_REQUIRE( !validity2->IsValid() );
			}


			/**	@brief		Testing of the validity times
			*/
			BOOST_AUTO_TEST_CASE( SingleTimeValidity_validities_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Validities;
				using namespace Utilities;
				using namespace Utilities::Time;
				auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;

				shared_ptr< Validities::CValidity > validity;
				vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validPeriods;

				// out of validity
				validity = make_shared< CSingleTimeValidity >( beginTimeSet1, endTimeSet1 );
				BOOST_REQUIRE( validity->GetValidityPeriods( 5, 2013 ).empty() );
				BOOST_REQUIRE( validity->GetValidityPeriods( 2, 2020 ).empty() );

				// summer time validity
				validity = make_shared< CSingleTimeValidity >( beginTimeSet1, endTimeSet1 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet1.Month(), beginTimeSet1.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().first ) == german_local_date_time( ToBoostTime( beginTimeSet1 ).date(), ToBoostTime( beginTimeSet1 ).time_of_day() ).utc_time() );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().second ) == german_local_date_time( ToBoostTime( endTimeSet1 ).date(), ToBoostTime( endTimeSet1 ).time_of_day() ).utc_time() );

				// winter time validity
				validity = make_shared< CSingleTimeValidity >( beginTimeSet2, endTimeSet2 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet2.Month() + 1, beginTimeSet2.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().first ) == german_local_date_time( ToBoostTime( beginTimeSet2 ).date(), ToBoostTime( beginTimeSet2 ).time_of_day() ).utc_time() );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().second ) == german_local_date_time( ToBoostTime( endTimeSet2 ).date(), ToBoostTime( endTimeSet2 ).time_of_day() ).utc_time() );

				// during time shift (start time is shifted to the beginning of summer time)
				validity = make_shared< CSingleTimeValidity >( beginTimeSet3, endTimeSet3 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet3.Month(), beginTimeSet3.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( beginTimeSet3.Day(), beginTimeSet3.Month(), beginTimeSet3.Year(), CTime( 1, 0, 0 ) ) );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().second ) == german_local_date_time( ToBoostTime( endTimeSet3 ).date(), ToBoostTime( endTimeSet3 ).time_of_day() ).utc_time() );

				// during time shift (end time is shifted to the beginning of summer time)
				validity = make_shared< CSingleTimeValidity >( beginTimeSet4, endTimeSet4 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet4.Month(), beginTimeSet4.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().first ) == german_local_date_time( ToBoostTime( beginTimeSet4 ).date(), ToBoostTime( beginTimeSet4 ).time_of_day() ).utc_time() );
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( endTimeSet4.Day(), endTimeSet4.Month(), endTimeSet4.Year(), CTime( 1, 0, 0 ) ) );

				// completely during time shift (results in an empty period)
				validity = make_shared< CSingleTimeValidity >( beginTimeSet5, endTimeSet5 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet5.Month(), beginTimeSet5.Year() );
				BOOST_REQUIRE( validPeriods.empty() );

				// during fall time shift (for the start time, time "A" has to be chosen)
				validity = make_shared< CSingleTimeValidity >( beginTimeSet6, endTimeSet6 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet6.Month(), beginTimeSet6.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( endTimeSet6.Day(), endTimeSet6.Month(), endTimeSet6.Year(), CTime( 00, 20, 00 ) ) );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().second ) == german_local_date_time( ToBoostTime( endTimeSet6 ).date(), ToBoostTime( endTimeSet6 ).time_of_day() ).utc_time() );

				// validity from last month is present due to local time -> UTC offset
				validity = make_shared< CSingleTimeValidity >( beginTimeSet7, endTimeSet7 );
				validPeriods = validity->GetValidityPeriods( beginTimeSet7.Month() - 1, beginTimeSet7.Year() );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().first ) == german_local_date_time( ToBoostTime( beginTimeSet7 ).date(), ToBoostTime( beginTimeSet7 ).time_of_day() ).utc_time() );
				BOOST_REQUIRE( ToBoostTime( validPeriods.front().second ) == german_local_date_time( ToBoostTime( endTimeSet7 ).date(), ToBoostTime( endTimeSet7 ).time_of_day() ).utc_time() );
			}


			/**	@brief		Testing of the default validity exception comparisons
			*/
			BOOST_AUTO_TEST_CASE( SingleTimeValidity_comparison_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Validities;
				shared_ptr< Validities::CValidity > validity1, validity2, validity3;

				validity1 = make_shared< CSingleTimeValidity >( beginTimeSet1, endTimeSet1 );
				validity2 = make_shared< CSingleTimeValidity >( beginTimeSet2, endTimeSet2 );
				validity3 = DEFAULT_VALIDITY;

				// test valid comparisons
				BOOST_REQUIRE( *validity1 == *validity1 );
				BOOST_REQUIRE( !( *validity1 == *validity2 ) );
				BOOST_REQUIRE( ( *validity1 != *validity2 ) );
				BOOST_REQUIRE( CSingleTimeValidity() == CSingleTimeValidity() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( *validity1 == *validity3 ) );	
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
