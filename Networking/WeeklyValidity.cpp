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

#include <algorithm>
#include <stdexcept>
#include "WeeklyValidity.h"
#include "BoostStdTimeConverter.h"
#include "german_local_date_time.h"



/**	@brief		Default constructor
*/
External::Validities::CWeeklyValidity::CWeeklyValidity(void)
	: isValid( false )
{
}



/**	@brief		Constructor
*	@param		weekInMonth						Valid week of the month. If all weeks are valid use the third constructor.
*	@param		weekDay							Valid day of week
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if an input is invalid
*	@remarks 									None
*/
External::Validities::CWeeklyValidity::CWeeklyValidity( const Utilities::Time::WeekType& weekInMonth, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	std::vector< Utilities::Time::WeekType > weeksInMonth;

	// convert the information for the week of the month
	weeksInMonth.push_back( weekInMonth );

	SetValidityDates( weeksInMonth, weekDay, beginTimeOfDay, endTimeOfDay );
}



/**	@brief		Constructor valid for any weeks of the month
*	@param		weekDay							Valid day of week
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if an input is invalid
*	@remarks 									None
*/
External::Validities::CWeeklyValidity::CWeeklyValidity( const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	using namespace Utilities::Time;
	std::vector< WeekType > weeksInMonth;

	// set all weeks of the month valid
	weeksInMonth.push_back( FIRST );
	weeksInMonth.push_back( SECOND );
	weeksInMonth.push_back( THIRD );
	weeksInMonth.push_back( FOURTH );
	weeksInMonth.push_back( LAST );

	SetValidityDates( weeksInMonth, weekDay, beginTimeOfDay, endTimeOfDay );
}



/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::Validities::CValidity > External::Validities::CWeeklyValidity::Clone() const
{
	return std::make_unique<CWeeklyValidity>( *this );
}



/**	@brief		Setting the validity dates
*	@param		weeksInMonth					Array containing all valid weeks of the month
*	@param		weekDay							Valid day of week
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@return										None
*	@exception 	std::out_of_range				Thrown if an input is invalid
*	@remarks 									None
*/
void External::Validities::CWeeklyValidity::SetValidityDates( const std::vector< Utilities::Time::WeekType >& weeksInMonth, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
{
	using namespace std;
	vector<Utilities::Time::WeekType> weeksInMonthInput;

	// check input values
	Utilities::Time::CBoostStdTimeConverter::GetBoostWeekDay( weekDay ); // throws std::out_of_range if the week day is not valid
	if ( weeksInMonth.empty() ) {
		throw std::out_of_range( "Weeks in month must not be empty." );
	}
	for (size_t i=0; i < weeksInMonth.size(); i++) {
		Utilities::Time::CBoostStdTimeConverter::GetBoostWeekNum( weeksInMonth[i] ); // throws std::out_of_range if the weeks in month are invalid
	}

	if ( !beginTimeOfDay.IsValid() || !endTimeOfDay.IsValid() ) {
		throw std::out_of_range( "The start or the end time of the weekly exception are invalid." );
	}

	// set data
	weeksInMonthInput = weeksInMonth;
	weeksInMonthInput.erase( unique( begin( weeksInMonthInput ), end( weeksInMonthInput ) ), end( weeksInMonthInput ) );

	CWeeklyValidity::validWeekDay = weekDay;
	CWeeklyValidity::validWeeksInMonth = weeksInMonthInput;
	CWeeklyValidity::beginTimeOfDay = beginTimeOfDay;
	CWeeklyValidity::endTimeOfDay = endTimeOfDay;
	CWeeklyValidity::isValid = true;
}



/**	@brief		Returns all validities STARTING in the given LOCAL month
*	@param		localMonth						Month for which the validities are calculated (local time)
*	@param		localYear						Year for which the validities are calculated (local time)
*	@return 									All validity periods starting in the given month [begin time, end time) (UTC-times, resolution: seconds)
*	@exception									None
*	@remarks 									It is assumed that the validity duration is maximum 24 hours. This helper method is based on the definition of the LOCAL month.
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CWeeklyValidity::GetValiditiesStartingInLocalMonth( const int& localMonth, const int& localYear ) const
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities;
	using namespace Utilities::Time;

	auto ToStdTime = &CBoostStdTimeConverter::ConvertToStdTime;
	auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;
	auto LocalToUTC = &CBoostStdTimeConverter::LocalToUTC;

	date startDateLocal;
	ptime currBeginTimeLocal, currEndTimeLocal, currBeginTimeUTC, currEndTimeUTC;
	vector< pair<CDateTime, CDateTime> > validityPeriods;

	// determine the occurrences of the time period in the month
	for ( size_t i = 0; i < validWeeksInMonth.size(); i++ ) {
		startDateLocal = nth_day_of_the_week_in_month( CBoostStdTimeConverter::GetBoostWeekNum( validWeeksInMonth[i] ), CBoostStdTimeConverter::GetBoostWeekDay( validWeekDay ), localMonth ).get_date( localYear );

		currBeginTimeLocal = ToBoostTime( CDateTime( startDateLocal.day(), startDateLocal.month().as_number(), startDateLocal.year(), beginTimeOfDay ) );
		currEndTimeLocal = ToBoostTime( CDateTime( startDateLocal.day(), startDateLocal.month().as_number(), startDateLocal.year(), endTimeOfDay ) );
		if ( endTimeOfDay < beginTimeOfDay ) {
			currEndTimeLocal += days( 1 );
		}

		// converting German local time to UTC-time (considering special cases during daylight saving shifts)
		currBeginTimeUTC = ToBoostTime( LocalToUTC( ToStdTime( currBeginTimeLocal ) ) );
		currEndTimeUTC = ToBoostTime( LocalToUTC( ToStdTime( currEndTimeLocal ) ) );

		if ( currEndTimeUTC > currBeginTimeUTC ) {
			validityPeriods.push_back( make_pair( ToStdTime( currBeginTimeUTC ), ToStdTime( currEndTimeUTC ) ) );
		}
	}

	return validityPeriods;
}



/**	@brief		Returns all validity periods for the given month (UTC-times)
*	@param		UTCMonth						Month for which the validities are calculated (UTC-time)
*	@param		UTCYear							Year for which the validities are calculated (UTC-time)
*	@return 									Storing all validity periods [begin time, end time) (UTC-times) intersecting with in the given month (resolution: seconds)
*	@exception	std::runtime_error				Thrown if the object was initialized before use
*	@exception	std::out_of_range				Thrown if either given month or year are invalid
*	@remarks 									Validities may overlap from the neighbouring months, this is considered. It is assumed that the exception validity is maximum 24 hours.
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CWeeklyValidity::GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities;
	auto ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;

	vector<date> consideredLocalMonths;
	date reqUTCmonth;
	vector< pair<CDateTime,CDateTime> > allValidityUTCPeriods, validityUTCPeriods, newUTCPeriods;

	// check validity of the object
	if ( !IsValid() ) {
		throw std::runtime_error( "Validity start times can only be obtained if the object was initialized before use." );
	}

	// determine the occurrences of the exception in the required month and the previous and next month (required due to validity periods that may overlap and UTC - local time differences)
	reqUTCmonth = date( UTCYear, greg_month( UTCMonth ), 1 ); // throws std::out_of_range if the month is invalid
	consideredLocalMonths = { reqUTCmonth - months( 1 ), reqUTCmonth, reqUTCmonth + months( 1 ) };
	for ( auto currLocalMonth : consideredLocalMonths ) {
		newUTCPeriods = GetValiditiesStartingInLocalMonth( currLocalMonth.month(), currLocalMonth.year() );
		allValidityUTCPeriods.insert( end( allValidityUTCPeriods ), begin( newUTCPeriods ), end( newUTCPeriods ) );
	}

	// determine all occurrences of the exception that either start or end in the required month
	time_period monthUTCPeriod( ptime( reqUTCmonth, time_duration( 0, 0, 0, 0 ) ), ptime( reqUTCmonth, time_duration( 0, 0, 0, 0 ) ) + months( 1 ) );
	for ( auto currUTCException : allValidityUTCPeriods ) {
		if ( time_period( ToBoostTime( currUTCException.first ), ToBoostTime( currUTCException.second ) ).intersects( monthUTCPeriod ) ) {
			validityUTCPeriods.push_back( currUTCException );
		}
	}

	// remove possible multiple entries (if fourth and last week are identical)
	validityUTCPeriods.erase( unique( begin( validityUTCPeriods ), end( validityUTCPeriods ) ), end( validityUTCPeriods ) );

	return validityUTCPeriods;
}



/**	@brief		Comparison of this with another object
*	@param		rhs								Object to be compared
*	@return 									True if the two object are identical, false otherwise
*	@remarks 									Only objects of type CWeeklyValidity can be compared
*/
bool External::Validities::CWeeklyValidity::CompareDerived(const Validities::CValidity& rhs) const
{
	std::vector< Utilities::Time::WeekType > rhsValidWeeksInMonth;
	Utilities::Time::DayType rhsValidWeekDay;
	Utilities::CTime rhsBeginTimeOfDay, rhsEndTimeOfDay;

	try {
		// convert to the correct derived class reference enforcing the correct class type
		const CWeeklyValidity& derivRhs = dynamic_cast< const CWeeklyValidity& >( rhs );

		// check for mixed validity
		if ( ( !derivRhs.IsValid() && IsValid() ) || ( derivRhs.IsValid() && !IsValid() ) ) {
			return false;
		}
	
		if ( derivRhs.IsValid() && IsValid() ) {
			derivRhs.GetValidityDates( std::back_inserter( rhsValidWeeksInMonth ), rhsValidWeekDay, rhsBeginTimeOfDay, rhsEndTimeOfDay );

			// compare identity of class data
			if ( validWeeksInMonth != rhsValidWeeksInMonth ) {
				return false;
			}
			if ( validWeekDay != rhsValidWeekDay ) {
				return false;
			}
			if ( beginTimeOfDay != rhsBeginTimeOfDay ) {
				return false;
			}
			if ( endTimeOfDay != rhsEndTimeOfDay ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Checking if the object is valid and can be used
*	@return 									True if the object is valid, false otherwise
*	@remarks 									None
*/
bool External::Validities::CWeeklyValidity::IsValid(void) const
{
	return isValid;
}



/**	@brief		Invalidates the object
*	@return 									None
*	@remarks 									None
*/
void External::Validities::CWeeklyValidity::Invalidate()
{
	CWeeklyValidity::validWeeksInMonth.clear();
	CWeeklyValidity::validWeekDay = Utilities::Time::MONDAY;
	CWeeklyValidity::beginTimeOfDay = Utilities::CTime();
	CWeeklyValidity::endTimeOfDay = Utilities::CTime();
	CWeeklyValidity::isValid = false;
}
