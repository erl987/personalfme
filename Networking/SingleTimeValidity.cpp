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

#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"
#include "SingleTimeValidity.h"


/**	@brief		Default constructor
*/
External::Validities::CSingleTimeValidity::CSingleTimeValidity(void)
	: isValid( false )
{
}


/**	@brief		Constructor
*	@param		beginTime						Begin date and time of the single time validity exception (local German time)
*	@param		endTime							End date and time of the single time validity exception [beginTime, endTime) (local German time)
*	@exception 	std::out_of_range				Thrown if the begin or end time are not valid or the end time is before the start time
*	@remarks 									None
*/
External::Validities::CSingleTimeValidity::CSingleTimeValidity( const Utilities::CDateTime& beginTime, const Utilities::CDateTime& endTime )
	: isValid( false )
{
	SetValidity( beginTime, endTime );
}


/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								None
*/
std::unique_ptr< External::Validities::CValidity > External::Validities::CSingleTimeValidity::Clone() const
{
	return std::make_unique<CSingleTimeValidity>( *this );
}


/**	@brief		Setting the validity of the exception
*	@param		beginTime						Begin date and time of the single time validity exception (local German time)
*	@param		endTime							End date and time of the single time validity exception [beginTime, endTime) (local German time)
*	@return										None
*	@exception 	std::out_of_range				Thrown if the begin or end time are not valid or the end time is not after the start time
*	@remarks 									None
*/
void External::Validities::CSingleTimeValidity::SetValidity( const Utilities::CDateTime& beginTime, const Utilities::CDateTime& endTime )
{
	if ( !beginTime.IsValid() || !endTime.IsValid() ) {
		throw std::out_of_range( "The start or the end time of the single time exception are invalid." );
	}
	if ( endTime <= beginTime ) {
		throw std::out_of_range( "The end time of the single time validity must be after the begin time." );
	}

	CSingleTimeValidity::beginTime = beginTime;
	CSingleTimeValidity::endTime = endTime;
	CSingleTimeValidity::isValid = true;
}


/**	@brief		Getting the start day of the validity
*	@param		beginTime						Begin date and time of the single time validity exception (local German time)
*	@param		endTime							End date and time of the single time validity exception [beginTime, endTime) (local German time)
*	@return										None
*	@exception 	std::runtime_error				Thrown if the object is not valid
*	@remarks 									None
*/
void External::Validities::CSingleTimeValidity::GetValidity( Utilities::CDateTime& beginTime, Utilities::CDateTime& endTime ) const
{
	if ( !isValid ) {
		throw std::runtime_error( "The validity is not valid." );
	}

	beginTime = CSingleTimeValidity::beginTime;
	endTime = CSingleTimeValidity::endTime;
}


/**	@brief		Returns all validity periods for the given month
*	@param		UTCMonth						Month for which the validities are calculated (UTC-time)
*	@param		UTCYear							Year for which the validities are calculated (UTC-time)
*	@return 									Storing all validity periods [begin time, end time) (UTC-times) intersecting with in the given month (resolution: seconds)
*	@exception	std::runtime_error				Thrown if the object is not valid
*	@exception	std::out_of_range				Thrown if either given month or year are invalid
*	@remarks 									Validities may overlap from the neighbouring months, this is considered
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CSingleTimeValidity::GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const
{
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	using namespace Utilities::Time;

	auto ToStdTime = &CBoostStdTimeConverter::ConvertToStdTime;
	auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;
	auto LocalToUTC = &CBoostStdTimeConverter::LocalToUTC;

	ptime beginTimeUTC, endTimeUTC, monthBeginUTC;
	std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > validityPeriods;

	// check validity of the object
	if ( !isValid ) {
		throw std::runtime_error( "The single time validity object is invalid." );
	}
	
	// converting local time to UTC-time (considering special cases during daylight saving shifts)
	beginTimeUTC = ToBoostTime( LocalToUTC( beginTime ) );
	endTimeUTC = ToBoostTime( LocalToUTC( endTime ) );

	// determine the occurrences of the time period in the required month
	if ( endTimeUTC > beginTimeUTC ) {
		monthBeginUTC = ptime( date( UTCYear, greg_month( UTCMonth ), 1 ) );  // std::out_of_range will be thrown if the date is invalid
		time_period UTCmonthPeriod( monthBeginUTC, monthBeginUTC + months( 1 ) );
		if ( time_period( beginTimeUTC, endTimeUTC ).intersects( UTCmonthPeriod ) ) {
			validityPeriods.push_back( std::make_pair( ToStdTime( beginTimeUTC ), ToStdTime( endTimeUTC ) ) );
		}
	}

	return validityPeriods;
}


/**	@brief		Comparison of this with another object
*	@param		rhs								Object to be compared
*	@return 									True if the two object are identical, false otherwise
*	@remarks 									Only objects of type CSingleTimeValidity can be compared
*/
bool External::Validities::CSingleTimeValidity::CompareDerived( const Validities::CValidity& rhs ) const
{
	Utilities::CDateTime lhsBeginTime, rhsBeginTime, lhsEndTime, rhsEndTime;

	try {
		// convert to the correct derived class reference enforcing the correct class type
		const CSingleTimeValidity& derivRhs = dynamic_cast< const CSingleTimeValidity& >( rhs );

		// check for mixed validity
		if ( ( !derivRhs.IsValid() && IsValid() ) || ( derivRhs.IsValid() && !IsValid() ) ) {
			return false;
		}

		if ( derivRhs.IsValid() && IsValid() ) {
			GetValidity( lhsBeginTime, lhsEndTime );
			derivRhs.GetValidity( rhsBeginTime, rhsEndTime );

			if ( lhsBeginTime != rhsBeginTime ) {
				return false;
			}
			if ( lhsEndTime != rhsEndTime ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}


/**	@brief		Determines if the object is valid
*	@return 									True if the object is valid, false otherwise
*	@remarks 									None
*/
bool External::Validities::CSingleTimeValidity::IsValid(void) const
{
	return CSingleTimeValidity::isValid;
}


/**	@brief		Invalidates the object
*	@return 									None
*	@remarks 									None
*/
void External::Validities::CSingleTimeValidity::Invalidate(void)
{
	beginTime.Invalidate();
	endTime.Invalidate();
	CSingleTimeValidity::isValid = false;
}
