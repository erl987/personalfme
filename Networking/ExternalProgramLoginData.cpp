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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include "ExternalProgramLoginData.h"


/** @brief	Standard constructor.
*/
External::ExternalProgram::CExternalProgramLoginData::CExternalProgramLoginData(void)
	: CGatewayLoginData()
{
}



/** @brief	Destructor.
*/
External::ExternalProgram::CExternalProgramLoginData::~CExternalProgramLoginData(void)
{
}



/** @brief		Provides a clone of the current object
*	@return									Clone of the current object
*	@exception								None
*	@remarks								None
*/
std::unique_ptr< External::CGatewayLoginData > External::ExternalProgram::CExternalProgramLoginData::Clone(void) const
{
	return std::make_unique<CExternalProgramLoginData>( *this );
}


/**	@brief		Determines if the current login data is valid
*	@return										True if the login data is valid, false otherwise
*	@exception									None
*	@remarks									None
*/
bool External::ExternalProgram::CExternalProgramLoginData::IsValid() const
{
	if ( !IsBaseClassValid() ) {
		return false;
	}

	return true;
}



/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::ExternalProgram::CExternalProgramLoginData::IsEqual( const CGatewayLoginData& rhs ) const
{
	// compare the base objects
	if ( !CGatewayLoginData::IsEqual( rhs ) ) {
		return false;
	}

	try {
		const CExternalProgramLoginData& derivRhs = dynamic_cast< const CExternalProgramLoginData& >( rhs );
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}
