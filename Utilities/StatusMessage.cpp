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

#include "StatusMessage.h"



/**	@brief		Standard constructor
*/
Utilities::Message::CStatusMessage::CStatusMessage(void)
	: isInit(false)
{
}


/**	@brief		Constructor
*	@param		messageType							Type of the message (success, error)
*	@param		time								Timestamp of the message
*	@exception										None
*	@remarks										None
*/
Utilities::Message::CStatusMessage::CStatusMessage( const MessageType& messageType, const boost::posix_time::ptime& time )
	: isInit( true )
{
	Reset( messageType, time );
}


/**	@brief		Destructor
*/
Utilities::Message::CStatusMessage::~CStatusMessage(void)
{
}


/**	@brief		Resets the message
*	@param		messageType							Type of the message (success, error)
*	@param		time								Timestamp of the message
*	@exception										None
*	@remarks										None
*/
void Utilities::Message::CStatusMessage::Reset( const MessageType& messageType, const boost::posix_time::ptime& time )
{
	message = std::make_tuple( messageType, time );
	isInit = true;
}


/**	@brief		Obtains the type of the message
*	@return 										Type of the message (success, error)
*	@exception 	std::logic_error					Thrown if the message has not been initialized
*	@remarks 										None
*/
Utilities::Message::MessageType Utilities::Message::CStatusMessage::GetType(void) const
{
	if ( isInit ) {
		return std::get<MessageType>( message );
	} else {
		throw std::logic_error( "The message is not set." );
	}
}


/**	@brief		Obtains the timestamp of the message
*	@return 										Timestamp of the message
*	@exception 	std::logic_error					Thrown if the message has not been initialized
*	@remarks 										None
*/
boost::posix_time::ptime Utilities::Message::CStatusMessage::GetTimestamp(void) const
{
	if ( isInit ) {
		return std::get<boost::posix_time::ptime>( message );
	} else {
		throw std::logic_error( "The message is not set." );
	}
}
