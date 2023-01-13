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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include <boost/algorithm/string.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include "ParserErrorHandler.h"
#include "XMLValidator.h"


/** @brief		Standard constructor
*	@remarks								None
*/
Utilities::XML::CXMLValidator::CXMLValidator()
{
	// initialize the Xerces-library
	xercesc::XMLPlatformUtils::Initialize( "de_DE" ); // currently the locale has no effect in Xerces
}


/** @brief		Destructor
*	@remarks								None
*/
Utilities::XML::CXMLValidator::~CXMLValidator()
{
	// terminate the Xerces-library
	xercesc::XMLPlatformUtils::Terminate();
}


/** @brief		Validates a XML-file if it fullfills its XML schema
*	@param		xmlFile						XML-file to be validated
*	@param		xmlSchemaLocation			External XML schema location (XML namespace, XSD file location) to be used instead of that possibly defined in the XML-file. Omit it, if that defined in the XML-file should be used.
*	@return									None
*	@exception	Exception:xml_error			Thrown if the XML-file is invalid or not well-formed
*	@remarks								None
*/
void Utilities::XML::CXMLValidator::ValidateSchema( const boost::filesystem::path& xmlFile, const std::pair<std::string, boost::filesystem::path>& xmlSchemaLocation )
{
	using namespace xercesc;

	std::string location;
	CParserErrorHandler parserErrorHandler;
	XercesDOMParser domParser;

	// set up the XML-parser
	domParser.setErrorHandler( &parserErrorHandler );
	domParser.setValidationScheme( XercesDOMParser::Val_Auto );
	domParser.setDoNamespaces( true );
	domParser.setDoSchema( true );
	domParser.setValidationConstraintFatal( true );
	if ( xmlSchemaLocation != std::pair<std::string, boost::filesystem::path>()  ) {
		location = "file:///" + xmlSchemaLocation.second.string();
		boost::replace_all( location, " ", "%20" ); // URI-encoding of whitespaces
		domParser.setExternalSchemaLocation( ( xmlSchemaLocation.first + " " +  location ).c_str() );
	}

	// validate the XML-file
	domParser.parse( xmlFile.string().c_str() ); // will throw Utilities::Exception::xml_error in case of a parsing error
}


/**	@brief		Returns the version information of the Xerces XML-library
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Utilities::XML::CXMLValidator::GetXercesVersion( std::string& versionString, std::string& dateString, std::string& licenseText )
{
	using namespace std;

	versionString = to_string( gXercesMajVersion ) + "." + to_string( gXercesMinVersion ) + "." + to_string( gXercesRevision );
	dateString = "19 March 2015";
	licenseText = "This product includes software developed by The Apache Software Foundation (http://www.apache.org/). Portions of this software were originally based on the following software copyright(c) 1999, IBM Corporation., http ://www.ibm.com.";
}