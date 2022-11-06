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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "XMLSerializableEmailLoginData.h"

const std::string SMTP_SERVER_KEY = "smtpServer";
const std::string PORT_KEY = "port";
const std::string SENDER_NAME_KEY = "senderName";
const std::string SENDER_ADDRESS_KEY = "senderMailAddress";

const std::string TYPE_ATTRIB_KEY = "[@type]";
const std::string SECURITY_KEY = "security";
	const std::string SECURITY_TLS_KEY = "TLS";
	const std::string SECURITY_TLSSTART_KEY = "TLSSTART";
	const std::string SECURITY_NONE_KEY = "none";

const std::string AUTH_KEY = "authentification";
	const std::string AUTH_SIMPLE_KEY = "simplePassword";
	const std::string AUTH_ENCRYPTED_KEY = "encryptedPassword";

const std::string TRIALS_KEY = "trials";
const std::string WAIT_TIME_KEY = "waitTime";
const std::string CONNECTIONS_KEY = "connections";

const std::string USER_KEY = "user";
const std::string PASSWORD_KEY = "password";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Email::CXMLSerializableEmailLoginData::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	string userName, password, senderAddress, senderName, smtpServerName;
	unsigned short port;
	float timeDistTrial;
	External::Email::AuthType authType;
	External::Email::ConnectionType connectionType;
	unsigned int numTrials, maxNumConnections;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	smtpServerName = boost::algorithm::trim_copy( xmlFile->getString( SMTP_SERVER_KEY ) );
	port = static_cast<unsigned short>( xmlFile->getInt( PORT_KEY ) );

	if ( xmlFile->getString( SECURITY_KEY + TYPE_ATTRIB_KEY ) == SECURITY_TLS_KEY ) {
		connectionType = External::Email::TLS_SSL_CONN;
	} else if ( xmlFile->getString( SECURITY_KEY + TYPE_ATTRIB_KEY ) == SECURITY_TLSSTART_KEY ) {
		connectionType = External::Email::TLSSTART_CONN;
	} else if ( xmlFile->getString( SECURITY_KEY + TYPE_ATTRIB_KEY ) == SECURITY_NONE_KEY ) {
		connectionType = External::Email::UNENCRYPTED_CONN;
	}

	if ( xmlFile->hasProperty( AUTH_KEY ) ) {
		if ( xmlFile->getString( AUTH_KEY + TYPE_ATTRIB_KEY ) == AUTH_SIMPLE_KEY ) {
			authType = External::Email::UNENCRYPTED_AUTH;
		} else if ( xmlFile->getString( AUTH_KEY + TYPE_ATTRIB_KEY ) == AUTH_ENCRYPTED_KEY ) {
			authType = External::Email::ENCRYPTED_AUTH;
		}
	} else {
		authType = External::Email::NO_AUTH;
	}

	if ( authType != External::Email::NO_AUTH ) {
		userName = boost::algorithm::trim_copy( xmlFile->getString( AUTH_KEY + "." + USER_KEY ) );
		password = boost::algorithm::trim_copy( xmlFile->getString( AUTH_KEY + "." + PASSWORD_KEY ) );
	} else {
		userName = "";
		password = "";
	}

	senderName = boost::algorithm::trim_copy( xmlFile->getString( SENDER_NAME_KEY, "" ) ); // default: empty
	senderAddress = boost::algorithm::trim_copy( xmlFile->getString( SENDER_ADDRESS_KEY ) );
	if ( !senderName.empty() ) {
		senderAddress = senderName + " <" + senderAddress + ">";
	}

	numTrials = xmlFile->getUInt( TRIALS_KEY, 10 ); // default value: 10 trials
	timeDistTrial = static_cast<float>( xmlFile->getDouble( WAIT_TIME_KEY, 60.0 ) ); // default value: 60s
	maxNumConnections = xmlFile->getUInt( CONNECTIONS_KEY, 1 ); // default value: 1 parallel connection

	if ( authType != External::Email::NO_AUTH ) {
		SetLoginInformation( senderAddress, authType, userName, password );
	} else {
		SetLoginInformation( senderAddress, authType );
	}
	SetServerInformation( connectionType, smtpServerName, port );
	SetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the data is empty, nothing will be written to the XML-file
*/
void External::Email::CXMLSerializableEmailLoginData::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace boost::algorithm;
	using tokenizer = boost::tokenizer<boost::char_separator<char> >;

	string userName, password, senderAddress, senderName, smtpServerName;
	unsigned short port;
	float timeDistTrial;
	External::Email::AuthType authType;
	External::Email::ConnectionType connectionType;
	unsigned int numTrials, maxNumConnections;
	vector<string> separated;
	boost::char_separator<char> separators( "<>" );

	if ( IsValid() ) {
		GetLoginInformation( senderAddress, authType, userName, password );
		GetServerInformation( connectionType, smtpServerName, port );
		GetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );

		// write the data to the XML-file
		xmlFile->setString( SMTP_SERVER_KEY, smtpServerName );
		xmlFile->setInt( PORT_KEY, port );

		if ( connectionType == External::Email::TLS_SSL_CONN ) {
			xmlFile->setString( SECURITY_KEY + TYPE_ATTRIB_KEY, SECURITY_TLS_KEY );
		} else if ( connectionType == External::Email::TLSSTART_CONN ) {
			xmlFile->setString( SECURITY_KEY + TYPE_ATTRIB_KEY, SECURITY_TLSSTART_KEY );
		} else if ( connectionType == External::Email::UNENCRYPTED_CONN ) {
			xmlFile->setString( SECURITY_KEY + TYPE_ATTRIB_KEY, SECURITY_NONE_KEY );
		}

		if ( authType != External::Email::NO_AUTH ) {
			if ( authType == External::Email::UNENCRYPTED_AUTH ) {
				xmlFile->setString( AUTH_KEY + TYPE_ATTRIB_KEY, AUTH_SIMPLE_KEY );
			} else if ( authType == External::Email::ENCRYPTED_AUTH ) {
				xmlFile->setString( AUTH_KEY + TYPE_ATTRIB_KEY, AUTH_ENCRYPTED_KEY );
			}

			xmlFile->setString( AUTH_KEY + "." + USER_KEY, userName );
			xmlFile->setString( AUTH_KEY + "." + PASSWORD_KEY, password );
		}

		// split the sender address into name and address
		tokenizer tokens( senderAddress, separators );
		separated.assign( begin( tokens ), end( tokens ) );
		if ( separated.size() > 1 ) {
			senderName = separated[0];
			senderAddress = separated[1];
		} else if ( !separated.empty() ) {
			senderName = "";
			senderAddress = separated.front();
		}
		trim( senderName );
		trim( senderAddress );

		if ( !senderName.empty() ) {
			xmlFile->setString( SENDER_NAME_KEY, senderName );
		}
		xmlFile->setString( SENDER_ADDRESS_KEY, senderAddress );

		xmlFile->setUInt( TRIALS_KEY, numTrials );
		xmlFile->setDouble( WAIT_TIME_KEY, timeDistTrial );
		xmlFile->setUInt( CONNECTIONS_KEY, maxNumConnections );
	}
}
