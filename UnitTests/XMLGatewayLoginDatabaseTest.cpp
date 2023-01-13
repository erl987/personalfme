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
#include "EmailGateway.h"
#include "Groupalarm2Gateway.h"
#include "EmailLoginData.h"
#include "Groupalarm2LoginData.h"
#include "XMLUtilities.h"
#include "XMLSerializableGatewayLoginDatabase.h"
#include "XMLGatewayLoginDatabaseTest.h"

const boost::filesystem::path xmlFileName = "./gatewayLoginDatabase.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestGatewayLoginDatabaseSchema.xsd";
#elif defined( __linux )
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestGatewayLoginDatabaseSchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLGatewayLoginDatabaseTest::Test()
{
	using namespace std;

	External::CXMLSerializableGatewayLoginDatabase setDatabase, getDatabase;
	External::Email::CEmailLoginData emailLoginData;
	External::Groupalarm::CGroupalarm2LoginData groupalarmLoginData;

	emailLoginData.SetConnectionTrialInfos( 7, 19.3f, 5 );
	emailLoginData.SetLoginInformation( "First Last <first.last@test.de>", External::Email::UNENCRYPTED_AUTH, "user3", "password10" );
	emailLoginData.SetServerInformation( External::Email::TLS_SSL_CONN, "smtp.host.de", 465 );		
	setDatabase.Add( make_unique<External::Email::CEmailGateway>(), make_unique<External::Email::CEmailLoginData>( emailLoginData ) );

	groupalarmLoginData.SetConnectionTrialInfos( 7, 19.3f, 5 );
	groupalarmLoginData.Set(12345, "aToken", "proxy.provider.org", 8080, "aUser", "aPasswd");
	setDatabase.Add( make_unique<External::Groupalarm::CGroupalarm2Gateway>(), make_unique<External::Groupalarm::CGroupalarm2LoginData>( groupalarmLoginData ) );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setDatabase, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getDatabase );

	return ( getDatabase == setDatabase );
}




/**	@brief		Test implementation with empty datasets.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLGatewayLoginDatabaseTest::EmptyTest()
{
	using namespace std;

	External::CXMLSerializableGatewayLoginDatabase setDatabase, getDatabase;

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setDatabase, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getDatabase );

	return ( getDatabase == setDatabase );
}
