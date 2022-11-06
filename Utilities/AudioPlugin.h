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
#include <stdexcept>
#include <vector>
#include <string>

/**	\defgroup	Plugins		Plugins of the program.
*/

/*@{*/
/** \ingroup Plugins
*/
namespace Utilities {
	namespace Plugins {
		/**		@brief	Base of all audio plugin class names */
		const std::string audioPluginBaseName = "AudioPlugin";

		/**	\ingroup Plugins
		*	Base class representing an audio file plugin. All plugins are required to have a class name as: "TYPEAudioPlugin" with "type" being "OGG", "WAV", ...
		*/
		class AudioPlugin
		{
		public:
			AudioPlugin() : isSamplingFreqValid( false ) {};
			virtual ~AudioPlugin( void ) {};
			template <typename In_It> void Save( const std::string& fileName, In_It dataFirst, In_It dataLast, const bool& isAmplify ) const;

			/** @brief		Returns the complete ID of a plugin type
			*	@param		pluginType				Type of the plugin (short ID) ("OGG", "WAV", ...)
			*	@return								Complete ID of the plugin type
			*	@remarks							All plugins are required to have a class name as: "TYPEAudioPlugin" with "type" being "OGG", "WAV", ...
			*/
			static std::string CompleteID( const std::string& pluginType ) {
				return std::string( pluginType + audioPluginBaseName );
			};

			/** @brief		Returns the plugin type (short ID) based on the complete ID
			*	@param		completeID				Complete ID of the plugin type
			*	@return								Type of the plugin ("OGG", "WAV", ...)
			*	@remarks							All plugins are required to have a class name as: "TYPEAudioPlugin" with "type" being "OGG", "WAV", ...
			*/
			static std::string PluginType( const std::string& completeID ) {
				return completeID.substr( 0, completeID.find( audioPluginBaseName ) );
			};

			/** @brief		Resets the sampling frequency of the audio files
			*	@param		samplingFreq			New sampling frequency [Hz]
			*	@return								None
			*	@remarks							Call this method before using the plugin
			*/
			virtual void SetSamplingFreq( int samplingFreq ) {
				AudioPlugin::samplingFreq = samplingFreq;
				isSamplingFreqValid = true;
			};

			/** @brief		Obtains information on the version and license of the plugins
			*	@param		pluginID				Short ID of the plugin (i.e. "OGG", "WAV", ...)
			*	@param		versionString			Version number information of the plugin
			*	@param		dateString				Build date of the version of the plugin
			*	@param		licenseText				License text of the plugin
			*	@return								None
			*	@remarks							The method has to be implemented by the plugin
			*/
			virtual void GetPluginVersionInfo( std::string& pluginID, std::string& versionString, std::string& dateString, std::string& licenseText ) const = 0;

			/** @brief		Obtains information on the version and license of the underlying library of the plugin
			*	@param		pluginID				Short ID of the plugin (i.e. "OGG", "WAV", ...)
			*	@param		libraryName				Name of the underlying audio file library
			*	@param		versionString			Version number information of the underlying audio file library
			*	@param		dateString				Build date of the version of the underlying audio file library
			*	@param		licenseText				License text of the underlying audio file library
			*	@return								None
			*	@remarks							The method has to be implemented by the plugin
			*/
			virtual void GetLibraryVersionInfo( std::string& pluginID, std::string& libraryName, std::string& versionString, std::string& dateString, std::string& licenseText ) const = 0;

			/** @brief		Obtains the extension of the audio files processed by the plugin
			*	@return								Extension of the audio files (including ".")
			*	@remarks							The method has to be implemented by the plugin
			*/
			virtual std::string GetExtension() const = 0;

			/** @brief		Obtains the MIME-type of the audio files processed by the plugin
			*	@return								MIME-type of the audio files (for example: "audio/wav")
			*	@remarks							The method has to be implemented by the plugin
			*/
			virtual std::string GetMIMEtype() const = 0;
		protected:
			int samplingFreq;
		private:
			/** @brief		Saving mono audio data to an audio file
			*	@param		fileName				Name of the audio-file (including file ending and path (if required))
			*	@param		data					Audio data container (format: float with a range of [-1.0 .. +1.0])
			*	@return								None
			*	@exception	std::logic_error		Thrown if the audio file could not be opened
			*	@exception	std::runtime_error		Thrown if an audio encoding error occurred
			*	@remarks							The method has to be implemented by the plugin. The plugin can assume that the sampling frequency is valid.
			*/
			virtual void Save( const std::string& fileName, const std::vector<float>& data ) const = 0;

			bool isSamplingFreqValid;
		};
	}
}
/*@}*/


/** @brief		Saving mono audio data to an audio file. It is relying on the private "Save"-method that has to be implemented by the plugin.
*	@param		fileName							Name of the audio-file (including file ending and path (if required))
*	@param		dataFirst							Iterator to beginning of the audio data container (format: float with a range of [-1.0 .. +1.0])
*	@param		dataLast							Iterator to one element after the end of the audio data container (format: float with a range of [-1.0 .. +1.0])
*	@param		isAmplify							Flag stating if the audio data will be amplified to the maximum value (true) or left unchanged (false).
*	@return											None
*	@exception	std::logic_error					Thrown if the audio file could not be opened
*	@exception	std::runtime_error					Thrown if the datatype of the data container was not float, the sampling frequency is not valid or an audio encoding error occurred
*	@remarks										In case of amplification the loudness is proportionally amplified to reach at maximum a value of abs(1) within the data
*/
template <typename In_It>
void Utilities::Plugins::AudioPlugin::Save( const std::string& fileName, In_It dataFirst, In_It dataLast, const bool& isAmplify ) const
{
	using namespace std;
	vector<float> data;
	float maxCurrLevel;

	// check container datatype
	if ( !is_same< float, typename iterator_traits< In_It >::value_type >::value ) {
		throw std::runtime_error( "Datatype is not float." );
	}

	if ( !isSamplingFreqValid ) {
		throw std::runtime_error( "The sampling frequency of the audio plugin has not been set before using it." );
	}

	// obtain input data
	data.assign( dataFirst, dataLast );

	// amplify the data to maximum loudness if required
	if ( isAmplify ) {
		maxCurrLevel = std::abs( *max_element( begin( data ), end( data ), []( auto val1, auto val2 ) { return ( std::abs( val1 ) < std::abs( val2 ) ); } ) );
		transform( begin( data ), end( data ), begin( data ), [=]( auto level ) { return ( level / maxCurrLevel ); } );
	}

	// calling the underlying audio format plugin
	Save( fileName, data );
}
