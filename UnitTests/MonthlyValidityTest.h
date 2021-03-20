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

#include <memory>
#include <boost/test/unit_test.hpp>
#include "DefaultValidity.h"
#include "MonthlyValidity.h"
#include "BoostStdTimeConverter.h"


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace ValiditiesTest {
		namespace MonthlyValidityTest {
			auto ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;
			
			const int monthSet1 = 1;
			const int monthSet2 = 5;
			const std::vector<int> monthsSet3 = { 5, 6, 2, 3 };
			
			const int daySet1 = 29;
			const int daySet2 = 18;
			const int daySet3 = 1;

			const Utilities::CTime beginTimeSet1 = Utilities::CTime( 15, 20, 30 );
			const Utilities::CTime endTimeSet1 = Utilities::CTime( 15, 50, 30 );

			const Utilities::CTime beginTimeSet2 = Utilities::CTime( 7, 5, 2 );
			const Utilities::CTime endTimeSet2 = Utilities::CTime( 7, 30, 20 );

			const Utilities::CTime beginTimeSet3 = Utilities::CTime( 1, 30, 0 );
			const Utilities::CTime endTimeSet3 = Utilities::CTime( 2, 30, 0 );

			const Utilities::CTime beginTimeSet4 = Utilities::CTime( 5, 30, 0 );
			const Utilities::CTime endTimeSet4 = Utilities::CTime( 3, 30, 0 );

			const Utilities::CTime beginTimeSet5 = Utilities::CTime( 2, 15, 0 );
			const Utilities::CTime endTimeSet5 = Utilities::CTime( 2, 30, 0 );


			// Test section
			BOOST_AUTO_TEST_SUITE( MonthlyValidity_test_suite );

			/**	@brief		Systematic testing of the validity times considering special cases
			*/
			BOOST_AUTO_TEST_CASE( MonthlyValidity_validities_test_case )
			{
				using namespace std;
				using namespace External::Validities;
				using namespace Utilities;

				shared_ptr< CValidity > validity;
				vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validPeriods;

				// during time shift (end time is shifted to the beginning of summer time)
				validity = make_shared< CMonthlyValidity >( 29, beginTimeSet3, endTimeSet3 );
				validPeriods = validity->GetValidityPeriods( 3, 2015 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 29, 3, 2015, CTime( 00, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 29, 3, 2015, CTime( 01, 00, 00 ) ) ); // UTC (02:30 local time does not exist on this day)

				// during fall time shift (for the end time, time "A" has to be chosen)
				validity = make_shared< CMonthlyValidity >( 25, beginTimeSet3, endTimeSet3 );
				validPeriods = validity->GetValidityPeriods( 10, 2015 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 24, 10, 2015, CTime( 23, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 25, 10, 2015, CTime( 00, 30, 00 ) ) ); // UTC (02:30A local time)

				// exception that will begin during different months due to UTC offset
				validity = make_shared< CMonthlyValidity >( daySet3, begin( monthsSet3 ), end( monthsSet3 ), beginTimeSet3, endTimeSet3 );
				validPeriods = validity->GetValidityPeriods( 5, 2016 );
				BOOST_REQUIRE( validPeriods.size() == 2 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 30, 4, 2016, CTime( 23, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 1, 5, 2016, CTime( 00, 30, 00 ) ) ); // UTC
				validPeriods = validity->GetValidityPeriods( 2, 2016 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 1, 2, 2016, CTime( 00, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 1, 2, 2016, CTime( 01, 30, 00 ) ) ); // UTC
				validPeriods = validity->GetValidityPeriods( 9, 2016 );
				BOOST_REQUIRE( validPeriods.empty() );

				// the end day does not exist in every month
				validity = make_shared< CMonthlyValidity >( 28, beginTimeSet4, endTimeSet4 );
				validPeriods = validity->GetValidityPeriods( 1, 2015 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 28, 1, 2015, CTime( 04, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 29, 1, 2015, CTime( 02, 30, 00 ) ) ); // UTC

				// the begin day does not exist in every month
				validity = make_shared< CMonthlyValidity >( 30, beginTimeSet4, endTimeSet4 );
				validPeriods = validity->GetValidityPeriods( 1, 2016 );
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 30, 1, 2016, CTime( 04, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 31, 1, 2016, CTime( 02, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validity->GetValidityPeriods( 2, 2016 ).empty() );

				// end day is in the next UTC-month
				validity = make_shared< CMonthlyValidity >( 29, beginTimeSet4, endTimeSet4 );
				validPeriods = validity->GetValidityPeriods( 2, 2016 ); // in 2016 the day exists
				BOOST_REQUIRE( validPeriods.size() == 1 );
				BOOST_REQUIRE( validPeriods.front().first == CDateTime( 29, 2, 2016, CTime( 04, 30, 00 ) ) ); // UTC
				BOOST_REQUIRE( validPeriods.front().second == CDateTime( 1, 3, 2016, CTime( 02, 30, 00 ) ) ); // UTC

				// completely during time shift (results in an empty period)
				validity = make_shared< CMonthlyValidity >( 29, beginTimeSet5, endTimeSet5 );
				validPeriods = validity->GetValidityPeriods( 3, 2015 );
				BOOST_REQUIRE( validPeriods.empty() );
				BOOST_REQUIRE( validity->GetValidityPeriods( 5, 2015 ).size() == 1 );
			}


			/**	@brief		Testing of the single time validity exception
			*/
			BOOST_AUTO_TEST_CASE( Constructors_test_case )
			{
				using namespace std;
				using namespace External::Validities;

				shared_ptr< CValidity > validity1, validity2, validity3, validity4;
				int dayGet;
				vector<int> monthsGet, allMonths;
				Utilities::CTime beginTimeGet, endTimeGet;
				
				// test constructor 1
				validity1 = make_shared< CMonthlyValidity >( daySet1, beginTimeSet1, endTimeSet1 );
				dynamic_pointer_cast<CMonthlyValidity>( validity1 )->GetValidityDays( dayGet, back_inserter( monthsGet ), beginTimeGet, endTimeGet );
				BOOST_REQUIRE( dayGet == daySet1 );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet1 );
				BOOST_REQUIRE( endTimeGet == endTimeSet1 );
				allMonths.resize( 12 );
				int n = 1;
				generate( begin( allMonths ), end( allMonths ), [&n] { return n++; } );
				BOOST_REQUIRE( monthsGet == allMonths );
			
				// test constructor 2
				monthsGet.clear();
				validity2 = make_shared< CMonthlyValidity >( daySet2, monthSet2, beginTimeSet2, endTimeSet2 );
				dynamic_pointer_cast<CMonthlyValidity>( validity2 )->GetValidityDays( dayGet, back_inserter( monthsGet ), beginTimeGet, endTimeGet );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet2 );
				BOOST_REQUIRE( endTimeGet == endTimeSet2 );
				BOOST_REQUIRE( monthsGet.size() == 1 );
				BOOST_REQUIRE( monthsGet.front() == monthSet2 );

				// test constructor 3
				monthsGet.clear();
				validity3 = make_shared< CMonthlyValidity >( daySet3, begin( monthsSet3 ), end( monthsSet3 ), beginTimeSet3, endTimeSet3 );
				dynamic_pointer_cast<CMonthlyValidity>( validity3 )->GetValidityDays( dayGet, back_inserter( monthsGet ), beginTimeGet, endTimeGet );
				BOOST_REQUIRE( dayGet == daySet3 );
				BOOST_REQUIRE( monthsGet == monthsSet3 );
				BOOST_REQUIRE( beginTimeGet == beginTimeSet3 );
				BOOST_REQUIRE( endTimeGet == endTimeSet3 );

				// check invalid inputs
				validity4 = make_shared< CMonthlyValidity >( daySet1, beginTimeSet1, endTimeSet1 );
				BOOST_CHECK_THROW( validity4->GetValidityPeriods( 2, 50 ), std::out_of_range );
				BOOST_CHECK_THROW( validity4->GetValidityPeriods( 3, 120000 ), std::out_of_range );
				BOOST_CHECK_THROW( validity4->GetValidityPeriods( 13, 2020 ), std::out_of_range );
				BOOST_CHECK_THROW( validity4->GetValidityPeriods( -4, 1990 ), std::out_of_range );

				BOOST_CHECK_NO_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( 2, { 5, 5, 11 }, beginTimeSet1, endTimeSet1 ) );

				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( -5, { 5, 11 }, beginTimeSet1, endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( 1, { 13 }, beginTimeSet1, endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( 1, {}, beginTimeSet1, endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( 1, { 5, 11 }, Utilities::CTime(), endTimeSet1 ), std::out_of_range );
				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity4 )->SetValidityDays( 1, { 5, 11 }, beginTimeSet1, Utilities::CTime() ), std::out_of_range );
			}


			/**	@brief		Testing of setting the object empty
			*/
			BOOST_AUTO_TEST_CASE( Empty_test_case )
			{
				using namespace std;
				using namespace External::Validities;

				shared_ptr< CValidity > validity;
				int dayGet;
				vector<int> monthsGet;
				Utilities::CTime beginTimeOfDay, endTimeOfDay;

				validity = make_shared< CMonthlyValidity >( daySet1, beginTimeSet1, endTimeSet1 );
				BOOST_REQUIRE( validity->IsValid() );
				dynamic_pointer_cast<CMonthlyValidity>( validity )->Invalidate();
				BOOST_CHECK_THROW( dynamic_pointer_cast<CMonthlyValidity>( validity )->GetValidityDays( dayGet, back_inserter( monthsGet ), beginTimeOfDay, endTimeOfDay ), std::runtime_error );
				BOOST_CHECK_THROW( validity->GetValidityPeriods( 5, 2015 ), std::runtime_error );
				BOOST_REQUIRE( !validity->IsValid() );
			}



			/**	@brief		Testing of the default validity exception comparisons
			*/
			BOOST_AUTO_TEST_CASE( MonthlyValidity_comparison_test_case )
			{
				using namespace std;
				using namespace External;
				using namespace External::Validities;
				shared_ptr< CValidity > validity1, validity2, validity3;

				validity1 = make_shared< CMonthlyValidity >( daySet1, monthSet1, beginTimeSet1, endTimeSet1 );
				validity2 = make_shared< CMonthlyValidity >( daySet2, monthSet2, beginTimeSet2, endTimeSet2 );
				validity3 = DEFAULT_VALIDITY;

				// test valid comparisons
				BOOST_REQUIRE( *validity1 == *validity1 );
				BOOST_REQUIRE( !( *validity1 == *validity2 ) );
				BOOST_REQUIRE( ( *validity1 != *validity2 ) );
				BOOST_REQUIRE( CMonthlyValidity() == CMonthlyValidity() );

				// test invalid comparison with other derived class type
				BOOST_REQUIRE( !( *validity1 == *validity3 ) );	
			}

			BOOST_AUTO_TEST_SUITE_END();
		}
	}
	/*@}*/
}
/*@}*/
