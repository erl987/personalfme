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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <regex>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <Poco/SharedPtr.h>
#include <Poco/Path.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/StringPartSource.h>
#include <Poco/Net/FilePartSource.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/SecureStreamSocket.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/RejectCertificateHandler.h>
#include <Poco/Net/SSLException.h>
#ifndef _WIN32
	#include <openssl/ssl.h>
#endif
#include "VersionInfo.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"
#include "EmailLoginData.h"
#include "EmailGatewayImpl.h"
#include "rust.h"


/** @brief		Standard constructor
*	@remarks								The used SSL-manager of Poco is a singleton. If other classes within the program use different settings, they are overwritten by the present constructor.
*/
External::Email::CEmailGateway::CEmailGatewayImpl::CEmailGatewayImpl( void )
{
	// the SSL system should only be initiated once in the program
	std::call_once( rootCertificatesLoadOnceFlag, &CEmailGatewayImpl::LoadRootCertificatesFromOSTrustStore );
}



/** @brief		Populates the trust store from the OS trust store
*	@return									None
*	@exception								None
*	@remarks								This method uses a singleton, call it only once in the application from the constructor.
*											Any self-signed certificate is rejected.
*/
void External::Email::CEmailGateway::CEmailGatewayImpl::LoadRootCertificatesFromOSTrustStore()
{

	using namespace Poco;
	using namespace Poco::Net;
	Poco::Net::Context::Ptr sslContext;

	SharedPtr<InvalidCertificateHandler> certHandler = new RejectCertificateHandler( false );

	#if defined _WIN32
		// Windows SChannel-SSL
		sslContext = new Poco::Net::Context( Context::CLIENT_USE, "", Context::VerificationMode::VERIFY_RELAXED, 9, true );
	#else
		// OpenSSL (system library on Linux)
		sslContext = new Poco::Net::Context( Context::CLIENT_USE, "", Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH" );
		SSL_CTX_set_default_verify_paths( sslContext->sslContext() );
	#endif

	SSLManager::instance().initializeClient( 0, certHandler, sslContext );
}



/** @brief		Send an e-mail message with the SMTP-protocol
*	@param		message						Message to be sent
*	@param		loginData					Login data for the e-mail server
*	@return									None
*	@exception	NetException				Thrown if the internet connection was missing (or if the host is unknown)
*	@exception	SSLException				Thrown if an error occured during setting up the TLS/SSL-connection
*	@exception	SMTPException				Thrown if the e-mail-sending failed due to a SMTP-connection error
*	@remarks								None
*/
void External::Email::CEmailGateway::CEmailGatewayImpl::SendSMTP( const Poco::Net::MailMessage& message, std::unique_ptr<CGatewayLoginData> loginData )
{
	using namespace std;
	using namespace Poco::Net;

	AuthType authType;
	ConnectionType connectionType;
	unsigned short currPort, port;
	bool loginSucceeded;
	stringstream errors;
	string hostName, senderAddress, userName, password;
	unique_ptr<SMTPClientSession> mailSession;
	map< AuthType, vector<SMTPClientSession::LoginMethod> > possibleLoginMethods;

    // TODO: some simple dummy code calling a Rust function
    std::cout << "From Rust: " << rust_function(5) << std::endl;

	// possible SMTP-login methods
	possibleLoginMethods[NO_AUTH].push_back( SMTPClientSession::AUTH_NONE );
	possibleLoginMethods[UNENCRYPTED_AUTH].push_back( SMTPClientSession::AUTH_LOGIN );
	possibleLoginMethods[UNENCRYPTED_AUTH].push_back( SMTPClientSession::AUTH_PLAIN );
	possibleLoginMethods[ENCRYPTED_AUTH].push_back( SMTPClientSession::AUTH_CRAM_SHA1 );
	possibleLoginMethods[ENCRYPTED_AUTH].push_back( SMTPClientSession::AUTH_CRAM_MD5 );

	dynamic_cast< const Email::CEmailLoginData& >( *loginData ).GetServerInformation( connectionType, hostName, port );
	dynamic_cast< const Email::CEmailLoginData& >( *loginData ).GetLoginInformation( senderAddress, authType, userName, password );

	if ( port == DEFAULT_PORT ) {
		currPort = CEmailLoginData::GetDefaultPort( connectionType, authType );
	}
	else {
		currPort = port;
	}

	// start the SMTP-session connection
	switch ( connectionType ) {
	case UNENCRYPTED_CONN:
		mailSession = make_unique<SMTPClientSession>( hostName, currPort );
		break;
	case TLSSTART_CONN:
		mailSession = make_unique<SecureSMTPClientSession>( hostName, currPort );
		mailSession->login();

		// the session is aborted if no encrypted connection can be established
		if ( !dynamic_cast< SecureSMTPClientSession* >( mailSession.get() )->startTLS( SSLManager::instance().defaultClientContext() ) ) {
			mailSession->close();
			throw SMTPException( "No encrypted connection could be established." );
		}
		break;
	case TLS_SSL_CONN:
		mailSession = make_unique<SMTPClientSession>( SecureStreamSocket( SocketAddress( hostName, currPort ) ) );
		break;
	default:
		throw std::runtime_error( "The connection type is not supported." );
		break;
	}

	// login with the required SMTP-session authentification
	loginSucceeded = false;
	for ( size_t methodID = 0; methodID < possibleLoginMethods[authType].size(); methodID++ ) {
		try {
			mailSession->login( possibleLoginMethods[authType][methodID], userName, password );
			loginSucceeded = true;
			break;
		}
		catch ( SMTPException e ) {
			errors << e.message() << endl;
		}
	}
	if ( !loginSucceeded ) {
		mailSession->close();
		throw SMTPException( errors.str() );
	}

	// send the message
	mailSession->sendMessage( message );
	mailSession->close();
}


/** @brief		Performs the sending of the alarm via the gateway
*	@param		code						Container with the alarm code
*	@param		alarmTime					Time of the alarm (UTC)
*	@param		isRealAlarm					Flag stating if the current alarm is a real (default) or test alarm
*	@param		loginData					Login data for the e-mail server
*	@param		message						Data for the alarm message
*	@param		audioFile					Audio file of the alarm message. If it is empty, no file will be attached. It is empty by default.
*	@return									None
*	@exception	std::domain_error			Thrown in case of recoverable connection errors (missing internet connection, timeout, ...)
*	@exception	std::runtime_error			Thrown in case of fatal connection errors (SMTP-connection error,  SSL-certificate could not be accepted, ...)
*	@remarks								All strings are supposed to be encoded in UTF-8
*/
void External::Email::CEmailGateway::CEmailGatewayImpl::Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile )
{
	using namespace std;
	using namespace Infoalarm;
	using namespace Email;
	using namespace Utilities::Time;
	using namespace Poco::Net;
	using namespace boost::filesystem;

	MailMessage mailMessage;
	AuthType authType;
	bool requiredState, isInfoalarm;;
	string alarmTypeString, siteID, alarmID, alarmText, senderAddress, userName, password;
	string versionString, dateString, licenseText, otherMessagesInfo;
	vector< pair<string, string> > recipients;
	stringstream contentStream, codeStream;

	// obtain the required information
	if ( typeid( *message ) == typeid( CInfoalarmMessageDecorator ) ) {
		auto infoalarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>( *message );
		auto alarmMessage = infoalarmMessage.GetContainedMessage();
		dynamic_cast<const CEmailMessage&>( *alarmMessage ).Get( siteID, alarmID, recipients, alarmText, requiredState ); // it is guaranteed by the caller that all alarm datasets are not empty
		otherMessagesInfo = CreateOtherMessagesInfo( infoalarmMessage );
		isInfoalarm = true;
	} else {
		dynamic_cast<const CEmailMessage&>( *message ).Get( siteID, alarmID, recipients, alarmText, requiredState ); // it is guaranteed by the caller that all alarm datasets are not empty
		isInfoalarm = false;
	}

	dynamic_cast<const CEmailLoginData&>( *loginData ).GetLoginInformation( senderAddress, authType, userName, password );

	try {
		mailMessage.setSender( senderAddress );
		for ( auto it = recipients.cbegin(); it != recipients.cend(); it++ ) {
			mailMessage.addRecipient( MailRecipient( MailRecipient::PRIMARY_RECIPIENT, it->second, it->first ) );
		}

		// generate the e-mail message text
		for_each( begin( code ), end( code ), [&]( int val ) { codeStream << val; } );
		if ( !isInfoalarm ) {
			if ( isRealAlarm ) {
				alarmTypeString = DefaultAlarmString;
			} else {
				alarmTypeString = TestAlarmString;
			}
		} else {
			if ( isRealAlarm ) {
				alarmTypeString = InfoAlarmString;
			} else {
				alarmTypeString = InfoAlarmString + " (" + TestAlarmString + ")";
			}
		}

		contentStream << u8"Zeitpunkt:\t\t" << german_local_date_time( CBoostStdTimeConverter::ConvertToBoostTime( alarmTime ) ) << u8"\r\n";
		contentStream << u8"Tonfolge (Schleife):\t" << codeStream.str() << u8"\r\n";
		contentStream << u8"Status:\t\t\t" << alarmTypeString << u8"\r\n" << u8"\r\n";
		contentStream << u8"Alarmtext:\r\n";
		contentStream << alarmText << u8"\r\n" << u8"\r\n";
		if ( isInfoalarm ) {
			contentStream << u8"Durchgef\u00FChrte Alarmierungen:\r\n";
			contentStream << otherMessagesInfo;
		}
		contentStream << u8"\r\n" << u8"\r\n";
		if ( !audioFile.IsEmpty() ) {
			contentStream << u8"Alarmdurchsage als Audiodatei im Anhang angef\u00FCgt.\r\n\r\n";
		}
		Utilities::CVersionInfo::GetSoftwareVersion( versionString, dateString, licenseText );
		contentStream << u8"--\r\nAutomatisch erzeugt von " + Utilities::CVersionInfo::SoftwareName() + " " + versionString + u8"\r\n";
		contentStream << u8"Kontakt: " << senderAddress << u8"\r\n";
		contentStream << u8"Internet: http://www.personalfme.de\r\n";

		mailMessage.setContentType( "text/plain; charset=UTF-8" );
		mailMessage.setSubject( MailMessage::encodeWord( alarmTypeString + " " + siteID + ", " + alarmID, "UTF-8" ) );
		mailMessage.addContent( new StringPartSource( contentStream.str(), "text/plain; charset=UTF-8" ) ); // ownership of the pointer is taken over by mailMessage
		if ( !audioFile.IsEmpty() ) {
			mailMessage.addAttachment( audioFile.GetFilePath().filename().string(), new FilePartSource( audioFile.GetFilePath().string(), audioFile.GetFilePath().filename().string(), audioFile.GetMIMEtype() ) );
		}

		// send the e-mail
		SendSMTP( mailMessage, move( loginData ) );
	} catch ( SMTPException &e ) {
		if ( IsTemporarySMTPServerError( e ) ) {
			throw std::domain_error( e.displayText() );		// temporary server error (code 4XX): non-fatal error
		} else {
			throw std::runtime_error( e.displayText() );	// permanent server error (code 5XX)
		}
	} catch ( SSLException &e ) {
		throw std::runtime_error( e.displayText() );
	} catch ( NetException &e ) {
		throw std::domain_error( e.displayText() );			// non-fatal exception
	} catch ( Poco::TimeoutException& e ) {
		throw std::domain_error( e.displayText() );			// non-fatal exception
	} catch ( Poco::Exception& e ) {
		throw std::runtime_error( e.displayText() );
	} catch ( std::exception& e ) {
		throw std::runtime_error( e.what() );
	}
}


/** @brief		Determines if an SMTP exception is a temporary server error (code 4XX)
*	@param		e							SMTP-exception
*	@return									True if the exception is a temporary server error, false otherwise
*	@exception								None
*	@remarks								Permanent server errors due to a wrong message setting have the code 5XX
*/
bool External::Email::CEmailGateway::CEmailGatewayImpl::IsTemporarySMTPServerError( const Poco::Net::SMTPException& e )
{
	std::regex reg( "\\b(4\\d{2})\\b" ); // match only 4XX
	
	return std::regex_search( e.displayText(), reg );
}


/** @brief		Provides a string containing a summary of the alarms corresponding to the infoalarm
*	@param		infoalarmMessage			Infoalarm message
*	@return									Summary string containing the alarms corresponding to the infoalarm
*	@exception								None
*	@remarks								None
*/
std::string External::Email::CEmailGateway::CEmailGatewayImpl::CreateOtherMessagesInfo( const Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage )
{
	using namespace std;
	using namespace Email;
	using namespace Groupalarm;
	using namespace ExternalProgram;

	unsigned int counter = 1;
	vector< shared_ptr<External::CAlarmMessage> > otherMessages;
	string otherMessagesInfo, newMessageInfo;

	otherMessages = infoalarmMessage.GetOtherMessages();

	for ( auto message : otherMessages ) {
		newMessageInfo.clear();

		// infoalarms are not included
		if ( typeid( CEmailMessage ) == typeid( *message ) ) {
			newMessageInfo = CreateEmailMessageInfo( dynamic_cast<const CEmailMessage&>( *message ) );
		} else if ( typeid( CGroupalarmMessage ) == typeid( *message ) ) {
			newMessageInfo = CreateGroupalarmMessageInfo( dynamic_cast<const CGroupalarmMessage&>( *message ) );
		} else if ( typeid( CExternalProgramMessage ) == typeid( *message ) ) {
			newMessageInfo = CreateExternalProgramMessageInfo( dynamic_cast<const CExternalProgramMessage&>( *message ) );
		}

		if ( !newMessageInfo.empty() ) {
			otherMessagesInfo += to_string( counter ) + u8". " + newMessageInfo + u8"\r\n";
			counter++;
		}
	}

	if ( otherMessagesInfo.empty() ) {
		otherMessagesInfo = "Keine";
	}

	return otherMessagesInfo;
}


/**	@brief		Provides a string containing information about an alarm e-mail message
*	@param		message							E-mail alarm message
*	@return										String containing the summary information of the e-mail message
*	@exception									None
*	@remarks									None
*/
std::string External::Email::CEmailGateway::CEmailGatewayImpl::CreateEmailMessageInfo( const External::Email::CEmailMessage& message )
{
	using namespace std;

	bool isWithoutAlarmAudio;
	string siteID, alarmID, alarmText, infoMessage;
	vector< pair<string, string> > recipients;

	message.Get( siteID, alarmID, recipients, alarmText, isWithoutAlarmAudio );
	infoMessage = u8"Email an " + siteID + ", " + alarmID + ": \"" + alarmText + "\"";
	if ( isWithoutAlarmAudio ) {
		infoMessage += u8", ohne Alarmdurchsage";
	} else {
		infoMessage += u8", mit Alarmdurchsage";
	}

	return infoMessage;
}


/**	@brief		Provides a string containing information about a groupalarm message
*	@param		message							Groupalarm message
*	@return										String containing the summary information of the groupalarm message
*	@exception									None
*	@remarks									None
*/
std::string External::Email::CEmailGateway::CEmailGatewayImpl::CreateGroupalarmMessageInfo( const External::Groupalarm::CGroupalarmMessage& message )
{
	using namespace std;

	unsigned int listCounter = 0;
	unsigned int groupCounter = 0;
	bool isUsingListOrGroups, isFreeText;
	string infoMessage, messageText, alarmPhoneNum;
	vector<int> alarmLists, alarmGroups;

	isUsingListOrGroups = message.GetAlarmData( alarmPhoneNum, alarmLists, alarmGroups );
	isFreeText = message.GetAlarmMessage( messageText );

	infoMessage = u8"Groupalarm-Nachricht an ";
	if ( isUsingListOrGroups ) {
		if ( !alarmLists.empty() ) {
			if ( alarmLists.size() == 1 ) {
				infoMessage += "Liste: ";
			} else {
				infoMessage += "Listen: ";
			}
			for ( auto permanentListID : alarmLists ) {
				if ( listCounter > 0 ) {
					infoMessage += ", ";
				}
				infoMessage += to_string( permanentListID );
				listCounter++;
			}
			infoMessage += ", ";

		}
		if ( !alarmGroups.empty() ) {
			if ( alarmGroups.size() == 1 ) {
				infoMessage += "Gruppe: ";
			} else {
				infoMessage += "Gruppen: ";
			}
			for ( auto permanentGroupID : alarmGroups ) {
				if ( groupCounter > 0 ) {
					infoMessage += ", ";
				}
				infoMessage += to_string( permanentGroupID );
				groupCounter++;
			}
			infoMessage += ", ";
		}
	} else {
		infoMessage += "Telefonnummer: " + alarmPhoneNum + ", ";
	}

	if ( isFreeText ) {
		infoMessage += "Alarmtext: ";
	} else {
		infoMessage += "Alarmtext (Groupalarm-Code): \"";
	}
	infoMessage += messageText;
	if ( !isFreeText ) {
		infoMessage += "\"";
	}

	return infoMessage;
}


/**	@brief		Provides a string containing information about an external program call
*	@param		message							External program call message
*	@return										String containing the summary information of the external program call
*	@exception									None
*	@remarks									None
*/
std::string External::Email::CEmailGateway::CEmailGatewayImpl::CreateExternalProgramMessageInfo( const External::ExternalProgram::CExternalProgramMessage& message )
{
	using namespace std;

	string infoMessage, command, programArguments;

	command = message.GetCommand();
	programArguments = message.GetProgramArguments();

	infoMessage += "Externes Programm: \"" + command + "\"";
	if ( !programArguments.empty() ) {
		infoMessage += ", Argumente: \"" + programArguments + "\"";
	}

	return infoMessage;
}


// instantiate the static class members
std::unique_ptr<External::Email::SSLInitializer> External::Email::CEmailGateway::CEmailGatewayImpl::sslInitializer = std::make_unique<SSLInitializer>();
std::once_flag External::Email::CEmailGateway::CEmailGatewayImpl::rootCertificatesLoadOnceFlag;
