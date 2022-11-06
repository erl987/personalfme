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
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include <stdexcept>
#include <boost/date_time/posix_time/ptime.hpp>
#include "BoostStdTimeConverter.h"
#include "DateTime.h"

auto ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;


/**	@brief		Standard constructor generating an invalid time
*/
Utilities::CDateTime::CDateTime()
	: isValid( false )
{
}


/**	@brief		Destructor
*/
Utilities::CDateTime::~CDateTime()
{
}


/**	@brief		Constructor
*	@param		day						Day
*	@param		month					Month
*	@param		year					Year
*	@param		timeOfDay				Time of day. If omitted, the object is only used for the date (i.e. the time of day is midnight).
*	@exception	std::out_of_range		Thrown if the date input is invalid
*	@remarks							None
*/
Utilities::CDateTime::CDateTime( const int& day, const int& month, const int& year, const Utilities::CTime& timeOfDay )
	: isValid( false )
{
	Set( day, month, year, timeOfDay );
}


/**	@brief		Resetting the time
*	@param		day						Day
*	@param		month					Month
*	@param		year					Year
*	@param		timeOfDay				Time of day. If omitted, the object is only used for the date (i.e. the time of day is midnight).
*	@return								None
*	@exception	std::out_of_range		Thrown if the date input is invalid
*	@remarks							None
*/
void Utilities::CDateTime::Set( const int& day, const int& month, const int& year, const Utilities::CTime& timeOfDay )
{
	using namespace boost::gregorian;

	if ( !timeOfDay.IsValid() ) {
		throw std::out_of_range( "The time of day is invalid." );
	}

	date( year, greg_month( month ), day );	// throws std::out_of_range in case of invalid dates

	CDateTime::dateTime = std::make_tuple( day, month, year, timeOfDay );
	CDateTime::isValid = true;
}


/**	@brief		Getting the time
*	@param		day						Day
*	@param		month					Month
*	@param		year					Year
*	@param		timeOfDay				Time of day. It is invalid if the object is only used for a date.
*	@return								None
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
void Utilities::CDateTime::Get( int& day, int& month, int& year, Utilities::CTime& timeOfDay ) const
{
	using namespace std;

	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	day = get<0>( dateTime );
	month = get<1>( dateTime );
	year = get<2>( dateTime );
	timeOfDay = get<3>( dateTime );
}


/**	@brief		Getting the day
*	@return								Day
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CDateTime::Day(void) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<0>( dateTime );
}


/**	@brief		Getting the month
*	@return								Month
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CDateTime::Month(void) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<1>( dateTime );
}


/**	@brief		Getting the year
*	@return								Year
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CDateTime::Year(void) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<2>( dateTime );
}


/**	@brief		Getting the time of day
*	@return								Time of day
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
Utilities::CTime Utilities::CDateTime::TimeOfDay(void) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<3>( dateTime );
}


/**	@brief		Returns if the stored time is valid
*	@return								True if the time is valid, false otherwise
*	@exception							None
*	@remarks							None
*/
bool Utilities::CDateTime::IsValid(void) const
{
	return isValid;
}


/**	@brief		Invalidates the current time object
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Utilities::CDateTime::Invalidate(void)
{
	isValid = false;
	dateTime = std::tuple<int, int, int, Utilities::CTime>();
}


namespace Utilities {
	/**	@brief		Equality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of equality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator==(const CDateTime& lhs, const CDateTime& rhs)
	{
		if ( lhs.IsValid() && rhs.IsValid() ) {
			return ( ToBoostTime( lhs ) == ToBoostTime( rhs ) );
		} else {
			if ( ( !lhs.IsValid() ) && ( !rhs.IsValid() ) ) {
				return true;
			} else {
				return false;
			}
		}
	}


	/**	@brief		Inequality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of ienquality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator!=(const CDateTime& lhs, const CDateTime& rhs)
	{
		return !( lhs == rhs );
	}


	/**	@brief		Smaller operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the left operand is smaller, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator<( const CDateTime& lhs, const CDateTime& rhs )
	{
		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			return ( ToBoostTime( lhs ) < ToBoostTime( rhs ) );
		}
	}


	/**	@brief		Greater operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the right operand is smaller, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator>( const CDateTime& lhs, const CDateTime& rhs )
	{
		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			return ( ToBoostTime( lhs ) > ToBoostTime( rhs ) );
		}
	}


	/**	@brief		Not-greater operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the left operand is not greater, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator<=( const CDateTime& lhs, const CDateTime& rhs )
	{
		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			return ( ToBoostTime( lhs ) <= ToBoostTime( rhs ) );
		}
	}


	/**	@brief		Not-smaller operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the right operand is not smaller, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator>=( const CDateTime& lhs, const CDateTime& rhs )
	{
		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			return ( ToBoostTime( lhs ) >= ToBoostTime( rhs ) );
		}
	}
}