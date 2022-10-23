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

#include <boost/locale.hpp>
#include "StringUtils.h"


/** @brief		Converts a string from the Western charset to UTF-8
*	@param		str							String to be converted
*	@return									UTF-8 string
*	@exception								None
*	@remarks								None
*/
std::string Utilities::String::GetUTF8FromWestern( const std::string& str ) {
	std::string strUTF8;

	try {
	#ifdef _WIN32
		strUTF8 = boost::locale::conv::to_utf<char>( str, "ISO-8859-1" );
	#else
		strUTF8 = boost::locale::conv::to_utf<char>( str, "de_DE.ISO-8859-1" );
	#endif
	} catch ( std::exception e ) {
		strUTF8 = str; // fallback if the locale is missing
	}

	return strUTF8;
}