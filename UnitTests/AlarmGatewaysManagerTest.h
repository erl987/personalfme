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
#include <set>
#include <map>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/test/unit_test.hpp>
#include "BoostStdTimeConverter.h"
#include "DefaultValidity.h"
#include "SingleTimeValidity.h"
#include "EmailMessage.h"
#include "EmailLoginData.h"
#include "EmailGateway.h"
#include "Groupalarm2Message.h"
#include "Groupalarm2LoginData.h"
#include "Groupalarm2Gateway.h"
#include "GatewayLoginDatabase.h"
#include "AlarmMessagesDatabase.h"
#include "SendStatusMessage.h"
#include "InfoalarmMessageDecorator.h"
#include "AlarmGatewaysManager.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup GatewayTests
	*/
	namespace AlarmGatewaysManagerTest {
		std::vector<int> code = { 2, 3, 7, 9, 9 }; // test FME-code

		std::vector< std::unique_ptr<External::CAlarmMessage> > receivedAlarmMessageList;
		std::mutex receivedAlarmMessageMutex;
		std::condition_variable receivedAlarmMessageCondition;
		bool isReceivedAlarmMessageReady = false;


		void OnStatusChangeAdapter( const Utilities::Message::CStatusMessage& statusMessage ) {
			using namespace Utilities::Message;
			unsigned int numTrials;
			SendStatus status;
			std::chrono::seconds timeDistTrials;

			dynamic_cast< const CSendStatusMessage< External::CAlarmMessage>& > ( statusMessage ).GetStatusInfo( status, numTrials, timeDistTrials );
			std::cout << "Status change: ";
			switch ( status.code ) {
			case IN_PROCESSING:
				std::cout << "in processing";
				break;
			case FATAL_FAILURE:
				std::cout << "fatal failure: " << status.text;
				break;
			case TIMEOUT_FAILURE:
				std::cout << "timeout failure";
				break;
			case NONFATAL_FAILURE:
				std::cout << "non-fatal failure: " << status.text;
				break;
			case SUCCESS:
				std::cout << "success";
				break;
			}

			std::cout << ", number of trials: " << numTrials << std::endl;
		}


		void OnStatusChange( std::unique_ptr<Utilities::Message::CStatusMessage> statusMessage )
		{
			OnStatusChangeAdapter( *statusMessage );
		}


		void OnStatusChangeWithCallback( std::unique_ptr<Utilities::Message::CStatusMessage> statusMessage )
		{
			using namespace Utilities::Message;
			std::unique_ptr<External::CAlarmMessage> receivedAlarmMessage;
			bool doNotify = false;

			OnStatusChangeAdapter( *statusMessage );

			{
				std::lock_guard<std::mutex> lock( receivedAlarmMessageMutex );
				dynamic_cast<const CSendStatusMessage< External::CAlarmMessage>&> ( *statusMessage ).GetAlarmMessage( receivedAlarmMessage );
				receivedAlarmMessageList.push_back( move( receivedAlarmMessage ) );
				if ( receivedAlarmMessageList.size() == 3 ) { // we should receive three messages
					doNotify = true;
					isReceivedAlarmMessageReady = true;
				}				
			}

			if ( doNotify ) {
				receivedAlarmMessageCondition.notify_all();
			}
		}


		void OnException( const std::exception_ptr& error )
		{
			std::rethrow_exception( error );
		}


		External::CGatewayLoginDatabase GetTestLoginDB()
		{
			using namespace std;
			using namespace External;

			unique_ptr< CGatewayLoginData > emailLoginData( new Email::CEmailLoginData );
			unique_ptr< CGatewayLoginData > groupalarmLoginData( new Groupalarm::CGroupalarm2LoginData );

			CGatewayLoginDatabase loginDatabase;

			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetServerInformation( Email::TLS_SSL_CONN, "mail.gmx.net" );
			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetLoginInformation( "bob.foo@provider.org", Email::UNENCRYPTED_AUTH, "bob.foo@provider.org", "" );
			dynamic_cast< Email::CEmailLoginData* >( emailLoginData.get() )->SetConnectionTrialInfos( 10, 30.0, 1 );

			dynamic_cast<Groupalarm::CGroupalarm2LoginData*>(groupalarmLoginData.get())->Set(12345, "aToken", "proxy.provider.org", 8080, "aUser", "aPasswd");
			dynamic_cast< Groupalarm::CGroupalarm2LoginData* >( groupalarmLoginData.get() )->SetConnectionTrialInfos( 10, 30.0, 1 );

			loginDatabase.Add( make_unique<Email::CEmailGateway>(), move( emailLoginData ) );
			loginDatabase.Add( make_unique<Groupalarm::CGroupalarm2Gateway>(), move( groupalarmLoginData ) );

			return loginDatabase;
		}


		External::CAlarmMessageDatabase GetTestMessageDB()
		{
			using namespace std;
			using namespace Utilities;
			using namespace External;

			CAlarmMessageDatabase messageDatabase;

			vector< pair<string, string> > tempRecipients = { { "Bob Foo", "bob.foo@provider.org" } };
			shared_ptr< CAlarmMessage > alarmMessage( new Email::CEmailMessage( "Organisation", "Role", tempRecipients, u8"Einsatz! Kontakt mit der Leitstelle aufnehmen.", true ) );
			messageDatabase.Add( code, Validities::DEFAULT_VALIDITY, alarmMessage );
			shared_ptr< CAlarmMessage > alarmMessage2(new Groupalarm::CGroupalarm2Message({ {"label 1", 2}, {"label 2", 1} }, { "scenario 1", "scenario 2" }, { "unit 1", "unit 2" }, { make_pair<>("Bob", "Foo"), make_pair<>("Alice", "Bar") }, "A message!", "", 2.53));
			messageDatabase.Add( code, Validities::DEFAULT_VALIDITY, alarmMessage2 );
			shared_ptr< Validities::CSingleTimeValidity > singleTimeException = make_shared<Validities::CSingleTimeValidity>( CDateTime( 1, 1, 2015, CTime( 2, 0, 0 ) ), CDateTime( 1, 1, 2015, CTime( 3, 0, 0 ) ) ); // local time
			messageDatabase.Add( code, singleTimeException, make_shared< Groupalarm::CGroupalarm2Message >() );

			Groupalarm::CGroupalarm2Message infoMessage({ {"label 1", 2}, {"label 2", 1} }, { "scenario 1", "scenario 2" }, { "unit 1", "unit 2" }, { make_pair<>("Bob", "Foo"), make_pair<>("Alice", "Bar") }, "An infoalarm message!", "", 2.53);
			unique_ptr<CAlarmMessage> infoGroupalarmMessage = make_unique<Groupalarm::CGroupalarm2Message>(infoMessage);
			unique_ptr<CAlarmMessage> infoalarmMessage = make_unique<Infoalarm::CInfoalarmMessageDecorator>( move( infoGroupalarmMessage ) );
			
			messageDatabase.Add( code, Validities::DEFAULT_VALIDITY, move( infoalarmMessage ) );

			return messageDatabase;
		}


			
		// Test section
		BOOST_AUTO_TEST_SUITE( AlarmGatewaysManager_test_suite, *label("default") );

		/**	@brief		Testing the login database handling
		*/
		BOOST_AUTO_TEST_CASE( loginDatabase_test_case )
		{
			External::CGatewayLoginDatabase loginDatabase, getLoginDatabase;
			External::CAlarmGatewaysManager gateways( OnStatusChange, OnException );

			// testing wrong inputs
			BOOST_REQUIRE_THROW( gateways.GetGatewayLoginDatabase( getLoginDatabase ), std::logic_error ); // no database has yet been loaded
			BOOST_CHECK_NO_THROW( gateways.ResetGatewayLoginDatabase( External::CGatewayLoginDatabase() ) );
			gateways.GetGatewayLoginDatabase( getLoginDatabase );
			BOOST_REQUIRE( getLoginDatabase.GetAllEntries().empty() );

			// test standard usage
			loginDatabase = GetTestLoginDB();
			BOOST_CHECK_NO_THROW( gateways.ResetGatewayLoginDatabase( loginDatabase ) );
			BOOST_CHECK_NO_THROW( gateways.GetGatewayLoginDatabase( getLoginDatabase ) );
			BOOST_REQUIRE( loginDatabase == getLoginDatabase );
		}


		/**	@brief		Testing the alarm message database handling
		*/
		BOOST_AUTO_TEST_CASE( alarmMessagesDatabase_test_case )
		{
			External::CAlarmMessageDatabase messageDatabase, getMessageDatabase;
			External::CAlarmGatewaysManager gateways( OnStatusChange, OnException );

			BOOST_REQUIRE_THROW( gateways.GetAlarmMessagesDatabase( getMessageDatabase ), std::logic_error ); // no database has yet been loaded
			BOOST_CHECK_NO_THROW( gateways.ResetAlarmMessagesDatabase( External::CAlarmMessageDatabase() ) );
			gateways.GetAlarmMessagesDatabase( getMessageDatabase );
			BOOST_REQUIRE( getMessageDatabase.Size() == 0 );

			// test standard usage
			messageDatabase = GetTestMessageDB();
			BOOST_CHECK_NO_THROW( gateways.ResetAlarmMessagesDatabase( messageDatabase ) );
			BOOST_CHECK_NO_THROW( gateways.GetAlarmMessagesDatabase( getMessageDatabase ) );
			BOOST_REQUIRE( getMessageDatabase == messageDatabase );
		}


		/**	@brief		Testing the sending method
		*/
		BOOST_AUTO_TEST_CASE( send_test_case )
		{
			using namespace std;
			using namespace External;
			using namespace External::Infoalarm;

			CAlarmMessageDatabase alarmDatabase;
			CGatewayLoginDatabase loginDatabase;
			External::CAlarmGatewaysManager gateways( OnStatusChangeWithCallback, OnException );
			vector<int> code2 = { 1, 2, 3, 4, 5 };
			vector< shared_ptr<CAlarmMessage> > otherMessagesSet;
			map< type_index, unique_ptr<CAlarmMessage> > messageTypesGet;
			set<type_index> otherMessagesTypeGet;

			loginDatabase = GetTestLoginDB();

			// test the behaviour in case of wrong initialization (not loading the databases)
			auto eventTime = Utilities::CDateTime( 10, 11, 2015, Utilities::CTime( 13, 58, 2, 234 ) ); // UTC
			BOOST_REQUIRE_THROW( gateways.Send( code, eventTime, Utilities::CMediaFile(), true ), std::runtime_error );

			// test the alarm gateway
			alarmDatabase = GetTestMessageDB();
			gateways.ResetGatewayLoginDatabase( loginDatabase );
			gateways.ResetAlarmMessagesDatabase( alarmDatabase );
			BOOST_CHECK_NO_THROW( gateways.Send( code, eventTime, Utilities::CMediaFile(), true ) );

			// asynchronous sending
			unique_lock<mutex> lock( receivedAlarmMessageMutex );
			receivedAlarmMessageCondition.wait_for( lock, 15s, []() { return isReceivedAlarmMessageReady; } );
			BOOST_REQUIRE( isReceivedAlarmMessageReady );

			for ( auto const& receivedMessage : receivedAlarmMessageList ) {
				messageTypesGet[typeid( *receivedMessage )] = receivedMessage->Clone();
			}
			BOOST_REQUIRE( messageTypesGet.size() == 3 ); // Infoalarm, Groupalarm and E-mail message should be present

			auto otherMessagesInInfoGet = dynamic_cast<const CInfoalarmMessageDecorator&>( *messageTypesGet[typeid( CInfoalarmMessageDecorator )] ).GetOtherMessages();
			for ( auto const& otherMessage : otherMessagesInInfoGet ) {
				otherMessagesTypeGet.insert( typeid( *otherMessage ) );
			}
			BOOST_REQUIRE( otherMessagesTypeGet.size() == 3 ); // based on the alarm time, all messages including the infoalarm need to be present

			// test the error conditions
			BOOST_REQUIRE_THROW( gateways.Send( code2, eventTime, Utilities::CMediaFile(), true ), std::logic_error ); // the code2 is not in the database

			auto eventTime2 = Utilities::CDateTime( 1, 1, 2015, Utilities::CTime( 1, 30, 0, 0 ) ); // UTC
			BOOST_REQUIRE_THROW( gateways.Send( code, eventTime2, Utilities::CMediaFile(), true ), std::range_error ); // there is no alarm for the present time (due to an exception)
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
