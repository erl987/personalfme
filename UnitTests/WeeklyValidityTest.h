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

#include <memory>
#include <boost/test/unit_test.hpp>
#include "DefaultValidity.h"
#include "WeeklyValidity.h"
#include "BoostStdTimeConverter.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ValiditiesTest {
		namespace WeeklyValidityTest {
			auto ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;
	
			const Utilities::Time::WeekType weekInMonthSet2 = Utilities::Time::SECOND;	
			const std::vector<Utilities::Time::WeekType> weeksInMonthSet3 = { Utilities::Time::FIRST, Utilities::Time::FOURTH };
			const Utilities::Time::WeekType weekInMonthSet4 = Utilities::Time::LAST;

			const Utilities::Time::DayType weekDaySet1 = Utilities::Time::MONDAY;
			const Utilities::Time::DayType weekDaySet2 = Utilities::Time::SUNDAY;
			const Utilities::Time::DayType weekDaySet3 = Utilities::Time::FRIDAY;

			const Utilities::CTime beginTimeSet1 = Utilities::CTime( 15, 20, 30 );
			const Utilities::CTime endTimeSet1 = Utilities::CTime( 15, 50, 30 );
			const Utilities::CTime beginTimeSet2 = Utilities::CTime( 1, 30, 00 );
			const Utilities::CTime endTimeSet2 = Utilities::CTime( 2, 30, 00 );
			const Utilities::CTime beginTimeSet3 = Utilities::CTime( 2, 15, 0 );
			const Utilities::CTime endTimeSet3 = Utilities::CTime( 2, 45, 0 );
			const Utilities::CTime beginTimeSet4 = Utilities::CTime( 13, 15, 0 );
			const Utilities::CTime endTimeSet4 = Utilities::CTime( 9, 20, 0 );


			// Test section
			BOOST_AUTO_TEST_SUITE( WeeklyValidity_test_suite, *label("default") );


			/**	@brief		Systematic testing of the validity times considering special cases
			*/
			BOOST_AUTO_TEST_CASE( Validities_test_case )
			{
				using namespace std;
				using namespace Utilities;
				using namespace Utilities::Time;
				using namespace External::Validities;

				shared_ptr<CValidity > validity;
				vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validPeriods;

				// test case in winter time
				validity = make_shared< CWeeklyValidity >( weekInMonthSet2, weekDaySet2, beginTimeSet2, endTimeSet2 );
				validPeriods = validity->GetValidityPeriods( 1, 2016 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 10, 01, 2016, CTime( 00, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 10, 01, 2016, CTime( 01, 30, 00 ) ) ); // UTC

				// test case in summer time (with the validity intersecting the months)
				validPeriods = validity->GetValidityPeriods( 9, 2016 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 10, 9, 2016, CTime( 23, 30, 0 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 11, 9, 2016, CTime( 0, 30, 0 ) ) ); // UTC

				// validity in all weeks
				validity = make_unique< CWeeklyValidity >( weekDaySet1, beginTimeSet4, endTimeSet4 );
				validPeriods = validity->GetValidityPeriods( 11, 2018 );
				BOOST_REQUIRE( validPeriods.size() == 4 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 5, 11, 2018, CTime( 12, 15, 0 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 6, 11, 2018, CTime( 8, 20, 0 ) ) ); // UTC

				// completely during time shift (results in an empty period)
				validity = make_shared< CWeeklyValidity >( weekInMonthSet4, weekDaySet2, beginTimeSet3, endTimeSet3 );
				validPeriods = validity->GetValidityPeriods( 3, 2016 );
				BOOST_REQUIRE( validPeriods.empty() );
			}


			/**	@brief		Testing of the weekly validity exception
			*/
			BOOST_AUTO_TEST_CASE( Constructors_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Validities;
				using namespace Utilities;
				using namespace Utilities::Time;
				using namespace boost::gregorian;

				shared_ptr< Validities::CValidity > validity1, validity2, validity3;
				vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validPeriods;
				DayType weekDayGet;
				vector<WeekType> weeksGet;
				Utilities::CTime beginTimeGet, endTimeGet;
				
				// test constructor 0
				validity1 = make_unique< CWeeklyValidity >( weekDaySet1, beginTimeSet1, endTimeSet1 );
				BOOST_REQUIRE( validity1->IsValid() );
				dynamic_pointer_cast<CWeeklyValidity>( validity1 )->GetValidityDates( back_inserter( weeksGet ), weekDayGet, beginTimeGet, endTimeGet );
				BOOST_REQUIRE( weekDayGet == weekDaySet1 );
				BOOST_REQUIRE( weeksGet.size() == 5 );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet1 );
				BOOST_REQUIRE( endTimeGet == endTimeSet1 );

				// test constructor 1
				weeksGet.clear();
				validity2 = make_shared< CWeeklyValidity >( weekInMonthSet2, weekDaySet2, beginTimeSet2, endTimeSet2 );
				dynamic_pointer_cast<CWeeklyValidity>( validity2 )->GetValidityDates( back_inserter( weeksGet ), weekDayGet, beginTimeGet, endTimeGet );
				BOOST_REQUIRE( weekDayGet == weekDaySet2 );
				BOOST_REQUIRE( weeksGet.size() == 1 );
				BOOST_REQUIRE( weeksGet.front() == weekInMonthSet2 );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet2 );
				BOOST_REQUIRE( endTimeGet == endTimeSet2 );
				
				// test constructor 2 with too large times
				weeksGet.clear();
				validity3 = make_shared< CWeeklyValidity >( begin( weeksInMonthSet3 ), end( weeksInMonthSet3 ), weekDaySet3, beginTimeSet3, endTimeSet3 );
				dynamic_pointer_cast<CWeeklyValidity>( validity3 )->GetValidityDates( back_inserter( weeksGet ), weekDayGet, beginTimeGet, endTimeGet );
				BOOST_REQUIRE( weekDayGet == weekDaySet3 );
				BOOST_REQUIRE( weeksGet == weeksInMonthSet3 );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet3 );
				BOOST_REQUIRE( endTimeGet == endTimeSet3 );

				// check invalid inputs
				BOOST_CHECK_THROW( validity3->GetValidityPeriods( 2, 50 ), std::out_of_range );
				BOOST_CHECK_THROW( validity3->GetValidityPeriods( 2, 120000 ), std::out_of_range );
				BOOST_CHECK_THROW( validity3->GetValidityPeriods( 13, 2020 ), std::out_of_range );
				BOOST_CHECK_THROW( validity3->GetValidityPeriods( -4, 1990 ), std::out_of_range );

				BOOST_CHECK_THROW( dynamic_pointer_cast<CWeeklyValidity>( validity2 )->SetValidityDates( {}, SUNDAY, beginTimeSet1, endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CWeeklyValidity>( validity2 )->SetValidityDates( weeksInMonthSet3, SUNDAY, CTime(), endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CWeeklyValidity>( validity2 )->SetValidityDates( weeksInMonthSet3, SUNDAY, beginTimeSet1, CTime() ), std::out_of_range );
			}



			/**	@brief		Testing of setting the object empty
			*/
			BOOST_AUTO_TEST_CASE( Empty_test_case )
			{
				using namespace std;
				using namespace External::Validities;

				shared_ptr<CValidity> validity;
				Utilities::Time::DayType weekDayGet;
				vector<Utilities::Time::WeekType> weeksGet;
				Utilities::CTime beginTimeGet, endTimeGet;

				validity = make_shared< CWeeklyValidity >( weekInMonthSet2, weekDaySet2, beginTimeSet2, endTimeSet2 );
				BOOST_REQUIRE( validity->IsValid() );
				dynamic_pointer_cast< CWeeklyValidity >( validity )->Invalidate();
				BOOST_REQUIRE( !validity->IsValid() );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CWeeklyValidity>( validity )->GetValidityDates( back_inserter( weeksGet ), weekDayGet, beginTimeGet, endTimeGet ), std::runtime_error );
				BOOST_CHECK_THROW( validity->GetValidityPeriods( 5, 2015 ), std::runtime_error );
			}



			/**	@brief		Testing of the default validity exception comparisons
			*/
			BOOST_AUTO_TEST_CASE( WeeklyValidity_comparison_test_case )
			{
				using namespace std;
				using namespace External::Validities;

				shared_ptr< CValidity > validity1, validity2, validity3;

				validity1 = make_shared< CWeeklyValidity >( weekInMonthSet2, weekDaySet2, beginTimeSet2, endTimeSet2 );
				validity2 = make_shared< CWeeklyValidity >( begin( weeksInMonthSet3 ), end( weeksInMonthSet3 ), weekDaySet3, beginTimeSet3, endTimeSet3 );
				validity3 = DEFAULT_VALIDITY;

				// test valid comparisons
				BOOST_REQUIRE( *validity1 == *validity1 );
				BOOST_REQUIRE( !( *validity1 == *validity2 ) );
				BOOST_REQUIRE( ( *validity1 != *validity2 ) );
				BOOST_REQUIRE( CWeeklyValidity() == CWeeklyValidity() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( *validity1 == *validity3 ) );	
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
