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

#include <iterator>
#include <sstream>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "BoostStdTimeConverter.h"
#include "XMLException.h"
#include "XMLSerializableSingleTimeValidity.h"

const std::string BEGIN_DATE_KEY = "beginDate";
const std::string END_DATE_KEY = "endDate";
const std::string BEGIN_TIME_KEY = "begin";
const std::string END_TIME_KEY = "end";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Validities::CXMLSerializableSingleTimeValidity::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace boost::posix_time;
	auto ToStdTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime;

	string beginTimeString, endTimeString, beginDateString, endDateString;
	ptime beginTime, endTime;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	beginDateString = boost::algorithm::trim_copy( xmlFile->getString( BEGIN_DATE_KEY ) );
	beginTimeString = boost::algorithm::trim_copy( xmlFile->getString( BEGIN_TIME_KEY ) ); // local German time
	beginTime = time_from_string( beginDateString + " " + beginTimeString );

	endDateString = boost::algorithm::trim_copy( xmlFile->getString( END_DATE_KEY ) );
	endTimeString = boost::algorithm::trim_copy( xmlFile->getString( END_TIME_KEY ) ); // local German time
	endTime = time_from_string( endDateString + " " + endTimeString );

	if ( endTime == beginTime ) {
		throw Utilities::Exception::xml_error( "error:\tBegin and end time of the exception validities must not be identical." );
	}
	if ( endTime < beginTime ) {
		throw Utilities::Exception::xml_error( "error:\tThe end time of the exception validities must not be before the begin time." );
	}

	SetValidity( ToStdTime( beginTime ), ToStdTime( endTime ) ); // local German time
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Validities::CXMLSerializableSingleTimeValidity::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	Utilities::CDateTime beginTime, endTime;
	stringstream beginTimeSS, endTimeSS, beginDateSS, endDateSS;

	if ( IsValid() ) {
		// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
		GetValidity( beginTime, endTime );

		// local German time
		beginDateSS << setw( 2 ) << setfill( '0' ) << beginTime.Year();
		beginDateSS << "-";
		beginDateSS << setw( 2 ) << setfill( '0' ) << beginTime.Month();
		beginDateSS << "-";
		beginDateSS << setw( 2 ) << setfill( '0' ) << beginTime.Day();
		xmlFile->setString( BEGIN_DATE_KEY, beginDateSS.str() );

		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.TimeOfDay().Hour();
		beginTimeSS << ":";
		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.TimeOfDay().Minute();
		beginTimeSS << ":";
		beginTimeSS << setw( 2 ) << setfill( '0' ) << beginTime.TimeOfDay().Second();
		xmlFile->setString( BEGIN_TIME_KEY, beginTimeSS.str() );

		// local German time
		endDateSS << setw( 2 ) << setfill( '0' ) << endTime.Year();
		endDateSS << "-";
		endDateSS << setw( 2 ) << setfill( '0' ) << endTime.Month();
		endDateSS << "-";
		endDateSS << setw( 2 ) << setfill( '0' ) << endTime.Day();
		xmlFile->setString( END_DATE_KEY, endDateSS.str() );

		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.TimeOfDay().Hour();
		endTimeSS << ":";
		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.TimeOfDay().Minute();
		endTimeSS << ":";
		endTimeSS << setw( 2 ) << setfill( '0' ) << endTime.TimeOfDay().Second();
		xmlFile->setString( END_TIME_KEY, endTimeSS.str() );
	}
}
