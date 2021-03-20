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
#pragma once
#include <boost/serialization/nvp.hpp>
#include "CTime.h"

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
	*	Serializable class representing a time (HH, MM, SS, millisec)
	*/
	class CSerializableTime : public CTime
	{
	public:	
		UTILITY_API CSerializableTime();
		UTILITY_API CSerializableTime( const CTime& time );
		UTILITY_API CSerializableTime( const int& hour, const int& minute, const int& second, const int& millisec = 0 );
		UTILITY_API virtual ~CSerializableTime();
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
template <class Archive> void Utilities::CSerializableTime::serialize(Archive& ar, const unsigned int version) {
	using namespace std;

	int hour, minute, second, millisec;

	// serialization of time information
	ar & BOOST_SERIALIZATION_NVP( isValid );

	if ( CTime::isValid ) {
		if ( Archive::is_saving::value ) {
			hour = get<0>( time );
			minute = get<1>( time );
			second = get<2>( time );
			millisec = get<3>( time );
		}

		ar & BOOST_SERIALIZATION_NVP( hour );
		ar & BOOST_SERIALIZATION_NVP( minute );
		ar & BOOST_SERIALIZATION_NVP( second );
		ar & BOOST_SERIALIZATION_NVP( millisec );

		if ( Archive::is_loading::value ) {
			get<0>( time ) = hour;
			get<1>( time ) = minute;
			get<2>( time ) = second;
			get<3>( time ) = millisec;
		}
	}
}
