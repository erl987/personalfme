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
#include "XMLSerializableWeeklyValidity.h"
#include "XMLWeeklyValidityTest.h"

const boost::filesystem::path xmlFileName = "./weeklyValidity.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestWeeklyValiditySchema.xsd";
#elif defined( __linux )
	// Linux
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestWeeklyValiditySchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLWeeklyValidityTest::Test()
{
	using namespace Utilities;
	using namespace Utilities::Time;

	External::Validities::CXMLSerializableWeeklyValidity setValidity, getValidity;
	std::vector<Utilities::Time::WeekType> validWeeks = { FIRST, THIRD };

	setValidity.SetValidityDates( validWeeks, SATURDAY, CTime( 3, 7, 5, 0 ), CTime(3, 17, 17, 0 ) );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setValidity, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getValidity );

	return( getValidity == setValidity );
}
