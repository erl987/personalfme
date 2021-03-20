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
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "german_local_date_time.h"


/**	@brief		Default constructor generating an invalid time
*/
Utilities::Time::german_local_date_time::german_local_date_time(void)
	: boost::local_time::local_date_time( boost::local_time::not_a_date_time, timeZone ),
	  weekdayNames( weekdayNameList )
{
}



/**	@brief		Constructor
*	@param		time								Date and UTC-time passed for formatted output
*	@exception 										None
*	@remarks 										This class relies on the Boost date time library
*/
Utilities::Time::german_local_date_time::german_local_date_time(const boost::posix_time::ptime& time)
	: boost::local_time::local_date_time( time, timeZone ),
	  weekdayNames( weekdayNameList )
{
}



/**	@brief		Constructor
*	@param		date								Date passed for formatted output
*	@param		localTimeOfDay						Local German time of the day
*	@exception 										None
*	@remarks 										Daylight saving is automatically deduced from the date. If it is ambiguous during daylight saving change time period (hours "A" / "B"), the daylight saving time is chosen (i.e. hour "A" according German standard).
*/
Utilities::Time::german_local_date_time::german_local_date_time(const boost::gregorian::date& date, const boost::posix_time::time_duration& localTimeOfDay)
	// this is a workaround for choosing the daylight saving time, if the local time is ambiguous in case of change in daylight saving time
	: boost::local_time::local_date_time( date, localTimeOfDay, timeZone, !( german_local_date_time::check_dst( date, localTimeOfDay, timeZone ) == boost::local_time::is_not_in_dst ) ),
	  weekdayNames( weekdayNameList )
{
}


namespace Utilities {
	namespace Time {
		/**	@brief		Output operator for formatted German time
		*	@param		stream								Output stream
		*	@param		localTime							Output object
		*	@exception 										None
		*	@remarks 										This function overloads the standard output operator
		*/
		std::ostream& operator<<(std::ostream &stream, Utilities::Time::german_local_date_time localTime)
		{
			using namespace std;
			using namespace boost::local_time;
			string changeString;

			// determine if localTime is during daylight saving changes
			if ( local_date_time::check_dst( localTime.date(), localTime.local_time().time_of_day(), localTime.zone() ) == ambiguous ) {
				if ( localTime.is_dst() ) {
					changeString = "A";
				} else {
					changeString = "B";
				}
			} else {
				changeString = "";
			}

			// generate output
			stream << localTime.weekdayNames.at( localTime.local_time().date().day_of_week() ) << ", ";
			stream << setw(2) << setfill('0') << localTime.local_time().date().day() << ".";
			stream << setw(2) << localTime.local_time().date().month().as_number() << ".";
			stream << setw(4) << localTime.local_time().date().year() << " ";
			stream << setw(2) << localTime.local_time().time_of_day().hours() << ":";
			stream << setw(2) << localTime.local_time().time_of_day().minutes() << ":";
			stream << setw(2) << localTime.local_time().time_of_day().seconds();
			if ( !changeString.empty() ) {
				stream << " " << changeString;
			}
	
			return stream;
		}
	}
}
