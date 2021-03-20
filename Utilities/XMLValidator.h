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
#pragma once
#include <boost/filesystem.hpp>

#if defined _WIN32 || defined __CYGWIN__
	#ifdef UTILITY_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define UTILITY_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define UTILITY_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define UTILITY_API __attribute__ ((visibility ("default")))
	#else
		#define UTILITY_API
	#endif		
#endif

/*@{*/
/** \ingroup Utilities
*/
namespace Utilities {
	namespace XML {
		/**	\ingroup Utilities
		*	Class for validating a XML-file.
		*/
		class CXMLValidator
		{
		public:
			UTILITY_API CXMLValidator();
			UTILITY_API virtual ~CXMLValidator();
			UTILITY_API void ValidateSchema( const boost::filesystem::path& xmlFile, const std::pair<std::string, boost::filesystem::path>& xmlSchemaLocation = std::pair< std::string, boost::filesystem::path>() );
			UTILITY_API static void GetXercesVersion( std::string& versionString, std::string& dateString, std::string& licenseText );
		private:
			CXMLValidator( const CXMLValidator& ) = delete;
			CXMLValidator& operator=( const CXMLValidator& ) = delete;
		};
	}
}
/*@}*/
