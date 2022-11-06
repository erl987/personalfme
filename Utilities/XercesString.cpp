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

#include "XercesString.h"


/** @brief		Constructor
*	@param		xercesStr					String in Xerces-format
*	@remarks								None
*/
Utilities::XML::CXercesString::CXercesString( const XMLCh *xercesStr )
{
	nativeStr = xercesc::XMLString::transcode( xercesStr );
}


/** @brief		Obtains the Xerces string as std::string
*	@return									Xerces string in std::string form
*	@exception								None
*	@remarks								None
*/
std::string Utilities::XML::CXercesString::str()
{
	return std::string( nativeStr );
}


/** @brief		Destructor
*	@remarks								None
*/
Utilities::XML::CXercesString::~CXercesString()
{
	xercesc::XMLString::release( &nativeStr );
}
