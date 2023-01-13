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

#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include "SendStatusMessage.h"
#include "DetectorStatusMessage.h"
#include "GeneralStatusMessage.h"
#include "Groupalarm2Message.h"
#include "EmailMessage.h"


/*@{*/
/**	\ingroup PersonalFME
*/
namespace Logger {
	/**	\ingroup PersonalFME
	*	Thread safe singleton event logger class
	*/
	class CLogger
	{
	public:
		static CLogger& Instance( const boost::filesystem::path& logFile = "");
		std::string Log( std::unique_ptr<Utilities::Message::CStatusMessage> message );
		virtual ~CLogger(void);
	protected:
		void PerformLogging(const Utilities::Message::MessageType& logLevel, const boost::posix_time::ptime& eventTime, const std::string& message);
		std::string GenerateSendStatusMessage( const Utilities::Message::CSendStatusMessage<External::CAlarmMessage>& message ) const;
		std::string GenerateDetectorStatusMessage( const Utilities::Message::CDetectorStatusMessage& message ) const;
		std::string GenerateGeneralStatusMessage( const Utilities::Message::CGeneralStatusMessage& message ) const;
		void GetGroupalarmInfo( const External::Groupalarm::CGroupalarm2Message& alarmMessage, std::string& messageInfoString, std::string& messageTypeString ) const;
		void GetEmailInfo( const External::Email::CEmailMessage& alarmMessage, std::string& messageInfoString, std::string& messageTypeString ) const;

		static std::unique_ptr<CLogger> instancePtr;
		static std::once_flag onceFlag;
		std::ofstream outFile;
	private:
		CLogger( const boost::filesystem::path& logFile ); // this enforces that no object can be created from the class - except in the static variable instancePtr during the first call of CLogger::Instance
		CLogger(const CLogger&) = delete;
		CLogger& operator=(const CLogger&) = delete;

		static std::mutex logMutex;
	};
}
/*@}*/
