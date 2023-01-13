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
#pragma once
#include <tuple>
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
	*	Class representing a date including the time of day: (DD, MM, YYYY, HH, MM, SS, millisec)
	*/
	class CDateTime
	{
	public:	
		UTILITY_API CDateTime();
		UTILITY_API CDateTime( const int& day, const int& month, const int& year, const Utilities::CTime& timeOfDay = Utilities::CTime( 0, 0, 0, 0 ) );
		UTILITY_API virtual ~CDateTime();
		UTILITY_API void Set( const int& day, const int& month, const int& year, const Utilities::CTime& timeOfDay = Utilities::CTime( 0, 0, 0, 0 ) );
		UTILITY_API void Get( int& day, int& month, int& year, Utilities::CTime& timeOfDay ) const;
		UTILITY_API int Day() const;
		UTILITY_API int Month() const;
		UTILITY_API int Year() const;
		UTILITY_API Utilities::CTime TimeOfDay() const;
		UTILITY_API bool IsValid() const;
		UTILITY_API void Invalidate();
		UTILITY_API friend bool operator==(const CDateTime& lhs, const CDateTime& rhs);
		UTILITY_API friend bool operator!=(const CDateTime& lhs, const CDateTime& rhs);
		UTILITY_API friend bool operator<( const CDateTime& lhs, const CDateTime& rhs );
		UTILITY_API friend bool operator>( const CDateTime& lhs, const CDateTime& rhs );
		UTILITY_API friend bool operator<=( const CDateTime& lhs, const CDateTime& rhs );
		UTILITY_API friend bool operator>=( const CDateTime& lhs, const CDateTime& rhs );
	protected:
		std::tuple<int,int,int,Utilities::CTime> dateTime;
		bool isValid;
	};
}
/*@}*/
