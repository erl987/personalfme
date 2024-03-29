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
#if defined _WIN32
	#include "stdafx.h"
#endif
#include "EmailMessage.h"
#include "DefaultValidity.h"
#include "WeeklyValidity.h"
#include "XMLUtilities.h"
#include "XMLSerializableAlarmMessagesDatabase.h"
#include "XMLAlarmMessagesDatabaseTest.h"

const boost::filesystem::path xmlFileName = "./alarmMessagesDatabase.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestAlarmMessagesDatabaseSchema.xsd";
#elif defined( __linux )
	// Linux
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestAlarmMessagesDatabaseSchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLAlarmMessagesDatabaseTest::Test()
{
	using namespace std;
	using namespace External;

	External::CXMLSerializableAlarmMessagesDatabase setDatabase, getDatabase;
	External::CAlarmValidities validity1, validity2;
	vector< shared_ptr<External::CAlarmMessage> > alarmMessages, alarmMessagesEmpty;
	Email::CEmailMessage emailMessage;
	Validities::CWeeklyValidity weeklyException;
	vector<int> code1 = { 2, 5, 6, 3, 4 };
	vector<int> code2 = { 1, 2, 3, 4, 5 };

	vector< pair<string, string> > recipients = { { "", "first.last@test.de" },{ "First2 Last2", "first2.last2@test.de" } };
	emailMessage.Set( "siteID", "alarmID", recipients, "alarm text" );
	emailMessage.SetRequiredState( false );
	alarmMessages.push_back( make_shared<Email::CEmailMessage>( emailMessage ) );

	validity1.AddEntry( Validities::DEFAULT_VALIDITY, begin( alarmMessages ), end( alarmMessages ) );
	weeklyException = Validities::CWeeklyValidity( Utilities::Time::SUNDAY, Utilities::CTime( 7, 9, 4, 0 ), Utilities::CTime( 7, 21, 6, 0 ) );
	alarmMessagesEmpty.clear();
	validity1.AddEntry( make_shared<Validities::CWeeklyValidity>( weeklyException ), begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

	validity2.AddEntry( Validities::DEFAULT_VALIDITY, begin( alarmMessagesEmpty ), end( alarmMessagesEmpty ) );

	setDatabase.Add( code1, validity1 );
	setDatabase.Add( code2, validity2 );

	setDatabase.ReplaceForAllCodes( validity1 );
	setDatabase.ReplaceFallback( validity2 );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setDatabase, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getDatabase );

	return ( getDatabase == setDatabase );
}
