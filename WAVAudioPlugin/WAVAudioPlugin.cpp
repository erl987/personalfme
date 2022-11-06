/*	WAV audio file handler plugin for the software PersonalFME
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
#include <algorithm>
#include "Poco/ClassLibrary.h"
#include "version.h"
#include "WAVHandler.h"
#include "WAVAudioPlugin.h"

/**	@brief	File type of the plugin */
const std::string fileType = "wav";


/** @brief	MIME-type of the plugin */
const std::string MIMEtype = "audio/wav";


/**	@brief		Constructor
*	@remarks 							None
*/
WAVAudioPlugin::WAVAudioPlugin()
{
}


/**	@brief		Destructor
*	@remarks 							None
*/
WAVAudioPlugin::~WAVAudioPlugin()
{
}


/** @brief		Saving mono audio data to an audio file
*	@param		fileName				Name of the audio-file (including file ending and path (if required))
*	@param		data					Audio data container (format: float with a range of [-1.0 .. +1.0])
*	@return								None
*	@exception	std::logic_error		Thrown if the audio file could not be opened
*	@exception	std::runtime_error		Thrown if an audio encoding error occurred
*	@remarks							This is an implemented plugin method
*/
void WAVAudioPlugin::Save( const std::string& fileName, const std::vector<float>& data ) const
{
	// the plugin can assume that the sampling frequency provided by the base class is valid
	CWAVHandler wavHandler( AudioPlugin::samplingFreq, 1 ); // only mono audio data is supported (i.e. one channel)
	wavHandler.Save( fileName, data );
}


/** @brief		Obtains information on the version and license of the plugins
*	@param		pluginID				Short ID of the plugin (i.e. "OGG", "WAV", ...)
*	@param		versionString			Version number information of the plugin
*	@param		dateString				Build date of the version of the plugin
*	@param		licenseText				License text of the plugin
*	@return								None
*	@remarks							The method has to be implemented by the plugin
*/
void WAVAudioPlugin::GetPluginVersionInfo( std::string& pluginID, std::string& versionString, std::string& dateString, std::string& licenseText ) const
{
	using namespace std;
	using namespace Utilities;

	string yearStr;

	pluginID = fileType;
	transform( begin( pluginID ), end( pluginID ), begin( pluginID ), ::toupper );
	versionString = to_string( MAJOR_VERSION ) + "." + to_string( MINOR_VERSION ) + "." + to_string( RELEASE_VERSION ) + VERSION_TYPE;
	if ( DATE_STRING != "unknown" ) {
		dateString = DATE_STRING.substr( 0, 19 ); // removing unnecessary information from the date string
		yearStr = DATE_STRING.substr( 0, 4 );
	} else {
		dateString = DATE_STRING;
		yearStr = DATE_STRING;
	}
	licenseText = "Copyright (c) 2010-" + yearStr + " Ralf Rettig. This program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions.";
}


/** @brief		Obtains information on the version and license of the plugin
*	@param		pluginID				Short ID of the plugin (i.e. "OGG", "WAV", ...)
*	@param		libraryName				Name of the underlying audio file library
*	@param		versionString			Version number information of the underlying audio file library
*	@param		dateString				Build date of the version of the underlying audio file library
*	@param		licenseText				License text of the underlying audio file library
*	@return								None
*	@remarks							This is an implemented plugin method
*/
void WAVAudioPlugin::GetLibraryVersionInfo( std::string& pluginID, std::string& libraryName, std::string& versionString, std::string& dateString, std::string& licenseText ) const
{
	pluginID = fileType;
	std::transform( begin( pluginID ), end( pluginID ), begin( pluginID ), ::toupper );
	libraryName = "libsndfile";
	CWAVHandler::GetLibsndfileVersion( versionString, dateString, licenseText );
}


/** @brief		Obtains the extension of the audio files processed by the plugin
*	@return								Extension of the audio files (including ".")
*	@remarks							This is an implemented plugin method
*/
std::string WAVAudioPlugin::GetExtension() const
{
	return "." + fileType;
}


/** @brief		Obtains the MIME-type of the audio files processed by the plugin
*	@return								MIME-type of the audio files (for example: "audio/wav")
*	@remarks							This is an implemented method of the plugin
*/
std::string WAVAudioPlugin::GetMIMEtype() const
{
	return MIMEtype;
}



// Poco-library plugin registration
POCO_BEGIN_MANIFEST( Utilities::Plugins::AudioPlugin )
	POCO_EXPORT_CLASS( WAVAudioPlugin )
POCO_END_MANIFEST

