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
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "SettingsParam.h"

/*@{*/
/** \ingroup PersonalFME
*	@param	TypeOfChoice				Command line options chosen by the user */
enum TypeOfChoice { NOT_VALID, AUDIO_INFO, VERSION_INFO, HELP, DETECTION, TEST, PRINT_WORKING_DIR, DAEMONIZE };

/** \ingroup PersonalFME
*	Class implementing basic methods for the console program
*/
class CBasicFunctionality
{
public:
	CBasicFunctionality();
	virtual ~CBasicFunctionality();
	static std::string GetAudioInputDeviceList();
	static std::string GetHelpText();
	static std::string GetBasicVersionInformation();
	static std::string GetCompleteVersionInformation();
	static Middleware::CSettingsParam ValidateXMLConfigFile( const boost::filesystem::path& configFile );
	static TypeOfChoice ProcessCommandLineArguments( const std::vector<std::string>& commandLineArgs, boost::filesystem::path& configFile, bool& doDaemonize );
};
/*@}*/

