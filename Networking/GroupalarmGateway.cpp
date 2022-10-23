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

#include <set>
#include <sstream>
#include <boost/locale.hpp>
#include <boost/algorithm/string.hpp>
#include "StringUtils.h"
#include "SimpleHTTPClient.h"
#include "GroupalarmMessage.h"
#include "ExternalProgramMessage.h"
#include "GroupalarmLoginData.h"
#include "GroupalarmGateway.h"



/** @brief		Standard constructor.
*	@remarks								None
*/
External::Groupalarm::CGroupalarmGateway::CGroupalarmGateway(void)
{
}


/**	@brief		Clone method
*	@return										Copy of the present instance
*	@exception									None
*	@remarks									None
*/
std::unique_ptr<External::CAlarmGateway> External::Groupalarm::CGroupalarmGateway::Clone() const
{
	return std::make_unique<CGroupalarmGateway>();
}


/** @brief		Performs the sending of the alarm via the gateway
*	@param		code						Container with the alarm code
*	@param		alarmTime					Time of the alarm (UTC)
*	@param		isRealAlarm					Flag stating if the current alarm is a real (default) or test alarm
*	@param		loginData					Login data for the Groupalarm.de server
*	@param		message						Data for the alarm message
*	@param		audioFile					Audio file of the alarm message. It will be ignored by this method.
*	@return									None
*	@exception	std::domain_error			Thrown if the internet connection was missing (or if the host is unknown)
*	@exception	std::runtime_error			Thrown if the SMS-sending failed or the Groupalarm database has not been loaded before
*	@remarks								None
*/
void External::Groupalarm::CGroupalarmGateway::Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile )
{
	using namespace std;
	using namespace Utilities::String;
	using namespace Infoalarm;
	using Networking::HTTP::CSimpleHTTPClient;

	unsigned short proxyServerPort;
	bool isCode, isFreeText, isLoadFeedback, isFlash, isNoFax, isHashedPassword, isInfoalarm;
	string alarmString, request, header, information, messageText, userName, password, proxyServerName;
	boost::locale::generator gen;
	stringstream codeStream;
	CGroupalarmMessage groupalarmMessage;
	CInfoalarmMessageDecorator infoalarmMessage;
	unique_ptr<CAlarmMessage> alarmMessage;

	// obtain the required information (it is guaranteed by the caller that all alarm datasets are not empty)
	if ( typeid( *message ) == typeid( CInfoalarmMessageDecorator ) ) {
		infoalarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>( *message );
		alarmMessage = infoalarmMessage.GetContainedMessage();
		groupalarmMessage = dynamic_cast<const CGroupalarmMessage&>( *alarmMessage );
		isInfoalarm = true;
	} else {
		groupalarmMessage = dynamic_cast<const CGroupalarmMessage&>( *message );
		isInfoalarm = false;
	}

	isFreeText = groupalarmMessage.GetAlarmMessage( messageText );
	groupalarmMessage.GetSettings( isLoadFeedback, isFlash, isNoFax );
	isCode = groupalarmMessage.GetAlarmString( alarmString );

	if ( isFreeText && isInfoalarm ) {
		// information on the info alarm is only added if the text type is "free text"
		for ( auto digit : code ) {
			codeStream << digit;
		}
		messageText += ". Infoalarm Schleife: " + codeStream.str() + ", " + CreateOtherMessagesInfo( infoalarmMessage );	
	}
	messageText = CGroupalarmMessage::GetSMSMessageWithValidLength( messageText );
	
	dynamic_cast< CGroupalarmLoginData* >( loginData.get() )->GetServerInformation( userName, isHashedPassword, password, proxyServerName, proxyServerPort );

	// generate the URL for the Groupalarm.de triggering
	request = documentName;
	request += "?log_user=" + CSimpleHTTPClient::URLEncoded( userName );
	if ( isHashedPassword ) {
		request += "&log_epass=" + CSimpleHTTPClient::URLEncoded( password );
	} else {
		request += "&log_pass=" + CSimpleHTTPClient::URLEncoded( password );
	}
	if ( isCode ) {
		request += "&xlistcode=" + CSimpleHTTPClient::URLEncoded( alarmString );
	} else {
		request += "&nummer=" + CSimpleHTTPClient::URLEncoded( alarmString );
	}
	if ( isFlash ) {
		request += "&flash=1";
	}
	if ( isLoadFeedback ) {
		request += "&fb=1";
	}
	if ( isNoFax ) {
		request += "&nofax=1";
	}
	if ( !isFreeText ) {
		request += "&xtext=" + CSimpleHTTPClient::URLEncoded( messageText );
	} else {
		request += "&free=" + CSimpleHTTPClient::URLEncoded( messageText );	
	}
	request += "&oem=" + CSimpleHTTPClient::URLEncoded( softwareVersionCode );

	// trigger SMS-sending at Groupalarm.de
	Networking::HTTP::CSimpleHTTPClient client( serverName, request, proxyServerName, proxyServerPort );
	client.Run(); // throws an exception in case of failing (std::domain_error in case of missing internet connection)
	client.GetData( header, information );

	// check for correct execution at Groupalarm.de
	boost::replace_all( information, "<br />", " " ); // replace the HTML break-line tags by whitespaces
	boost::trim( information );
	if ( information.find( successString ) != 0 ) {
		boost::erase_all( information, failString );
		throw runtime_error( "SMS sending failed: " + GetUTF8FromWestern( information ) ); // the error message is encoded in UTF-8, Groupalarm.de delivers the status information in German using Western encoding
	}
}


/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::Groupalarm::CGroupalarmGateway::IsEqual( const CAlarmGateway& rhs ) const
{
	try {
		dynamic_cast< const CGroupalarmGateway& >( rhs );
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}


/**	@brief		Provides a string containing a summary of the alarms corresponding to the infoalarm
*	@param		infoalarmMessage				Infoalarm message
*	@return										Summary string containing the alarms corresponding to the infoalarm
*	@exception									None
*	@remarks									None
*/
std::string External::Groupalarm::CGroupalarmGateway::CreateOtherMessagesInfo( const Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage )
{
	using namespace std;
	using namespace Email;
	using namespace Groupalarm;
	using namespace ExternalProgram;

	unsigned int emailCounter = 0;
	unsigned int typeCounter = 0;
	set<string> otherMessageTypes;
	string receiverIDs;
	vector< shared_ptr<External::CAlarmMessage> > otherMessages;
	string otherMessagesInfo;

	otherMessages = infoalarmMessage.GetOtherMessages();

	for ( auto message : otherMessages ) {
		// infoalarms are not included
		if ( typeid( *message ) == typeid( CEmailMessage ) ) {
			otherMessageTypes.insert( "E-Mail" );
			if ( emailCounter > 0 ) {
				receiverIDs += " / ";
			}
			receiverIDs += GetAlarmReceiverID( dynamic_cast<const Email::CEmailMessage&>( *message ) );
			emailCounter++;
		} else if ( typeid( *message ) == typeid( CGroupalarmMessage ) ) {
			otherMessageTypes.insert( "Groupalarm" );
		} else if ( typeid( *message ) == typeid( CExternalProgramMessage ) ) {
			otherMessageTypes.insert( "Externes Programm" );
		}
	}

	// add information on the receiver ids (only available for e-mail messages)
	if ( !receiverIDs.empty() ) {
		otherMessagesInfo += "für: " + receiverIDs;
	}

	// add information on all message types that were used
	if ( !otherMessageTypes.empty() ) {
		if ( !receiverIDs.empty() ) {
			otherMessagesInfo += ". ";
		}
		otherMessagesInfo += "Ausgelöst: ";
		for ( auto type : otherMessageTypes ) {
			if ( typeCounter > 0 ) {
				otherMessagesInfo += ", ";
			}
			otherMessagesInfo += type;
			typeCounter++;
		}
	}

	return otherMessagesInfo;
}


/**	@brief		Provides a string containing the receiver ID of an alarm e-mail message
*	@param		message							E-mail alarm message
*	@return										String containing the receiver ID of the e-mail message
*	@exception									None
*	@remarks									None
*/
std::string External::Groupalarm::CGroupalarmGateway::GetAlarmReceiverID( const External::Email::CEmailMessage& message )
{
	using namespace std;

	bool isWithAlarmMessage;
	string siteID, alarmID, alarmText, alarmReceiverID;
	vector< pair<string, string> > recipients;

	message.Get( siteID, alarmID, recipients, alarmText, isWithAlarmMessage );
	alarmReceiverID = siteID + ", " + alarmID;

	return alarmReceiverID;
}
