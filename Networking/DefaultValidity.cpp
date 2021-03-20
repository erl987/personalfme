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

#include "DefaultValidity.h"



/**	@brief		Default constructor
*/
External::Validities::CDefaultValidity::CDefaultValidity(void)
	: Validities::CValidity()
{
}



/**	@brief		Returns all validity periods for the given month (UTC time)
*	@param		UTCMonth						Month for which the validities are calculated (UTC-time)
*	@param		UTCYear							Year for which the validities are calculated (UTC-time)
*	@return 									Storing all validity periods [begin time, end time) (UTC-times) intersecting with in the given month (resolution: seconds)
*	@remarks 									Validities may overlap from the neighbouring months, this is considered
*/
std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > External::Validities::CDefaultValidity::GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const
{
	// in the default case exceptions are never valid
	return std::vector< std::pair< Utilities::CDateTime, Utilities::CDateTime > >();
}



/**	@brief		Comparison of this with another object
*	@param		rhs								Object to be compared
*	@return 									True if the two object are identical, false otherwise
*	@remarks 									Only objects of type CDefaultValidity can be compared
*/
bool External::Validities::CDefaultValidity::CompareDerived(const Validities::CValidity& rhs) const
{
	// convert to the correct derived class reference enforcing the correct class type
	try {
		dynamic_cast< const CDefaultValidity& >( rhs );
	} catch ( std::bad_cast e ) {
		return false;
	}

	// the default validity objects are identical by definition
	return true; 
}


/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::Validities::CValidity > External::Validities::CDefaultValidity::Clone() const
{
	return std::make_unique<CDefaultValidity>( *this );
}


/**	@brief		Checking if the object is valid and can be used
*	@return 									True if the object is valid, false otherwise
*	@remarks 									None
*/
bool External::Validities::CDefaultValidity::IsValid( void ) const
{
	// the default validity is always valid
	return true;
}


/**	@brief		Invalidates the object
*	@return 									None
*	@remarks 									None
*/
void External::Validities::CDefaultValidity::Invalidate( void )
{
	// the default validity is always valid
}
