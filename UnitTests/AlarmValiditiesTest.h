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
#include <boost/test/included/unit_test.hpp>
#include "AlarmValidities.h"
#include "GroupalarmMessage.h"
#include "SingleTimeValidity.h"
#include "WeeklyValidity.h"
#include "MonthlyValidity.h"
#include "DefaultValidity.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace AlarmValiditiesTest {
		const std::string alarmPhoneNum1 = "091316120400";
		const bool isFreeTextFlagSet1 = true;
		const std::string messageTextSet1 = "test";
		const bool isFeedbackFlagSet1 = true;
		const bool isFlashSMSflagSet1 = false;
		const bool isNoFaxFlagSet1 = true;

		const std::vector<int> alarmLists2 = { 3 };
		const std::vector<int> alarmGroups2 = { 7, 9 };
		const bool isFreeTextFlagSet2 = false;
		const std::string messageTextSet2 = "5";
		const bool isFeedbackFlagSet2 = true;
		const bool isFlashSMSflagSet2 = false;
		const bool isNoFaxFlagSet2 = true;

		/**	@brief		Generate test data
		*/
		External::CAlarmValidities GenerateTestset(void)
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace Utilities;

			CAlarmValidities dataset;
			shared_ptr< Validities::CValidity > validitySet;
			vector< shared_ptr< CAlarmMessage > > groupalarm1, groupalarm2;

			validitySet = make_shared< Validities::CSingleTimeValidity >( Validities::CSingleTimeValidity( CDateTime( 25, 3, 2013, CTime( 15, 21, 30, 0 ) ), CDateTime( 25, 3, 2013, CTime( 18, 29, 45, 0 ) ) ) );
			groupalarm1.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmPhoneNum1, isFreeTextFlagSet1, messageTextSet1, isFeedbackFlagSet1, isFlashSMSflagSet1, isNoFaxFlagSet1 ) ) );
			groupalarm1.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmLists2, alarmGroups2, isFreeTextFlagSet2, messageTextSet2, isFeedbackFlagSet2, isFlashSMSflagSet2, isNoFaxFlagSet2 ) ) );
			dataset.AddEntry( validitySet, groupalarm1.begin(), groupalarm1.end() );
				
			groupalarm2.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage() ) );
			dataset.AddEntry( Validities::DEFAULT_VALIDITY, groupalarm2.begin(), groupalarm2.end() );

			return dataset;
		}



		/**	@brief		Generate test data
		*/
		External::CAlarmValidities GenerateTestsetDifficult(void)
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace External::Validities;
			CAlarmValidities dataset;
			shared_ptr< Validities::CValidity > validitySet;
			vector< shared_ptr< CAlarmMessage > > groupalarm1, groupalarm2;

			vector< shared_ptr< CAlarmMessage > > groupalarmDefault( 1, make_shared< CGroupalarmMessage >( CGroupalarmMessage( { 1, 4 }, { 5, 7 }, true, "default", true, false, true ) ) );
			vector< shared_ptr< CAlarmMessage > > groupalarmException( 1, make_shared< CGroupalarmMessage >( CGroupalarmMessage( { 1, 4 }, { 5, 7 }, true, "exception", true, false, true ) ) );
			vector< shared_ptr< CAlarmMessage > > groupalarmEmpty( 1, make_shared< CGroupalarmMessage >( CGroupalarmMessage() ) );
			CWeeklyValidity weeklyException( Utilities::Time::THURSDAY, Utilities::CTime( 21, 30, 00 ), Utilities::CTime( 23, 30, 00 ) );
			CMonthlyValidity monthlyException( 5, Utilities::CTime( 19, 12, 30 ), Utilities::CTime( 20, 12, 30 ) );

			// generate dataset
			dataset.AddEntry( DEFAULT_VALIDITY, groupalarmDefault.begin(), groupalarmDefault.end() );
			dataset.AddEntry( make_shared< CWeeklyValidity >( weeklyException ), groupalarmEmpty.begin(), groupalarmEmpty.end() );
			dataset.AddEntry( make_shared< CMonthlyValidity >( monthlyException ), groupalarmException.begin(), groupalarmException.end() );

			return dataset;
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( AlarmValidities_test_suite, *label("basic") );

		/**	@brief		Testing of the CAlarmValidities constructor
		*/
		BOOST_AUTO_TEST_CASE( Constructor_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace Utilities;

			CAlarmValidities dataset1, dataset2, dataset3;
			shared_ptr< Validities::CValidity > validitySet;
			vector< std::shared_ptr< CAlarmMessage > > groupalarmSet, groupalarmGet;

			// test empty construction
			dataset1 = CAlarmValidities();
			BOOST_REQUIRE( dataset1.GetAllEntries().empty() );

			// test normal construction
			validitySet = make_shared< Validities::CSingleTimeValidity >( CDateTime( 25, 3, 2013, CTime( 15, 21, 30, 0 ) ), CDateTime( 25, 3, 2013, CTime( 18, 29, 45, 0 ) ) );
			groupalarmSet.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmPhoneNum1, isFreeTextFlagSet1, messageTextSet1, isFeedbackFlagSet1, isFlashSMSflagSet1, isNoFaxFlagSet1 ) ) );
			groupalarmSet.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmLists2, alarmGroups2, isFreeTextFlagSet2, messageTextSet2, isFeedbackFlagSet2, isFlashSMSflagSet2, isNoFaxFlagSet2 ) ) );
			dataset2 = CAlarmValidities( validitySet, groupalarmSet.begin(), groupalarmSet.end() );
			BOOST_REQUIRE( dataset2.GetAllEntries().size() == 1 );
			BOOST_REQUIRE( *( dataset2.GetAllEntries().front().first ) == *validitySet );
			dataset2.GetEntry( validitySet, back_inserter( groupalarmGet ) );
			BOOST_REQUIRE( groupalarmSet.size() == groupalarmGet.size() );
			for ( size_t i = 0; i < groupalarmSet.size(); i++ ) {
				BOOST_REQUIRE( *groupalarmSet[i] == *groupalarmGet[i] );
			}

			// test wrong construction
			vector< shared_ptr< CAlarmMessage > > emptyGroupalarm;
			BOOST_CHECK_NO_THROW( dataset3 = CAlarmValidities( validitySet, emptyGroupalarm.begin(), emptyGroupalarm.end() ) );
			shared_ptr< Validities::CValidity > emptyValiditySet;
			BOOST_CHECK_THROW( dataset3 = CAlarmValidities( emptyValiditySet, groupalarmSet.begin(), groupalarmSet.end() ), std::runtime_error );
		}



		/**	@brief		Testing of the entry handling
		*/
		BOOST_AUTO_TEST_CASE( EntryHandling_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace External::Validities;
			using namespace Utilities;
			CAlarmValidities dataset;
			vector< std::shared_ptr< CAlarmMessage > > groupalarmSet, groupalarmGet, groupalarmSet2, groupalarmGet2;
			shared_ptr< Validities::CValidity > validitySet, validitySet2;

			// test adding entries
			validitySet = make_shared< CSingleTimeValidity >( CSingleTimeValidity( CDateTime( 25, 3, 2013, CTime( 15, 21, 30, 0 ) ), CDateTime( 25, 3, 2013, CTime( 18, 29, 45, 0 ) ) ) );
			groupalarmSet.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmPhoneNum1, isFreeTextFlagSet1, messageTextSet1, isFeedbackFlagSet1, isFlashSMSflagSet1, isNoFaxFlagSet1 ) ) );
			dataset.AddEntry( validitySet, groupalarmSet.begin(), groupalarmSet.end() );
			validitySet2 = make_shared< CWeeklyValidity >( CWeeklyValidity( Utilities::Time::MONDAY, Utilities::CTime( 5, 20, 41 ), Utilities::CTime( 5, 32, 58 ) ) );
			groupalarmSet2.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage( alarmLists2, alarmGroups2, isFreeTextFlagSet2, messageTextSet2, isFeedbackFlagSet2, isFlashSMSflagSet2, isNoFaxFlagSet2 ) ) );
			dataset.AddEntry( validitySet2, groupalarmSet2.begin(), groupalarmSet2.end() );
				
			// test getting entries
			dataset.GetEntry( validitySet, back_inserter( groupalarmGet ) );
			BOOST_REQUIRE( groupalarmSet.size() == groupalarmGet.size() );
			for ( size_t i = 0; i < groupalarmSet.size(); i++ ) {
				BOOST_REQUIRE( *groupalarmSet[i] == *groupalarmGet[i] );
			}
			BOOST_REQUIRE( dataset.IsEntry( validitySet ) );
			dataset.GetEntry( validitySet2, back_inserter( groupalarmGet2 ) );
			BOOST_REQUIRE( groupalarmSet2.size() == groupalarmGet2.size() );
			for ( size_t i = 0; i < groupalarmSet2.size(); i++ ) {
				BOOST_REQUIRE( *groupalarmSet2[i] == *groupalarmGet2[i] );
			}
			BOOST_REQUIRE( dataset.IsEntry( validitySet2 ) );

			// test replacing entries
			dataset.ReplaceEntry( validitySet, groupalarmSet2.begin(), groupalarmSet2.end() );
			groupalarmGet2.clear();
			dataset.GetEntry( validitySet, back_inserter( groupalarmGet2 ) );
			BOOST_REQUIRE( groupalarmSet2.size() == groupalarmGet2.size() );
			for ( size_t i = 0; i < groupalarmSet2.size(); i++ ) {
				BOOST_REQUIRE( *groupalarmSet2[i] == *groupalarmGet2[i] );
			}

			// test removing entries
			dataset.RemoveEntry( validitySet );
			BOOST_CHECK_THROW( dataset.GetEntry( validitySet, back_inserter( groupalarmGet ) ), std::logic_error );

			// test invalid inputs
			shared_ptr< Validities::CValidity > emptyValiditySet;
			BOOST_CHECK_THROW( dataset.AddEntry( emptyValiditySet, groupalarmSet.begin(), groupalarmSet.end() ), std::runtime_error );
			vector< std::shared_ptr< CAlarmMessage > > groupalarmEmpty;
			BOOST_CHECK_THROW( dataset.AddEntry( validitySet2, groupalarmEmpty.begin(), groupalarmEmpty.end() ), std::logic_error );
			BOOST_CHECK_THROW( dataset.RemoveEntry( emptyValiditySet ), std::runtime_error );
			BOOST_CHECK_THROW( dataset.RemoveEntry( validitySet ), std::logic_error );
			BOOST_CHECK_THROW( dataset.ReplaceEntry( emptyValiditySet, groupalarmSet.begin(), groupalarmSet.end() ), std::runtime_error );
			BOOST_CHECK_THROW( dataset.ReplaceEntry( validitySet, groupalarmEmpty.begin(), groupalarmEmpty.end() ), std::logic_error );
			BOOST_CHECK_THROW( dataset.GetEntry( emptyValiditySet, back_inserter( groupalarmSet ) ), std::runtime_error );
			BOOST_CHECK_THROW( dataset.GetEntry( validitySet, back_inserter( groupalarmSet ) ), std::logic_error );
			BOOST_CHECK_THROW( dataset.IsEntry( emptyValiditySet ), std::runtime_error );
		}



		/**	@brief		Testing of global handling functions
		*/
		BOOST_AUTO_TEST_CASE( AlarmValidities_globalHandling_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			CAlarmValidities dataset;
			vector< CGroupalarmMessage > groupalarm;

			dataset = GenerateTestset();
			BOOST_REQUIRE( dataset.Size() == 2 );
			BOOST_REQUIRE( dataset.GetAllEntries().size() == 2 );
			dataset.Clear();
			BOOST_REQUIRE( dataset.Size() == 0 );
		}



		/**	@brief		Testing of the comparison operators
		*/
		BOOST_AUTO_TEST_CASE( AlarmValidities_comparison_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			CAlarmValidities dataset1, dataset2;
			vector< shared_ptr< CAlarmMessage > > groupalarm;

			dataset1 = GenerateTestset();
			groupalarm.push_back( make_shared< CGroupalarmMessage >( CGroupalarmMessage() ) );
			dataset2.AddEntry( Validities::DEFAULT_VALIDITY, groupalarm.begin(), groupalarm.end() );

			// test valid comparisons
			BOOST_REQUIRE( dataset1 == dataset1 );
			BOOST_REQUIRE( !( dataset1 == dataset2 ) );
			BOOST_REQUIRE( ( dataset1 != dataset2 ) );
			BOOST_REQUIRE( CAlarmValidities() == CAlarmValidities() );	
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
