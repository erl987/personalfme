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
#include "CTime.h"


/**	@brief		Standard constructor generating an invalid time
*/
Utilities::CTime::CTime()
	: isValid( false )
{
}


/**	@brief		Destructor
*/
Utilities::CTime::~CTime()
{
}


/**	@brief		Constructor
*	@param		hour					Hour
*	@param		minute					Minute
*	@param		second					Second
*	@param		millisec				Millisecond. If omitted, 0 ms are assumed.
*	@exception	std::out_of_range		Thrown if the time input is invalid
*	@remarks							None
*/
Utilities::CTime::CTime( const int& hour, const int& minute, const int& second, const int& millisec )
	: isValid( false )
{
	Set( hour, minute, second, millisec );
}


/**	@brief		Resetting the time
*	@param		hour					Hour
*	@param		minute					Minute
*	@param		second					Second
*	@param		millisec				Millisecond. If omitted, 0 ms are assumed.
*	@return								None
*	@exception	std::out_of_range		Thrown if the time input is invalid
*	@remarks							None
*/
void Utilities::CTime::Set( const int& hour, const int& minute, const int& second, const int& millisec )
{
	if ( ( hour < 0 ) || ( hour >= 24 ) ) {
		throw std::out_of_range( "The hour is not in the range [0-24)." );
	}
	if ( ( minute < 0 ) || ( minute >= 60 ) ) {
		throw std::out_of_range( "The minute is not in the range [0-60)." );
	}
	if ( ( second < 0 ) || ( second >= 60 ) ) {
		throw std::out_of_range( "The second is not in the range [0-60)." );
	}
	if ( ( millisec < 0 ) || ( millisec >= 1000 ) ) {
		throw std::out_of_range( "The millisecond is not in the range [0-1000)." );
	}

	// stores the time
	CTime::time = std::make_tuple( hour, minute, second, millisec );
	CTime::isValid = true;
}


/**	@brief		Getting the time
*	@param		hour					Hour
*	@param		minute					Minute
*	@param		second					Second
*	@param		millisec				Millisecond
*	@return								None
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
void Utilities::CTime::Get( int& hour, int& minute, int& second, int& millisec ) const
{
	using namespace std;

	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	hour = get<0>( time );
	minute = get<1>( time );
	second = get<2>( time );
	millisec = get<3>( time );
}


/**	@brief		Getting the hour
*	@return								Hour
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CTime::Hour() const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<0>( time );
}


/**	@brief		Getting the minute
*	@return								Minute
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CTime::Minute() const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<1>( time );
}


/**	@brief		Getting the second
*	@return								Second
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CTime::Second(void) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<2>( time );
}


/**	@brief		Getting the millisecond
*	@return								Millisecond
*	@exception	std::runtime_error		Thrown if the time is invalid
*	@remarks							None
*/
int Utilities::CTime::Millisec() const
{
	if ( !isValid ) {
		throw std::runtime_error( "The time is invalid" );
	}

	return std::get<3>( time );
}


/**	@brief		Returns if the stored time is valid
*	@return								True if the time is valid, false otherwise
*	@exception							None
*	@remarks							None
*/
bool Utilities::CTime::IsValid(void) const
{
	return isValid;
}


/**	@brief		Invalidates the current time object
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Utilities::CTime::Invalidate(void)
{
	isValid = false;
	time = std::tuple<int, int, int, int>();
}


namespace Utilities {
	/**	@brief		Obtains the resolution of the Boost Date Time library
	*	@return								Ticks for the resolution (nanoseconds or milliseconds)
	*	@exception							None
	*	@remarks							None
	*/
	int GetTicks() {
		int ticks;

		if ( boost::posix_time::time_duration::num_fractional_digits() == 9 ) { // nanosecond resolution of  Boost Date Time library
			ticks = 1'000'000;
		} else { // microsecond resolution of  Boost Date Time library
			ticks = 1'000;
		}

		return ticks;
	}


	/**	@brief		Equality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of equality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator==(const CTime& lhs, const CTime& rhs)
	{
		using namespace boost::posix_time;

		time_duration lhsTime, rhsTime;

		if ( lhs.IsValid() && rhs.IsValid() ) {
			lhsTime = time_duration( lhs.Hour(), lhs.Minute(), lhs.Second(), lhs.Millisec() * GetTicks() );
			rhsTime = time_duration( rhs.Hour(), rhs.Minute(), rhs.Second(), rhs.Millisec() * GetTicks() );

			return ( lhsTime == rhsTime );
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
	bool operator!=(const CTime& lhs, const CTime& rhs)
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
	bool operator<( const CTime& lhs, const CTime& rhs )
	{
		using namespace boost::posix_time;

		time_duration lhsTime, rhsTime;

		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			lhsTime = time_duration( lhs.Hour(), lhs.Minute(), lhs.Second(), lhs.Millisec() * GetTicks() );
			rhsTime = time_duration( rhs.Hour(), rhs.Minute(), rhs.Second(), rhs.Millisec() * GetTicks() );

			return ( lhsTime < rhsTime );
		}
	}


	/**	@brief		Larger operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the left operand is larger, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator>( const CTime& lhs, const CTime& rhs )
	{
		using namespace boost::posix_time;

		time_duration lhsTime, rhsTime;

		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			lhsTime = time_duration( lhs.Hour(), lhs.Minute(), lhs.Second(), lhs.Millisec() * GetTicks() );
			rhsTime = time_duration( rhs.Hour(), rhs.Minute(), rhs.Second(), rhs.Millisec() * GetTicks() );

			return ( lhsTime > rhsTime );
		}
	}


	/**	@brief		Not-larger operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the left operand is not greater, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator<=( const CTime& lhs, const CTime& rhs )
	{
		using namespace boost::posix_time;

		time_duration lhsTime, rhsTime;

		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			lhsTime = time_duration( lhs.Hour(), lhs.Minute(), lhs.Second(), lhs.Millisec() * GetTicks() );
			rhsTime = time_duration( rhs.Hour(), rhs.Minute(), rhs.Second(), rhs.Millisec() * GetTicks() );

			return ( lhsTime <= rhsTime );
		}
	}


	/**	@brief		Not-smaller operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case the right operand is not smaller, false otherwise. If one is invalid, always false is returned.
	*	@exception							None
	*	@remarks							None
	*/
	bool operator>=( const CTime& lhs, const CTime& rhs )
	{
		using namespace boost::posix_time;

		time_duration lhsTime, rhsTime;

		if ( !lhs.IsValid() || ( !rhs.IsValid() ) ) {
			return false;
		} else {
			lhsTime = time_duration( lhs.Hour(), lhs.Minute(), lhs.Second(), lhs.Millisec() * GetTicks() );
			rhsTime = time_duration( rhs.Hour(), rhs.Minute(), rhs.Second(), rhs.Millisec() * GetTicks() );

			return ( lhsTime >= rhsTime );
		}
	}
}