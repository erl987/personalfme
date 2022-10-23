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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include <string>
#include "XMLUtilities.h"
#include "XercesString.h"
#include "ParserErrorHandler.h"


/** @brief		Constructor
*	@remarks								None
*/
Utilities::XML::CParserErrorHandler::CParserErrorHandler()
{
}


/** @brief		Destructor
*	@remarks								None
*/
Utilities::XML::CParserErrorHandler::~CParserErrorHandler()
{
}


/** @brief		Overwritten warning handler of Xerces throwing Exception::xml_error
*	@param		exception					Xerces SAX parse exception
*	@return									None
*	@exception	Exception::xml_error		Always thrown
*	@remarks								None
*/
void Utilities::XML::CParserErrorHandler::warning( const xercesc::SAXParseException & exception )
{
	XMLParseException( exception );
}


/** @brief		Overwritten error handler of Xerces throwing Exception::xml_error
*	@param		exception					Xerces SAX parse exception
*	@return									None
*	@exception	Exception::xml_error		Always thrown
*	@remarks								None
*/
void Utilities::XML::CParserErrorHandler::error( const xercesc::SAXParseException & exception )
{
	XMLParseException( exception );
}


/** @brief		Overwritten fatal error handler of Xerces throwing Exception::xml_error
*	@param		exception					Xerces SAX parse exception
*	@return									None
*	@exception	Exception::xml_error		Always thrown
*	@remarks								None
*/
void Utilities::XML::CParserErrorHandler::fatalError( const xercesc::SAXParseException & exception )
{
	XMLParseException( exception );
}


/** @brief		Overwritten reset error handler of Xerces
*	@return									None
*	@exception								None
*	@remarks								The error resetting is switched off
*/
void Utilities::XML::CParserErrorHandler::resetErrors()
{
}


/** @brief		Handler for all Xerces exceptions throwing Exception::xml_error
*	@param		exception					Xerces SAX parse exception
*	@return									None
*	@exception	Exception::xml_error		Always thrown
*	@remarks								None
*/
void Utilities::XML::CParserErrorHandler::XMLParseException( const xercesc::SAXParseException & exception )
{
	using namespace std;
	using namespace xercesc;

	stringstream errorMessageSS;
	XMLFileLoc errorLine, errorColumn;
	string parserErrorMessage;

	parserErrorMessage = CXercesString( exception.getMessage() ).str();
	errorLine = exception.getLineNumber();
	errorColumn = exception.getColumnNumber();

	errorMessageSS << "line:\t" + to_string( errorLine ) << ", column: " + to_string( errorColumn ) << endl;
	errorMessageSS << "error:\t" + parserErrorMessage;

	throw Utilities::Exception::xml_error( errorMessageSS.str() );
}