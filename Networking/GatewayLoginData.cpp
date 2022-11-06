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

#include <stdexcept>
#include "GatewayLoginData.h"


/** @brief		Standard constructor.
*/
External::CGatewayLoginData::CGatewayLoginData(void)
	: isBaseClassValid( false )
{
}



/** @brief		Destructor.
*/
External::CGatewayLoginData::~CGatewayLoginData(void)
{
}



/**	@brief		Getting the connection trials informations for the gateway.
*	@param		numTrials						Maximum number of trials for connecting if no internet connection is available. In case of wrong settings, it will be aborted immediately.
*	@param		timeDistTrial					Time between two trials for connecting (in seconds)
*	@param		maxNumConnections				Maximum number of parallel connections
*	@return										None
*	@exception	std::runtime_error				Thrown if the login data is not completely set
*	@remarks									None
*/
void External::CGatewayLoginData::GetConnectionTrialInfos(unsigned int& numTrials, float& timeDistTrial, unsigned int& maxNumConnections ) const
{
	if ( !IsBaseClassValid() ) {
		throw std::runtime_error( "The login data is not completely set." );
	}

	numTrials = CGatewayLoginData::numTrials;
	timeDistTrial = CGatewayLoginData::timeDistTrial;
	maxNumConnections = CGatewayLoginData::maxNumConnections;
}



/**	@brief		Setting the class variables for the Groupalarm.de settings.
*	@param		numTrials						Maximum number of trials for connecting if no internet connection is available. In case of wrong settings, it will be aborted immediately.
*	@param		timeDistTrial					Time between two trials for connecting
*	@param		maxNumConnections				Maximum number of parallel connections
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::CGatewayLoginData::SetConnectionTrialInfos(const unsigned int& numTrials, const float& timeDistTrial, const unsigned int& maxNumConnections )
{
	CGatewayLoginData::numTrials = numTrials;
	CGatewayLoginData::timeDistTrial = timeDistTrial;
	CGatewayLoginData::maxNumConnections = maxNumConnections;
	isBaseClassValid = true;
}



/**	@brief		Determines is the base class is set.
*	@return										True if the base class is set, false otherwise
*	@exception									None
*	@remarks									None
*/
bool External::CGatewayLoginData::IsBaseClassValid(void) const
{
	return isBaseClassValid;
}



/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This pure virtual method can only be used by the derived classes for implementing their equality comparison methods
*/
bool External::CGatewayLoginData::IsEqual( const CGatewayLoginData& rhs ) const
{
	unsigned int rhsNumTrials, rhsMaxNumConnections;
	float rhsTimeDistTrial;

	try {
		// checks if both datasets are either empty or non-empty
		if ( ( IsBaseClassValid() && !rhs.IsBaseClassValid() ) || ( !IsBaseClassValid() && rhs.IsBaseClassValid() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( IsBaseClassValid() && rhs.IsBaseClassValid() ) {
			rhs.GetConnectionTrialInfos( rhsNumTrials, rhsTimeDistTrial, rhsMaxNumConnections );
			if ( numTrials != rhsNumTrials ) {
				return false;
			}
			if ( timeDistTrial != rhsTimeDistTrial ) {
				return false;
			}
			if ( maxNumConnections != rhsMaxNumConnections ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Equality operator.
*	@param		lhs					Left-hand side of the equality operator
*	@param		rhs					Right-hand side of the quality operator
*	@return							True in case of equality of the objects, otherwise false
*	@exception						None
*	@remarks						The comparison in performed for each object based on the IsEqual-function implementation of the derived classes
*/
bool External::operator==( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs )
{
	return lhs.IsEqual( rhs );
}



/**	@brief		Unequality operator.
*	@param		lhs					Left-hand side of the unequality operator
*	@param		rhs					Right-hand side of the unquality operator
*	@return							True in case of unequality of the objects, otherwise false
*	@exception						None
*	@remarks						The comparison in performed for each object based on the IsEqual-function implementation of the derived classes
*/
bool External::operator!=( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs )
{
	return !( lhs == rhs );
}
