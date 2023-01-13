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

#include "AudioSettings.h"
#include "AudioPlugin.h"
#include "PluginLoader.h"
#include "FileSettings.h"
#include "VersionInfo.h"
#include "XMLUtilities.h"
#include "XMLSerializableSettingsParam.h"
#include "SettingsParam.h"


/** @brief	Standard constructor.
*/
Middleware::CSettingsParam::CSettingsParam(void)
	: isInitFunctionality( false ),
	  isInitRecording( false )
{
}



/** @brief	Destructor.
*/
Middleware::CSettingsParam::~CSettingsParam(void)
{
}



/**	@brief		Resets the object using the data in the XML-file
*	@param		xmlFile							XML-file
*	@param		schemaDir						XML schema file directory
*	@param		pluginDir						Directory of the audio plugins
*	@return										None
*	@exception	Exception::xml_error			Thrown in case of XML-parsing errors
*	@remarks									The XML-file is validated with the XSD-file located in the user app folder, the XSD-file specified in the XML-file is ignored
*/
void Middleware::CSettingsParam::GetFromXML( const boost::filesystem::path& xmlFile, const boost::filesystem::path& schemaDir, const boost::filesystem::path& pluginDir )
{
	using namespace std;
	using namespace Utilities::Plugins;

	CXMLSerializableSettingsParam params;
	map< string, unique_ptr<AudioPlugin> > plugins;
	string audioFormatID;
	float recordingLength;
	bool isDefaultRecording;

	string namespaceName = CFileSettings::GetXMLNamespace( Utilities::CVersionInfo::SoftwareName() );
	Utilities::XML::ReadXML( xmlFile, params, std::make_pair( namespaceName, boost::filesystem::canonical( CFileSettings::GetSettingsParamXMLSchemaFile(), schemaDir ) ) );

	// check for the existence of the audio plugin
	plugins = LoadPlugins<AudioPlugin>( pluginDir );
	params.GetRecordingSettings( recordingLength, isDefaultRecording, audioFormatID );
	auto requiredPluginIt = find_if( begin( plugins ), end( plugins ), [=]( auto& plugin ) { return ( plugin.first == AudioPlugin::CompleteID( audioFormatID ) ); } );
	if ( requiredPluginIt == end( plugins ) ) {
		throw Utilities::Exception::xml_error( "error:\tNo audio plugin is installed on this computer for the format \"" + audioFormatID + "\"." );
	}

	*this = params;
}



/**	@brief		Generats a XML-file from the data in the object
*	@param		xmlFile							XML-file
*	@param		schemaDir						XML schema file directory
*	@return										None
*	@exception									None
*	@remarks									The XSD-file located in the user app folder is written into the XML-file
*/
void Middleware::CSettingsParam::GenerateXML( const boost::filesystem::path& xmlFile, const boost::filesystem::path& schemaDir )
{
	CXMLSerializableSettingsParam params( *this );

	std::string namespaceName = CFileSettings::GetXMLNamespace( Utilities::CVersionInfo::SoftwareName() );
	Utilities::XML::WriteXML( xmlFile, params, "config", std::make_pair( namespaceName, boost::filesystem::canonical( CFileSettings::GetSettingsParamXMLSchemaFile(), schemaDir ) ) );
}



/**	@brief		Setting the class variables.
*	@param		device							Sound input device (sound card) to be used for detection
*	@param		minDistanceRepetition			If two sequences have a time distance [s] smaller than this value, they are disregarded, set this value to 0.0 if you do not require it
*	@param		isPlayTone						Flag stating if a tone is played when a sequence is received
*	@param		loginDatabase					Database storing the login informations for all gateways
*	@param		alarmDatabase					Database storing all information connection FME-sequence codes with the external alarm gateways
*	@param		recordingLength					Length of the recording (in seconds). If no recording is required, set it to 0.
*	@param		isDefaultRecording				Flag stating if by default all sequences are recorded and proctolled (true / false)
*	@param		audioFormatID					Audio format processing plugin ID
*	@return										None
*	@exception									None
*	@remarks									None
*/
void Middleware::CSettingsParam::Set( const Core::Processing::CAudioDevice& device, const float& minDistanceRepetition, const bool& isPlayTone, const External::CGatewayLoginDatabase& loginDatabase, const External::CAlarmMessageDatabase& alarmDatabase, const float& recordingLength, const bool& isDefaultRecording, const std::string& audioFormatID )
{
	SetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );
	SetGatewaySettings( loginDatabase, alarmDatabase );
	SetRecordingSettings( recordingLength, isDefaultRecording, audioFormatID );
}



/**	@brief		Setting the class variables for the functionality settings.
*	@param		device							Sound input device (sound card) to be used for detection
*	@param		minDistanceRepetition			If two sequences have a time distance [s] smaller than this value, they are disregarded, set this value to 0.0 if you do not require it
*	@param		isPlayTone						Flag stating if a tone is played when a sequence is received
*	@return										None
*	@exception									None
*	@remarks									None
*/
void Middleware::CSettingsParam::SetFunctionalitySettings( const Core::Processing::CAudioDevice& device, const float& minDistanceRepetition, const bool& isPlayTone )
{
	CSettingsParam::device = device;
	CSettingsParam::minDistanceRepetition = minDistanceRepetition;
	CSettingsParam::isPlayTone = isPlayTone;

	isInitFunctionality = true;
}



/**	@brief		Getting the class variables.
*	@param		device							Sound input device (sound card) to be used for detection
*	@param		minDistanceRepetition			If two sequences have a time distance [s] smaller than this value, they are disregarded, set this value to 0 if you do not require it
*	@param		isPlayTone						Flag stating if a tone is played when a sequence is received
*	@param		loginDatabase					Database storing the login informations for all gateways
*	@param		alarmDatabase					Database storing all information connection FME-sequence codes with the external alarm gateways
*	@param		recordingLength					Length of the recording (in seconds). If no recording is required, set it to 0.
*	@param		isDefaultRecording				Flag stating if by default all sequences are recorded and proctolled (true / false)
*	@param		audioFormatID					Audio format processing plugin ID
*	@return										None
*	@exception	std::runtime_error				Thrown if the data has not been initialized completely before getting it
*	@remarks									None
*/
void Middleware::CSettingsParam::Get( Core::Processing::CAudioDevice& device,float& minDistanceRepetition, bool& isPlayTone, External::CGatewayLoginDatabase& loginDatabase, External::CAlarmMessageDatabase& alarmDatabase, float& recordingLength, bool& isDefaultRecording, std::string& audioFormatID ) const
{
	GetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );
	GetGatewaySettings( loginDatabase, alarmDatabase );
	GetRecordingSettings( recordingLength, isDefaultRecording, audioFormatID );
}



/**	@brief		Getting the class variables.
*	@param		device							Sound input device (sound card) to be used for detection
*	@param		minDistanceRepetition			If two sequences have a time distance [s] smaller than this value, they are disregarded, set this value to 0 if you do not require it
*	@param		isPlayTone						Flag stating if a tone is played when a sequence is received
*	@return										None
*	@exception	std::runtime_error				Thrown if the data has not been initialized completely before getting it
*	@remarks									None
*/
void Middleware::CSettingsParam::GetFunctionalitySettings( Core::Processing::CAudioDevice& device, float& minDistanceRepetition, bool& isPlayTone ) const
{
	if ( !IsValid() ) {
		throw std::runtime_error( "The data has not been initialized completely before use." );
	}

	device = CSettingsParam::device;
	minDistanceRepetition = CSettingsParam::minDistanceRepetition;
	isPlayTone = CSettingsParam::isPlayTone;
}



/**	@brief		Getting the data for the alarm gateways
*	@param		loginDatabase					Database storing the login informations for all gateways
*	@param		alarmDatabase					Database storing all information connection FME-sequence codes with the external alarm gateways
*	@return										None
*	@exception	std::runtime_error				Thrown if the data has not been initialized completely before getting it
*	@remarks									None
*/
void Middleware::CSettingsParam::GetGatewaySettings(External::CGatewayLoginDatabase& loginDatabase, External::CAlarmMessageDatabase& alarmDatabase) const
{
	if ( !IsValid() ) {
		throw std::runtime_error( "The data has not been initialized completely before use." );
	}

	loginDatabase = CSettingsParam::loginDatabase;
	alarmDatabase = CSettingsParam::alarmDatabase;
}



/**	@brief		Setting the data for the alarm gateways
*	@param		loginDatabase					Database storing the login informations for all gateways
*	@param		alarmDatabase					Database storing all information connection FME-sequence codes with the external alarm gateways
*	@return										None
*	@exception	std::runtime_error				Thrown if the not for all alarm message types the login data is provided
*	@remarks									None
*/
void Middleware::CSettingsParam::SetGatewaySettings(const External::CGatewayLoginDatabase& loginDatabase, const External::CAlarmMessageDatabase& alarmDatabase)
{
	using namespace std;

	bool isCorrect = true;
	vector<External::GatewayDatasetType> loginGateways;
	vector<type_index> loginTypeList, messageTypeList;

	loginGateways = loginDatabase.GetAllGateways();
	for ( auto& currLogin : loginGateways ) {
		loginTypeList.push_back( currLogin.first );
	}

	messageTypeList = alarmDatabase.GetAllMessageTypes();

	// ensure that for each message type the login data is available
	for ( auto messageType : messageTypeList ) {
		if ( find( begin( loginTypeList ), end( loginTypeList ), messageType ) == end( loginTypeList ) ) {
			isCorrect = false;
		}
	}

	if ( !isCorrect ) {
		throw std::runtime_error( "Not for all alarm message types the login data is provided." );
	}

	CSettingsParam::loginDatabase = loginDatabase;
	CSettingsParam::alarmDatabase = alarmDatabase;

	UpdateWhitelist();
}



/**	@brief		Getting the class variables for the audio recording.
*	@param		recordingLength					Length of the recording (in seconds). If no recording is required, set it to 0.
*	@param		isDefaultRecording				Flag stating if by default all sequences are recorded and proctolled (true / false)
*	@param		audioFormatID					Audio format processing plugin ID
*	@return										None
*	@exception	std::runtime_error				Thrown if the data has not been initialized completely before getting it
*	@remarks									None
*/
void Middleware::CSettingsParam::GetRecordingSettings( float& recordingLength, bool& isDefaultRecording, std::string& audioFormatID ) const
{
	if ( !IsValid() ) {
		throw std::runtime_error( "The data has not been initialized completely before use." );
	}

	recordingLength = CSettingsParam::recordingLength;
	isDefaultRecording = CSettingsParam::isDefaultRecording;
	audioFormatID = CSettingsParam::audioFormatID;
}



/**	@brief		Setting the class variables for the audio recording.
*	@param		recordingLength					Length of the recording (in seconds). If no recording is required, set it to 0.
*	@param		isDefaultRecording				Flag stating if by default all sequences are recorded and proctolled (true / false)
*	@param		audioFormatID					Audio format processing plugin ID
*	@return										None
*	@exception									None
*	@remarks									None
*/
void Middleware::CSettingsParam::SetRecordingSettings( const float& recordingLength, const bool& isDefaultRecording, const std::string& audioFormatID )
{
	CSettingsParam::recordingLength = recordingLength;
	CSettingsParam::isDefaultRecording = isDefaultRecording;
	CSettingsParam::audioFormatID = audioFormatID;

	isInitRecording = true;

	UpdateWhitelist();
}



/**	@brief		Equality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are identical, otherwise false
*	@exception									None
*	@remarks 									If an object is not completely set, it is assumed to be invalid
*/
bool Middleware::operator==( const CSettingsParam& lhs, const CSettingsParam& rhs )
{
	Core::Processing::CAudioDevice deviceLhs, deviceRhs;
	bool isDefaultRecordingLhs, isDefaultRecordingRhs;
	float minDistanceRepetitionLhs, minDistanceRepetitionRhs;
	bool isPlayToneLhs, isPlayToneRhs;
	External::CGatewayLoginDatabase loginDatabaseLhs, loginDatabaseRhs;
	External::CAlarmMessageDatabase alarmDatabaseLhs, alarmDatabaseRhs;
	float recordingLengthLhs, recordingLengthRhs;
	std::string audioFormatIDLhs, audioFormatIDRhs;

	if ( lhs.IsValid() != rhs.IsValid() ) {
		return false;
	}

	if ( lhs.IsValid() ) { // this implies also that the right-hand side is valid
		lhs.Get( deviceLhs, minDistanceRepetitionLhs, isPlayToneLhs, loginDatabaseLhs, alarmDatabaseLhs, recordingLengthLhs, isDefaultRecordingLhs, audioFormatIDLhs );
		rhs.Get( deviceRhs, minDistanceRepetitionRhs, isPlayToneRhs, loginDatabaseRhs, alarmDatabaseRhs, recordingLengthRhs, isDefaultRecordingRhs, audioFormatIDRhs );

		if ( deviceLhs != deviceRhs ) {
			return false;
		}
		if ( minDistanceRepetitionLhs != minDistanceRepetitionRhs ) {
			return false;
		}
		if ( isPlayToneLhs != isPlayToneRhs ) {
			return false;
		}
		if ( loginDatabaseLhs != loginDatabaseRhs ) {
			return false;
		}
		if ( alarmDatabaseLhs != alarmDatabaseRhs ) {
			return false;
		}
		if ( recordingLengthLhs != recordingLengthRhs ) {
			return false;
		}
		if ( isDefaultRecordingLhs != isDefaultRecordingRhs ) {
			return false;
		}
		if ( audioFormatIDLhs != audioFormatIDRhs ) {
			return false;
		}
	}

	return true;
}



/**	@brief		Unequality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are not identical, otherwise false
*	@exception									None
*	@remarks 									If an object is not completely set, it is assumed to be invalid
*/
bool Middleware::operator!=( const CSettingsParam& lhs, const CSettingsParam& rhs )
{
	return !( lhs == rhs );
}



/**	@brief		Determines if the settings are completely set
*	@return										True if the settings are completely set, false otherwise
*	@exception									None
*	@remarks 									Even if the result is false, some parts of the settings may be valid
*/
bool Middleware::CSettingsParam::IsValid() const
{
	if ( isInitFunctionality && isInitRecording ) {
		return true;
	} else {
		return false;
	}
}



/**	@brief		Generates the whitelist based on the current settings
*	@return										Whitelist based on the 'isDefaultRecording' and alarm database settings
*	@exception	std::runtime_error				Thrown if the data has not been initialized completely before getting it
*	@remarks									The whitelist is empty (all codes allowed) if 'isDefaultRecording' is true, otherwise it contains all codes present in the alarm database
*/
std::vector< std::vector<int> > Middleware::CSettingsParam::GetWhitelist() const
{
	if ( !IsValid() ) {
		throw std::runtime_error( "The data has not been initialized completely before use." );
	}

	return whitelist;
}



/**	@brief		Updates the whitelist based on the 'isDefaultRecording' and alarm database settings
*	@return										None
*	@exception									None
*	@remarks									The whitelist is empty (all codes allowed) if 'isDefaultRecording' is true, otherwise it contains all codes present in the alarm database
*/
void Middleware::CSettingsParam::UpdateWhitelist()
{
	if ( isInitRecording ) {
		if ( isDefaultRecording ) {
			// all codes are allowed
			whitelist.clear();
		} else {
			// if recording is generally forbidden, all codes present in the alarm database are anyway accepted
			whitelist = alarmDatabase.GetAllCodes();
		}
	}
}