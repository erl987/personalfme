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

#include "EmailGateway.h"
#include "EmailMessage.h"


/** @brief		Default constructor
*	@remarks									The required state is set to true, but the data is still invalid until setting it completely
*/
External::Email::CEmailMessage::CEmailMessage(void)
	: CAlarmMessage( true ),
	  isEmpty(true)
{
}


/**	@brief		Constructor setting the container for storing the configuration data for an e-mail alarm
*	@param		siteID							Name of the site / organisation, it will appear in the subject of the alarm e-mail
*	@param		alarmID							Name of the current alarm, it will appear in the subject and content of the alarm e-mail
*	@param		recipients						List containing the recipient e-mail addresses
*	@param		alarmText						Specific alarm text of the current alarm, it will appear in the alarm e-mail content
*	@param		requiredState					Flag stating when the alarm set is valid (true: send out immediately after the detection, false: send out after the recording of the audio sequence)
*	@exception									None	
*	@remarks									Use CEmailMessage::CEmailMessage(void) to generate an empty dataset
*/
External::Email::CEmailMessage::CEmailMessage(const std::string& siteID, const std::string& alarmID, const std::vector< std::pair<std::string, std::string> >& recipients, const std::string& alarmText, const bool& requiredState)
	: CAlarmMessage( requiredState ),
	  isEmpty(true)
{
	Set( siteID, alarmID, recipients, alarmText );
}


/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::CAlarmMessage > External::Email::CEmailMessage::Clone() const
{
	return std::make_unique<CEmailMessage>( *this );
}


/**	@brief		Setting the container for storing the configuration data for an e-mail alarm
*	@param		siteID							Name of the site / organisation, it will appear in the subject of the alarm e-mail
*	@param		alarmID							Name of the current alarm, it will appear in the subject and content of the alarm e-mail
*	@param		recipients						List containing the recipient e-mail addresses
*	@param		alarmText						Specific alarm text of the current alarm, it will appear in the alarm e-mail content
*	@return										None
*	@exception									None
*	@remarks									The requires state can only be changed by the base class method CAlarmMessage::SetRequiredState(const bool&). If initially the empty constructor was used, the required state is true.
*/
void External::Email::CEmailMessage::Set(const std::string& siteID, const std::string& alarmID, const std::vector< std::pair<std::string, std::string> >& recipients, const std::string& alarmText)
{
	using namespace std;

	data = make_tuple( siteID, alarmID, recipients, alarmText );

	// set the empty flag to false
	isEmpty = false;
}



/**	@brief		Setting the IDs valid for the e-mail alarm
*	@param		siteID							Name of the site / organisation, it will appear in the subject of the alarm e-mail
*	@param		alarmID							Name of the current alarm, it will appear in the subject and content of the alarm e-mail
*	@return										None
*	@exception	std::domain_error				Thrown if the dataset is empty	
*	@remarks									None
*/
void External::Email::CEmailMessage::SetIDs(const std::string& siteID, const std::string& alarmID)
{
	using namespace std;

	bool requiredState;
	string dummySiteID, dummyAlarmID;
	vector< pair<string, string> > recipients;
	string alarmText;

	// replace the site and alarm IDs
	Get( dummySiteID, dummyAlarmID, recipients, alarmText, requiredState );
	Set( siteID, alarmID, recipients, alarmText );
}



/**	@brief		Getting the stored configuration data for an e-mail alarm
*	@param		siteID							Name of the site / organisation, it will appear in the subject of the alarm e-mail
*	@param		alarmID							Name of the current alarm, it will appear in the subject and content of the alarm e-mail
*	@param		recipients						List containing the recipient e-mail addresses
*	@param		alarmText						Specific alarm text of the current alarm, it will appear in the alarm e-mail content
*	@param		requiredState					Flag stating when the alarm set is valid (true: send out immediately after the detection, false: send out after the recording of the audio sequence)
*	@return										None
*	@exception	std::domain_error				Thrown if the dataset is empty		
*	@remarks									Do not use the function if the dataset is empty, otherwise an exception will be thrown. This can be checked with CEmailMessage::IsEmpty().
*/
void External::Email::CEmailMessage::Get(std::string& siteID, std::string& alarmID, std::vector< std::pair<std::string, std::string> >& recipients, std::string& alarmText, bool& requiredState) const
{
	using namespace std;

	// check if the dataset is empty
	if ( IsEmpty() ) {
		throw std::domain_error( "The dataset is empty" );
	}

	// set output data from the dataset
	siteID = get<0>( data );
	alarmID = get<1>( data );
	recipients = get<2>( data );
	alarmText = get<3>( data );

	requiredState = CAlarmMessage::RequiredState();
}



/**	@brief		Determines if this object and onother CEmailAlarm dataset are equivalent
*	@param		rhs								Dataset to be compared
*	@return										True if the datasets are equal, false otherwise.
*	@exception									None		
*	@remarks									At first it is tested if both datasets are either empty or non-empty. If this is true, the data is compared component per component. Comparing with any other object than CEmailMessage will return false.
*/
bool External::Email::CEmailMessage::IsEqual(const CAlarmMessage& rhs) const
{
	try {
		const CEmailMessage& derivRhs = dynamic_cast< const CEmailMessage& >( rhs );
		
		// checks if both datasets are either empty or non-empty
		if ( ( IsEmpty() && !derivRhs.IsEmpty() ) || ( !IsEmpty() && derivRhs.IsEmpty() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( dynamic_cast< const CEmailMessage& >( *this ).data != derivRhs.data ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Determines if this object is smaller than another CEMailMessage object
*	@param		rhs								Dataset to be compared
*	@return										True if this dataset is smaller, false otherwise
*	@exception									None		
*	@remarks									At first it is tested if both datasets are either empty or non-empty. If this is not valid, false is returned. If it is true, the data is compared component per component. Comparing with any other object than CEmailMessage will return false.
*/
bool External::Email::CEmailMessage::IsSmaller(const CAlarmMessage& rhs) const
{
	try {
		const CEmailMessage& derivRhs = dynamic_cast< const CEmailMessage& >( rhs );
		
		// checks if both datasets are either empty or non-empty
		if ( ( IsEmpty() && !derivRhs.IsEmpty() ) || ( !IsEmpty() && derivRhs.IsEmpty() ) ) {
			return false;
		}

		// check for is-smaller for all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( !( dynamic_cast< const CEmailMessage& >( *this ).data < derivRhs.data ) ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Checking if the dataset is empty.
*	@return										True if the dataset is empty, if it contains valid data it is false
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the e-mail sending
*/
bool External::Email::CEmailMessage::IsEmpty(void) const
{
	return isEmpty;
}



/**	@brief		Setting the dataset empty.
*	@return										None
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the e-mail sending
*/
void External::Email::CEmailMessage::SetEmpty(void)
{
	isEmpty = true;
}



/**	@brief		Obtains the type index of the gateway class corresponding to the present message class.
*	@return										Type index of the gateway class corresponding to the present message class
*	@exception									None		
*	@remarks									None
*/
std::type_index External::Email::CEmailMessage::GetGatewayType() const
{
	return std::type_index( typeid( CEmailGateway ) );
}