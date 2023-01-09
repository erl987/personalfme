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
#if defined _WIN32
	#include "stdafx.h"
#endif
#include "XMLUtilities.h"
#include "EmailMessage.h"
#include "Groupalarm2Message.h"
#include "InfoalarmMessageDecorator.h"
#include "DefaultValidity.h"
#include "WeeklyValidity.h"
#include "MonthlyValidity.h"
#include "SingleTimeValidity.h"
#include "XMLSerializableAlarmValidities.h"
#include "XMLAlarmValiditiesTest.h"

const boost::filesystem::path xmlFileName = "./alarmValidities.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestAlarmValiditiesSchema.xsd";
#elif defined( __linux )
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestAlarmValiditiesSchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLAlarmValiditiesTest::Test()
{
	using namespace std;
	using namespace External;
	using namespace Utilities;
	using namespace Utilities::Time;

	vector< shared_ptr<External::CAlarmMessage> > alarmMessages, alarmMessagesException, alarmMessagesEmpty, alarmMessagesMonthly, infoalarmMessages;
	Email::CEmailMessage emailMessage;
	Groupalarm::CGroupalarm2Message groupalarmMessage, groupalarmMessage2, groupalarmMessage3;
	Validities::CWeeklyValidity weeklyException, weeklyException2;
	Validities::CMonthlyValidity monthlyException;
	Validities::CSingleTimeValidity singleTimeException;
	vector<WeekType> exceptionWeeks;
	CXMLSerializableAlarmValidities setAlarmValidities, getAlarmValidities;

	groupalarmMessage.SetAlarmToAllUsers("Alarm for all!", "", 2.5);
	alarmMessages.push_back(make_shared<Groupalarm::CGroupalarm2Message>(groupalarmMessage));

	vector< pair<string, string> > recipients = { { "", "first.last@test.de" },{ "First2 Last2", "first2.last2@test.de" } };
	emailMessage.Set("siteID", "alarmID", recipients, "alarm text");
	emailMessage.SetRequiredState(false);
	alarmMessages.push_back(make_shared<Email::CEmailMessage>(emailMessage));
	alarmMessages.push_back(make_shared<Infoalarm::CInfoalarmMessageDecorator>(emailMessage.Clone()));

	groupalarmMessage2.SetAlarmToDefinedUsers({ {"label1", 2}, {"label2", 1} }, { "scenario1", "scenario2" }, { "unit1", "unit2" }, {make_pair<>("First", "Person1"), make_pair<>("First", "Person2")}, "A message.", "", 2.3);
	alarmMessages.push_back( make_shared<Infoalarm::CInfoalarmMessageDecorator>( groupalarmMessage2.Clone() ) );

	setAlarmValidities.AddEntry( Validities::DEFAULT_VALIDITY, begin( alarmMessages ), end( alarmMessages ) );

	singleTimeException = Validities::CSingleTimeValidity( CDateTime( 5, 11, 2016, CTime( 3, 51, 40, 0 ) ), CDateTime( 5, 11, 2016, CTime( 9, 51, 40, 0 ) ) );
	alarmMessagesEmpty.clear();
	setAlarmValidities.AddEntry( make_shared<Validities::CSingleTimeValidity>( singleTimeException ), begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

	groupalarmMessage3.SetAlarmToDefinedUsers({}, {}, {"unit1"}, {}, "", "template1", 2.0);
	alarmMessagesException.push_back( make_shared<Groupalarm::CGroupalarm2Message>( groupalarmMessage2 ) );
	alarmMessagesException.push_back( make_shared<Groupalarm::CGroupalarm2Message>( groupalarmMessage3 ) );
	exceptionWeeks = { FIRST, THIRD };
	weeklyException = Validities::CWeeklyValidity( begin( exceptionWeeks ), end( exceptionWeeks ), SUNDAY, Utilities::CTime( 7, 9, 4, 0 ), Utilities::CTime( 7, 21, 23, 0 ) );
	weeklyException2 = Validities::CWeeklyValidity( begin( exceptionWeeks ), end( exceptionWeeks ), TUESDAY, Utilities::CTime( 14, 9, 4, 0 ), Utilities::CTime( 3, 21, 25, 0 ) );
	setAlarmValidities.AddEntry( make_shared<Validities::CWeeklyValidity>( weeklyException ), begin( alarmMessagesException ), end( alarmMessagesException ) );

	setAlarmValidities.AddEntry( make_shared<Validities::CWeeklyValidity>( weeklyException2 ), begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

	monthlyException = Validities::CMonthlyValidity( 7, Utilities::CTime( 13, 21, 49, 0 ), Utilities::CTime( 13, 51, 49, 0 ) );
	alarmMessagesMonthly.push_back( make_shared<Email::CEmailMessage>( "siteIDMonth", "alarmIDMonth", recipients, "monthly exception", true ) );
	alarmMessagesMonthly.push_back( make_shared<Groupalarm::CGroupalarm2Message>( groupalarmMessage ) );
	setAlarmValidities.AddEntry( make_shared<Validities::CMonthlyValidity>( monthlyException ), begin( alarmMessagesMonthly ), end( alarmMessagesMonthly ) );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setAlarmValidities, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getAlarmValidities );

	return( getAlarmValidities == setAlarmValidities );
}


/**	@brief		Test implementation of testing storing empty data.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLAlarmValiditiesTest::EmptyTest()
{
	using namespace std;
	using namespace External;

	vector< shared_ptr<External::CAlarmMessage> > alarmMessages;
	CXMLSerializableAlarmValidities setAlarmValidities, getAlarmValidities;

	setAlarmValidities.AddEntry( External::Validities::DEFAULT_VALIDITY, begin( alarmMessages ), end( alarmMessages ) );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setAlarmValidities, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getAlarmValidities );

	return( getAlarmValidities == setAlarmValidities );
}