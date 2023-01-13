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
#include "XMLUtilities.h"
#include "XMLSerializableSingleTimeValidity.h"
#include "XMLSingleTimeValidityTest.h"

const boost::filesystem::path xmlFileName = "./singleTimeValidity.xml";
#if defined( _WIN32 )
	const boost::filesystem::path schemaDefinitionFilePath = "../../UnitTests/TestSingleTimeValiditySchema.xsd";
#elif defined( __linux )
	// Linux
	const boost::filesystem::path schemaDefinitionFilePath = "../UnitTests/TestSingleTimeValiditySchema.xsd";
#endif
const std::string namespaceName = "http://www.personalfme.de/v1";
const std::string rootTag = "config";


/**	@brief		Test implementation.
*	@return										True if the test succeeded, false otherwise
*	@exception									None
*	@remarks									The XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems
*/
bool Utilitites::XMLTest::XMLSingleTimeValidityTest::Test()
{
	using namespace Utilities;

	External::Validities::CXMLSerializableSingleTimeValidity setValidity, getValidity;

	setValidity.SetValidity( CDateTime( 19, 7, 2016, CTime( 19, 21, 5 ) ), CDateTime( 30, 9, 2021, CTime( 10, 7, 29 ) ) );

	// write the XML-file
	Utilities::XML::WriteXML( xmlFileName, setValidity, rootTag, make_pair( namespaceName, schemaDefinitionFilePath ) );

	// read the XML-file (with a validating parser)
	Utilities::XML::ReadXML( xmlFileName, getValidity );

	return( getValidity == setValidity );
}
