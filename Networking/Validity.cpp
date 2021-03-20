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

#include "Validity.h"

/**	@brief		Equality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are identical, otherwise false
*	@exception									None
*	@remarks 									The comparison has to be implemented in the derived class using the abstract function Validities::CValidity::CompareDerived. It can use Validities::CValidity::CompareBaseValidity() for comparing the base classes.
*/
bool External::Validities::operator==( const External::Validities::CValidity& lhs, const External::Validities::CValidity& rhs ) {
	return ( lhs.CompareDerived( rhs ) );
}


/**	@brief		Unequality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are not identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::Validities::operator!=( const External::Validities::CValidity& lhs, const External::Validities::CValidity& rhs ) {
	return !( lhs == rhs );
}
