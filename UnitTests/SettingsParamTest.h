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
#include "AudioDevice.h"
#include "GatewayLoginDatabase.h"
#include "EmailGateway.h"
#include "Groupalarm2Gateway.h"
#include "EmailLoginData.h"
#include "Groupalarm2LoginData.h"
#include "EmailMessage.h"
#include "Groupalarm2Message.h"
#include "AlarmMessagesDatabase.h"
#include "WeeklyValidity.h"
#include "DefaultValidity.h"
#include "SettingsParam.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Middleware
*/
namespace Middleware {
	namespace SettingsParamTest {
		// Test section
		BOOST_AUTO_TEST_SUITE( SettingsParam_test_suite, *label("default") );

		/**	@brief		Testing of the class
		*/
		BOOST_AUTO_TEST_CASE( test_case )
		{
			using namespace std;
			using namespace External;

			External::CGatewayLoginDatabase gatewayDatabase;
			External::Email::CEmailLoginData emailLoginData;
			External::Groupalarm::CGroupalarm2LoginData groupalarmLoginData;
			Middleware::CSettingsParam settings;
			External::CAlarmMessageDatabase alarmDatabase;
			External::CAlarmValidities validity1, validity2;
			vector< shared_ptr<External::CAlarmMessage> > alarmMessages, alarmMessagesEmpty;
			Email::CEmailMessage emailMessage;
			Validities::CWeeklyValidity weeklyException;
			Middleware::CSettingsParam setSettings, getSettings;

			setSettings.SetFunctionalitySettings( Core::Processing::CAudioDevice( Core::IN_DEVICE, "Microsoft", "MME" ), 10.0, false );
			setSettings.SetRecordingSettings( 15.0, false, "OGG" );

			// prepare the gateways login database
			emailLoginData.SetConnectionTrialInfos( 7, 19.3f, 5 );
			emailLoginData.SetLoginInformation( "First Last <first.last@test.de>", External::Email::UNENCRYPTED_AUTH, "user3", "password10" );
			emailLoginData.SetServerInformation( External::Email::TLS_SSL_CONN, "smtp.host.de", 465 );
			gatewayDatabase.Add( make_unique<External::Email::CEmailGateway>(), make_unique<External::Email::CEmailLoginData>( emailLoginData ) );

			groupalarmLoginData.SetConnectionTrialInfos( 7, 19.3f, 5 );
			groupalarmLoginData.Set(12345, "aToken", "proxy.provider.org", 8080, "aUser", "aPasswd");
			gatewayDatabase.Add( make_unique<External::Groupalarm::CGroupalarm2Gateway>(), make_unique<External::Groupalarm::CGroupalarm2LoginData>( groupalarmLoginData ) );

			// prepare the alarm messages database
			vector<int> code1 = { 2, 5, 6, 3, 4 };
			vector<int> code2 = { 1, 2, 3, 4, 5 };

			vector< pair<string, string> > recipients = { { "", "first.last@test.de" },{ "First2 Last2", "first2.last2@test.de" } };
			emailMessage.Set( "siteID", "alarmID", recipients, "alarm text" );
			emailMessage.SetRequiredState( false );
			alarmMessages.push_back( make_shared<Email::CEmailMessage>( emailMessage ) );

			validity1.AddEntry( Validities::DEFAULT_VALIDITY, begin( alarmMessages ), end( alarmMessages ) );
			weeklyException = Validities::CWeeklyValidity( Utilities::Time::SUNDAY, Utilities::CTime( 7, 9, 4 ), Utilities::CTime( 7, 19, 52 ) );
			alarmMessagesEmpty.clear();
			validity1.AddEntry( make_shared<Validities::CWeeklyValidity>( weeklyException ), begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

			validity2.AddEntry( Validities::DEFAULT_VALIDITY, begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

			alarmDatabase.Add( code1, validity1 );
			alarmDatabase.Add( code2, validity2 );

			setSettings.SetGatewaySettings( gatewayDatabase, alarmDatabase );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/