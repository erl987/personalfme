/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

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

#include "GeneralStatusMessage.h"


/**	@brief		Constructor
*	@param		messageType							Type of the message
*	@param		time								Timestamp of the message
*	@param		text								Content of the message
*	@exception										None
*	@remarks										None
*/
Utilities::Message::CGeneralStatusMessage::CGeneralStatusMessage( const MessageType& messageType, const boost::posix_time::ptime& time, const std::string& text )
	: CStatusMessage( messageType, time ),
	  text( text )
{
}


/**	@brief		Obtains the text of this general status message
*	@param		text								Content of the message
*	@return											None
*	@exception										None
*	@remarks										None
*/
void Utilities::Message::CGeneralStatusMessage::GetMessageText( std::string& text ) const
{
	text = CGeneralStatusMessage::text;
}


/**	@brief		Destructor
*/
Utilities::Message::CGeneralStatusMessage::~CGeneralStatusMessage( void )
{
}
