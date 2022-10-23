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
#include <boost/serialization/nvp.hpp>
#include "DateTime.h"
#include "SerializableTime.h"

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
	/**	\ingroup Utilities
	*	Serializable class representing a date including the time of day: (DD, MM, YYYY, HH, MM, SS, millisec)
	*/
	class CSerializableDateTime : public CDateTime
	{
	public:	
		UTILITY_API CSerializableDateTime();
		UTILITY_API CSerializableDateTime( const CDateTime& time );
		UTILITY_API CSerializableDateTime( const int& day, const int& month, const int& year, const Utilities::CTime& timeOfDay = Utilities::CTime( 0, 0, 0 ) );
		UTILITY_API virtual ~CSerializableDateTime();
		template <class Archive> void serialize( Archive & ar, const unsigned int version );
	};
}
/*@}*/



/**	@brief		Serialization using boost::serialize
*	@param		ar						See Boost Serialize documentation for details
*	@param		version					See Boost Serialize documentation for details
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <class Archive> void Utilities::CSerializableDateTime::serialize( Archive& ar, const unsigned int version ) {
	using namespace std;

	Utilities::CSerializableTime timeOfDay;
	int day, month, year;

	// serialization of time information
	ar & BOOST_SERIALIZATION_NVP( isValid );

	if ( CDateTime::isValid ) {
		if ( Archive::is_saving::value ) {
			day = get<0>( dateTime );
			month = get<1>( dateTime );
			year = get<2>( dateTime );
			timeOfDay = get<3>( CDateTime::dateTime );
		}

		ar & BOOST_SERIALIZATION_NVP( day );
		ar & BOOST_SERIALIZATION_NVP( month );
		ar & BOOST_SERIALIZATION_NVP( year );
		ar & BOOST_SERIALIZATION_NVP( timeOfDay );

		if ( Archive::is_loading::value ) {
			get<0>( dateTime ) = day;
			get<1>( dateTime ) = month;
			get<2>( dateTime ) = year;
			get<3>( CDateTime::dateTime ) = timeOfDay;
		}
	}
}
