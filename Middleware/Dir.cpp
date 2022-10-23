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
		#define Middleware_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define Middleware_API __declspec(dllexport)
	#endif
#endif

#include <algorithm>
#ifdef _WIN32
	#include <windows.h>
	#include <shlobj.h> 
	#include <shobjidl.h> 
#endif
#include "Dir.h"


/**	@brief		Standard constructor
*/
Middleware::CDir::CDir()
	: isInit( false )
{
}


/**	@brief		Constructor
*	@param		softwareName						Name of the software (used for naming the directories)
*	@exception	std::logic_error					Thrown if the standard Windows folder where not found (only possible on Windows)
*	@remarks										None
*/
Middleware::CDir::CDir( const std::string& softwareName )
	: isInit( false )
{
	using namespace boost;
	std::string lowercaseSoftwareName;

#ifdef _WIN32
	WCHAR path[MAX_PATH];
	HRESULT hr = SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path );
	if ( hr == S_OK ) {
		appSettingsDir = filesystem::canonical( softwareName, path );
	} else {
		throw std::logic_error( u8"Standard Windows-Ordner für die applikationsspezifischen Daten kann nicht gefunden werden." );
	}

	#ifdef NDEBUG
		pluginDir = filesystem::canonical( "./plugins", appSettingsDir );
	#else
		pluginDir = filesystem::canonical( "./plugins/debug", appSettingsDir );
	#endif

	schemaDir = filesystem::canonical( "./schema", appSettingsDir );

	hr = SHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path );
	if ( hr == S_OK ) {
		userSettingsDir = filesystem::canonical( softwareName, path );
	} else {
		throw std::logic_error( u8"Standard Windows-Ordner für die Benutzerdaten kann nicht gefunden werden." );
	}

	logDir = userSettingsDir;
	audioDir = userSettingsDir;
#elif __linux
	lowercaseSoftwareName = softwareName;
	std::transform( begin( lowercaseSoftwareName ), end( lowercaseSoftwareName ), begin( lowercaseSoftwareName ), ::tolower );

	userSettingsDir = filesystem::canonical( lowercaseSoftwareName, "/etc" );
	logDir = filesystem::canonical( lowercaseSoftwareName, "/var/log" );
	audioDir = filesystem::canonical( lowercaseSoftwareName, "/var/lib" );
	appSettingsDir = filesystem::canonical( lowercaseSoftwareName, "/etc" );
	appSettingsDir = filesystem::canonical( "./basicsettings", appSettingsDir );
	pluginDir = filesystem::canonical( lowercaseSoftwareName, "/usr/lib" );
	#ifdef NDEBUG
		pluginDir = filesystem::canonical( "./plugins", pluginDir );
	#else
		pluginDir = filesystem::canonical( "./plugins/debug", pluginDir );
	#endif
	schemaDir = filesystem::canonical( lowercaseSoftwareName, "/etc" );
	schemaDir = filesystem::canonical( "./schema", schemaDir );
#endif

	isInit = true;
}


/**	@brief		Destructor
*/
Middleware::CDir::~CDir(void)
{
}


/**	@brief		Obtains the directory containing the basic settings of the program
*	@return 										Basic settings directory of the program
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetAppSettingsDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return appSettingsDir;
}


/**	@brief		Obtains the directory containing the plugins of the program
*	@return 										Plugin directory
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetPluginDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return pluginDir;
}


/**	@brief		Obtains the directory containing the XML-schema files
*	@return 										Directory containing the XML-schema files
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetSchemaDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return schemaDir;
}


/**	@brief		Obtains the directory containing the user settings
*	@return 										User settings directory
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetUserSettingsDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return userSettingsDir;
}


/**	@brief		Obtains the directory containing the log files
*	@return 										Log file directory
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetLogDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return logDir;
}


/**	@brief		Obtains the directory containing the audio files of protocolled alarms
*	@return 										Directory containing the audio files of the protocolled alarms
*	@exception	std::runtime_error					Thrown if the class has not been initialized properly
*	@remarks 										None
*/
boost::filesystem::path Middleware::CDir::GetAudioDir()
{
	if ( !isInit ) {
		throw std::runtime_error( "The directories have not been determined." );
	}

	return audioDir;
}
