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
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include <stdexcept>
#include "AudioDevice.h"


/** @brief		Standard constructor
*/
Core::Processing::CAudioDevice::CAudioDevice()
	: isSet( false )
{
}


/** @brief		Constructor
*	@param		type				Type of the audio device (input or output device)
*	@param		deviceName			Name of the device. Omit it if the standard device of the current system should be used.
*	@param		driverName			Name of the audio driver. Omit it if the standard driver of the current system should be used.
*	@exception	std::runtime_error	Thrown if the default device is specified, but the driver name is not empty
*	@remarks						None
*/
Core::Processing::CAudioDevice::CAudioDevice( const AudioDeviceType& type, const std::string& deviceName, const std::string& driverName )
	: type( type ),
	  deviceName( deviceName ),
	  driverName( driverName ),
	  isSet( true )
{
	if ( deviceName.empty() && !driverName.empty() ) {
		throw std::runtime_error( "For specifying the default device, also the driver name must be empty." );
	}
}


/** 	@brief		Destructor
*/
Core::Processing::CAudioDevice::~CAudioDevice()
{
}


/** @brief		Obtains the name of the device
*	@return							Name of the device. Empty if the standard device of the current system should be used.
*	@exception	std::runtime_error	Thrown if the audio device is not set
*	@remarks						None
*/
std::string Core::Processing::CAudioDevice::GetDeviceName() const
{
	if ( !isSet ) {
		throw std::runtime_error( "The audio device is not set." );
	}

	return deviceName;
}


/** @brief		Obtains the name of the used audio driver
*	@return							Name of the audio driver. Empty if the standard driver of the current system should be used.
*	@exception	std::runtime_error	Thrown if the audio device is not set
*	@remarks						None
*/
std::string Core::Processing::CAudioDevice::GetDriverName() const
{
	if ( !isSet ) {
		throw std::runtime_error( "The audio device is not set." );
	}

	return driverName;
}



/** @brief		Obtains the type of the device
*	@return							Type of the device
*	@exception	std::runtime_error	Thrown if the audio device is not set
*	@remarks						None
*/
Core:: AudioDeviceType Core::Processing::CAudioDevice::GetType() const
{
	if ( !isSet ) {
		throw std::runtime_error( "The audio device is not set." );
	}

	return type;
}


/** @brief		Determines if the audio device is set
*	@return							True if the device is set, false otherwise
*	@exception						None
*	@remarks						It is not checked if the device is available on the current system
*/
bool Core::Processing::CAudioDevice::IsSet() const
{
	return isSet;
}


/**	@brief		Equality operator.
*	@param		lhs					Left-hand side of the equality operator
*	@param		rhs					Right-hand side of the quality operator
*	@return							True in case of equality of the objects, otherwise false
*	@exception						None
*	@remarks						None
*/
bool Core::Processing::operator==( const CAudioDevice& lhs, const CAudioDevice& rhs )
{
	if ( ( lhs.IsSet() && !rhs.IsSet() ) || ( !lhs.IsSet() && rhs.IsSet() ) ) {
		return false;
	}

	if ( lhs.IsSet() && ( rhs.IsSet() ) ) {
		if ( ( lhs.GetDeviceName() == rhs.GetDeviceName() ) && ( lhs.GetType() == rhs.GetType() ) && ( lhs.GetDriverName() == rhs.GetDriverName() ) ) {
			return true;
		} else {
			return false;
		}
	} else {
		return true;
	}
}



/**	@brief		Unequality operator.
*	@param		lhs					Left-hand side of the unequality operator
*	@param		rhs					Right-hand side of the unquality operator
*	@return							True in case of unequality of the objects, otherwise false
*	@exception						None
*	@remarks						None
*/
bool Core::Processing::operator!=( const CAudioDevice& lhs, const CAudioDevice& rhs )
{
	return !( lhs == rhs );
}