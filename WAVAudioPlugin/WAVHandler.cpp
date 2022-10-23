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
#include <stdexcept>
#include "sndfile.hh"
#include "WAVHandler.h"



/** @brief		Constructor.
*	@param		samplingFreq				Sampling frequency of the WAV-files [Hz]
*	@param		channels					Number of channels of the WAV-files (1: mono, 2: stereo)
*	@exception								None
*	@remarks								None
*/
CWAVHandler::CWAVHandler( const int& samplingFreq, const int& channels )
		: samplingFreq( samplingFreq ),
		  channels( channels ),
		  format( SF_FORMAT_WAV | SF_FORMAT_PCM_16 )
{
}



/** @brief		Saving audio data to a WAV-file (16-bit).
*	@param		fileName					Name of the WAV-file (including *.wav ending and path (if required))
*	@param		data						Audio data container
*	@return									None
*	@exception	std::logic_error			Thrown if the audio file could not be opened
*	@remarks								None
*/
void CWAVHandler::Save( const std::string& fileName, const std::vector<float>& data )
{
	// prepare WAV-file for writing
	SndfileHandle outFile( fileName.c_str(), SFM_WRITE, format, channels, samplingFreq );
	if ( !outFile || outFile.error() ) {
		throw std::logic_error( "Audio file could not be opened for writing." );
	}

	// write data to file
	outFile.write( data.data(), data.size() );

	if ( outFile.error() ) {
		throw std::logic_error( "Writing the audio file failed (after successfull opening of the file)." );
	}
}



/**	@brief		Returns the version information of the sndfile-library
*	@param		versionString					Version number information
*	@param		dateString						Build date of the version
*	@param		licenseText						License text of the library
*	@return 									None
*	@exception 									None
*	@remarks 									None
*/
void CWAVHandler::GetLibsndfileVersion( std::string& versionString, std::string& dateString, std::string& licenseText )
{
	std::string versionInfo;
	std::string baseStr = "libsndfile-";

	versionInfo = sf_version_string();
	versionString = versionInfo.substr( baseStr.size() );

	if ( versionString == "1.0.26" ) {
		dateString = "Nov 22 2015";
	} else if ( versionString == "1.0.25" ) {
		dateString = "Jul 13 2011";
	} else {
		dateString = "unknown";
	}

	licenseText = "Copyright (c) 2005-2015 Erik de Castro Lopo <erikd@mega-nerd.com>\n";
	licenseText += "It contains libvorbis:\nCopyright (c) 2002-2015 Xiph.org Foundation\n";
}
