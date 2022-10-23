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
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "SerializableTime.h"


/**	@brief		Standard constructor
*/
Utilities::CSerializableTime::CSerializableTime()
	: CTime()
{
}


/**	@brief		Destructor
*/
Utilities::CSerializableTime::~CSerializableTime()
{
}


/**	@brief		Constructor
*	@param		hour					Hour
*	@param		minute					Minute
*	@param		second					Second
*	@param		millisec				Millisecond. If omitted, 0 ms are assumed.
*	@exception							None
*	@remarks							None
*/
Utilities::CSerializableTime::CSerializableTime( const int& hour, const int& minute, const int& second, const int& millisec )
	: CTime( hour, minute, second, millisec )
{
}


/**	@brief		Constructor using the non-serializable class format
*	@param		time					Time in non-serializable format
*	@exception							None
*	@remarks							None
*/
Utilities::CSerializableTime::CSerializableTime( const CTime& time )
	: CTime()
{
	// only in case of a valid time the data is set, otherwise the time will be invalid by default
	if ( time.IsValid() ) {
		Set( time.Hour(), time.Minute(), time.Second(), time.Millisec() );
	}
}