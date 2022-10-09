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

#include <chrono>
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "BoostStdTimeConverter.h"
#include "ConnectionMocks.h"
#include "ConnectionThread.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup ConnectionThreadTest
	*/
	namespace ConnectionThreadTest {
		const unsigned int maxNumTrials = 10;
		const float timeDistTrials = 1.0; // in s
		const unsigned int maxNumConnections = 4;

		const Utilities::CMediaFile audioFile;
		const bool isRealAlarm = true;
		const std::vector<int> sequence = { 1, 2, 3, 4, 5 };		
		
		
		/** @brief	Callback function used for signalling that the connection thread has finished sending the message */
		void OnFinishedSending() {};


		/** @brief	Callback function used for signalling exception in the thread class */
		void OnThreadException( const std::exception_ptr& error ) {
			// in production code the notification of the caller thread is required here (followed by std::rethrow_exception( error )
		}


		// Test section
		BOOST_AUTO_TEST_SUITE( ConnectionThread_test_suite, *label("basic") );

		/**	@brief		Testing of the construction of the class
		*/
		BOOST_AUTO_TEST_CASE( basic_test_case )
		{
			using namespace std;
			using namespace std::literals;
			using namespace ConnectionMocks;

			bool isValid;
			unsigned int numTrials;
			Utilities::Message::SendStatus status;
			Utilities::CDateTime time, messageTime;
			unique_ptr< External::Message > messageOut;
			vector<int> messageCode;
			Utilities::CDateTime inputTimeMessage2;
			std::unique_ptr< External::CGatewayLoginData > login( new CMockLoginData );
			login->SetConnectionTrialInfos( maxNumTrials, timeDistTrials, maxNumConnections );

			External::CConnectionThread connectionThread( make_unique< CMockGateway >(), OnFinishedSending, OnThreadException );
			unique_ptr< External::Message > message1{ new External::Message };
			message1->message = make_unique< CMockMessage >( "message1" );
			message1->audioFile = audioFile;
			message1->isRealAlarm = isRealAlarm;
			message1->login = login->Clone();
			message1->sequence = sequence;
			message1->time = Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( boost::posix_time::microsec_clock::universal_time() );

			unique_ptr< External::Message > message2{ new External::Message };
			message2->message = make_unique< CMockMessage >( "message2" );
			message2->audioFile = audioFile;
			message2->isRealAlarm = isRealAlarm;
			message2->login = login->Clone();
			message2->sequence = sequence;
			inputTimeMessage2 = Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( boost::posix_time::microsec_clock::universal_time() );
			message2->time = inputTimeMessage2;

			BOOST_CHECK_NO_THROW( connectionThread.SendMessage( std::move( message1 ), 1 ) );

			// immediately add the next message (this causes blocking in the method SendMessage)
			BOOST_CHECK_NO_THROW( connectionThread.SendMessage( std::move( message2 ), 5 ) );

			std::this_thread::sleep_for( 150ms );
			BOOST_CHECK_NO_THROW( isValid = connectionThread.GetStatus( status, messageTime, messageCode, numTrials, messageOut ) );
			BOOST_REQUIRE( isValid == true );
			BOOST_REQUIRE( ( status.code == Utilities::Message::SUCCESS ) || ( status.code == Utilities::Message::NONFATAL_FAILURE ) );
			BOOST_REQUIRE( messageTime == inputTimeMessage2 );
			BOOST_REQUIRE( messageCode == sequence );
			BOOST_REQUIRE( numTrials == 6 );
		}


		/**	@brief		Testing of the behaviour in case of wrong inputs
		*/
		BOOST_AUTO_TEST_CASE( exceptions_test_case )
		{
			using namespace std;
			using namespace ConnectionMocks;

			BOOST_CHECK_THROW( External::CConnectionThread( make_unique< CMockGateway >(), OnFinishedSending, function<void( const exception_ptr& error )>() ), std::runtime_error );
			BOOST_CHECK_THROW( External::CConnectionThread( make_unique< CMockGateway >(), nullptr, OnThreadException ), std::runtime_error );
			BOOST_CHECK_THROW( External::CConnectionThread( nullptr, OnFinishedSending, OnThreadException ), std::runtime_error );

			External::CConnectionThread connectionThread( make_unique< CMockGateway >(), OnFinishedSending, OnThreadException );
			BOOST_CHECK_THROW( connectionThread.SendMessage( nullptr, 1 ), std::runtime_error );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
