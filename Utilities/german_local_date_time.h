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
#pragma once

#include <vector>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/assign/list_of.hpp>

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
		const std::string timeZoneString = "MEZ+1MESZ,M3.5.0/02:00,M10.5.0/03:00"; // according to "German Sommerzeitverordnung SoZV issued on 12 July 2001, changed on 3rd July 2008"
		const boost::local_time::time_zone_ptr timeZone = boost::local_time::time_zone_ptr( new boost::local_time::posix_time_zone( timeZoneString ) );
		const std::vector< std::string > weekdayNameList = boost::assign::list_of("Sonntag")("Montag")("Dienstag")("Mittwoch")("Donnerstag")("Freitag")("Samstag");

		/**	\ingroup Utilities
		*	Class for formatting time output according to "German Sommerzeitverordnung SoZV issued on 12 July 2001, changed on 3rd July 2008". This is MEZ / MESZ, but during change from DST to standard time according to §2(2) SoZV, the nomenclatures 2A (MESZ) and 2B (MEZ) are used.
		*/	
		class german_local_date_time
			: public boost::local_time::local_date_time
		{
		public:
			UTILITY_API german_local_date_time(void);
			UTILITY_API german_local_date_time(const boost::posix_time::ptime& time);
			UTILITY_API german_local_date_time(const boost::gregorian::date& date, const boost::posix_time::time_duration& localTimeOfDay);
			UTILITY_API friend std::ostream &operator<<(std::ostream &stream, german_local_date_time localTime);
		private:
			std::vector<std::string> weekdayNames;
		};
	}
}

/*@}*/