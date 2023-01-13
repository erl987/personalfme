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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "version.h"
#include "VersionInfo.h"


/**	@brief		Returns the version information of the software
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the software
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void Utilities::CVersionInfo::GetSoftwareVersion( std::string& versionString, std::string& dateString, std::string& licenseText )
{
	using namespace std;
	string yearString;

	versionString = to_string( MAJOR_VERSION ) + "." + to_string( MINOR_VERSION ) + "." + to_string( RELEASE_VERSION ) + VERSION_TYPE + " (rev. " + REVISION + ")";
	dateString = DATE_STRING;
	if ( dateString != "unknown" ) {
		dateString.erase( dateString.begin() + dateString.find( " +" ), dateString.end() );
	}
	yearString = DATE_STRING;
	if ( yearString != "unknown" ) {
		yearString.erase( yearString.begin() + yearString.find( "-" ), yearString.end() );
	}
	licenseText = "Copyright (c) 2010-" + yearString + " " + VendorName() + "\n" + "This program comes with ABSOLUTELY NO WARRANTY. ";
	licenseText += "This is free software, and you are welcome to redistribute it under certain conditions. See the documentation for details.";
}


/**	@brief		Returns the name of the software
*	@return 									Name of the software
*	@exception 									None
*	@remarks 									None
*/
const std::string Utilities::CVersionInfo::SoftwareName()
{
	return SOFTWARE_NAME_STRING;
}


/**	@brief		Returns the name of the software owner
*	@return 									Name of the software owner
*	@exception 									None
*	@remarks 									None
*/
const std::string Utilities::CVersionInfo::VendorName()
{
	return VENDOR_NAME_STRING;
}
