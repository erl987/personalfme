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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include <sstream>
#include "XMLUtilities.h"


/** @brief		Adds a complete XML-header to a XML-data structure
*	@param		xmlString					String containing the complete XML-data structure to be modified
*	@param		rootTag						Root tag of the XML-data structure
*	@param		xmlSchemaLocation			XML schema location (XML namespace, XSD file location) to be used in the XML-file
*	@return									None
*	@exception	Exception:xml_error			Thrown if the root tag was not found
*	@remarks								The initial root tag is replaced.
*/
std::string Utilities::XML::AddXMLHeader( const std::string& xmlString, const std::string& rootTag, const std::pair<std::string, boost::filesystem::path>& xmlSchemaLocation )
{
	using namespace std;

	bool isSingleTag = false;
	stringstream ss;
	string xmlFileString, rootTagString, rootTagSingleString, namespaceName;
	boost::filesystem::path schemaDefinitionFile;

	namespaceName = xmlSchemaLocation.first;
	schemaDefinitionFile = xmlSchemaLocation.second;
	
	// remove the begin root tag
	xmlFileString = xmlString;
	rootTagString = "<" + rootTag + ">";
	rootTagSingleString = "<" + rootTag + "/>";

	if ( xmlFileString.find( rootTagString ) == string::npos ) {
		if ( xmlFileString.find( rootTagSingleString ) == string::npos ) {
			throw Utilities::Exception::xml_error( "error:\tThe root tag is not found." );		
		} else {
			isSingleTag = true;
		}
	}

	if ( !isSingleTag ) {
		xmlFileString.erase( xmlFileString.find( rootTagString ), string( rootTagString ).length() );
	} else {
		xmlFileString.erase( xmlFileString.find( rootTagSingleString ), string( rootTagSingleString ).length() );
	}

	// add the XML-header
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<" << rootTag << "\txmlns=\"" + namespaceName + "\"\n";
	ss << "\t\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
	ss << "\t\txsi:schemaLocation=\"" + namespaceName + " " + schemaDefinitionFile.string() + "\">\n";

	ss << xmlFileString;

	if ( isSingleTag ) {
		ss << "</" << rootTag << ">\n";
	}

	return ss.str();
}
