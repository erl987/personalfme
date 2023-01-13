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
#pragma once
#include <memory>
#include <vector>
#include <chrono>
#include "DateTime.h"
#include "StatusMessage.h"

/*@{*/
/** \ingroup Utilities
*/
namespace Utilities {
	namespace Message {
		/** @param SendStatusCode	Possible stati of a message after a sending trial
		*/
		enum SendStatusCode { IN_PROCESSING, NOT_IN_DB, NO_MESSAGE, FATAL_FAILURE, TIMEOUT_FAILURE, NONFATAL_FAILURE, SUCCESS };

		/**	@param	SendStatus		Status containing the status code and an optional status message
		*/
		struct SendStatus {
			Utilities::Message::SendStatusCode code;
			std::string text;
		};


		/**	\ingroup Utilities
		*	Class representing a status message emitted by an alarm message sender
		*/
		template <typename T>
		class CSendStatusMessage : public CStatusMessage
		{
		public:
			CSendStatusMessage( const boost::posix_time::ptime& messageTime, const CDateTime& sequenceTime, const std::vector<int>& code, std::unique_ptr<T> alarmMessage, const SendStatus& sendStatus, const unsigned int& numTrials, const std::chrono::seconds& timeDistTrials );
			virtual void GetMessageContent( CDateTime& sequenceTime, std::vector<int>& code, std::unique_ptr<T>& alarmMessage, SendStatus& sendStatus, unsigned int& numTrials, std::chrono::seconds& timeDistTrials ) const;
			virtual void GetSequenceInfo( CDateTime& sequenceTime, std::vector<int>& code ) const;
			virtual void GetAlarmMessage( std::unique_ptr<T>& alarmMessage ) const;
			virtual void GetStatusInfo( SendStatus& sendStatus, unsigned int& numTrials, std::chrono::seconds& timeDistTrials ) const;
			virtual ~CSendStatusMessage();
		private:
			CDateTime sequenceTime;
			std::vector<int> code;
			std::unique_ptr<T> alarmMessage;
			SendStatus sendStatus;
			unsigned int numTrials;
			std::chrono::seconds timeDistTrials;
		};
	}
}
/*@}*/



/**	@brief		Constructor
*	@param		messageTime							Timestamp of the message
*	@param		sequenceTime						Timestamp of the code sequence for which the message is valid
*	@param		code								Code of the sequence for which the message is valid
*	@param		alarmMessage						Alarm message
*	@param		sendStatus							Sending status of the alarm message
*	@param		numTrials							Current number of sending trials
*	@param		timeDistTrials						Time between the sending trials
*	@exception										None
*	@remarks										None
*/
template <typename T>
Utilities::Message::CSendStatusMessage<T>::CSendStatusMessage( const boost::posix_time::ptime& messageTime, const CDateTime& sequenceTime, const std::vector<int>& code, std::unique_ptr<T> alarmMessage, const SendStatus& sendStatus, const unsigned int& numTrials, const std::chrono::seconds& timeDistTrials )
	: CStatusMessage(),
	  sequenceTime( sequenceTime ),
	  code( code ),
	  alarmMessage( std::move( alarmMessage ) ),
	  sendStatus( sendStatus ),
	  numTrials( numTrials ),
	  timeDistTrials( timeDistTrials )
{
	if ( ( sendStatus.code == SUCCESS ) || ( sendStatus.code == NOT_IN_DB ) || ( sendStatus.code == NO_MESSAGE ) ) {
		CStatusMessage::Reset( MESSAGE_SUCCESS, messageTime );
	} else {
		CStatusMessage::Reset( MESSAGE_ERROR, messageTime );
	}
}


/**	@brief		Obtains the complete contents of the send status message
*	@param		sequenceTime						Timestamp of the code sequence for which the message is valid
*	@param		code								Code of the sequence for which the message is valid
*	@param		alarmMessage						Alarm message
*	@param		sendStatus							Sending status of the alarm message
*	@param		numTrials							Current number of sending trials
*	@param		timeDistTrials						Time between the sending trials
*	@return											None
*	@exception										None
*	@remarks										None
*/
template <typename T>
void Utilities::Message::CSendStatusMessage<T>::GetMessageContent( CDateTime& sequenceTime, std::vector<int>& code, std::unique_ptr<T>& alarmMessage, SendStatus& sendStatus, unsigned int& numTrials, std::chrono::seconds& timeDistTrials ) const
{
	GetSequenceInfo( sequenceTime, code );
	GetAlarmMessage( alarmMessage );
	GetStatusInfo( sendStatus, numTrials, timeDistTrials );
}


/**	@brief		Obtains the sequence information of the send status message
*	@param		sequenceTime						Timestamp of the code sequence for which the message is valid
*	@param		code								Code of the sequence for which the message is valid
*	@return											None
*	@exception										None
*	@remarks										None
*/
template <typename T>
void Utilities::Message::CSendStatusMessage<T>::GetSequenceInfo( CDateTime& sequenceTime, std::vector<int>& code ) const
{
	sequenceTime = CSendStatusMessage::sequenceTime;
	code = CSendStatusMessage::code;
}


/**	@brief		Obtains the status information of the send status message
*	@param		sendStatus							Sending status of the alarm message
*	@param		numTrials							Current number of sending trials
*	@param		timeDistTrials						Time between the sending trials
*	@return											None
*	@exception										None
*	@remarks										None
*/
template <typename T>
void Utilities::Message::CSendStatusMessage<T>::GetStatusInfo( SendStatus& sendStatus, unsigned int& numTrials, std::chrono::seconds& timeDistTrials ) const
{
	sendStatus = CSendStatusMessage::sendStatus;
	numTrials = CSendStatusMessage::numTrials;
	timeDistTrials = CSendStatusMessage::timeDistTrials;
}


/**	@brief		Obtains the alarm message corresponding to the send status message
*	@param		alarmMessage						Alarm message
*	@return											None
*	@exception										None
*	@remarks										None
*/
template <typename T>
void Utilities::Message::CSendStatusMessage<T>::GetAlarmMessage( std::unique_ptr<T>& alarmMessage ) const
{
	if ( CSendStatusMessage::alarmMessage != nullptr ) {
		alarmMessage = CSendStatusMessage::alarmMessage->Clone();
	} else {
		alarmMessage = nullptr;
	}
}


/**	@brief		Destructor
*/
template <typename T>
Utilities::Message::CSendStatusMessage<T>::~CSendStatusMessage( void )
{
}
