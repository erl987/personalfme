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
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"


/** @brief		Converting time from standard format (Utilities::CDateTime) to Boost Date Time library format
*	@param		stdTime					Tuple containing the time in standard format (DD, MM, YYYY, HH, MM, SS)
*	@return 							Time in Boost ptime-format		
*	@exception							None
*	@remarks							None
*/
boost::posix_time::ptime Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime(const Utilities::CDateTime& stdTime)
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	long year, month, day, hour, minute, second, millisec;
	boost::posix_time::ptime boostTime;

	if ( stdTime.IsValid() ) {
		day = stdTime.Day();
		month = stdTime.Month();
		year = stdTime.Year();
		hour = stdTime.TimeOfDay().Hour();
		minute = stdTime.TimeOfDay().Minute();
		second = stdTime.TimeOfDay().Second();
		millisec = stdTime.TimeOfDay().Millisec();
		try {
			if ( time_duration::num_fractional_digits() == 9 ) { // nanosecond resolution of  Boost Time library
				boostTime = ptime( date( static_cast<unsigned short>(year), static_cast<unsigned short>(month), static_cast<unsigned short>(day) ), time_duration( hour, minute, second, millisec * 1'000'000 ) );
			} else { // microsecond resolution of Boost Time library
				boostTime = ptime( date( static_cast<unsigned short>(year), static_cast<unsigned short>(month), static_cast<unsigned short>(day) ), time_duration( hour, minute, second, millisec * 1'000 ) );
			}
		} catch (std::out_of_range e) {
			// set to "not a date / time"
			boostTime = ptime();
		}
	} else {
		boostTime = ptime( not_a_date_time );
	}

	return boostTime;
}



/** @brief		Converting time from Boost Date Time library format to standard format (Utilities::CDateTime)
*	@param		boostTime				Time in Boost ptime-format	
*	@return 							Tuple containing the time in standard format (DD, MM, YYYY, HH, MM, SS)	
*	@exception							None
*	@remarks							None
*/
Utilities::CDateTime Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime(const boost::posix_time::ptime& boostTime)
{
	int year, month, day, hour, minute, second, millisec;
	Utilities::CDateTime stdTime;

	if ( !boostTime.is_not_a_date_time() ) {
		year = boostTime.date().year();
		month = boostTime.date().month();
		day = boostTime.date().day();
		hour = static_cast<int>( boostTime.time_of_day().hours() );
		minute = static_cast<int>( boostTime.time_of_day().minutes() );
		second = static_cast<int>( boostTime.time_of_day().seconds() );
		if ( boost::posix_time::time_duration::num_fractional_digits() == 9 ) { // nanosecond resolution of  Boost Time library
			millisec = static_cast<int>( boostTime.time_of_day().fractional_seconds() / 1.0e6 );
		} else { // microsecond resolution of Boost Time library
			millisec = static_cast<int>( boostTime.time_of_day().fractional_seconds() / 1.0e3 );
		}
		stdTime.Set( day, month, year, Utilities::CTime( hour, minute, second, millisec ) );
	} else {
		stdTime = Utilities::CDateTime();
	}

	return stdTime;
}



/**	@brief		Converts local time to UTC
*	@param		localTime						Local time to be converted
*	@return										Converted UTC-time
*	@exception 									None
*	@remarks 									In case of ambiguity during daylight saving shift -> the hour "A" is implicitly chosen
*												In case the time is not existing during daylight saving shift -> the next existing time is taken (in case of German local time: 03:00 am)
*/
Utilities::CDateTime Utilities::Time::CBoostStdTimeConverter::LocalToUTC( const Utilities::CDateTime& localTime )
{
	using namespace boost::posix_time;
	using namespace Utilities::Time;
	auto ToBoostTime = &CBoostStdTimeConverter::ConvertToBoostTime;
	auto ToStdTime = &CBoostStdTimeConverter::ConvertToStdTime;

	boost::posix_time::ptime utcTime;
	boost::local_time::time_zone_ptr timeZone;

	try {
		// convert to UTC-time (in case of ambiguity during daylight saving shift -> the hour "A" is implicitly chosen)
		utcTime = german_local_date_time( ToBoostTime( localTime ).date(), ToBoostTime( localTime ).time_of_day() ).utc_time();
	} catch ( boost::local_time::time_label_invalid e ) {
		// local time is not existing during daylight saving shift -> take the next existing time (in case of German local time: 03:00 am)
		timeZone = german_local_date_time().zone();
		if ( timeZone->dst_offset() > hours( 0 ) ) {
			// shift in spring
			auto beginDST = timeZone->dst_local_start_time( localTime.Year() ) + timeZone->dst_offset();
			utcTime = german_local_date_time( beginDST.date(), beginDST.time_of_day() ).utc_time();
		} else {
			// shift in fall
			auto endDST = timeZone->dst_local_end_time( localTime.Year() ) - timeZone->dst_offset();
			utcTime = german_local_date_time( endDST.date(), endDST.time_of_day() ).utc_time();
		}
	}

	return ToStdTime( utcTime );
}



/**	@brief		Converts a week number to the Boost Gregorian library format
*	@param		weekNum							Week number to be converted
*	@return 									Converted week number
*	@exception	std::out_of_range				Thrown if the week number is not valid
*	@remarks 									None
*/
boost::gregorian::nth_day_of_the_week_in_month::week_num Utilities::Time::CBoostStdTimeConverter::GetBoostWeekNum(const WeekType& weekNum)
{
	using namespace boost::gregorian;

	switch ( weekNum ) {
	case FIRST:
		return nth_day_of_the_week_in_month::first;
		break;
	case SECOND:
		return nth_day_of_the_week_in_month::second;
		break;
	case THIRD:
		return nth_day_of_the_week_in_month::third;
		break;
	case FOURTH:
		return nth_day_of_the_week_in_month::fourth;
		break;
	case LAST:
		return nth_day_of_the_week_in_month::fifth;
		break;
	default:
		throw std::out_of_range( "The week number is not valid." );
		break;
	}
}



/**	@brief		Converts a month number to the Boost Gregorian library format
*	@param		monthNum						Month number to be converted
*	@return 									Converted month number
*	@exception	std::out_of_range				Thrown if the month number is not valid
*	@remarks 									None
*/
boost::gregorian::date::month_type Utilities::Time::CBoostStdTimeConverter::GetBoostMonthNum(const unsigned int& monthNum)
{
	using namespace boost::gregorian;

	switch ( monthNum ) {
	case 1:
		return Jan;
		break;
	case 2:
		return Feb;
		break;
	case 3:
		return Mar;
		break;
	case 4:
		return Apr;
		break;
	case 5:
		return May;
		break;
	case 6:
		return Jun;
		break;
	case 7:
		return Jul;
		break;
	case 8:
		return Aug;
		break;
	case 9:
		return Sep;
		break;
	case 10:
		return Oct;
		break;
	case 11:
		return Nov;
		break;
	case 12:
		return Dec;
		break;
	default:
		throw std::out_of_range( "The month number is not valid." );
		break;
	}
}



/**	@brief		Converts a week day number to the Boost Gregorian library format
*	@param		weekDay							Day of week to be converted
*	@return 									Converted week day
*	@exception	std::out_of_range				Thrown if the week day is not valid
*	@remarks 									None
*/
boost::gregorian::greg_weekday Utilities::Time::CBoostStdTimeConverter::GetBoostWeekDay(const DayType& weekDay)
{
	using namespace boost::gregorian;

	switch ( weekDay ) {
	case MONDAY:
		return Monday;
		break;
	case TUESDAY:
		return Tuesday;
		break;
	case WEDNESDAY:
		return Wednesday;
		break;
	case THURSDAY:
		return Thursday;
		break;
	case FRIDAY:
		return Friday;
		break;
	case SATURDAY:
		return Saturday;
		break;
	case SUNDAY:
		return Sunday;
		break;
	default:
		throw std::out_of_range( "The week day is not valid." );
		break;
	}
}


/**	@brief		Determines if a certain day given by day and month can exist in any year
*	@param		day								Day of month to be checked
*	@param		month							Month to be checked
*	@return										False if the day is not existing in the month, true otherwise. Also false if the month is invalid
*	@exception									None
*	@remarks 									None
*/
bool Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist(const unsigned int& day, const int& month)
{
	std::vector<int> months;

	months.push_back( month );
	return DayOfYearCanExist( day, months );
}



/**	@brief		Determines if a certain day given by day and month can exist in any year
*	@param		day								Day of month to be checked
*	@param		months							Container with the months to be checked
*	@return										False if the day is not existing in any month, true otherwise. Also falso if any of the months is invalid
*	@exception									None
*	@remarks 									None
*/
bool Utilities::Time::CBoostStdTimeConverter::DayOfYearCanExist(const unsigned int& day, const std::vector<int>& months)
{
	for (size_t i=0; i < months.size(); i++) {
		// check the validity of the month
		if ( ( months[i] < 1 ) || ( months[i] > 12 ) ) {
			return false; // in this case the result is definitively false
		}
	}

	// check the validity of the day
	if ( day < 1 ) {
		return false; // in this case the result is definitively false
	}

	for (size_t i=0; i < months.size(); i++) {
		switch ( months[i] ) {
			case 1 : case 3: case 5: case 7: case 8: case 10: case 12 :
				if ( day <= 31 ) {
					return true;
				}
				break;
			case 4 : case 6: case 9: case 11:
				if ( day <= 30 ) {
					return true;
				}
			case 2:
				if ( day <= 29 ) {
					return true;
				}
				break;
		}
	}

	return false;
}
