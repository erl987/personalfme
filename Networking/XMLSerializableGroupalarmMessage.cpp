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

#include <boost/algorithm/string/trim.hpp>
#include "XMLSerializableGroupalarmMessage.h"

const std::string TYPE_ATTRIB_KEY = "[@type]";
const std::string ALARM_KEY = "alarm";
const std::string CODE_KEY = "code";
const std::string LIST_KEY = "list";
const std::string GROUP_KEY = "group";
const std::string PHONE_NUM_KEY = "phoneNumber";
const std::string MESSAGE_TYPE = "message";
const std::string CODE_ATTRIB_KEY = "code";
const std::string FREE_TEXT_ATTRIB_KEY = "freeText";
const std::string FEEDBACK_RESET_TYPE = "feedbackReset";
const std::string NORMAL_SMS_TYPE = "normalSMS";
const std::string SUPPRESS_FAX_TYPE = "suppressFax";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Groupalarm::CXMLSerializableGroupalarmMessage::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	unsigned int currCode;
	bool isFreeText, isLoadFeedback, isFlashSMS, isNoFax;
	vector<int> alarmLists, alarmGroups;
	vector<string> tags;
	string alarmPhoneNum, messageText;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	if ( xmlFile->hasProperty( ALARM_KEY + "." + CODE_KEY ) ) {
		xmlFile->keys( ALARM_KEY + "." + CODE_KEY, tags );
		for ( auto tag : tags ) {
			currCode = static_cast<unsigned int>( xmlFile->getInt( ALARM_KEY + "." + CODE_KEY + "." + tag ) );
			if ( tag.find( LIST_KEY ) == 0 ) {
				alarmLists.push_back( currCode );
			} else {
				alarmGroups.push_back( currCode );
			}
		}
	} else {
		alarmPhoneNum = boost::algorithm::trim_copy( xmlFile->getString( ALARM_KEY + "." + PHONE_NUM_KEY ) );
	}

	messageText = boost::algorithm::trim_copy( xmlFile->getString( MESSAGE_TYPE ) );
	if ( xmlFile->getString( MESSAGE_TYPE + TYPE_ATTRIB_KEY ) == FREE_TEXT_ATTRIB_KEY ) {
		isFreeText = true;
	} else {
		isFreeText = false;
	}

	isLoadFeedback = xmlFile->getBool( FEEDBACK_RESET_TYPE );
	isFlashSMS = !xmlFile->getBool( NORMAL_SMS_TYPE );
	isNoFax = xmlFile->getBool( SUPPRESS_FAX_TYPE );

	if ( alarmPhoneNum.empty() ) {
		Set( alarmLists, alarmGroups, isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
	} else {
		Set( alarmPhoneNum, isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Groupalarm::CXMLSerializableGroupalarmMessage::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	bool isCode, isFreeText, isLoadFeedback, isFlashSMS, isNoFax;
	vector<int> alarmLists, alarmGroups;
	string alarmPhoneNum, messageText;
 
	if ( !IsEmpty() ) {
		isFreeText = GetAlarmMessage( messageText );
		GetSettings( isLoadFeedback, isFlashSMS, isNoFax );
		isCode = GetAlarmData( alarmPhoneNum, alarmLists, alarmGroups );

		// write the data to the XML-file
		if ( isCode ) {
			auto listCounter = 0;
			for ( auto list : alarmLists ) {
				xmlFile->setInt( ALARM_KEY + "." + CODE_KEY + "." + LIST_KEY + "[" + to_string( listCounter ) + "]", list );
				listCounter++;
			}

			auto groupCounter = 0;
			for ( auto group : alarmGroups ) {
				xmlFile->setInt( ALARM_KEY + "." + CODE_KEY + "." + GROUP_KEY + "[" + to_string( groupCounter ) + "]", group );
				groupCounter++;
			}
		} else {
			xmlFile->setString( ALARM_KEY + "." + PHONE_NUM_KEY, alarmPhoneNum );
		}

		xmlFile->setString( MESSAGE_TYPE, messageText );
		if ( isFreeText ) {
			xmlFile->setString( MESSAGE_TYPE + TYPE_ATTRIB_KEY, FREE_TEXT_ATTRIB_KEY );
		} else {
			xmlFile->setString( MESSAGE_TYPE + TYPE_ATTRIB_KEY, CODE_ATTRIB_KEY );
		}

		xmlFile->setBool( FEEDBACK_RESET_TYPE, isLoadFeedback );
		xmlFile->setBool( NORMAL_SMS_TYPE, !isFlashSMS );
		xmlFile->setBool( SUPPRESS_FAX_TYPE, isNoFax );
	}
}