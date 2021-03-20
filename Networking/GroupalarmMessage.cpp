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

#include <vector>
#include <algorithm>
#include "GroupalarmGateway.h"
#include "GroupalarmMessage.h"


/** @brief		Default constructor
*	@remarks									None
*/
External::Groupalarm::CGroupalarmMessage::CGroupalarmMessage()
	: CAlarmMessage( true ), // Groupalarm.de alarms are only allowed directly after detecting an alarm
	  isEmpty(true)
{
}


/**	@brief		Constructor configuring www.Groupalarm.de to send the alarm to a single phone number. More details are found in: http://prowiki.groupalarm.de/ga/handbuch_alt#ausloesung_ueber_weblink
*	@param		alarmPhoneNum					Phone number (international / national phone number (allowed formats "+492 343 3434-345", "(03434) 3434 234-342" or "23434 34234-34"))
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@exception									See CGroupalarmMessage::Set()		
*	@remarks									Use CGroupalarmMessage::CGroupalarmMessage() to generate an empty dataset
*/
External::Groupalarm::CGroupalarmMessage::CGroupalarmMessage( const std::string& alarmPhoneNum, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax )
	: CAlarmMessage( true ), // Groupalarm.de alarms are only allowed directly after detecting an alarm
	  isEmpty(true)
{
	Set( alarmPhoneNum, isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
}


/**	@brief		Constructor configuring www.Groupalarm.de to send the alarm to preconfigured lists / groups. More details are found in: http://prowiki.groupalarm.de/ga/handbuch_alt#ausloesung_ueber_weblink
*	@param		alarmLists						Groupalarm permanent list codes (LID) of the alarm (example: {1234, 2345} )
*	@param		alarmGroups						Groupalarm permanent group codes (GID) of the alarm (example: {1234, 2345} )
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@exception									See CGroupalarmMessage::Set()
*	@remarks									Use CGroupalarmMessage::CGroupalarmMessage(void) to generate an empty dataset
*/
External::Groupalarm::CGroupalarmMessage::CGroupalarmMessage( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax )
	: CAlarmMessage( true ), // Groupalarm.de alarms are only allowed directly after detecting an alarm
	  isEmpty( true )
{
	Set( alarmLists, alarmGroups, isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
}


/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::CAlarmMessage > External::Groupalarm::CGroupalarmMessage::Clone() const
{
	return std::make_unique<CGroupalarmMessage>( *this );
}


/**	@brief		Configuring www.Groupalarm.de to send the alarm to a single phone number. More details are found in: http://prowiki.groupalarm.de/ga/handbuch_alt#ausloesung_ueber_weblink
*	@param		alarmPhoneNum					Phone number (international / national phone number (allowed formats "+492 343 3434-345", "(03434) 3434 234-342" or "23434 34234-34"))
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@return										None
*	@exception	GroupalarmPhoneNumberIncorrect	Thrown if the phone number format is not correct
*	@exception	GroupalarmTextCodeIncorrect		Thrown if the Groupalarm text code is not in correct format
*	@exception	GroupalarmFreeTextLength		Thrown if the free text is not of correct format
*	@remarks									Use CGroupalarmMessage::SetEmpty to generate an empty dataset
*/
void External::Groupalarm::CGroupalarmMessage::Set( const std::string& alarmPhoneNum, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax )
{
	using namespace std;

	// check if the dataset is valid - it will throw an exception if the dataset is not valid
	CheckPhoneNumberFormat( alarmPhoneNum );
	CheckMessageFormat( isFreeText, messageText );

	Set( FormatPhoneNumber( alarmPhoneNum ), vector<int>(), vector<int>(), isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
}


/**	@brief		Configuring www.Groupalarm.de to send the alarm to preconfigured lists / groups. More details are found in: http://prowiki.groupalarm.de/ga/handbuch_alt#ausloesung_ueber_weblink
*	@param		alarmLists						Groupalarm permanent list codes (LID) of the alarm (example: {1234, 2345} )
*	@param		alarmGroups						Groupalarm permanent group codes (GID) of the alarm (example: {1234, 2345} )
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@return										None
*	@exception	GroupalarmTextCodeIncorrect		Thrown if the Groupalarm text code is not in correct format
*	@exception	GroupalarmFreeTextLength		Thrown if the Free text is not of correct format
*	@exception	GroupalarmListCodeIncorrect		Thrown if the list code string is in wrong format (containing negative numbers)
*	@remarks									Use CGroupalarmMessage::SetEmpty to generate an empty dataset
*/
void External::Groupalarm::CGroupalarmMessage::Set( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax )
{
	// check if the dataset is valid - it will throw an exception if the dataset is not valid
	CheckListGroupFormat( alarmLists, alarmGroups );	
	CheckMessageFormat( isFreeText, messageText );
	
	Set( std::string(), alarmLists, alarmGroups, isFreeText, messageText, isLoadFeedback, isFlashSMS, isNoFax );
}


/**	@brief		Internal central setting method
*	@param		alarmPhoneNum					Phone number (international / national phone number (allowed formats "+492 343 3434-345", "(03434) 3434 234-342" or "23434 34234-34"))
*	@param		alarmLists						Groupalarm permanent list codes (LID) of the alarm (example: {1234, 2345} )
*	@param		alarmGroups						Groupalarm permanent group codes (GID) of the alarm (example: {1234, 2345} )
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::Groupalarm::CGroupalarmMessage::Set( const std::string& alarmPhoneNum, const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax )
{
	using namespace std;
	vector<int> uniqueAlarmLists, uniqueAlarmGroups;

	// ensure that unique sets of lists / groups are stored
	unique_copy( begin( alarmLists ), end( alarmLists ), back_inserter( uniqueAlarmLists ) );
	unique_copy( begin( alarmGroups ), end( alarmGroups ), back_inserter( uniqueAlarmGroups ) );

	// simplify if code "0" (full alarm) is present (any other codes are then irrelevant)
	if ( ( find( begin( alarmLists ), end( alarmLists ), 0 ) != end( alarmLists ) ) || ( find( begin( alarmGroups ), end( alarmGroups ), 0 ) != end( alarmGroups ) ) ) {
		uniqueAlarmGroups.clear();
		uniqueAlarmLists = { 0 };
	}

	CGroupalarmMessage::alarmPhoneNum = alarmPhoneNum;
	CGroupalarmMessage::alarmLists = uniqueAlarmLists;
	CGroupalarmMessage::alarmGroups = uniqueAlarmGroups;
	CGroupalarmMessage::isFreeText = isFreeText;
	CGroupalarmMessage::messageText = messageText;
	CGroupalarmMessage::isLoadFeedback = isLoadFeedback;
	CGroupalarmMessage::isFlashSMS = isFlashSMS;
	CGroupalarmMessage::isNoFax = isNoFax;

	CGroupalarmMessage::isEmpty = false;
}


/**	@brief		Get the stored alarm data
*	@param		alarmPhoneNum					Phone number (international / national phone number (allowed formats "+492 343 3434-345", "(03434) 3434 234-342" or "23434 34234-34"))
*	@param		alarmLists						Groupalarm permanent list codes (LID) of the alarm (example: {1234, 2345} )
*	@param		alarmGroups						Groupalarm permanent group codes (GID) of the alarm (example: {1234, 2345} )
*	@return										True if the alarm represents Groupalarm list codes / groups and false if it represents a single phone number
*	@exception	std::domain_error				Thrown if the dataset is empty		
*	@remarks									If the method returns true 'alarmPhoneNum' is invalid, otherwise 'alarmLists' and 'alarmGroups' are invalid
*/
bool External::Groupalarm::CGroupalarmMessage::GetAlarmData( std::string& alarmPhoneNum, std::vector<int>& alarmLists, std::vector<int>& alarmGroups ) const
{
	if ( IsEmpty() ) {
		throw std::domain_error( "The groupalarm.de dataset is empty." );
	}

	alarmPhoneNum = CGroupalarmMessage::alarmPhoneNum;
	alarmLists = CGroupalarmMessage::alarmLists;
	alarmGroups = CGroupalarmMessage::alarmGroups;

	return alarmPhoneNum.empty(); // it is guaranteed that the state of the string reflects the setting codes / phone number
}


/**	@brief		Get an alarm string compatible to the Groupalarm.de weblink
*	@param		alarmString						Alarm string compatible to the Groupalarm.de weblink
*	@return										True if the alarm represents Groupalarm list codes / groups and false if it represents a single phone number
*	@exception	std::domain_error				Thrown if the dataset is empty
*	@remarks									None
*/
bool External::Groupalarm::CGroupalarmMessage::GetAlarmString( std::string& alarmString ) const
{
	bool isCode;

	if ( IsEmpty() ) {
		throw std::domain_error( "The groupalarm.de dataset is empty." );
	}

	isCode = alarmPhoneNum.empty();
	if ( isCode ) {
		if ( ( alarmLists.size() == 1 ) && ( alarmLists.front() == 0 ) ) {
			// full alarm
			alarmString = "0";
		} else {
			alarmString.clear();
			for ( auto list : alarmLists ) {
				alarmString += "-" + std::to_string( list );
			}

			for ( auto group : alarmGroups ) {
				alarmString += "+" + std::to_string( group );
			}
		}
	}  else {
		alarmString = alarmPhoneNum;
	}

	return isCode;
}


/**	@brief		Get the stored alarm message				
*	@param		messageText						Message (free text (example: "Dies ist ein Test!") or permanent Groupalarm x-text code (example: "5"))
*	@return										Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@exception	std::domain_error				Thrown if the dataset is empty
*	@remarks									None
*/
bool External::Groupalarm::CGroupalarmMessage::GetAlarmMessage( std::string& messageText ) const
{
	if ( IsEmpty() ) {
		throw std::domain_error( "The groupalarm.de dataset is empty." );
	}

	messageText = CGroupalarmMessage::messageText;

	return CGroupalarmMessage::isFreeText;
}


/**	@brief		Get the stored settings of the alarm
*	@param		isLoadFeedback					Flag stating if Groupalarm feedback should be loaded (true) or not (false). This is overwriting the global settings of Groupalarm.
*	@param		isFlashSMS						Flag stating if SMS should be sent as flash SMS (true) or normal SMS (false)
*	@param		isNoFax							Flag stating if no result fax should be sent (true). This is overwriting the global settings of Groupalarm.
*	@return										None
*	@exception	std::domain_error				Thrown if the dataset is empty
*	@remarks									None
*/
void External::Groupalarm::CGroupalarmMessage::GetSettings( bool& isLoadFeedback, bool& isFlashSMS, bool& isNoFax ) const
{
	if ( IsEmpty() ) {
		throw std::domain_error( "The groupalarm.de dataset is empty." );
	}

	isLoadFeedback = CGroupalarmMessage::isLoadFeedback;
	isFlashSMS = CGroupalarmMessage::isFlashSMS;
	isNoFax = CGroupalarmMessage::isNoFax;
}


/**	@brief		Checking if the message length is not too long for SMS-sending
*	@param		text							Message text to be checked
*	@return										True if the message length is ok and false if it is too long
*	@exception									None
*	@remarks									None
*/
bool External::Groupalarm::CGroupalarmMessage::CheckSMSMessageLength( std::string text )
{
	using namespace std;

	if ( text.length() > maxSMSLength ) {
		return false;
	} else {
		return true;
	}
}


/**	@brief		Checking if the phone number format is syntactically correct
*	@param		alarmPhoneNum					Phone number
*	@return										None
*	@exception	GroupalarmPhoneNumberIncorrect	Thrown if the phone number format is not correct
*	@remarks									The method returns without further notice if the format is correct
*/
void External::Groupalarm::CGroupalarmMessage::CheckPhoneNumberFormat( const std::string& alarmPhoneNum )
{
	if ( !( std::regex_match( alarmPhoneNum, regExPhoneNumber ) ) ) {
		throw Exception::GroupalarmPhoneNumberIncorrect( "Phone number is not in correct format!" );
	}
}


/**	@brief		Checking if the message format is syntactically correct
*	@param		isFreeText						Flag stating if message is given as free text (true) or Groupalarm text code (false)
*	@param		messageText						Message text
*	@exception	GroupalarmTextCodeIncorrect		Thrown if the Groupalarm text code is not in correct format
*	@exception	GroupalarmFreeTextLength		Thrown if the Free text is not of correct format
*	@remarks									The method returns without further notice if the format is correct
*/
void External::Groupalarm::CGroupalarmMessage::CheckMessageFormat( const bool& isFreeText, const std::string& messageText )
{
	if ( !isFreeText ) { // Groupalarm text code
		if ( !( std::regex_match( messageText, regExGroupalarmTextCode ) ) ) {
			throw Exception::GroupalarmTextCodeIncorrect( "Groupalarm text code is not in correct format!" );
		}
	} else { // free text
		if ( !CheckSMSMessageLength( messageText ) ) {
			throw Exception::GroupalarmFreeTextLength( "Free text is more than 160 characters!" );
		}
	}
}


/**	@brief		Checking if the lists and groups are syntactically correct
*	@param		alarmLists						Groupalarm permanent list codes (LID) of the alarm (example: {1234, 2345} )
*	@param		alarmGroups						Groupalarm permanent group codes (GID) of the alarm (example: {1234, 2345} )
*	@exception	GroupalarmListCodeIncorrect		Thrown if the list code string is in wrong format (containing negative numbers or empty)
*	@remarks									The method returns without further notice if the format is correct
*/
void External::Groupalarm::CGroupalarmMessage::CheckListGroupFormat( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups )
{
	using namespace std;

	if ( alarmLists.empty() && alarmGroups.empty() ) {
		throw Exception::GroupalarmListCodeIncorrect( "Groupalarm list / group code is empty!" );
	}

	// check if all IDs are valid unsigned integers
	if ( find_if( begin( alarmLists ), end( alarmLists ), []( auto val ) { return ( val < 0 ); } ) != end( alarmLists ) ) {
		throw Exception::GroupalarmListCodeIncorrect( "Groupalarm list code must not be negative!" );
	}

	if ( find_if( begin( alarmGroups ), end( alarmGroups ), []( auto val ) { return ( val < 0 ); } ) != end( alarmGroups ) ) {
		throw Exception::GroupalarmListCodeIncorrect( "Groupalarm list code must not be negative!" );
	}
}


/**	@brief		Determines if this object and onother CGroupalarm dataset are equivalent
*	@param		rhs								Dataset to be compared
*	@return										True if the datasets are equal, false otherwise.
*	@exception									None		
*	@remarks									Comparing with any other object than CGroupalarmMessage will return false
*/
bool External::Groupalarm::CGroupalarmMessage::IsEqual( const CAlarmMessage& rhs ) const
{
	try {
		const CGroupalarmMessage& derivRhs = dynamic_cast< const CGroupalarmMessage& >( rhs );
		
		if ( ( IsEmpty() && !derivRhs.IsEmpty() ) || ( !IsEmpty() && derivRhs.IsEmpty() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( alarmPhoneNum != derivRhs.alarmPhoneNum ) {
				return false;
			}
			if ( alarmLists != derivRhs.alarmLists ) {
				return false;
			}
			if ( alarmGroups != derivRhs.alarmGroups ) {
				return false;
			}
			if ( isFreeText != derivRhs.isFreeText ) {
				return false;
			}
			if ( messageText != derivRhs.messageText ) {
				return false;
			}
			if ( isLoadFeedback != derivRhs.isLoadFeedback ) {
				return false;
			}
			if ( isFlashSMS != derivRhs.isFlashSMS ) {
				return false;
			}
			if ( isNoFax != derivRhs.isNoFax ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}


/**	@brief		Determines if this object is smaller than another CGroupalarm object
*	@param		rhs								Dataset to be compared
*	@return										True if this dataset is smaller, false otherwise
*	@exception									None		
*	@remarks									Comparing with any other object than CGroupalarmMessage will return false
*/
bool External::Groupalarm::CGroupalarmMessage::IsSmaller( const CAlarmMessage& rhs ) const
{
	try {
		const CGroupalarmMessage& derivRhs = dynamic_cast< const CGroupalarmMessage& >( rhs );

		// check for is-smaller for all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( alarmPhoneNum >= derivRhs.alarmPhoneNum ) {
				return false;
			}
			if ( alarmLists >= derivRhs.alarmLists ) {
				return false;
			}
			if ( alarmGroups >= derivRhs.alarmGroups ) {
				return false;
			}
			if ( isFreeText >= derivRhs.isFreeText ) {
				return false;
			}
			if ( messageText >= derivRhs.messageText ) {
				return false;
			}
			if ( isLoadFeedback >= derivRhs.isLoadFeedback ) {
				return false;
			}
			if ( isFlashSMS >= derivRhs.isFlashSMS ) {
				return false;
			}
			if ( isNoFax >= derivRhs.isNoFax ) {
				return false;
			}
		} else {
			return false;
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}


/**	@brief		Checking if the dataset is empty
*	@return										True if the dataset is empty, if it contains valid data it is false
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the alarm weblink call
*/
bool External::Groupalarm::CGroupalarmMessage::IsEmpty(void) const
{
	return isEmpty;
}


/**	@brief		Setting the dataset empty
*	@return										None
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the alarm weblink call
*/
void External::Groupalarm::CGroupalarmMessage::SetEmpty(void)
{
	isEmpty = true;
}


/**	@brief		Formats a phone number according to the standardized format
*	@param		phoneNumberString				String containing the phone number to be formated
*	@return										Formated phone number according to the format "+493454545", "0934234234" or "3434234"	
*	@exception	GroupalarmPhoneNumberIncorrect	Thrown if the phone number format is not correct
*	@remarks									None								
*/
std::string External::Groupalarm::CGroupalarmMessage::FormatPhoneNumber(const std::string& phoneNumberString)
{
	using namespace std;
	string formattedPhoneNumber;

	if ( !( regex_match( phoneNumberString, regExPhoneNumber ) ) ) {
		throw Exception::GroupalarmPhoneNumberIncorrect( "Phone number is not in correct format!" );
	}
	
	// replace the phone number by the standard format "+493454545", "0934234234" or "3434234"
	formattedPhoneNumber = regex_replace( phoneNumberString, regExReplacePhoneNumber, string( "" ) ); // remove the characters

	return formattedPhoneNumber;
}


/**	@brief		Obtains the type index of the gateway class corresponding to the present message class
*	@return										Type index of the gateway class corresponding to the present message class
*	@exception									None		
*	@remarks									None
*/
std::type_index External::Groupalarm::CGroupalarmMessage::GetGatewayType() const
{
	return std::type_index( typeid( CGroupalarmGateway ) );
}


/**	@brief		Returns a text with a length valid for sending via SMS, truncates the input if necessary
*	@param		text							SMS-message text to be validated
*	@return										SMS-message text with valid length (160 characters)
*	@exception									None
*	@remarks									None
*/
std::string External::Groupalarm::CGroupalarmMessage::GetSMSMessageWithValidLength( const std::string& text )
{
	std::string uncheckedText = text;
	std::string textWithValidLength;

	if ( !CheckSMSMessageLength( uncheckedText ) ) {
		uncheckedText.resize( maxSMSLength );
	}
	textWithValidLength = uncheckedText;

	return textWithValidLength;
}
