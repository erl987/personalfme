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

#include "AlarmGateway.h"



/**	@brief		Default constructor
*	@remarks									None
*/
External::CAlarmGateway::CAlarmGateway()
{
}



/**	@brief		Default destructor
*	@remarks									None
*/
External::CAlarmGateway::~CAlarmGateway()
{
}



/**	@brief		Equality operator.
*	@param		lhs					Left-hand side of the equality operator
*	@param		rhs					Right-hand side of the quality operator
*	@return							True in case of equality, otherwise false
*	@exception						None
*	@remarks						The comparison is performed based on the IsEqual-function implementation of the derived classes
*/
bool External::operator==( const CAlarmGateway& lhs, const CAlarmGateway& rhs )
{
	return lhs.IsEqual( rhs );
}



/**	@brief		Unequality operator.
*	@param		lhs					Left-hand side of the unequality operator
*	@param		rhs					Right-hand side of the unquality operator
*	@return							True in case of unequality of the datasets, otherwise false
*	@exception						None
*	@remarks						The comparison in performed for each dataset based on the IsEqual-function implementation of the derived classes
*/
bool External::operator!=( const CAlarmGateway& lhs, const CAlarmGateway& rhs )
{
	return !( lhs == rhs );
}
