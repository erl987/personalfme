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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <algorithm>
#include <stdexcept>
#include "InfoalarmMessageDecorator.h"


/** @brief		Default constructor
*	@remarks									The required state is set to true, but the data is still invalid until setting it completely
*/
External::Infoalarm::CInfoalarmMessageDecorator::CInfoalarmMessageDecorator()
	: CAlarmMessage( true ),
	  isEmpty( true )
{
}


/** @brief		Constructor
*	@param		containedMessage				Alarm message to be decorated by the infoalarm object
*	@exception	std::runtime_error				If the alarm message pointer is empty
*	@remarks									The "other messages" corresponding to the sent out alarm need to be set separately
*/
External::Infoalarm::CInfoalarmMessageDecorator::CInfoalarmMessageDecorator( std::unique_ptr<External::CAlarmMessage> containedMessage )
	: CAlarmMessage( true ), // is reset later
	  isEmpty( true )
{
	if ( containedMessage ) {
		SetRequiredState( containedMessage->RequiredState() );
	}
	SetContainedMessage( std::move( containedMessage ) );
}


/** @brief		Copy constructor
*	@param		src								Infoalarm source object
*	@exception									None
*	@remarks									Complete deep copy including the "other messages" corresponding to the sent out alarm
*/
External::Infoalarm::CInfoalarmMessageDecorator::CInfoalarmMessageDecorator( const External::Infoalarm::CInfoalarmMessageDecorator& src )
	: CAlarmMessage( src.RequiredState() ),
	  isEmpty( true )
{
	if ( !src.IsEmpty() ) {
		SetContainedMessage( src.GetContainedMessage() );
		SetOtherMessages( src.GetOtherMessages() );
	} else {
		SetEmpty();
	}
}


/** @brief		Assignment operator
*	@param		rhs								Infoalarm object on the right side
*	@return										Reference to this object
*	@exception									None
*	@remarks									Complete deep copy including the "other messages" corresponding to the sent out alarm
*/
External::Infoalarm::CInfoalarmMessageDecorator& External::Infoalarm::CInfoalarmMessageDecorator::operator=( const External::Infoalarm::CInfoalarmMessageDecorator& rhs )
{
	if ( this == &rhs ) {
		return *this;
	}

	if ( !rhs.IsEmpty() ) {
		SetRequiredState( rhs.RequiredState() );
		SetContainedMessage( rhs.GetContainedMessage() );
		SetOtherMessages( rhs.GetOtherMessages() );
	} else {
		SetEmpty();
	}

	return *this;
}


/** @brief		(Re-) sets the alarm message that is decorated by the infoalarm object
*	@param		containedMessage				Alarm message to be decorated by the infoalarm object
*	@return										None
*	@exception	std::runtime_error				If the alarm message pointer is empty
*	@remarks									None
*/
void External::Infoalarm::CInfoalarmMessageDecorator::SetContainedMessage( std::unique_ptr<External::CAlarmMessage> containedMessage )
{
	if ( !containedMessage ) {
		throw std::runtime_error( "The contained message pointer must not be empty" );
	}

	this->containedMessage = move( containedMessage );
	isEmpty = false;
}


/** @brief		Obtains a copy of the alarm message that is decorated by the infoalarm object
*	@return										Copy of the alarm message
*	@exception	std::domain_error				Thrown if the object is empty
*	@remarks									None
*/
std::unique_ptr<External::CAlarmMessage> External::Infoalarm::CInfoalarmMessageDecorator::GetContainedMessage() const
{
	// check if the dataset is empty
	if ( IsEmpty() ) {
		throw std::domain_error( "There is no contained message present in the object" );
	}

	return containedMessage->Clone();
}


/**	@brief		Checking if the infoalarm object is empty
*	@return										True if the object is empty, if it contains valid data it is false
*	@exception									None
*	@remarks									None
*/
bool External::Infoalarm::CInfoalarmMessageDecorator::IsEmpty() const
{
	return isEmpty;
}


/**	@brief		Setting the infoalarm object as empty
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::Infoalarm::CInfoalarmMessageDecorator::SetEmpty()
{
	isEmpty = true;
	containedMessage = nullptr;
	otherMessages.clear();
	SetRequiredState( true );
}


/**	@brief		Cloning method duplicating the object
*	@return										Duplicate base class pointer of the object
*	@exception									None
*	@remarks									None
*/
std::unique_ptr<External::CAlarmMessage> External::Infoalarm::CInfoalarmMessageDecorator::Clone() const
{
	using namespace std;
	unique_ptr<External::CAlarmMessage> clone;

	if ( !IsEmpty() ) {
		clone = make_unique<CInfoalarmMessageDecorator>( containedMessage->Clone() );
		dynamic_cast<CInfoalarmMessageDecorator&>( *clone ).SetOtherMessages( this->GetOtherMessages() );
	} else {
		clone = make_unique<CInfoalarmMessageDecorator>();
	}

	return clone;
}


/**	@brief		Obtains the type index of the gateway class corresponding to the present message class.
*	@return										Type index of the gateway class corresponding to the present message class
*	@exception	std::domain_error				Thrown if the object is empty
*	@remarks									None
*/
std::type_index External::Infoalarm::CInfoalarmMessageDecorator::GetGatewayType() const
{
	// check if the dataset is empty
	if ( IsEmpty() ) {
		throw std::domain_error( "There is no contained message present in the object" );
	}

	return containedMessage->GetGatewayType();
}


/**	@brief		Sets the "other messages" corresponding to the sent out alarm
*	@param		otherMessages					"Other messages" corresponding to the sent out alarm
*	@return										None
*	@exception	std::domain_error				Thrown if the object is empty
*	@remarks									A deep copy of each message is stored, i.e. the original container can still safely be used
*/
void External::Infoalarm::CInfoalarmMessageDecorator::SetOtherMessages( const std::vector< std::shared_ptr<CAlarmMessage> >& otherMessages )
{
	// check if the dataset is empty
	if ( IsEmpty() ) {
		throw std::domain_error( "Other messages can only be set if the infoalarm object is in a valid state" );
	}

	this->otherMessages.clear();
	this->otherMessages.reserve( otherMessages.size() );
	for ( auto const& message : otherMessages ) {
		this->otherMessages.push_back( message->Clone() );
	}
}


/**	@brief		Obtains the "other messages" corresponding to the sent out alarm
*	@return										"Other messages" corresponding to the sent out alarm
*	@exception	std::domain_error				Thrown if the object is empty
*	@remarks									The returned objects are owned by the caller
*/
std::vector< std::shared_ptr<External::CAlarmMessage> > External::Infoalarm::CInfoalarmMessageDecorator::GetOtherMessages() const
{
	std::vector< std::shared_ptr<External::CAlarmMessage> > clonedOtherMessages;

	// check if the dataset is empty
	if ( IsEmpty() ) {
		throw std::domain_error( "The infoalarm object is in an invalid state" );
	}

	clonedOtherMessages.reserve( otherMessages.size() );
	for ( auto const& message : otherMessages ) {
		clonedOtherMessages.push_back( message->Clone() );
	}

	return clonedOtherMessages;
}


/**	@brief		Determines if this object and onother CInfoalarmMessageDecorator dataset are equivalent
*	@param		rhs								Object to be compared
*	@return										True if the objects are equal, false otherwise.
*	@exception									None
*	@remarks									At first it is tested if both objects are either empty or non-empty. If this is true, the data is compared component per component. Comparing with any other object than CInfoalarmMessageDeocator will return false.
*/
bool External::Infoalarm::CInfoalarmMessageDecorator::IsEqual( const CAlarmMessage & rhs ) const
{
	using namespace std;
	bool areOtherMessagesEqual;

	if ( typeid( rhs ) == typeid( CInfoalarmMessageDecorator ) ) {
		auto derivRhs = dynamic_cast< const CInfoalarmMessageDecorator& >( rhs );

		// checks if both objects are either empty or non-empty
		if ( ( IsEmpty() && !derivRhs.IsEmpty() ) || ( !IsEmpty() && derivRhs.IsEmpty() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( *containedMessage != *derivRhs.GetContainedMessage() ) {
				return false;
			}

			auto otherMessagesRhs = derivRhs.GetOtherMessages();
			areOtherMessagesEqual = equal( begin( otherMessages ), end( otherMessages ), begin( otherMessagesRhs ), end( otherMessagesRhs ),
				[]( auto const& val1, auto const& val2 ) { return ( *val1 == *val2 ); } );
			if ( !areOtherMessagesEqual ) {
				return false;
			}
		}
	} else {
		return false;
	}

	return true;
}


/**	@brief		Determines if this object is lexicographically smaller than another CInfoalarmMessageDecorator object
*	@param		rhs								Object to be compared
*	@return										True if this object is lexicographically smaller, false otherwise
*	@exception									None
*	@remarks									The objects are compared lexicographically, element by element. The definition of "smaller" is inspired by that of std::vector.
*/
bool External::Infoalarm::CInfoalarmMessageDecorator::IsSmaller( const CAlarmMessage & rhs ) const
{
	using namespace std;

	if ( typeid( rhs ) != typeid( CInfoalarmMessageDecorator ) ) {
		// compared to an object of another type, this object is smaller
		return true;
	}

	if ( IsEmpty() && !rhs.IsEmpty() ) {
		// an empty object is lexicographically less than any non-empty object
		return true;
	} else if ( !IsEmpty() && rhs.IsEmpty() ) {
		// an empty object is lexicographically less than any non-empty object
		return false;
	} else if ( IsEmpty() && rhs.IsEmpty() ) {
		// two empty objects are lexicographically equal
		return false;
	}

	// both objects are non-empty: the first mismatching element defines which object is lexicographically less or greater than the other
	auto derivRhs = dynamic_cast<const CInfoalarmMessageDecorator&>( rhs );
	if ( *containedMessage != *derivRhs.GetContainedMessage() ) {
		return *containedMessage < *derivRhs.GetContainedMessage();
	}

	auto otherMessagesRhs = derivRhs.GetOtherMessages();
	auto areOtherMessagesEqual = equal( begin( otherMessages ), end( otherMessages ), begin( otherMessagesRhs ), end( otherMessagesRhs ),
		[]( auto const& val1, auto const& val2 ) { return ( *val1 == *val2 ); } );

	if ( !areOtherMessagesEqual ) {
		auto otherMessagesRhs = derivRhs.GetOtherMessages();
		auto otherMessagesCompare = lexicographical_compare( begin( otherMessages ), end( otherMessages ), begin( otherMessagesRhs ), end( otherMessagesRhs ),
			[]( auto const& val1, auto const& val2 ) { return ( *val1 < *val2 ); } );
		return otherMessagesCompare;
	}

	// if two objects have equivalent elements, then the objects are lexicographically equal
	return false;
}
