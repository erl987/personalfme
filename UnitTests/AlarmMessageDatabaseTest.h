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
#include <set>
#include <boost/test/unit_test.hpp>
#include "AlarmMessagesDatabase.h"
#include "GroupalarmMessage.h"
#include "EmailMessage.h"
#include "InfoalarmMessageDecorator.h"
#include "DefaultValidity.h"
#include "WeeklyValidity.h"
#include "MonthlyValidity.h"
#include "SingleTimeValidity.h"
#include "AlarmValidities.h"
#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"



/*@{*/
/** \ingroup UnitTest
*/


/**	\defgroup	GatewayTests	Unit tests for the classes used for internet Weblinks
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/	
	namespace AlarmMessageDatabaseTest {
		// test parameters
		const std::vector<int> code1 = { 1, 2, 3, 4, 5 };
		const std::vector<int> code2 = { 5, 4, 3, 2, 1 };
		const std::vector<int> code3 = { 2, 3, 4, 5, 6 };
		const std::vector<int> code4 = { 9, 8, 7, 6, 5 };
		const std::vector<int> notExistingCode = { 3, 2, 1, 5, 6 };
		const External::Groupalarm::CGroupalarmMessage groupalarmDefault( { 4, 1 }, { 5, 7 }, true, "default", true, false, true );
		const External::Groupalarm::CGroupalarmMessage groupalarmException( "0151123456", true, "exception", true, false, true );
		const External::Groupalarm::CGroupalarmMessage groupalarmEmpty;
		const External::Groupalarm::CGroupalarmMessage groupalarmInfo( { 734234, 28343 }, { 2345234 }, false, "234523", false, false, true );
		const External::Email::CEmailMessage email( "test", "test", { {"test1", "test1" }, { "test2", "test2" } }, "test", true );
		const External::Email::CEmailMessage emailFallback( "fallback", "fallback", { { "test3", "test3" },{ "test4", "test4" } }, "fallback", true );
		const External::Validities::CWeeklyValidity weeklyException( Utilities::Time::THURSDAY, Utilities::CTime( 21, 30, 00 ), Utilities::CTime( 23, 30, 00 ) );
		const External::Validities::CWeeklyValidity weeklyException2( Utilities::Time::SUNDAY, Utilities::CTime( 19, 15, 30 ), Utilities::CTime( 20, 45, 30 ) );
		const External::Validities::CMonthlyValidity monthlyException( 5, Utilities::CTime( 19, 12, 30 ), Utilities::CTime( 20, 12, 30 ) );
		const External::Validities::CMonthlyValidity monthlyException2( 2, Utilities::CTime( 5, 3, 9 ), Utilities::CTime( 5, 11, 9 ) );
		const External::Validities::CMonthlyValidity monthlyException3( 12, Utilities::CTime( 9, 13, 21 ), Utilities::CTime( 9, 13, 36 ) );
		const External::Validities::CSingleTimeValidity singleException( Utilities::CDateTime( 9, 11, 2013, Utilities::CTime( 12, 30, 15, 0 ) ), Utilities::CDateTime( 9, 11, 2013, Utilities::CTime( 13, 00, 15, 0 ) ) );



		/**	@brief		Generating the test database
		*/
		External::CAlarmMessageDatabase GenerateDatabase(void)
		{
			using namespace std;
			using namespace External;
			using namespace External::Validities;
			using namespace External::Groupalarm;
			using namespace External::Validities;
			using namespace External::Email;
			using namespace Utilities::Time;

			CAlarmMessageDatabase database;

			database.Add( code1, DEFAULT_VALIDITY, make_shared< CGroupalarmMessage >( groupalarmDefault ) );
			database.Add( code1, make_shared< CWeeklyValidity >( weeklyException ), make_shared< CGroupalarmMessage >( groupalarmEmpty ) );
			database.Add( code1, make_shared< CMonthlyValidity >( monthlyException ), make_shared< CGroupalarmMessage >( groupalarmException ) );
			database.Add( code3, DEFAULT_VALIDITY, make_shared< CGroupalarmMessage >( groupalarmDefault ) );
			database.Add( code3, DEFAULT_VALIDITY, make_shared< CEmailMessage>( email ) );

			auto groupalarmDefaultVec = { make_shared< CGroupalarmMessage >( groupalarmDefault ) };
			CAlarmValidities alarmDataset( DEFAULT_VALIDITY, begin( groupalarmDefaultVec ), end( groupalarmDefaultVec ) );
			database.Add( code2, alarmDataset );
			database.Add(code2, make_shared< CMonthlyValidity > ( monthlyException ), make_shared< CGroupalarmMessage >( groupalarmException ) );

			return database;
		}



		/** @brief		Generating a test database containing "all" and "fallback" alarms
		*/
		External::CAlarmMessageDatabase GenerateDatabaseWithAllAndFallback( void ) {
			using namespace std;
			using namespace External;
			using namespace External::Validities;
			using namespace External::Groupalarm;
			using namespace External::Email;
			using namespace External::Infoalarm;

			CAlarmMessageDatabase database = GenerateDatabase();

			auto infoalarmMessages = { make_shared<CInfoalarmMessageDecorator>( make_unique<CGroupalarmMessage>( groupalarmInfo ) ) };
			CAlarmValidities infoalarmDataset( DEFAULT_VALIDITY, begin( infoalarmMessages ), end( infoalarmMessages ) );
			auto allExceptionMessages = { make_shared<CGroupalarmMessage>( groupalarmDefault ) };
			infoalarmDataset.AddEntry( make_shared<CWeeklyValidity>( weeklyException2 ), begin( allExceptionMessages ), end( allExceptionMessages ) );
			database.ReplaceForAllCodes( infoalarmDataset );

			auto fallbackMessages = { make_shared<CEmailMessage>( emailFallback ) };
			CAlarmValidities fallbackDataset( DEFAULT_VALIDITY, begin( fallbackMessages ), end( fallbackMessages ) );
			auto fallbackExceptionMessages = { make_shared<CGroupalarmMessage>( groupalarmException ) };
			fallbackDataset.AddEntry( make_shared<CWeeklyValidity>( weeklyException2 ), begin( fallbackExceptionMessages ), end( fallbackExceptionMessages ) );
			database.ReplaceFallback( fallbackDataset );

			return database;
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( AlarmMessageDatabase_test_suite );

		/**	@brief		Testing of addition and removal of the data to the database
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_add_remove_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace Validities;

			// generate database
			CAlarmMessageDatabase database = GenerateDatabaseWithAllAndFallback();
		
			// check database size
			BOOST_REQUIRE( database.Size() == 3 );

			// delete database completely
			database.Remove( code1, make_shared< CWeeklyValidity >( weeklyException ) );
			database.Remove( code2, make_shared< CMonthlyValidity >( monthlyException ) );
			database.Remove( code1, make_shared< CMonthlyValidity >( monthlyException ) );
			database.Remove( code2, DEFAULT_VALIDITY );
			database.Remove( code3 );
			database.Remove( code1, DEFAULT_VALIDITY );
			BOOST_REQUIRE( database.Size() == 0 );

			// testing invalid arguments
			vector<int> emptyCode;
			auto groupalarmDefaultVec = { make_shared< CGroupalarmMessage >( groupalarmDefault ) };
			CAlarmValidities alarmDataset( DEFAULT_VALIDITY, begin( groupalarmDefaultVec ), end( groupalarmDefaultVec ) );
			CAlarmValidities emptyAlarmDataset;
			BOOST_CHECK_THROW( database.Add( emptyCode, alarmDataset ), std::length_error );
			BOOST_CHECK_THROW( database.Add( code1, emptyAlarmDataset ), std::logic_error );	// this should be caught by the non-existing default dataset
			BOOST_CHECK_THROW( database.Remove( emptyCode ), std::logic_error );				// this should be simply caught because of the non-existing empty code in the database

			BOOST_CHECK_THROW( database.Add( emptyCode, DEFAULT_VALIDITY, make_shared< CGroupalarmMessage >( groupalarmDefault ) ), std::length_error );
			BOOST_CHECK_NO_THROW( database.Add( code4, DEFAULT_VALIDITY, make_shared< CGroupalarmMessage >( groupalarmEmpty ) ) );
			vector< std::shared_ptr< CAlarmMessage > > groupalarmGet4;
			database.GetDataset( code4 ).GetEntry( DEFAULT_VALIDITY, back_inserter( groupalarmGet4 ) );
			BOOST_REQUIRE( groupalarmGet4.size() == 1 );
			BOOST_REQUIRE( *( dynamic_pointer_cast< CGroupalarmMessage >( groupalarmGet4.front() ) ) == groupalarmEmpty );

			shared_ptr< Validities::CValidity > emptyValidity;
			BOOST_CHECK_THROW( database.Add( code4, emptyValidity, make_shared< CGroupalarmMessage >( groupalarmDefault ) ), std::runtime_error );

			BOOST_CHECK_THROW( database.Remove( emptyCode, DEFAULT_VALIDITY ), std::logic_error );	// this should only be caught by the exception signalling the non-exisiting code
			BOOST_CHECK_THROW( database.Remove( code4, emptyValidity ), std::runtime_error );
		}



		/**	@brief		Testing of addition and removal of the data to the database
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_replace_all_fallback_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace External::Email;
			using namespace External::Validities;

			// generate database
			CAlarmMessageDatabase database = GenerateDatabaseWithAllAndFallback();

			auto allMessages = { make_shared<CEmailMessage>( email ) };
			CAlarmValidities allAlarmsSet( DEFAULT_VALIDITY, begin( allMessages ), end( allMessages ) );
			database.ReplaceForAllCodes( allAlarmsSet );

			auto allAlarmsGet = database.GetAlarmsForAllCodes();
			BOOST_REQUIRE( allAlarmsSet == allAlarmsGet );

			auto fallbackMessages = { make_shared<CGroupalarmMessage>( groupalarmDefault ) };
			CAlarmValidities fallbackAlarmsSet( DEFAULT_VALIDITY, begin( fallbackMessages ), end( fallbackMessages ) );
			database.ReplaceFallback( fallbackAlarmsSet );

			auto fallbackAlarmsGet = database.GetFallbackAlarms();
			BOOST_REQUIRE( fallbackAlarmsSet == fallbackAlarmsGet );

			// test invalid arguments
			BOOST_CHECK_THROW( database.ReplaceForAllCodes( CAlarmValidities() ), std::logic_error );
			BOOST_CHECK_THROW( database.ReplaceFallback( CAlarmValidities() ), std::logic_error );
		}



		/**	@brief		Testing of the comparison operators
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_comparison_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			CAlarmMessageDatabase database1, database2;
			vector< CGroupalarmMessage > groupalarm;

			database1 = GenerateDatabaseWithAllAndFallback();
			database2 = GenerateDatabaseWithAllAndFallback();
			database2.Add( code3, make_shared< Validities::CWeeklyValidity >( weeklyException ), make_shared< CGroupalarmMessage >( groupalarmDefault ) );

			// test valid comparisons
			BOOST_REQUIRE( database1 == database1 );
			BOOST_REQUIRE( !( database1 == database2 ) );
			BOOST_REQUIRE( ( database1 != database2 ) );
			BOOST_REQUIRE( CAlarmMessageDatabase() == CAlarmMessageDatabase() );
		}



		/**	@brief		Testing of searching in the database
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_search_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace Utilities::Time;
			using namespace boost::posix_time;
			using namespace boost::gregorian;
			string messageText, messageText2;
			bool isDefaultDataset1, isDefaultDataset2;

			// generate database (without "all" and "fallback" alarms)
			CAlarmMessageDatabase database = GenerateDatabase();

			// search in the database
			german_local_date_time currTime( date( 2013, Apr, 5 ), hours( 19 ) + minutes( 45 ) + seconds( 0 ) );
			dynamic_pointer_cast< CGroupalarmMessage >( database.Search( code1, CBoostStdTimeConverter::ConvertToStdTime( currTime.utc_time() ), isDefaultDataset1 ).front() )->GetAlarmMessage( messageText );
			BOOST_REQUIRE( !isDefaultDataset1 );
			BOOST_REQUIRE( messageText == "exception" );

			german_local_date_time currTime2( date( 2013, Apr, 5 ), hours( 19 ) + minutes( 11 ) + seconds( 59 ) );
			dynamic_pointer_cast< CGroupalarmMessage >( database.Search(code1, CBoostStdTimeConverter::ConvertToStdTime( currTime2.utc_time() ), isDefaultDataset2 ).front() )->GetAlarmMessage( messageText2 );
			BOOST_REQUIRE( isDefaultDataset2 );
			BOOST_REQUIRE( messageText2 == "default" );

			// testing invalid arguments
			BOOST_CHECK_THROW( database.Search( notExistingCode, CBoostStdTimeConverter::ConvertToStdTime( currTime.utc_time() ) ), std::logic_error );

			vector<int> emptyCode;
			BOOST_CHECK_THROW( database.Search( emptyCode, CBoostStdTimeConverter::ConvertToStdTime( currTime.utc_time() ) ), std::logic_error );

			BOOST_CHECK_THROW( database.Search( code1, Utilities::CDateTime() ), std::out_of_range );
		}



		/**	@brief		Testing of searching in the database when "all" and "fallback" datasets are chosen
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_search_all_fallback_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Email;
			using namespace External::Groupalarm;
			using namespace External::Infoalarm;
			using namespace Utilities::Time;
			using namespace boost::posix_time;
			using namespace boost::gregorian;

			map<type_index, unsigned int> foundMessageTypes1, foundMessageTypes2, foundMessageTypes3;
			bool isDefaultDataset1, isDefaultDataset2, isDefaultDataset3, isDefaultDataset4;
			string messageText4;

			// generate database
			CAlarmMessageDatabase database = GenerateDatabaseWithAllAndFallback();

			german_local_date_time currTimeA( date( 2013, Apr, 5 ), hours( 19 ) + minutes( 45 ) + seconds( 0 ) );
			auto timeA = CBoostStdTimeConverter::ConvertToStdTime( currTimeA.utc_time() );

			german_local_date_time currTimeB( date( 2013, Apr, 7 ), hours( 19 ) + minutes( 30 ) + seconds( 0 ) );
			auto timeB = CBoostStdTimeConverter::ConvertToStdTime( currTimeB.utc_time() );

			// search in the database
			// case with exception valid for the specified code, default valid for all alarms
			auto messages = database.Search( code1, timeA, isDefaultDataset1 );
			for ( auto const& message : messages ) {
				foundMessageTypes1[typeid( *message )]++;
			}
			BOOST_REQUIRE( !isDefaultDataset1 );
			BOOST_REQUIRE( foundMessageTypes1.size() == 2 );
			BOOST_REQUIRE( foundMessageTypes1.at( typeid( CInfoalarmMessageDecorator ) ) == 1 ); // an "all" infoalarm message needs to be present

			// case with only the default of the fallback valid
			auto messages2 = database.Search( notExistingCode, timeA, isDefaultDataset2 );
			for ( auto const& message2 : messages2 ) {
				foundMessageTypes2[typeid( *message2 )]++;
			}
			BOOST_REQUIRE( isDefaultDataset2 );
			BOOST_REQUIRE( foundMessageTypes2.size() == 2 ); // "fallback" and "infoalarm" need to be present
			BOOST_REQUIRE( foundMessageTypes2.at( typeid( CEmailMessage ) ) == 1 ); // the "fallback" message needs to be present
			BOOST_REQUIRE( foundMessageTypes2.at( typeid( CInfoalarmMessageDecorator ) ) == 1 ); // an "all" infoalarm message needs to be present

			// case with exception valid in "all" messages
			auto messages3 = database.Search( code1, timeB, isDefaultDataset3 );
			for ( auto const& message3 : messages3 ) {
				foundMessageTypes3[typeid( *message3 )]++;
			}
			BOOST_REQUIRE( isDefaultDataset3 ); // the "all" alarms are irrelevant for default / exception status
			BOOST_REQUIRE( foundMessageTypes3.size() == 1 ); // only a groupalarm message should be present
			BOOST_REQUIRE( foundMessageTypes3.at( typeid( CGroupalarmMessage ) ) == 2 );
			BOOST_REQUIRE( foundMessageTypes3.count( typeid( CInfoalarmMessageDecorator ) ) == 0 ); // no infoalarm message should be present

			// case with exception valid in the "fallback" message
			auto messages4 = database.Search( notExistingCode, timeB, isDefaultDataset4 );
			BOOST_REQUIRE( !isDefaultDataset4 ); // the "fallback" alarm is relevant
			BOOST_REQUIRE( messages4.size() == 2 ); // only the "fallback" and "all" groupalarm messages should be present
			for ( auto const& message4 : messages4 ) {
				string messageText;
				dynamic_pointer_cast<CGroupalarmMessage>( message4 )->GetAlarmMessage( messageText );
				BOOST_REQUIRE( ( messageText == "default" ) || ( messageText == "exception" ) );
			}
		}



		/**	@brief		Testing of helper functions of the database
		*/
		BOOST_AUTO_TEST_CASE( AlarmMessageDatabase_helper_functions_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Groupalarm;
			using namespace Utilities::Time;
			using namespace boost::posix_time;
			using namespace boost::gregorian;

			// generate database
			CAlarmMessageDatabase database = GenerateDatabaseWithAllAndFallback();

			// check database entries
			CAlarmValidities getAlarmDataset1 = database.GetDataset( code1 );
			vector< std::shared_ptr< CAlarmMessage > > getAlarmData;
			getAlarmDataset1.GetEntry( make_shared< Validities::CWeeklyValidity >( weeklyException ), back_inserter( getAlarmData ) );
			BOOST_REQUIRE( getAlarmData.size() == 1 );
			BOOST_REQUIRE( getAlarmData.front()->IsEmpty() == true );

			Types::AlarmDatabaseType getDatabase = database.GetDatabase();
			BOOST_REQUIRE( getDatabase.size() == 3 );

			BOOST_REQUIRE( database.GetAllMessageTypes().size() == 2 );

			BOOST_REQUIRE( database.GetAllCodes().size() == 3 );

			database.Clear();
			BOOST_REQUIRE( database.Size() == 0 );

			// testing invalid arguments
			vector<int> emptyCode;
			BOOST_CHECK_THROW( database.GetDataset( emptyCode ), std::logic_error );
			BOOST_CHECK_THROW( database.GetDataset( notExistingCode ), std::logic_error );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
