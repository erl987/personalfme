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

#include <sstream>
#include <boost/lexical_cast.hpp>
#include "XMLSerializableAlarmValidities.h"
#include "XMLException.h"
#include "XMLSerializableAlarmMessagesDatabase.h"

const std::string CODE_KEY = "code";
const std::string CALL_ATTRIB_KEY = "call";
const std::string ALL_KEY = "all";
const std::string FALLBACK_KEY = "fallback";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception	Exception::xml_error		Thrown if the same code is defined multiple times
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::CXMLSerializableAlarmMessagesDatabase::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace Poco::Util;

	vector<string> availableCodes;
	vector< vector<int> > processedCodes;
	vector<int> code;
	string codeString;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	xmlFile->keys( "", availableCodes );
	for ( auto currCode : availableCodes ) {
		bool isForAllCodes = false;
		bool isFallback = false;
		CXMLSerializableAlarmValidities alarmValidities;

		if ( currCode.find( ALL_KEY ) == 0 ) {
			isForAllCodes = true;
		} else if ( currCode.find( FALLBACK_KEY ) == 0 ) {
			isFallback = true;
		} else if ( currCode.find( CODE_KEY ) == 0 ) {
			codeString = xmlFile->getString( currCode + "[@" + CALL_ATTRIB_KEY + "]" );
			code.clear();
			for ( auto digit : codeString ) {
				code.push_back( boost::lexical_cast<int>( digit ) );
			}

			if ( find( begin( processedCodes ), end( processedCodes ), code ) != end( processedCodes ) ) {
				throw Utilities::Exception::xml_error( "error:\tThe call code " + codeString + " is defined multiple times. This is not allowed." );
			}
			processedCodes.push_back( code );
		}

		Poco::AutoPtr<AbstractConfiguration> callView( xmlFile->createView( currCode ) );
		alarmValidities.SetFromXML( callView, isForAllCodes ); // infoalarms are only allowed for <all>-tag

		if ( isForAllCodes ) {
			ReplaceForAllCodes( alarmValidities );
		} else if ( isFallback ) {
			ReplaceFallback( alarmValidities );
		} else {
			Add( code, alarmValidities );
		}
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the database is empty, nothing will be written to the XML-file
*/
void External::CXMLSerializableAlarmMessagesDatabase::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace Poco::Util;

	string currCodeKey;
	External::Types::AlarmDatabaseType alarmDB;

	// write the data to the XML-file
	alarmDB = GetDatabase();
	auto alarmsForAllCodes = CXMLSerializableAlarmValidities( GetAlarmsForAllCodes() );
	auto alarmsAsFallback = CXMLSerializableAlarmValidities( GetFallbackAlarms() );

	// write the data for the <all> / <fallback> alarms
	Poco::AutoPtr<AbstractConfiguration> allView( xmlFile->createView( ALL_KEY ) );
	alarmsForAllCodes.GenerateXML( allView );

	Poco::AutoPtr<AbstractConfiguration> fallbackView( xmlFile->createView( FALLBACK_KEY ) );
	alarmsAsFallback.GenerateXML( fallbackView );

	// write the data for all alarms
	auto counter = 0;
	for ( auto entry : alarmDB ) {
		currCodeKey = CODE_KEY + "[" + to_string( counter ) + "]";

		stringstream codeSS;
		for ( auto digit : entry.first ) {
			codeSS << digit;
		}
		xmlFile->setString( currCodeKey + "[@" + CALL_ATTRIB_KEY + "]", codeSS.str() );

		Poco::AutoPtr<AbstractConfiguration> callView( xmlFile->createView( currCodeKey ) );
		auto alarmValidities = CXMLSerializableAlarmValidities( entry.second );
		alarmValidities.GenerateXML( callView );
		counter++;
	}
}