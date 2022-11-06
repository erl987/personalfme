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

#include "DefaultValidity.h"
#include "EmailGateway.h"
#include "GroupalarmGateway.h"
#include "ExternalProgramGateway.h"
#include "InfoalarmMessageDecorator.h"
#include "XMLSerializableWeeklyValidity.h"
#include "XMLSerializableMonthlyValidity.h"
#include "XMLSerializableSingleTimeValidity.h"
#include "XMLSerializableEmailMessage.h"
#include "XMLSerializableGroupalarmMessage.h"
#include "XMLSerializableExternalProgramMessage.h"
#include "XMLException.h"
#include "XMLSerializableAlarmValidities.h"

const std::string DEFAULT_VALIDITY_KEY = "default";
const std::string VALIDITY_KEY = "validity";
const std::string WEEKLY_VALIDITY_KEY = "weeklyException";
const std::string MONTHLY_VALIDITY_KEY = "monthlyException";
const std::string SINGLE_VALIDITY_KEY = "singleTimeException";
const std::string ALARMS_KEY = "alarms";
const std::string EMAIL_KEY = "email";
const std::string GROUPALARM_KEY = "groupalarm";
const std::string EXTERNAL_PROGRAM_KEY = "external";
const std::string INFOALARM_KEY = "infoalarm";


/** @brief		Unmarshalling the alarm message from the XML-configuration section
*	@param		messageKey					Key of the message section in the XML-file (in Poco notation)
*	@param		configurationView			Pointer to the Poco AbstractConfiguration representing the XML-configuration section
*	@param		isInfoalarmAllowed			Flag defining if infoalarms within the configuration are allowed or rejected
*	@return									The alarm message
*	@exception	Exception::xml_error		Thrown if an infoalarm is present in the configuration and this is not allowed
*	@exception								None
*	@remarks								None
*/
std::unique_ptr<External::CAlarmMessage> External::CXMLSerializableAlarmValidities::GetAlarmMessage( const std::string& messageKey, Poco::AutoPtr<Poco::Util::AbstractConfiguration> configurationView, const bool& isInfoalarmAllowed ) {
	using namespace std;
	using namespace Poco::Util;
	using namespace External::Groupalarm;
	using namespace External::Email;
	using namespace External::ExternalProgram;
	using namespace External::Infoalarm;

	vector<string> availableInfoMessage;
	CXMLSerializableEmailMessage emailMessage;
	CXMLSerializableGroupalarmMessage groupalarmMessage;
	CXMLSerializableExternalProgramMessage externalProgramMessage;
	unique_ptr<External::CAlarmMessage> alarmMessage, infoalarmMessage;
	
	if ( messageKey.find( EMAIL_KEY ) == 0 ) {
		Poco::AutoPtr<AbstractConfiguration> emailView( configurationView );
		emailMessage.SetFromXML( emailView );
		alarmMessage = make_unique<CEmailMessage>( emailMessage );

	} else if ( messageKey.find( GROUPALARM_KEY ) == 0 ) {
		Poco::AutoPtr<AbstractConfiguration> groupalarmView( configurationView );
		groupalarmMessage.SetFromXML( groupalarmView );
		alarmMessage = make_unique<CGroupalarmMessage>( groupalarmMessage );

	} else if ( messageKey.find( EXTERNAL_PROGRAM_KEY ) == 0 ) {
		Poco::AutoPtr<AbstractConfiguration> externalProgramView( configurationView );
		externalProgramMessage.SetFromXML( externalProgramView );
		alarmMessage = make_unique<CExternalProgramMessage>( externalProgramMessage );

	} else if ( messageKey.find( INFOALARM_KEY ) == 0 ) {
		if ( isInfoalarmAllowed ) {
			Poco::AutoPtr<AbstractConfiguration> infoalarmView( configurationView );
			infoalarmView->keys( "", availableInfoMessage );
			infoalarmMessage = GetAlarmMessage( availableInfoMessage.front(), infoalarmView->createView( availableInfoMessage.front() ), false );
			alarmMessage = make_unique<CInfoalarmMessageDecorator>( move( infoalarmMessage ) );
		} else {
			throw Utilities::Exception::xml_error( "error:\tInfoalarms are only allowed within the <all> tag" );
		}
	}

	return alarmMessage;
}


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::CXMLSerializableAlarmValidities::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	SetFromXML( xmlFile, true );
}


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@param		isInfoalarmAllowed			Flag defining if infoalarms within the configuration are allowed or rejected
*	@return									None
*	@exception	Exception::xml_error		Thrown if an infoalarm is present in the configuration and this is not allowed
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::CXMLSerializableAlarmValidities::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile, const bool& isInfoalarmAllowed )
{
	using namespace std;
	using namespace Poco::Util;

	vector<string> availableValidities, availableMessages;
	vector< shared_ptr<CAlarmMessage> > alarmMessages;
	Validities::CXMLSerializableWeeklyValidity weeklyValidity;
	Validities::CXMLSerializableMonthlyValidity monthlyValidity;
	Validities::CXMLSerializableSingleTimeValidity singleTimeValidity;
	shared_ptr<Validities::CValidity> validityPtr;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	xmlFile->keys( "", availableValidities );
	for ( auto validity : availableValidities ) {
		alarmMessages.clear();
		Poco::AutoPtr<AbstractConfiguration> validityView( xmlFile->createView( validity ) );

		validityView->keys( ALARMS_KEY, availableMessages );
		for ( auto message : availableMessages ) {
			alarmMessages.push_back( GetAlarmMessage( message, validityView->createView( ALARMS_KEY + "." + message ), isInfoalarmAllowed ) );
		}

		if ( validity == DEFAULT_VALIDITY_KEY ) {
			validityPtr = Validities::DEFAULT_VALIDITY;
		} else {
			Poco::AutoPtr<AbstractConfiguration> exceptionValidityView( validityView->createView( VALIDITY_KEY ) );
			if ( validity.find( WEEKLY_VALIDITY_KEY ) == 0 ) {
				weeklyValidity.SetFromXML( exceptionValidityView );
				validityPtr = make_shared<Validities::CWeeklyValidity>( weeklyValidity );

			} else if ( validity.find( MONTHLY_VALIDITY_KEY ) == 0 ) {
				monthlyValidity.SetFromXML( exceptionValidityView );
				validityPtr = make_shared<Validities::CMonthlyValidity>( monthlyValidity );

			} else if ( validity.find( SINGLE_VALIDITY_KEY ) == 0 ) {
				singleTimeValidity.SetFromXML( exceptionValidityView );
				validityPtr = make_shared<Validities::CSingleTimeValidity>( singleTimeValidity );
			}
		}
		AddEntry( validityPtr, begin( alarmMessages ), end( alarmMessages ) );
	}
}


/** @brief		Marshalling the alarm message object to the XML-configuration section
*	@param		messageKey					Key of the message section in the XML-file (in Poco notation)
*	@param		message						The alarm message
*	@param		configurationView			Pointer to the Poco AbstractConfiguration of the XML-configuration section that is modified
*	@param		messageCounters				Map containing counters to track the number of each type of message, will be updated
*	@return									None
*	@exception								None
*	@remarks								None
*/
void External::CXMLSerializableAlarmValidities::SetAlarmMessage( const std::string& messageKey, const External::CAlarmMessage& message, Poco::AutoPtr<Poco::Util::AbstractConfiguration> configurationView, std::map<std::string, unsigned int>& messageCounters ) const
{
	using namespace std;
	using namespace External;
	using namespace External::Groupalarm;
	using namespace External::Email;
	using namespace External::ExternalProgram;
	using namespace External::Infoalarm;
	using namespace Poco::Util;

	bool isInfoalarmMessage;
	string key;

	if ( !messageKey.empty() ) {
		key = messageKey + ".";
	}

	if ( typeid( message ) == typeid( CInfoalarmMessageDecorator ) ) {
		auto infoalarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>( message ).GetContainedMessage();
		Poco::AutoPtr<AbstractConfiguration> gatewayView( configurationView->createView( key + INFOALARM_KEY + "[" + to_string( messageCounters[INFOALARM_KEY] ) + "]" ) );
		std::map<std::string, unsigned int> infoalarmMessageCounters; // separate counting within the infoalarm section required
		SetAlarmMessage( "", *infoalarmMessage, gatewayView, infoalarmMessageCounters );
		messageCounters[INFOALARM_KEY]++;
		isInfoalarmMessage = true;

	} else {
		isInfoalarmMessage = false;
	}

	if ( !isInfoalarmMessage ) {
		if ( message.GetGatewayType() == typeid( CEmailGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( configurationView->createView( key + EMAIL_KEY + "[" + to_string( messageCounters[EMAIL_KEY] ) + "]" ) );
			auto emailMessage = CXMLSerializableEmailMessage( dynamic_cast<const CEmailMessage&>( message ) );
			emailMessage.GenerateXML( gatewayView );
			messageCounters[EMAIL_KEY]++;

		} else if ( message.GetGatewayType() == typeid( CGroupalarmGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( configurationView->createView( key + GROUPALARM_KEY + "[" + to_string( messageCounters[GROUPALARM_KEY] ) + "]" ) );
			auto groupalarmMessage = CXMLSerializableGroupalarmMessage( dynamic_cast<const CGroupalarmMessage&>( message ) );
			groupalarmMessage.GenerateXML( gatewayView );
			messageCounters[GROUPALARM_KEY]++;

		} else if ( message.GetGatewayType() == typeid( CExternalProgramGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( configurationView->createView( key + EXTERNAL_PROGRAM_KEY + "[" + to_string( messageCounters[EXTERNAL_PROGRAM_KEY] ) + "]" ) );
			auto externalProgramMessage = CXMLSerializableExternalProgramMessage( dynamic_cast<const CExternalProgramMessage&>( message ) );
			externalProgramMessage.GenerateXML( gatewayView );
			messageCounters[EXTERNAL_PROGRAM_KEY]++;
		}
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If there are no validities present, nothing will be written to file. If a validity does not contain alarm messages, an empty validity tag will be written to the XML-file
*/
void External::CXMLSerializableAlarmValidities::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace Poco::Util;

	Types::AlarmValiditiesType allAlarmValidities;
	Poco::AutoPtr<AbstractConfiguration> validityView;
	vector<string> setKeys;
	map<string, unsigned int> exceptionCounters, messageCounters;
	
	// write the data to the XML-file
	allAlarmValidities = GetAllEntries();
	for ( auto alarmValidity : allAlarmValidities ) {
		// handle the validity type
		if ( dynamic_pointer_cast<Validities::CDefaultValidity>( alarmValidity.first ) != nullptr ) {
			// default validity
			validityView = Poco::AutoPtr<AbstractConfiguration>( xmlFile->createView( DEFAULT_VALIDITY_KEY ) );
		} else {
			// exception validity
			if ( dynamic_pointer_cast<Validities::CWeeklyValidity>( alarmValidity.first ) != nullptr ) {
				validityView = Poco::AutoPtr<AbstractConfiguration>( xmlFile->createView( WEEKLY_VALIDITY_KEY + "[" + to_string( exceptionCounters[WEEKLY_VALIDITY_KEY] ) + "]" ) );

				Poco::AutoPtr<AbstractConfiguration> exceptionValidityView( validityView->createView( VALIDITY_KEY ) );
				auto weeklyValidity = Validities::CXMLSerializableWeeklyValidity( *dynamic_pointer_cast<Validities::CWeeklyValidity>( alarmValidity.first ) );
				weeklyValidity.GenerateXML( exceptionValidityView );
				exceptionCounters[WEEKLY_VALIDITY_KEY]++;

			} else if ( dynamic_pointer_cast<Validities::CMonthlyValidity>( alarmValidity.first ) != nullptr ) {
				validityView = Poco::AutoPtr<AbstractConfiguration>( xmlFile->createView( MONTHLY_VALIDITY_KEY + "[" + to_string( exceptionCounters[MONTHLY_VALIDITY_KEY] ) + "]" ) );

				Poco::AutoPtr<AbstractConfiguration> exceptionValidityView( validityView->createView( VALIDITY_KEY ) );
				auto monthlyValidity = Validities::CXMLSerializableMonthlyValidity( *dynamic_pointer_cast<Validities::CMonthlyValidity>( alarmValidity.first ) );
				monthlyValidity.GenerateXML( exceptionValidityView );
				exceptionCounters[MONTHLY_VALIDITY_KEY]++;

			} else {
				validityView = Poco::AutoPtr<AbstractConfiguration>( xmlFile->createView( SINGLE_VALIDITY_KEY + "[" + to_string( exceptionCounters[SINGLE_VALIDITY_KEY] ) + "]" ) );

				Poco::AutoPtr<AbstractConfiguration> exceptionValidityView( validityView->createView( VALIDITY_KEY ) );
				auto singleTimeValidity = Validities::CXMLSerializableSingleTimeValidity( *dynamic_pointer_cast<Validities::CSingleTimeValidity>( alarmValidity.first ) );
				singleTimeValidity.GenerateXML( exceptionValidityView );
				exceptionCounters[SINGLE_VALIDITY_KEY]++;
			}
		}

		// write all messages to the XML-file
		messageCounters.clear();
		for ( auto const& message : alarmValidity.second ) {
			SetAlarmMessage( ALARMS_KEY, *message, validityView, messageCounters );
		}

		// ensure that the default / alarm tag always exists
		if ( typeid( *( alarmValidity.first ) ) == typeid( Validities::CDefaultValidity ) ) {
			xmlFile->keys( "", setKeys );
			if ( setKeys.empty() ) {
				validityView->setString( "", "" );
			}
		} else {
			validityView->keys( "", setKeys );
			if ( find( begin( setKeys ), end( setKeys ), ALARMS_KEY ) == end( setKeys ) ) {
				validityView->setString( ALARMS_KEY, "" );
			}
		}
	}
}
