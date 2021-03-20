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

#include <boost/iterator/counting_iterator.hpp>
#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"
#include "MonthlyValidity.h"



/**	@brief		Default constructor
*/
External::Validities::CMonthlyValidity::CMonthlyValidity(void)
	: isValid( false )
{
}



/**	@brief		Constructor
*	@param		day								Valid day of the month
*	@param		month							Valid month. If all months are valid use the appropriate constructor.
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if a the given day of the year is not valid (i.e. either day or months)
*	@remarks 									None
*/
External::Validities::CMonthlyValidity::CMonthlyValidity( const int& day, const int& month, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	std::vector<int> months;

	// convert the information on the valid month
	months.push_back( month );

	SetValidityDays( day, months, beginTimeOfDay, endTimeOfDay );
}



/**	@brief		Constructor valid for each month
*	@param		day								Valid day of the month
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if the input is invalid
*	@remarks 									None
*/
External::Validities::CMonthlyValidity::CMonthlyValidity(const int& day, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	std::vector<int> months;

	// set validity for any month
	std::copy( boost::counting_iterator<int>( 1 ), boost::counting_iterator<int>( 13 ), std::back_inserter( months ) ); // validity from January to December

	SetValidityDays( day, months, beginTimeOfDay, endTimeOfDay );
}


/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::Validities::CValidity > External::Validities::CMonthlyValidity::Clone() const
{
	return std::make_unique<CMonthlyValidity>( *this );
}



/**	@brief		Setting the validity dates
*	@param		day								Valid day of the month
*	@param		months							Array of all valid months
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@return										None
*	@exception 	std::out_of_range				Thrown if the input is invalid
*	@remarks 									None
*/
void External::Validities::CMonthlyValidity::SetValidityDays( const int& day, const std::vector<int>& months, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
{
	using namespace Utilities::Time;

	std::vector<int> monthsInput;

	monthsInput = months;
	monthsInput.erase( std::unique( begin( monthsInput ), end( monthsInput ) ), end( monthsInput ) );

	// checking if the date exists at least in one year
	if ( !CBoostStdTimeConverter::DayOfYearCanExist( day, monthsInput ) ) {
		throw std::out_of_range( "The day of the year can never exist." );
	}

	if ( !beginTimeOfDay.IsValid() || !endTimeOfDay.IsValid() ) {
		throw std::out_of_range( "The start or the end time of the monthly exception are invalid." );
	}

	// set data
	CMonthlyValidity::validDay = day;
	CMonthlyValidity::validMonths.assign( begin( monthsInput ), end( monthsInput ) );
	CMonthlyValidity::beginTimeOfDay = beginTimeOfDay;
	CMonthlyValidity::endTimeOfDay = endTimeOfDay;
	CMonthlyValidity::isValid = true;
}



/**	@brief		Returns all validities STARTING in the given LOCAL month
*	@param		localMonth						Month for which the validities are calculated (local time)
*	@param		localYear						Year for which the validities are calculated (local time)
*	@return 									All validity UTC-periods [start time, end time) beginning in the given LOCAL month
*	@exception									None
*	@remarks 									It is assumed that the validity duration is maximum 24 hours. This helper method is based on the definition of the LOCAL month.
*												If the valid day does not exist for the month, the exception is skipped.
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CMonthlyValidity::GetValiditiesStartingInLocalMonth( const int& localMonth, const int& localYear ) const
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities;
	using namespace Utilities::Time;

	auto ToStdTime = &CBoostStdTimeConverter::ConvertToStdTime;
	auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;
	auto LocalToUTC = &CBoostStdTimeConverter::LocalToUTC;

	bool isBeginDayExists;
	ptime beginTimeUTC, endTimeUTC, beginTimeLocal, endTimeLocal;
	date nextLocalMonth;
	vector< pair<Utilities::CDateTime, Utilities::CDateTime> > validityPeriods;

	// determine the occurrences of the time period in the required (local) month
	for ( auto currLocalMonth : validMonths ) {
		if ( currLocalMonth == localMonth ) {
			try {
				date( localYear, greg_month( currLocalMonth ), validDay );
				isBeginDayExists = true;
			} catch ( std::out_of_range e ) {
				isBeginDayExists = false;
			}
			
			// if the day does not exist in the month, the exception is completely skipped
			if ( isBeginDayExists ) {
				beginTimeLocal = ToBoostTime( CDateTime( validDay, currLocalMonth, localYear, beginTimeOfDay ) );
				endTimeLocal = ToBoostTime( CDateTime( validDay, currLocalMonth, localYear, endTimeOfDay ) );
				if ( endTimeOfDay < beginTimeOfDay ) {
					endTimeLocal += days( 1 );
				}

				// converting German local time to UTC-time (considering special cases during daylight saving shifts)
				beginTimeUTC = ToBoostTime( LocalToUTC( ToStdTime( beginTimeLocal ) ) );
				endTimeUTC = ToBoostTime( LocalToUTC( ToStdTime( endTimeLocal ) ) );

				if ( endTimeUTC > beginTimeUTC ) {
					validityPeriods.push_back( make_pair( ToStdTime( beginTimeUTC ), ToStdTime( endTimeUTC ) ) );
				}
			}
		}
	}

	return validityPeriods;
}



/**	@brief		Returns all validity periods for the given month (defined in UTC-time)
*	@param		UTCMonth						Month for which the validities are calculated (UTC-time)
*	@param		UTCYear							Year for which the validities are calculated (UTC-time)
*	@return 									All validity periods [begin time, end time) (UTC-times) intersecting with in the given UTC-month (resolution: seconds)
*	@exception	std::runtime_error				Thrown if the object was initialized before use
*	@exception	std::out_of_range				Thrown if either given month or year are invalid
*	@remarks 									Validities may overlap from the neighbouring months, this is considered. It is assumed that the exception validity is maximum 24 hours.
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CMonthlyValidity::GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities;
	auto ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;

	vector<date> consideredLocalMonths;
	date reqUTCMonth;
	vector< pair<CDateTime, CDateTime> > allValidityPeriods, validityPeriods, newPeriods;

	// check validity of the object
	if ( !IsValid() ) {
		throw std::runtime_error( "Validity start times can only be obtained if the object was initialized before use." );
	}

	// determine the occurrences of the exception in the required month and the previous and next month (required due to validity periods that may overlap and UTC - local time differences)
	reqUTCMonth = date( UTCYear, greg_month( UTCMonth ), 1 ); // may throw std::out_of_range
	consideredLocalMonths = { reqUTCMonth - months( 1 ), reqUTCMonth, reqUTCMonth + months( 1 ) };
	for ( auto currLocalMonth : consideredLocalMonths ) {
		newPeriods = GetValiditiesStartingInLocalMonth( currLocalMonth.month(), currLocalMonth.year() );
		allValidityPeriods.insert( end( allValidityPeriods ), begin( newPeriods ), end( newPeriods ) ); // UTC-times
	}

	// determine all occurrences of the exception that either start or end in the required UTC month
	time_period UTCmonthPeriod( ptime( reqUTCMonth, time_duration( 0, 0, 0, 0 ) ), ptime( reqUTCMonth, time_duration( 0, 0, 0, 0 ) ) + months( 1 ) );
	for ( auto currUTCexception : allValidityPeriods ) {
		if ( time_period( ToBoostTime( currUTCexception.first ), ToBoostTime( currUTCexception.second ) ).intersects( UTCmonthPeriod ) ) {
			validityPeriods.push_back( currUTCexception );
		}
	}

	return validityPeriods;
}



/**	@brief		Comparison of this with another object
*	@param		rhs								Object to be compared
*	@return 									True if the two object are identical, false otherwise
*	@remarks 									Only objects of type CMonthlyValidity can be compared
*/
bool External::Validities::CMonthlyValidity::CompareDerived( const Validities::CValidity& rhs ) const
{
	std::vector<int> rhsValidMonths;
	int rhsValidDay;
	Utilities::CTime rhsBeginTimeOfDay, rhsEndTimeOfDay;

	try {
		// convert to the correct derived class reference enforcing the correct class type
		const CMonthlyValidity& derivRhs = dynamic_cast< const CMonthlyValidity& >( rhs );
		
		// check for mixed validity
		if ( ( !derivRhs.IsValid() && IsValid() ) || ( derivRhs.IsValid() && !IsValid() ) ) {
			return false;
		}

		if ( derivRhs.IsValid() && IsValid() ) {
			derivRhs.GetValidityDays( rhsValidDay, std::back_inserter( rhsValidMonths ), rhsBeginTimeOfDay, rhsEndTimeOfDay );

			// compare identity of class data
			if ( validDay != rhsValidDay ) {
				return false;
			}
			if ( validMonths != rhsValidMonths ) {
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
bool External::Validities::CMonthlyValidity::IsValid() const
{
	return isValid;
}



/**	@brief		Invalidates the object
*	@return 									None
*	@remarks 									None
*/
void External::Validities::CMonthlyValidity::Invalidate()
{
	CMonthlyValidity::validMonths.clear();
	CMonthlyValidity::validDay = -1;
	CMonthlyValidity::beginTimeOfDay = Utilities::CTime();
	CMonthlyValidity::endTimeOfDay = Utilities::CTime();
	CMonthlyValidity::isValid = false;
}