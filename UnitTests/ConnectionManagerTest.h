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

#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "ConnectionMocks.h"
#include "SendStatusMessage.h"
#include "BoostStdTimeConverter.h"
#include "ConnectionManager.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup ConnectionManagerTest
	*/
	namespace ConnectionManagerTest {
		const unsigned int maxNumTrials = 10;
		const float timeDistTrials = 0.2f; // in s
		const unsigned int maxNumConnections = 4;

		const Utilities::CMediaFile audioFile;
		const bool isRealAlarm = true;
		const std::vector<int> sequence = { 1, 2, 3, 4, 5 };

		const unsigned int numTestMessages = 20;
		const int minDelayTime = 0;		// in ms
		const int maxDelayTime = 10;	// in ms

		typedef std::pair< boost::posix_time::ptime, std::vector<int> > KeyType;
		typedef std::vector< std::pair< Utilities::Message::SendStatus, std::chrono::time_point<std::chrono::steady_clock> > > ValueType;
		std::map< KeyType, ValueType > messagesDB;

		
		/**	@brief	Callback function for receiving the status changes of the messages
		*/
		void OnStatusChange( std::unique_ptr<Utilities::Message::CStatusMessage> message )
		{
			using namespace std;
			using namespace Utilities::Time;
			using namespace Utilities::Message;
			SendStatus status;
			unsigned int numTrials;
			chrono::seconds timeDistTrials;
			Utilities::CDateTime sequenceTime;
			vector<int> code;

			dynamic_cast<CSendStatusMessage<External::CAlarmMessage>*>( message.get() )->GetSequenceInfo( sequenceTime, code );
			auto currTimestamp = chrono::steady_clock::now();
			dynamic_cast<CSendStatusMessage<External::CAlarmMessage>*>( message.get() )->GetStatusInfo( status, numTrials, timeDistTrials );
			if ( !messagesDB.empty() ) { // stores the data only if required for the test
				messagesDB.at( make_pair( CBoostStdTimeConverter::ConvertToBoostTime( sequenceTime ), code ) ).push_back( make_pair( status, currTimestamp ) );
			}
		}


		/** @brief	Callback function used for signalling exception in the thread class */
		void OnThreadException( const std::exception_ptr& error ) {
			// in production code the notification of the caller thread is required here (followed by std::rethrow_exception( error )
		}


		// Test section
		BOOST_AUTO_TEST_SUITE( ConnectionManager_test_suite, *label("basic") );

		/**	@brief		Testing of the construction of the class
		*/
		BOOST_AUTO_TEST_CASE( construction_test_case )
		{
			using namespace std::literals;
			using namespace ConnectionMocks;

			std::unique_ptr< External::CGatewayLoginData > login( new CMockLoginData );
			login->SetConnectionTrialInfos( maxNumTrials, timeDistTrials, maxNumConnections );
			std::unique_ptr< External::CAlarmMessage > message1( new CMockMessage( "message 1" ) );
			std::unique_ptr< External::CAlarmMessage > message2( new CMockMessage( "message 2" ) );
			Utilities::CDateTime time;

			BOOST_CHECK_NO_THROW( External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), login->Clone(), OnStatusChange, OnThreadException ) );
			BOOST_CHECK_THROW( External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), login->Clone(), OnStatusChange, nullptr ), std::runtime_error );
			BOOST_CHECK_THROW( External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), login->Clone(), nullptr, OnThreadException ), std::runtime_error );
			BOOST_CHECK_THROW( External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), nullptr, OnStatusChange, OnThreadException ), std::runtime_error );
		}


		/**	@brief		Basic testing
		*/
		BOOST_AUTO_TEST_CASE( basic_test_case )
		{
			using namespace std::literals;
			using namespace ConnectionMocks;

			std::unique_ptr< External::CGatewayLoginData > login( new CMockLoginData );
			login->SetConnectionTrialInfos( maxNumTrials, timeDistTrials, maxNumConnections );
			std::unique_ptr< External::CAlarmMessage > message1( new CMockMessage( "message 1" ) );
			std::unique_ptr< External::CAlarmMessage > message2( new CMockMessage( "message 2" ) );
			std::unique_ptr< External::CAlarmMessage > message3( new CMockMessage( "message 3" ) );
			Utilities::CDateTime time;

			External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), login->Clone(), OnStatusChange, OnThreadException );
			time = Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( boost::posix_time::microsec_clock::universal_time() );

			BOOST_CHECK_NO_THROW( connectionManager.AddMessage( sequence, time, isRealAlarm, std::move( message1 ), audioFile ) );

			// wait before adding the next message
			std::this_thread::sleep_for( 200ms );
			BOOST_CHECK_NO_THROW( connectionManager.AddMessage( sequence, time, isRealAlarm, std::move( message2 ), audioFile ) );

			// wait for some time before finishing
			std::this_thread::sleep_for( 200ms );

			BOOST_CHECK_THROW( connectionManager.AddMessage( sequence, Utilities::CDateTime(), isRealAlarm, std::move( message3 ), audioFile ), std::runtime_error );
			BOOST_CHECK_THROW( connectionManager.AddMessage( sequence, time, isRealAlarm, nullptr, audioFile ), std::runtime_error );
		}


		/**	@brief		High load test case
		*/
		BOOST_AUTO_TEST_CASE( load_test_case )
		{
			using namespace std;
			using namespace literals;
			using namespace ConnectionMocks;
			using namespace Utilities::Time;

			vector< chrono::milliseconds > timeDists;
			vector<bool> isCorrectEnd, isCorrectNumTrials;
			map<unsigned int, unsigned int> numTrialsDistrib;
			Utilities::CDateTime time;
			vector<int> currCode( 5 );

			mt19937_64 eng{ random_device{}( ) };
			uniform_int_distribution<> timeDist{ minDelayTime, maxDelayTime };
			uniform_int_distribution<> codeDist{ 0, 9 };
			unique_ptr< External::CGatewayLoginData > login( new CMockLoginData );
			login->SetConnectionTrialInfos( maxNumTrials, timeDistTrials, maxNumConnections );
			vector< unique_ptr< External::CAlarmMessage > > testMessages( numTestMessages );
			vector< vector<int> > codes( numTestMessages );
			External::CConnectionManager connectionManager( std::make_unique<CMockGateway>(), login->Clone(), OnStatusChange, OnThreadException );

			// prepare the test
			for ( size_t i = 0; i < testMessages.size(); i++) {
				testMessages[i] = make_unique<CMockMessage>( "message" + boost::lexical_cast<string>( i ) );
				for ( size_t n = 0; n < currCode.size(); n++ ) {
					currCode[n] = codeDist( eng );
				}
				codes[i] = currCode;
			}

			// perform the load test sending the messages within the random times
			for ( size_t i = 0; i < testMessages.size(); i++ ) {
				time = CBoostStdTimeConverter::ConvertToStdTime( boost::posix_time::microsec_clock::universal_time() );
				messagesDB[ make_pair( CBoostStdTimeConverter::ConvertToBoostTime( time ), codes[i] ) ] = ValueType();
				connectionManager.AddMessage( codes[i], time, isRealAlarm, testMessages[i]->Clone(), audioFile );

				// wait for random time
				this_thread::sleep_for( chrono::milliseconds{ timeDist( eng ) } );
			}

			// wait for some time before finally finishing
			this_thread::sleep_for( 3s );

			// analyze the correctness of the results
			// check if all messages are finished either because of success or timeout failure
			transform( begin( messagesDB ), end( messagesDB ), back_inserter( isCorrectEnd ), [=]( auto entry ) { 
				return ( entry.second.back().first.code == Utilities::Message::SUCCESS ) || ( ( entry.second.back().first.code == Utilities::Message::TIMEOUT_FAILURE ) && ( entry.second.size() == maxNumTrials ) ); 
			} );
			BOOST_REQUIRE( all_of( begin( isCorrectEnd ), end( isCorrectEnd ), []( auto entry ) { return entry; } ) );

			// check if all messages have between 1 and maxNumTrials trials
			transform( begin( messagesDB ), end( messagesDB ), back_inserter( isCorrectNumTrials ), [=]( auto entry ) {
				return ( entry.second.size() >= 1 ) && ( entry.second.size() <= maxNumTrials );
			} );
			BOOST_REQUIRE( all_of( begin( isCorrectNumTrials ), end( isCorrectNumTrials ), []( auto entry ) { return entry; } ) );

			// determine the distribution of the number of trials (only manual checking possible: the distribution should ideally follow geometric series controlled by the failure probability (i.e. for example: 100 * 0.1 * 0.1 * ...))
			for ( auto entry : messagesDB ) {
				numTrialsDistrib[ entry.second.size() ]++;
			}

			// determine the distribution of the times between two trials (automatic checking of maximum times due to overload effects not possible)
			for ( auto entry : messagesDB ) {
				transform( begin( entry.second ), end( entry.second ) - 1, begin( entry.second ) + 1, back_inserter( timeDists ), []( auto entry1, auto entry2 ) {
					return chrono::duration_cast<chrono::milliseconds>( entry2.second - entry1.second );
				} );
			}
			BOOST_REQUIRE( all_of( begin( timeDists ), end( timeDists ), []( auto entry ) { return ( entry >= chrono::milliseconds( static_cast<long long>( timeDistTrials * 1000 ) ) ); } ) );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
