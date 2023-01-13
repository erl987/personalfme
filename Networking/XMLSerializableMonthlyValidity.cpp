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

#include <iterator>
#include <sstream>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "BoostStdTimeConverter.h"
#include "XMLException.h"
#include "XMLSerializableMonthlyValidity.h"

const std::string VALID_MONTHS_KEY = "validMonths";
	const std::string ALL_VALID_MONTHS_KEY = "all";
	const std::string SINGLE_VALID_MONTH_KEY = "month";
const std::string DAY_KEY = "day";
const std::string BEGIN_TIME_KEY = "begin";
const std::string END_TIME_KEY = "end";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Validities::CXMLSerializableMonthlyValidity::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;
	auto ToStdTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime;

	int currMonth, day;
	vector<int> months;
	vector<string> tags;
	string beginTimeString, endTimeString;
	ptime beginTime, endTime;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	if ( xmlFile->hasProperty( VALID_MONTHS_KEY + "." + ALL_VALID_MONTHS_KEY ) ) {
		months.resize( 12 );
		int n = 1;
		generate( begin( months ), end( months ), [&n] { return n++; } );
	} else {
		xmlFile->keys( VALID_MONTHS_KEY, tags );
		for ( auto tag : tags ) {
			if ( tag.find( SINGLE_VALID_MONTH_KEY ) == 0 ) {
				currMonth = xmlFile->getInt( VALID_MONTHS_KEY + "." + tag );
				months.push_back( currMonth );
			}
		}
		months.erase( unique( begin( months ), end( months ) ), end( months ) );
	}

	day = xmlFile->getInt( DAY_KEY );

	beginTimeString = boost::algorithm::trim_copy( xmlFile->getString( BEGIN_TIME_KEY ) ); // local German time
	beginTime = time_from_string( "2000-01-01 " + beginTimeString ); // the exact date is not relevant

	endTimeString = boost::algorithm::trim_copy( xmlFile->getString( END_TIME_KEY ) ); // local German time
	endTime = time_from_string( "2000-01-01 " + endTimeString ); // the exact date is not relevant

	if ( endTime == beginTime ) {
		throw Utilities::Exception::xml_error( "error:\tThe start and end time of the monthly validity exception must not be identical." );
	}

	try {
		SetValidityDays( day, months, ToStdTime( beginTime ).TimeOfDay(), ToStdTime( endTime ).TimeOfDay() ); // local German time
	} catch ( std::out_of_range e ) {
		throw Utilities::Exception::xml_error( "error:\tThe day of the monthly exception is never valid." );
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Validities::CXMLSerializableMonthlyValidity::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	int day;
	vector<int> months, allMonths;
	string dayString;
	stringstream beginTimeSS, endTimeSS;
	Utilities::CTime beginTime, endTime;

	// generate a list containing all months for comparison purposes
	allMonths.resize( 12 );
	int n = 1;
	generate( begin( allMonths ), end( allMonths ), [&n] { return n++; } );

	if ( IsValid() ) {
		GetValidityDays( day, back_inserter( months ), beginTime, endTime );

		// write the data to the XML-file
		sort( begin( months ), end( months ) );
		if ( months == allMonths ) {
			xmlFile->setString( VALID_MONTHS_KEY + "." + ALL_VALID_MONTHS_KEY, "" );
		} else {
			auto counter = 0;
			for ( auto month : months ) {
				xmlFile->setInt( VALID_MONTHS_KEY + "." + SINGLE_VALID_MONTH_KEY + "[" + to_string( counter ) + "]", month );
				counter++;
			}
		}

		xmlFile->setInt( DAY_KEY, day );

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
