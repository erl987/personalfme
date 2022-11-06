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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <iterator>
#include <sstream>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "BoostStdTimeConverter.h"
#include "XMLException.h"
#include "XMLSerializableWeeklyValidity.h"

const std::string VALID_WEEKS_KEY = "validWeeks";
	const std::string ALL_VALID_WEEKS_KEY = "all";
	const std::string SINGLE_VALID_WEEK_KEY = "week";
const std::string WEEKDAY_KEY = "weekday";
	const std::string MONDAY_KEY = "Mo";
	const std::string TUESDAY_KEY = "Di";
	const std::string WEDNESDAY_KEY = "Mi";
	const std::string THURSDAY_KEY = "Do";
	const std::string FRIDAY_KEY = "Fr";
	const std::string SATURDAY_KEY = "Sa";
	const std::string SUNDAY_KEY = "So";
const std::string BEGIN_TIME_KEY = "begin";
const std::string END_TIME_KEY = "end";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Validities::CXMLSerializableWeeklyValidity::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace Utilities::Time;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	auto ToStdTime = &CBoostStdTimeConverter::ConvertToStdTime;

	int currWeekRaw;
	vector<string> tags;
	vector<WeekType> weeksInMonth;
	DayType weekDay;
	WeekType currWeek;
	string weekdayString, beginTimeString, endTimeString;
	ptime beginTime, endTime;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	if ( xmlFile->hasProperty( VALID_WEEKS_KEY + "." + ALL_VALID_WEEKS_KEY ) ) {
		weeksInMonth = { FIRST, SECOND, THIRD, FOURTH, LAST };
	} else {
		xmlFile->keys( VALID_WEEKS_KEY, tags );
		for ( auto tag : tags ) {
			if ( tag.find( SINGLE_VALID_WEEK_KEY ) == 0 ) {
				currWeekRaw = xmlFile->getInt( VALID_WEEKS_KEY + "." + tag );
				switch ( currWeekRaw ) {
				case 1:
					currWeek = FIRST;
					break;
				case 2:
					currWeek = SECOND;
					break;
				case 3:
					currWeek = THIRD;
					break;
				case 4:
					currWeek = FOURTH;
					break;
				case 5:
					currWeek = LAST;
					break;
				}
				weeksInMonth.push_back( currWeek );
			}
		}
		weeksInMonth.erase( unique( begin( weeksInMonth ), end( weeksInMonth ) ), end( weeksInMonth ) );
	}

	weekdayString = boost::algorithm::trim_copy( xmlFile->getString( WEEKDAY_KEY ) );
	if ( weekdayString == MONDAY_KEY ) {
		weekDay = MONDAY;
	} else if ( weekdayString == TUESDAY_KEY ) {
		weekDay = TUESDAY;
	} else if ( weekdayString == WEDNESDAY_KEY ) {
		weekDay = WEDNESDAY;
	} else if ( weekdayString == THURSDAY_KEY ) {
		weekDay = THURSDAY;
	} else if ( weekdayString == FRIDAY_KEY ) {
		weekDay = FRIDAY;
	} else if ( weekdayString == SATURDAY_KEY ) {
		weekDay = SATURDAY;
	} else if ( weekdayString == SUNDAY_KEY ) {
		weekDay = SUNDAY;
	}

	beginTimeString = boost::algorithm::trim_copy( xmlFile->getString( BEGIN_TIME_KEY ) ); // local German time
	beginTime = time_from_string( "2000-01-01 " + beginTimeString ); // the exact date is not relevant

	endTimeString = boost::algorithm::trim_copy( xmlFile->getString( END_TIME_KEY ) ); // local German time
	endTime = time_from_string( "2000-01-01 " + endTimeString ); // the exact date is not relevant

	if ( endTime == beginTime ) {
		throw Utilities::Exception::xml_error( "error:\tThe start and end times of the weekly exception must not be identical." );
	}

	SetValidityDates( weeksInMonth, weekDay, ToStdTime( beginTime ).TimeOfDay(), ToStdTime( endTime ).TimeOfDay() ); // local German time
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Validities::CXMLSerializableWeeklyValidity::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace Utilities::Time;

	unsigned int singleValidWeek;
	vector<WeekType> weeksInMonth;
	DayType weekDay;
	string dayString;
	stringstream beginTimeSS, endTimeSS;
	Utilities::CTime beginTime, endTime;

	if ( IsValid() ) {
		GetValidityDates( back_inserter( weeksInMonth ), weekDay, beginTime, endTime );

		// write the data to the XML-file
		if ( find( begin( weeksInMonth ), end( weeksInMonth ), FIRST ) != end( weeksInMonth ) &&
			find( begin( weeksInMonth ), end( weeksInMonth ), SECOND ) != end( weeksInMonth ) &&
			find( begin( weeksInMonth ), end( weeksInMonth ), THIRD ) != end( weeksInMonth ) &&
			find( begin( weeksInMonth ), end( weeksInMonth ), FOURTH ) != end( weeksInMonth ) &&
			find( begin( weeksInMonth ), end( weeksInMonth ), LAST ) != end( weeksInMonth ) ) {
			xmlFile->setString( VALID_WEEKS_KEY + "." + ALL_VALID_WEEKS_KEY, "" );
		} else {
			auto counter = 0;
			for ( auto week : weeksInMonth ) {
				switch ( week ) {
				case FIRST:
					singleValidWeek = 1;
					break;
				case SECOND:
					singleValidWeek = 2;
					break;
				case THIRD:
					singleValidWeek = 3;
					break;
				case FOURTH:
					singleValidWeek = 4;
					break;
				case LAST:
					singleValidWeek = 5;
					break;
				}

				xmlFile->setInt( VALID_WEEKS_KEY + "." + SINGLE_VALID_WEEK_KEY + "[" + to_string( counter ) + "]", singleValidWeek );
				counter++;
			}
		}

		switch ( weekDay ) {
		case MONDAY:
			dayString = MONDAY_KEY;
			break;
		case TUESDAY:
			dayString = TUESDAY_KEY;
			break;
		case WEDNESDAY:
			dayString = WEDNESDAY_KEY;
			break;
		case THURSDAY:
			dayString = THURSDAY_KEY;
			break;
		case FRIDAY:
			dayString = FRIDAY_KEY;
			break;
		case SATURDAY:
			dayString = SATURDAY_KEY;
			break;
		case SUNDAY:
			dayString = SUNDAY_KEY;
			break;
		}
		xmlFile->setString( WEEKDAY_KEY, dayString );

		// local German time
		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.Hour();
		beginTimeSS << ":";
		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.Minute();
		beginTimeSS << ":";
		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.Second();
		xmlFile->setString( BEGIN_TIME_KEY, beginTimeSS.str() );

		// local German time

		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.Hour();
		endTimeSS << ":";
		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.Minute();
		endTimeSS << ":";
		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.Second();
		xmlFile->setString( END_TIME_KEY, endTimeSS.str() );
	}
}
