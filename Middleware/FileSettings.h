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
#pragma once
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#if defined _WIN32 || defined __CYGWIN__
	#ifdef Middleware_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define Middleware_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define Middleware_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define Middleware_API __attribute__ ((visibility ("default")))
	#else
		#define Middleware_API
	#endif		
#endif


/*@{*/
/** \ingroup Middleware
*/
namespace Middleware {
	/** \ingroup Middleware
	*	Class defining basic file setting constants
	*/
	class CFileSettings
	{
	public:
		/**	@brief								Standard extension for configuration files */
		Middleware_API static const std::string GetParamExtension(void)
		{
			return ".xml";
		};
		/**	@brief								XML-schema definition file for CSettingsParam-serialization (path relative to the programm settings directory) */
		Middleware_API static const boost::filesystem::path GetSettingsParamXMLSchemaFile( void )
		{
			return boost::filesystem::path( "config.xsd" );
		};
		/**	@brief								XML-namespace for the XML-serialization */
		Middleware_API static const std::string GetXMLNamespace( std::string softwareName )
		{
			boost::algorithm::to_lower( softwareName );
			return "http://www." + softwareName + ".de/v1";
		};
	private:
		Middleware_API CFileSettings(void) {};
		Middleware_API ~CFileSettings(void) {};
	};
}
/*@}*/
