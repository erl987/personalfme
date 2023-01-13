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
#if defined(_MSC_VER)
	#include "stdafx.h"
#endif
#include <algorithm>
#include <list>
#include <sstream>
#include "AudioSettings.h"
#include "ExecutionDetectorRuntime.h"
#include "VersionInfo.h"
#include "sync_cout.h"
#include "PluginLoader.h"
#include "AudioPlugin.h"
#include "XMLException.h"
#include "XMLValidator.h"
#include "EmailGateway.h"
#include "Dir.h"
#include "BasicFunctionality.h"



/**	@brief		Standard constructor
*	@remarks 										None
*/
CBasicFunctionality::CBasicFunctionality()
{
}



/**	@brief		Standard destructor
*	@remarks 										None
*/
CBasicFunctionality::~CBasicFunctionality()
{
}



/**	@brief		Print the available audio input devices on the screen
*	@return											None
*	@remarks 										None
*/
std::string CBasicFunctionality::GetAudioInputDeviceList()
{
	using namespace std;
	using namespace Core::Processing;

	stringstream ss;
	double maxStandardSamplingFreq;
	CAudioDevice stdInputDevice;
	vector<CAudioDevice> inputDevices;
	map< string, vector<CAudioDevice> > inputDevicesByDriver;

	// obtain the available audio devices
	Middleware::CDir directories( Utilities::CVersionInfo::SoftwareName() );
	Middleware::GetAvailableAudioDevices( inputDevices, stdInputDevice, maxStandardSamplingFreq, directories.GetAppSettingsDir() );
	if ( inputDevices.empty() ) {
		throw std::logic_error( u8"Kein Audio-Eingabegerät verfügbar. Eventuell ist in der Mikrofonbuchse kein Kabel eingesteckt." );
	}

	// sort the input devices by the driver type
	for ( auto currInputDevice : inputDevices ) {
		if ( currInputDevice.IsSet() ) {
			inputDevicesByDriver[ currInputDevice.GetDriverName() ].push_back( currInputDevice );
		}
	}

	// generate the output for the user
	ss << u8"Verfügbare Audio-Eingabegeräte:" << endl << endl;

	for ( auto currDriver : inputDevicesByDriver ) {
		ss << u8"Treiber: " << currDriver.first << endl;
		for ( auto currDevice : currDriver.second ) {
			ss << u8"- " << currDevice.GetDeviceName();
			if ( currDevice == stdInputDevice ) {
				ss << u8" (-----> Standardgerät)";
			}
			
			ss << endl;
		}
		ss << endl;
	}

	if ( !stdInputDevice.IsSet() ) {
		ss << u8"Kein Standardgerät verfügbar. Eventuell ist in der Mikrofonbuchse kein Kabel eingesteckt." << endl;
	}

	return ss.str();
}



/**	@brief		Get the help text of the console program
*	@return 										Help text
*	@exception 										None
*	@remarks 										None
*/
std::string CBasicFunctionality::GetHelpText()
{
	using namespace std;

	stringstream ss;
	string softwareName = Utilities::CVersionInfo::SoftwareName();
	
	ss << endl;
	ss << u8"Software-Gateway zur Umsetzung von 5-Tonfolgen der BOS in Alarm Messenger Apps und E-Mail-Nachrichten" << endl;
	ss << endl;
	ss << u8"Mögliche Aufrufoptionen über die Kommandozeile:" << endl;
	ss << endl;
	ss << softwareName << u8" -r \"config.xml\"    : Startet das Gateway mit der Konfiguration"<< endl;
#ifdef __linux
	ss << softwareName << u8" -r \"config.xml\" -d : Startet das Gateway als Hintergrunddienst" << endl;
#endif
	ss << softwareName << u8" -a                 : Listet die verfügbaren Audioaufnahmegeräte auf" << endl;
	ss << softwareName << u8" -h                 : Information über die Benutzung des Programms" << endl;
	ss << softwareName << u8" -pwd               : Zeigt das Konfigurationsverzeichnis" << endl;
	ss << softwareName << u8" -t \"config.xml\"    : Testet die Konfigurationsdatei ohne das" << endl;
	ss << u8"                                 Gateway zu starten" << endl;
	ss << softwareName << u8" -v                 : Versionsinformation" << endl;
	ss << endl;  
	ss << u8"Dateiangaben sind relativ zum Konfigurationsverzeichnis zu verstehen." << endl;
	ss << endl;

	return ss.str();
}



/**	@brief		Get version and license information of the program
*	@return 										License and version information
*	@exception 										None
*	@remarks 										None
*/
std::string CBasicFunctionality::GetBasicVersionInformation()
{
	using namespace std;

	stringstream ss;
	string audioSPversion, audioSPdate, audioSPlicText;

	Core::CAudioInput::GetAudioSPVersion( audioSPversion, audioSPdate, audioSPlicText );
	ss << Utilities::CVersionInfo::SoftwareName() << " " << audioSPversion << u8" (erstellt " << audioSPdate << ")" << endl;
	ss << audioSPlicText << endl << endl;

	return ss.str();
}



/**	@brief		Get version and license information of the program and the libraries
*	@return 										License and version information
*	@exception 										None
*	@remarks 										None
*/
std::string CBasicFunctionality::GetCompleteVersionInformation()
{
	using namespace std;

	stringstream ss;
	string portaudioVersion, portaudioLicText, alglibVersion, alglibDate, alglibLicText, xmlVersion, xmlReleaseDate, xmlLicText;
	string pluginName, pluginLibraryName, pluginLibraryVersion, pluginLibraryDate, pluginLibraryLicText, openSSLVersion, openSSLReleaseDate, openSSLLicText;
	string pluginVersion, pluginDate, pluginLicText;
	int portaudioBuildNum;
	map< string, unique_ptr<Utilities::Plugins::AudioPlugin> > plugins;

	ss << u8"Benutzte Fremdbibliotheken:" << endl << endl;
	Core::CAudioInput::GetPortaudioVersion( portaudioVersion, portaudioBuildNum, portaudioLicText );
	ss << portaudioVersion << u8" (rev. " << portaudioBuildNum << ")" << endl << portaudioLicText << endl << endl;
	
	Core::CAudioInput::GetAlglibVersion( alglibVersion, alglibDate, alglibLicText );
	ss << u8"Alglib " << alglibVersion;
	#ifdef _WIN32
		ss << u8" (veröffentlicht " << alglibDate << ")" << endl << alglibLicText;
	#endif
	ss << endl << endl;

	Utilities::XML::CXMLValidator::GetXercesVersion( xmlVersion, xmlReleaseDate, xmlLicText );
	ss << u8"Apache Xerces " << xmlVersion << u8" (veröffentlicht " << xmlReleaseDate << ")" << endl << xmlLicText << endl;
	
	ss << endl << endl << u8"Audiodatei-Plugins:" << endl << endl;
	Middleware::CDir directories( Utilities::CVersionInfo::SoftwareName() );
	plugins = Utilities::Plugins::LoadPlugins<Utilities::Plugins::AudioPlugin>( directories.GetPluginDir() );
	for ( auto& plugin : plugins ) {
		plugin.second->GetLibraryVersionInfo( pluginName, pluginLibraryName, pluginLibraryVersion, pluginLibraryDate, pluginLibraryLicText );
		plugin.second->GetPluginVersionInfo( pluginName, pluginVersion, pluginDate, pluginLicText );
		ss << pluginName << "-FORMAT " << "(Version " << pluginVersion << ", erstellt am " << pluginDate << ")" << endl;
		ss << pluginLicText << endl << endl;
		ss << "benutzte Fremdbibliothek: " << pluginLibraryName << " " << pluginLibraryVersion << u8" (veröffentlicht " << pluginLibraryDate << ")" << endl << pluginLibraryLicText << endl << endl << endl;
	}
	
	return ss.str();
}


/**	@brief		Checks if the given XML-configuration file is valid and well-formed
*	@param		configFile							XML-configuration file to be checked
*	@return 										Configuration read from the XML-file
*	@exception 	Utilities::Exception::xml_error		Thrown in case of parsing errors
*	@remarks 										The validation is performed using the XML schema definition files located in the user app folder of the program
*/
Middleware::CSettingsParam CBasicFunctionality::ValidateXMLConfigFile( const boost::filesystem::path& configFile )
{
	std::stringstream audioDeviceInfoSS;
	Core::Processing::CAudioDevice device;
	bool isPlayTone;
	float minDistanceRepetition;
	double maxPossibleSamplingFreq;
	Middleware::CSettingsParam params;

	// obtain standard folders (the program uses a subfolder containing its name)
	Middleware::CDir directories( Utilities::CVersionInfo::SoftwareName() );

	// read the XML-file and test for parsing errors
	params.GetFromXML( configFile, directories.GetSchemaDir(), directories.GetPluginDir() ); // will throw an XML-exception in case of a parsing error
	
	// check for the validity of the given audio device on the current system (which is not check by the XML-parser)
	params.GetFunctionalitySettings( device, minDistanceRepetition, isPlayTone );
	if ( !Middleware::IsDeviceAvailable( maxPossibleSamplingFreq, device, directories.GetAppSettingsDir() ) ) {
		if ( device.IsSet() ) {
			audioDeviceInfoSS << u8"(Name: ";
			if ( !device.GetDeviceName().empty() ) {
				audioDeviceInfoSS << device.GetDeviceName();
			} else {
				audioDeviceInfoSS << u8"Standard";
			}
			audioDeviceInfoSS << u8", Treiber: ";
			if ( !device.GetDriverName().empty() ) {
				audioDeviceInfoSS << device.GetDriverName();
			} else {
				audioDeviceInfoSS << u8"Standard";
			}
			audioDeviceInfoSS << u8")";
		} else {
			audioDeviceInfoSS << u8"(ungültiges Gerät)";
		}
		throw Utilities::Exception::xml_error( u8"XML-Fehler: Das angegebene Audiogerät " + audioDeviceInfoSS.str() + u8" ist auf dem System nicht verfügbar. Eventuell ist kein Audiokabel eingesteckt." );
	}

	sync_cout::Inst() << u8"Die Konfigurationsdatei konnte ohne Fehler gelesen werden und ist gültig" << std::endl;

	return params;
}


/**	@brief		Determines the user wish from the command line arguments
*	@param		commandLineArgs						Vector containing all command line arguments in the original order
*	@param		configFile							Will contain the config file (for detection or testing) set by the user. If another option is chosen, it will be empty.
*	@param		doDaemonize							Will be set to true if the progra should be a daemon (only relevant on linux), false otherwise
*	@return 										Choice of the user
*	@exception 	std::logic_error					Thrown if the user choice is invalid
*	@remarks 										None
*/
TypeOfChoice CBasicFunctionality::ProcessCommandLineArguments( const std::vector<std::string>& commandLineArgs, boost::filesystem::path& configFile, bool& doDaemonize )
{
	using namespace std;
	using namespace boost::filesystem;

	bool isWrong;
	string fileName;
	TypeOfChoice choice;
	list< pair<TypeOfChoice, path> > paramList;

	for ( auto arg : commandLineArgs ) {
		if ( ( arg == "--daemon" ) || ( arg == "-d" ) ) {
			paramList.push_back( make_pair( DAEMONIZE, path() ) );

		} else if ( ( arg == "--help" ) || ( arg == "-h" ) ) {
			paramList.push_back( make_pair( HELP, path() ) );

		} else if ( ( arg == "--version" ) || ( arg == "-v" ) ) {
			paramList.push_back( make_pair( VERSION_INFO, path() ) );

		} else if ( (arg == "--audiodevices" ) || ( arg == "-a" ) ) {
			paramList.push_back( make_pair( AUDIO_INFO, path() ) );

		} else if ( ( arg == "--printworkingdir" ) || ( arg == "-pwd" ) ) {
			paramList.push_back( make_pair( PRINT_WORKING_DIR, path() ) );

		} else if ( ( arg == "--test" ) || ( arg == "-t" ) ) {
			paramList.push_back( make_pair( TEST, path() ) );

		} else if ( ( arg == "--run" ) || ( arg == "-r" ) ) {
			paramList.push_back( make_pair( DETECTION, path() ) );

		} else {
			// the argument may be a config file name
			isWrong = true;
			if ( !paramList.empty() ) {
				if ( ( paramList.back().first == TEST ) || ( paramList.back().first == DETECTION ) ) {
					// check for wrong position of arguments
					if ( arg.find( "-" ) == string::npos ) {
						if ( paramList.back().second.empty() ) {
							// remove any quotation marks from the file name ("" and '')
							fileName = arg;
							fileName.erase( remove( begin( fileName ), end( fileName ), '"' ), end( fileName ) );
							fileName.erase( remove( begin( fileName ), end( fileName ), '\'' ), end( fileName ) );
							paramList.back().second = path( fileName );
							isWrong = false;
						}
					}
				}
			}

			if ( isWrong ) {
				throw std::logic_error( u8"Aufrufparameter \"" + arg + u8"\" ist nicht gültig." );
			}
		}
	}

	// check if daemonizing is chosen correctly with the detection mode only
	doDaemonize = false;
	auto daemonizeIt = find_if( begin( paramList ), end( paramList ), []( auto val ) { return ( val.first == DAEMONIZE ); } );
	if ( daemonizeIt != end( paramList ) ) {
		if ( find_if( begin( paramList ), end( paramList ), []( auto val ) { return ( val.first == DETECTION ); } ) == end( paramList ) ) {
			throw std::logic_error( u8"Die Option \"--daemon\" / \"-d\" kann nur in Verbindung mit \"--run\" / \"-r\" genutzt werden." );
		} else {
			doDaemonize = true;
			paramList.erase( daemonizeIt );
		}
	}

	// only exactly one parameter is allowed (except for using daemonize additionally)
	if ( paramList.size() > 1 ) {
		throw std::logic_error( u8"Anzahl der Aufrufparameter falsch." );
	}

	// determine the user wish
	if ( paramList.empty() ) {
		choice = HELP;
	} else {
		choice = paramList.front().first;
	}
	if ( ( choice == DETECTION ) || ( choice == TEST ) ) {
		configFile = paramList.front().second;
		if ( configFile.empty() ) {
			throw std::logic_error( u8"Die Optionen \"--run\" / \"-r\" oder \"--test\" / \"-t\" erfordern die Angabe einer Konfigurationsdatei." );
		}
	}

	return choice;
}