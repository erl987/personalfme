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
#pragma once
#include <string>
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


/**	\ingroup PersonalFME
*/
namespace Middleware {
	/**	\ingroup PersonalFME
	*	Representing all persistent directories on disk
	*/
	class CDir
	{
	public:
		Middleware_API CDir();
		Middleware_API CDir( const std::string& softwareName );
		virtual Middleware_API ~CDir( void );
		Middleware_API boost::filesystem::path GetAppSettingsDir();
		Middleware_API boost::filesystem::path GetPluginDir();
		Middleware_API boost::filesystem::path GetSchemaDir();
		Middleware_API boost::filesystem::path GetUserSettingsDir();
		Middleware_API boost::filesystem::path GetLogDir();
		Middleware_API boost::filesystem::path GetAudioDir();
	protected:
		boost::filesystem::path appSettingsDir;
		boost::filesystem::path pluginDir;
		boost::filesystem::path schemaDir;
		boost::filesystem::path userSettingsDir;
		boost::filesystem::path logDir;
		boost::filesystem::path audioDir;
	private:
		bool isInit;
	};
}
/*@}*/
