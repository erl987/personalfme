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

#include <vector>
#include <algorithm>
#include "ExternalProgramGateway.h"
#include "ExternalProgramMessage.h"


/** @brief		Default constructor
*	@remarks									None
*/
External::ExternalProgram::CExternalProgramMessage::CExternalProgramMessage()
	: CAlarmMessage( true ), // external program commands are only allowed directly after detecting an alarm
	  isEmpty(true)
{
}


/** @brief		Constructor
*	@param		command							Program command / script to be called (the command is called relative to the working directory)
*	@param		programArguments				Arguments to that command, if several arguments are given, it may be useful to separate them using quotation marks
*	@remarks									None
*/
External::ExternalProgram::CExternalProgramMessage::CExternalProgramMessage( const std::string& command, const std::string& programArguments )
	: CAlarmMessage( true ), // external program commands are only allowed directly after detecting an alarm
	  isEmpty(true)
{
	Set( command, programArguments );
}


/**	@brief		Cloning method duplicating the object
*	@return										Duplicate base class pointer of the object
*	@exception									None
*	@remarks									None
*/
std::unique_ptr< External::CAlarmMessage > External::ExternalProgram::CExternalProgramMessage::Clone() const
{
	return std::make_unique<CExternalProgramMessage>( *this );
}


/** @brief		(Re-) set the message
*	@param		command							Program command / script to be called (the command is called relative to the working directory)
*	@param		programArguments				Arguments to that command, if several arguments are given, it may be useful to separate them using quotation marks
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::ExternalProgram::CExternalProgramMessage::Set( const std::string& command, const std::string& programArguments )
{
	CExternalProgramMessage::command = command;
	CExternalProgramMessage::programArguments = programArguments;
	CExternalProgramMessage::isEmpty = false;
}


/** @brief		Obtains the command of that message
*	@return										Program command / script to be called (the command is called relative to the working directory)
*	@exception									None
*	@remarks									None
*	@exception	std::domain_error				Thrown if the dataset is empty
*/
std::string External::ExternalProgram::CExternalProgramMessage::GetCommand() const
{
	if ( isEmpty ) {
		throw std::domain_error( "The message has not been initialized." );
	}

	return command;
}


/** @brief		Obtains the program call arguments of that message
*	@return										Arguments to that command, if several arguments are given, it may be useful to separate the using quotation marks
*	@exception									None
*	@remarks									None
*	@exception	std::domain_error				Thrown if the dataset is empty
*/
std::string External::ExternalProgram::CExternalProgramMessage::GetProgramArguments() const
{
	if ( isEmpty ) {
		throw std::domain_error( "The message has not been initialized." );
	}

	return programArguments;
}


/**	@brief		Determines if this object and onother external program dataset are equivalent
*	@param		rhs								Dataset to be compared
*	@return										True if the datasets are equal, false otherwise.
*	@exception									None		
*	@remarks									Comparing with any other object than CExternalProgramMessage will return false
*/
bool External::ExternalProgram::CExternalProgramMessage::IsEqual( const CAlarmMessage& rhs ) const
{
	try {
		const CExternalProgramMessage& derivRhs = dynamic_cast< const CExternalProgramMessage& >( rhs );
		
		if ( ( IsEmpty() && !derivRhs.IsEmpty() ) || ( !IsEmpty() && derivRhs.IsEmpty() ) ) {
			return false;
		}

		// checks for equality of all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( command != derivRhs.command ) {
				return false;
			}
			if ( programArguments != derivRhs.programArguments ) {
				return false;
			}
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Determines if this object is smaller than another CExternalProgramMessage object
*	@param		rhs								Dataset to be compared
*	@return										True if this dataset is smaller, false otherwise
*	@exception									None		
*	@remarks									Comparing with any other object than CExternalProgramMessage will return false
*/
bool External::ExternalProgram::CExternalProgramMessage::IsSmaller( const CAlarmMessage& rhs ) const
{
	try {
		const CExternalProgramMessage& derivRhs = dynamic_cast< const CExternalProgramMessage& >( rhs );

		// check for is-smaller for all components
		if ( !IsEmpty() && !derivRhs.IsEmpty() ) {
			if ( command >= derivRhs.command ) {
				return false;
			}
			if ( programArguments >= derivRhs.programArguments ) {
				return false;
			}
		} else {
			return false;
		}
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}



/**	@brief		Checking if the dataset is empty.
*	@return										True if the dataset is empty, if it contains valid data it is false
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the external program call
*/
bool External::ExternalProgram::CExternalProgramMessage::IsEmpty(void) const
{
	return isEmpty;
}



/**	@brief		Setting the dataset empty.
*	@return										None
*	@exception									None		
*	@remarks									An empty dataset is interpreted as the suppression of the external program call
*/
void External::ExternalProgram::CExternalProgramMessage::SetEmpty(void)
{
	isEmpty = true;
}


/**	@brief		Obtains the type index of the gateway class corresponding to the present message class.
*	@return										Type index of the gateway class corresponding to the present message class
*	@exception									None		
*	@remarks									None
*/
std::type_index External::ExternalProgram::CExternalProgramMessage::GetGatewayType() const
{
	return std::type_index( typeid( CExternalProgramGateway ) );
}