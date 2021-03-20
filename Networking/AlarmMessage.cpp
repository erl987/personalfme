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

#include "AlarmMessage.h"


/**	@brief		Constructor.
*	@param		requiredState		Flag stating the state (true: send out immediately after the detection, false: send out after the recording of the audio sequence)
*	@exception						None		
*	@remarks						None
*/
External::CAlarmMessage::CAlarmMessage(const bool& requiredState)
	: requiredState( requiredState )
{
}


/**	@brief		Equality operator.
*	@param		lhs					Left-hand side of the equality operator
*	@param		rhs					Right-hand side of the quality operator
*	@return							True in case of equality of the datasets, otherwise false
*	@exception						None		
*	@remarks						The comparison in performed for each dataset based on the IsEqual-function implementation of the derived classes
*/
bool External::operator==(const CAlarmMessage& lhs, const CAlarmMessage& rhs)
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
bool External::operator!=(const CAlarmMessage& lhs, const CAlarmMessage& rhs)
{
	return !( lhs == rhs );
}



/**	@brief		Is-smaller operator.
*	@param		lhs					Left-hand side of the is-smaller operator
*	@param		rhs					Right-hand side of the is-smaller operator
*	@return							True if the left dataset is smaller, otherwise false
*	@exception						None		
*	@remarks						The comparison in performed for each dataset based on the IsSmaller-function implementation of the derived classes
*/
bool External::operator<(const CAlarmMessage& lhs, const CAlarmMessage& rhs)
{
	return lhs.IsSmaller( rhs );
}



/**	@brief		Obtains the state for which the alarm dataset is valid (i.e. if it should be sent out immediately after the detection or after the recording of the audio sequence)
*	@return							Flag stating the state (true: send out immediately after the detection, false: send out after the recording of the audio sequence)
*	@exception						None		
*	@remarks						None
*/
bool External::CAlarmMessage::RequiredState(void) const
{
	return requiredState;
}


/**	@brief		Resets the state for which the alarm dataset is valid (i.e. if it should be sent out immediately after the detection or after the recording of the audio sequence)
*	@param		requiredState		Flag stating the state (true: send out immediately after the detection, false: send out after the recording of the audio sequence)
*	@return							None
*	@exception						None		
*	@remarks						None
*/
void External::CAlarmMessage::SetRequiredState(const bool& requiredState)
{
	CAlarmMessage::requiredState = requiredState;
}