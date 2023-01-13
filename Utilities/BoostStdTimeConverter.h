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
/**	\defgroup	Utilities	Library for support classes required in the Core-modules as well as in client modules.
*/
#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "TimeDatatypes.h"
#include "DateTime.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef UTILITY_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define UTILITY_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define UTILITY_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define UTILITY_API __attribute__ ((visibility ("default")))
	#else
		#define UTILITY_API
	#endif		
#endif

/*@{*/
/** \ingroup Utilities
*/


namespace Utilities {
	namespace Time {
		/**	\ingroup Utilities
		*	Class for converting between STL-based and Boost Date Time based time representation.
		*/
		class CBoostStdTimeConverter
		{
		public:
			UTILITY_API CBoostStdTimeConverter(void){};
			UTILITY_API ~CBoostStdTimeConverter(void){};
			UTILITY_API static boost::posix_time::ptime ConvertToBoostTime(const Utilities::CDateTime& stdTime);
			UTILITY_API static Utilities::CDateTime ConvertToStdTime(const boost::posix_time::ptime& boostTime);
			UTILITY_API static Utilities::CDateTime LocalToUTC( const Utilities::CDateTime& localTime );
			UTILITY_API static bool DayOfYearCanExist(const unsigned int& day, const int& month);
			UTILITY_API static bool DayOfYearCanExist(const unsigned int& day, const std::vector<int>& months);
			UTILITY_API static boost::gregorian::nth_day_of_the_week_in_month::week_num GetBoostWeekNum(const WeekType& weekNum);
			UTILITY_API static boost::gregorian::date::month_type GetBoostMonthNum(const unsigned int& monthNum);
			UTILITY_API static boost::gregorian::greg_weekday GetBoostWeekDay(const DayType& weekDay);
		private:
			CBoostStdTimeConverter(const CBoostStdTimeConverter &);					// prevent copying
    		CBoostStdTimeConverter & operator= (const CBoostStdTimeConverter &);	// prevent assignment
		};
	}
}
/*@}*/
