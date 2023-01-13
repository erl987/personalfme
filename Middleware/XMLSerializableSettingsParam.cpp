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
		#define Middleware_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define Middleware_API __declspec(dllexport)
	#endif
#endif

#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Poco/Util/ConfigurationView.h>
#include "AlarmMessagesDatabase.h"
#include "XMLException.h"
#include "XMLSerializableGatewayLoginDatabase.h"
#include "XMLSerializableAlarmMessagesDatabase.h"
#include "XMLSerializableSettingsParam.h"

const std::string AUDIO_KEY = "audio";
	const std::string DRIVER_KEY = "driver";
	const std::string DEVICE_KEY = "device";
const std::string CAPTURE_TIME_KEY = "voiceCaptureTime";
const std::string PROTOCOL_DEFAULT_KEY = "protocol";
	const std::string DEFAULT_KEY = "default";
		const std::string ON_KEY = "on";
		const std::string OFF_KEY = "off";
const std::string MIN_DETECTION_DIST_KEY = "minDetectionDist";
const std::string PLAY_TONE_KEY = "playTone";
const std::string AUDIO_FORMAT_KEY = "audioFormat";
const std::string LOGINS_KEY = "logins";
const std::string ALARMS_KEY = "alarms";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void Middleware::CXMLSerializableSettingsParam::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace Poco::Util;

	bool isPlayTone, isProtocolDefault;
	Core::Processing::CAudioDevice audioDevice;
	string audioDeviceName, audioDriverName, protocolDefaultSetting, audioFormatID;
	float minDetectionDist, captureTime;
	External::CXMLSerializableGatewayLoginDatabase loginDatabase;
	External::CXMLSerializableAlarmMessagesDatabase alarmsDatabase;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	audioDriverName = boost::algorithm::trim_copy( xmlFile->getString( AUDIO_KEY + "." + DRIVER_KEY, "" ) ); // default: standard driver
	audioDeviceName = boost::algorithm::trim_copy( xmlFile->getString( AUDIO_KEY + "." + DEVICE_KEY, "" ) ); // default: standard device
	if ( audioDeviceName.empty() && !audioDriverName.empty() ) {
		throw Utilities::Exception::xml_error( "error:\tFor specifying the default audio device, both tags \"" + DRIVER_KEY + "\" and \"" + DEVICE_KEY + "\" must be empty." );
	}
	audioDevice = Core::Processing::CAudioDevice( Core::IN_DEVICE, audioDeviceName, audioDriverName );
	
	captureTime = static_cast<float>( xmlFile->getDouble( CAPTURE_TIME_KEY, 30.0f ) ); // default value in seconds
	protocolDefaultSetting = xmlFile->getString( PROTOCOL_DEFAULT_KEY + "[@" + DEFAULT_KEY + "]", OFF_KEY ); // default value: do not protocol and record any code
	if ( protocolDefaultSetting == ON_KEY ) {
		isProtocolDefault = true;
	} else {
		isProtocolDefault = false;
	}

	minDetectionDist = static_cast<float>( xmlFile->getDouble( MIN_DETECTION_DIST_KEY, 0.0f ) ); // default value in seconds
	isPlayTone = xmlFile->getBool( PLAY_TONE_KEY, false ); // default value
	audioFormatID = boost::algorithm::trim_copy( xmlFile->getString( AUDIO_FORMAT_KEY, "WAV" ) ); // default is the WAV-format
	boost::to_upper( audioFormatID );

	SetFunctionalitySettings( audioDevice, minDetectionDist, isPlayTone );
	SetRecordingSettings( captureTime, isProtocolDefault, audioFormatID );

	Poco::AutoPtr<AbstractConfiguration> loginView( xmlFile->createView( LOGINS_KEY ) );
	loginDatabase.SetFromXML( loginView );

	Poco::AutoPtr<AbstractConfiguration> alarmsView( xmlFile->createView( ALARMS_KEY ) );
	alarmsDatabase.SetFromXML( alarmsView );

	try {
		SetGatewaySettings( loginDatabase, alarmsDatabase );
	} catch ( std::runtime_error e ) {
		throw Utilities::Exception::xml_error( "error:\tNot for every alarm message type (Groupalarm, Email, External Program ...) the required login data is defined." );
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the object is not completely valid, nothing will be written to the XML-file. If no logins are present, this tag will not be present.
*/
void Middleware::CXMLSerializableSettingsParam::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace Poco::Util;

	bool isPlayTone, isProtocolDefault;
	Core::Processing::CAudioDevice audioDevice;
	string audioDeviceName, audioDriverName, audioFormatID;
	float minDetectionDist, captureTime;
	External::CGatewayLoginDatabase loginDB;
	External::CAlarmMessageDatabase alarmsDB;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	if ( IsValid() ) {
		GetFunctionalitySettings( audioDevice, minDetectionDist, isPlayTone );
		GetRecordingSettings( captureTime, isProtocolDefault, audioFormatID );
		GetGatewaySettings( loginDB, alarmsDB );

		if ( audioDevice.IsSet() ) {
			audioDriverName = audioDevice.GetDriverName();
			if ( !audioDriverName.empty() ) {
				xmlFile->setString( AUDIO_KEY + "." + DRIVER_KEY, audioDriverName );
			}

			audioDeviceName = audioDevice.GetDeviceName();
			if ( !audioDeviceName.empty() ) {
				xmlFile->setString( AUDIO_KEY + "." + DEVICE_KEY, audioDeviceName );
			}
		}
		if ( audioDriverName.empty() && audioDeviceName.empty() ) {
			xmlFile->setString( AUDIO_KEY, "" ); // ensures that the tag is always generated
		}
				
		xmlFile->setDouble( CAPTURE_TIME_KEY, captureTime );

		if ( isProtocolDefault ) {
			xmlFile->setString( PROTOCOL_DEFAULT_KEY + "[@" + DEFAULT_KEY + "]", ON_KEY );
		} else {
			xmlFile->setString( PROTOCOL_DEFAULT_KEY + "[@" + DEFAULT_KEY + "]", OFF_KEY );
		}

		xmlFile->setDouble( MIN_DETECTION_DIST_KEY, minDetectionDist );
		xmlFile->setBool( PLAY_TONE_KEY, isPlayTone );
		xmlFile->setString( AUDIO_FORMAT_KEY, audioFormatID );

		Poco::AutoPtr<AbstractConfiguration> loginView( xmlFile->createView( LOGINS_KEY ) );
		auto serializableLoginDB = External::CXMLSerializableGatewayLoginDatabase( loginDB );
		serializableLoginDB.GenerateXML( loginView );

		Poco::AutoPtr<AbstractConfiguration> alarmsView( xmlFile->createView( ALARMS_KEY ) );
		auto serializableAlarmsDB = External::CXMLSerializableAlarmMessagesDatabase( alarmsDB );
		serializableAlarmsDB.GenerateXML( alarmsView );
	}
}