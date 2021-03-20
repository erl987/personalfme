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
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "DetectorStatusMessage.h"


/**	@brief		Constructor
*	@param		messageTime							Timestamp of the message
*	@param		status								Status of the detector
*	@param		samplingFreq						Sampling frequency of the detection. If omitted it is set to 0 Hz by default.
*	@exception										None
*	@remarks										None
*/
Utilities::Message::CDetectorStatusMessage::CDetectorStatusMessage( const boost::posix_time::ptime& messageTime, const DetectorStatusCode& status, const int& samplingFreq )
	: CStatusMessage(),
	  status( status ),
	  samplingFreq( samplingFreq )
{
	if ( ( status == RUNNING ) || ( status == STOPPED ) ) {
		CStatusMessage::Reset( MESSAGE_SUCCESS, messageTime );
	} else {
		CStatusMessage::Reset( MESSAGE_ERROR, messageTime );
	}
}


/**	@brief		Obtains the complete contents of the send status message
*	@param		status								Status of the detector
*	@param		samplingFreq						Sampling frequency of the detection
*	@return											None
*	@exception										None
*	@remarks										None
*/
void Utilities::Message::CDetectorStatusMessage::GetMessageContent( DetectorStatusCode& status, int& samplingFreq ) const
{
	status = CDetectorStatusMessage::status;
	samplingFreq = CDetectorStatusMessage::samplingFreq;
}


/**	@brief		Destructor
*/
Utilities::Message::CDetectorStatusMessage::~CDetectorStatusMessage( void )
{
}
