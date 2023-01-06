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
#if defined( _WIN32 )
	#include "stdafx.h"
#endif
#include <boost/filesystem.hpp>
#include "DateTime.h"
#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"
#include "StringUtilities.h"
#include "InfoalarmMessageDecorator.h"
#include "Logger.h"



/**	@brief		Access to the singleton instance
*	@param		logFile					Log file (use it only for setting the log file)
*	@return								Singleton instance that can be used for calling the CLogger::Log() method
*	@exception	std::invalid_argument	Thrown if the method is called without a file name during the first call
*	@exception	std::runtime_error		Thrown if the log file could not be opened
*	@remarks							Calling this method with the file name after initialization will have no effect on the log file
*/
Logger::CLogger& Logger::CLogger::Instance( const boost::filesystem::path& logFile )
{
	// instantiate the singleton in case of the setting the log file
	std::call_once( onceFlag, [=]() {
		if ( logFile.empty() ) {
			std::invalid_argument( "The logger singleton can only be instantiated with a log file name." );
		}

		instancePtr.reset( new CLogger( logFile ) );
	} );

	// return the singleton instance
	return *instancePtr;
}



/**	@brief		Destructor
*/
Logger::CLogger::~CLogger(void)
{
	outFile.close();
}



/**	@brief		Constructor
*	@param		logFile					Log file
*	@return								None
*	@exception	std::runtime_error		Thrown if the log file could not be opened
*	@remarks							The constructor is private and can therefore only be used for instantiating the singleton
*/
Logger::CLogger::CLogger( const boost::filesystem::path& logFile )
{
	using namespace boost;
	bool  isNewFile;

	if ( filesystem::exists( logFile ) ) {
		isNewFile = false;
	} else {
		isNewFile = true;
	}

	outFile.open( logFile.string(), std::ios_base::app );
	if ( !outFile.good() ) {
		throw std::runtime_error( u8"Die Logdatei kann nicht initialisiert werden." );
	}

	if ( isNewFile ) {
		filesystem::permissions( logFile.string(), filesystem::owner_write | filesystem::owner_read | filesystem::group_write | filesystem::group_read | filesystem::others_write | filesystem::others_read );
	}
}



/**	@brief		Non-thread safe logging worker method
*	@param		logLevel				Level of the event to be stored
*	@param		eventTime				Time of the event (UTC-time)
*	@param		message					Message to be stored
*	@return								None
*	@exception	std::runtime_error		Thrown if the storage of the event message on file failed. The event time is stored in German local time.
*	@remarks							The method itself is not thread-safe
*/
void Logger::CLogger::PerformLogging( const Utilities::Message::MessageType& logLevel, const boost::posix_time::ptime& eventTime, const std::string& message)
{
	using namespace std;
	using namespace Utilities::Time;
	using namespace Utilities::Message;

	outFile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
	try {
		outFile << german_local_date_time( eventTime ) << "\t";
		if ( logLevel == MESSAGE_ERROR ) {
			outFile << u8"FEHLER: ";
		}
		outFile << message << endl;
		outFile.flush();
	} catch ( std::exception& e ) {
		throw std::runtime_error( std::string( u8"Fehler beim Schreiben der Log-Datei: " ) + e.what() );
	}
}


/**	@brief		Adds a status message to the protocol
*	@param		message								Status message to be added
*	@return											Text logged for the present status message
*	@exception	std::runtime_error					Thrown if the storage of the event message on file failed or if the message was empty
*	@remarks 										None
*/
std::string Logger::CLogger::Log( std::unique_ptr<Utilities::Message::CStatusMessage> message )
{
	using namespace std;
	using namespace External;
	using namespace Utilities::Message;

	string messageText;

	// check if the message is empty
	if ( !message ) {
		throw std::runtime_error( "Logger called without a log message" );
	}

	std::lock_guard< std::mutex > lock( logMutex );
	auto messageType = message->GetType();
	auto messageTimestamp = message->GetTimestamp();

	// generate the protocol message
	if ( typeid( *message ) == typeid( CSendStatusMessage<CAlarmMessage> ) ) {
		messageText = GenerateSendStatusMessage( dynamic_cast< const CSendStatusMessage<CAlarmMessage>& >( *message ) );
	} else if ( typeid( *message ) == typeid( CDetectorStatusMessage ) ) {
		messageText = GenerateDetectorStatusMessage( dynamic_cast<const CDetectorStatusMessage& >( *message ) );
	} else if ( typeid( *message ) == typeid( CGeneralStatusMessage ) ) {
		messageText = GenerateGeneralStatusMessage( dynamic_cast<const CGeneralStatusMessage&>( *message ) );
	}

	// saving in the log file
	PerformLogging( messageType, messageTimestamp, messageText );

	return messageText;
}


/**	@brief		Generate a send status message to the protocol
*	@param		message								Send status message to be added
*	@return											Generated status message for the protocol
*	@exception										None
*	@remarks 										None
*/
std::string Logger::CLogger::GenerateSendStatusMessage( const Utilities::Message::CSendStatusMessage<External::CAlarmMessage>& message ) const
{
	using namespace std;
	using namespace External;
	using namespace External::Infoalarm;
	using namespace Utilities::Message;
	using namespace Utilities::Time;

	unsigned int numTrials;
	vector<int> code;
	unique_ptr<CAlarmMessage> receivedAlarmMessage, alarmMessage;
	Utilities::CDateTime sequenceTime;
	SendStatus sendStatus;
	string messageText, timeString, messageTypeString, messageInfoString;
	stringstream codeStream, timeStream;
	chrono::seconds timeDistTrials;

	// obtain the required information in the dependency of the alarm gateway type (Groupalarm, E-mail, ...)
	messageTypeString = "";
	messageInfoString = "";

	message.GetMessageContent( sequenceTime, code, receivedAlarmMessage, sendStatus, numTrials, timeDistTrials );
	if ( receivedAlarmMessage ) {
		if ( typeid( *receivedAlarmMessage ) == typeid( CInfoalarmMessageDecorator ) ) {
			alarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>( *receivedAlarmMessage ).GetContainedMessage();
		} else {
			alarmMessage = std::move( receivedAlarmMessage );
		}

		if ( typeid( *alarmMessage ) == typeid( Groupalarm::CGroupalarm2Message ) ) {
			GetGroupalarmInfo( dynamic_cast<const Groupalarm::CGroupalarm2Message&> ( *alarmMessage ), messageInfoString, messageTypeString );
		} else if ( typeid( *alarmMessage ) == typeid( Email::CEmailMessage ) ) {
			GetEmailInfo( dynamic_cast<const Email::CEmailMessage&>( *alarmMessage ), messageInfoString, messageTypeString );
		}
	}

	// extract the time string
	for ( auto val : code ) { codeStream << val; };
	timeStream << german_local_date_time( CBoostStdTimeConverter::ConvertToBoostTime( sequenceTime ) );
	timeString = timeStream.str();
	timeString.erase( begin( timeString ), begin( timeString ) + timeString.find( ":" ) - 2 );

	// generate the protocol message
	messageText = messageTypeString + u8"Versand für Schleife " + codeStream.str() + " (" + timeString;
	if ( !messageInfoString.empty() ) {
		messageText += ", " + messageInfoString;
	}
	messageText += "): ";

	switch ( sendStatus.code ) {
	case SUCCESS:
		messageText += messageTypeString + u8"Versand erfolgreich.";
		break;
	case NONFATAL_FAILURE:
		messageText += u8"Verbindungsfehler (" + sendStatus.text + u8"). Nächster Versuch in " + to_string( timeDistTrials.count() ) + u8" Sekunden ...";
		break;
	case FATAL_FAILURE:
		messageText += sendStatus.text;
		break;
	case TIMEOUT_FAILURE:
		messageText += u8"Maximale Anzahl von Versuchen erreicht. " + messageTypeString + u8"Versand endgültig gescheitert.";
		break;
	case NOT_IN_DB:
		messageText += u8"Kein Versand, da nicht in der Datenbank.";
		break;
	case NO_MESSAGE:
		messageText += u8"Kein Versand, da alle Nachrichten unterdrückt sind.";
		break;
	}

	return messageText;
}


/**	@brief		Generates a detector status message to the protocol
*	@param		message								Detector status message to be added
*	@return											Generated status message for the protocol
*	@exception										None
*	@remarks 										None
*/
std::string Logger::CLogger::GenerateDetectorStatusMessage( const Utilities::Message::CDetectorStatusMessage& message ) const
{
	using namespace std;
	using namespace External;
	using namespace Utilities::Message;

	string messageText;
	DetectorStatusCode detectorStatus;
	int samplingFreq;

	message.GetMessageContent( detectorStatus, samplingFreq );
	switch ( detectorStatus ) {
	case RUNNING:
		messageText = u8"5-Tonfolgen-Detektion gestartet (Abtastrate: " + to_string( samplingFreq ) + u8" Hz).";
		break;
	case STOPPED:
		messageText = u8"5-Tonfolgen-Detektion gestoppt.";
		break;
	}

	return messageText;
}


/**	@brief		Generates a general status message to the protocol
*	@param		message								General status message to be added
*	@return											Generated status message for the protocol
*	@exception										None
*	@remarks 										None
*/
std::string Logger::CLogger::GenerateGeneralStatusMessage( const Utilities::Message::CGeneralStatusMessage& message ) const
{
	std::string messageText;

	message.GetMessageText( messageText );

	return messageText;
}


/**	@brief		Short information string on a set of Groupalarm.de configurations
*	@param		alarmMessage						Groupalarm.de configuration
*	@param		messageInfoString					Contains short information string on the set after the method call
*	@param		messageTypeString					Contains the type of the message after the call
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void Logger::CLogger::GetGroupalarmInfo( const External::Groupalarm::CGroupalarm2Message& alarmMessage, std::string& messageInfoString, std::string& messageTypeString ) const
{
	using namespace Utilities;

	std::stringstream infoStream;

	if ( alarmMessage.IsEmpty() ) {
		infoStream << u8"keine Groupalarm-Auslösung";
	} else {
		if (alarmMessage.ToAllUsers()) {
			infoStream << u8"Vollalarm" << ", ";
		} else {
			if (alarmMessage.ToScenarios()) {
				if (alarmMessage.GetScenarios().size() == 1) {
					infoStream << u8"Szenario: ";
				} else {
					infoStream << u8"Szenarien: ";
				}
				infoStream << CStringUtilities().Join(alarmMessage.GetScenarios(), ",") << ", ";
			}
			if (alarmMessage.ToLabels()) {
				if (alarmMessage.GetLabels().size() == 1) {
					infoStream << u8"Label: ";
				} else {
					infoStream << u8"Labels: ";
				}
				for (const auto& labelInfo : alarmMessage.GetLabels()) {
					infoStream << labelInfo.first << ": " << labelInfo.second << ", ";
				}
			}
			if (alarmMessage.ToUsers()) {
				infoStream << u8"Teilnehmer: " << CStringUtilities().Join(alarmMessage.GetUsers(), ",") << ", ";
			}
			if (alarmMessage.ToUnits()) {
				if (alarmMessage.GetUnits().size() == 1) {
					infoStream << u8"Einheit: ";
				}
				else {
					infoStream << u8"Einheiten: ";
				}
				infoStream << CStringUtilities().Join(alarmMessage.GetUnits(), ",") << ", ";
			}
		}

		if (alarmMessage.ToAlarmTemplate()) {
			infoStream << u8"Alarmvorlage: " << alarmMessage.GetAlarmTemplate() << ", ";
		} else {
			if (alarmMessage.HasMessageText()) {
				infoStream << u8"Alarmtext: " << alarmMessage.GetMessageText() << ", ";
			} else {
				infoStream << u8"Textvorlage: " << alarmMessage.GetMessageTemplate() << ", ";
			}
		}

		infoStream << u8"Ereignis aktiv für " << alarmMessage.GetEventOpenPeriodInHours() << " Stunden";
	}

	messageInfoString = infoStream.str();
	messageTypeString = u8"Groupalarm-";
}


/**	@brief		Short information string on a set of e-mail configurations
*	@param		alarmMessage						E-mail configuration
*	@param		messageInfoString					Contains short information string on the set after the method call
*	@param		messageTypeString					Contains the type of the message after the call
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
void Logger::CLogger::GetEmailInfo( const External::Email::CEmailMessage& alarmMessage, std::string& messageInfoString, std::string& messageTypeString ) const
{
	using namespace std;

	stringstream infoStream;
	vector< pair<string, string> > recipients;
	string siteID, alarmID, alarmText;
	bool isImmediateSending;

	if ( alarmMessage.IsEmpty() ) {
		infoStream << u8"kein E-Mail-Versand";
	} else {
		alarmMessage.Get( siteID, alarmID, recipients, alarmText, isImmediateSending );
		infoStream << u8"Empfänger: ";
		for ( auto recipient : recipients ) {
			infoStream << recipient.second + ", ";
		}
		infoStream << u8"Alarmtext: " + alarmText;
		if ( !isImmediateSending ) {
			infoStream << u8", mit Alarmdurchsage";
		} else {
			infoStream << u8", ohne Alarmdurchsage";
		}
	}

	messageInfoString = infoStream.str();
	messageTypeString = u8"Email-";
}


// instantiation of static class members
std::mutex Logger::CLogger::logMutex;
std::unique_ptr<Logger::CLogger> Logger::CLogger::instancePtr = nullptr;
std::once_flag Logger::CLogger::onceFlag;