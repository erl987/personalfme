// DO NOT CHANGE THIS FILE MANUALLY. CHANGE INSTEAD "VERSION.TEMPLATE" FROM WHICH IT IS GENERATED AUTOMATICALLY BY SVN.
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

/** @mainpage
*	This is a software continously scanning the audio input device for detecting and managing BOS selcalls.
*	Currently the detection of 5-tone sequences is implemented. The detected sequences are logged and further
*	actions can automatically be started, when detecting a sequence.
*	@author		Ralf Rettig (info@personalfme.de, www.personalfme.de)
*	@version	2c1a21f
*	@date		"2023-01-09 21:07:01 +0100"
*
*	The object-oriented code is currently designed for 5-tone sequences only, but it can be flexibly extended to other 
*	types of sequences, because the basic class "CSearch" is an abstract interface.
*	The basis of the algorithm is the Short Time Fourier Transformation (STFT) with two different timesteps, to ensure
*	good time and good frequency resolution at the same time.
*
*	Major design goals:
*		- full conformity with the German standard TR-BOS FME
*		- reliable real-time detection system
*		- simple adoption to other detection applications, for example FMS-codes ("Funkmeldesystem")
*		- fully portable code
*		- use of Portaudio acquisition system for full portability
*
*/

#pragma once

#include <string>

namespace Utilities {
	/**	@param	MAJOR_VERSION		storing the major version number of the library */
	const unsigned int MAJOR_VERSION = 1;
	/**	@param	MINOR_VERSION		storing the minor version number of the library */
	const unsigned int MINOR_VERSION = 1;
	/**	@param	RELEASE_VERSION		storing the release version number of the library */
	const unsigned int RELEASE_VERSION = 0;
	/**	@param	VERSION_TYPE		storing the version type (i.e. "" = Release to Manufacturing, "RC1" = Release candidate 1, "pre-release" = Development version) */
	const std::string VERSION_TYPE = "";
	/**	@param	REVISION			storing the revision number from the current build, automatically updated by SVN */
	const std::string REVISION = "2c1a21f";
	/**	@param	DATE_STRING			storing the build date from the current revision, automatically updated by SVN */
	const std::string DATE_STRING = "2023-01-09 21:07:01 +0100";
	/**	@param	SOFTWARE_NAME		storing the name of the software, automatically updated by SVN */
	const std::string SOFTWARE_NAME_STRING = "PersonalFME";
	/**	@param	VENDOR_NAME			storing the name of the software owner, automatically updated by SVN */
	const std::string VENDOR_NAME_STRING = "Ralf Rettig";	
}
