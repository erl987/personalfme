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
#include "XMLSerializableGroupalarm2Message.h"
#include "XMLGroupalarm2MessageTest.h"

const boost::filesystem::path xmlFileName = "./groupalarmMessage.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestGroupalarmMessageSchema.xsd";
#elif defined( __linux )
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestGroupalarmMessageSchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLGroupalarmMessageTest::TestDefinedUsers()
{
	External::Groupalarm::CXMLSerializableGroupalarm2Message setMessage, getMessage;

	setMessage.SetAlarmToDefinedUsers({ {"label1", 2}, {"label", 1} }, { "scenario 1", "scenario 2" }, { "unit 1", "unit 2" }, { std::make_pair<>("Bob", "Foo"), std::make_pair<>("Alice", "Bar") }, "", "Message Template 1", 2.53);

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setMessage, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getMessage );

	return( getMessage == setMessage );
}

/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLGroupalarmMessageTest::TestAllUsers()
{
	External::Groupalarm::CXMLSerializableGroupalarm2Message setMessage, getMessage;

	setMessage.SetAlarmToAllUsers("This is an alarm!", "", 2.53);

	// write the XML-file
	Utilities::XML::WriteXML(xmlFileName, setMessage, rootTag, make_pair(namespaceName, schemaDefinitionFilePath));

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML(xmlFileName, getMessage);

	return(getMessage == setMessage);
}

/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLGroupalarmMessageTest::TestAlarmTemplate()
{
	External::Groupalarm::CXMLSerializableGroupalarm2Message setMessage, getMessage;

	setMessage.SetAlarmTemplate("Alarm Template 1", 2.53);

	// write the XML-file
	Utilities::XML::WriteXML(xmlFileName, setMessage, rootTag, make_pair(namespaceName, schemaDefinitionFilePath));

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML(xmlFileName, getMessage);

	return(getMessage == setMessage);
}
