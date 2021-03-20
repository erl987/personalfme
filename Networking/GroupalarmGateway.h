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
#pragma once

#include <string>
#include "GatewayLoginData.h"
#include "AlarmMessage.h"
#include "InfoalarmMessageDecorator.h"
#include "EmailMessage.h"
#include "GroupalarmMessage.h"
#include "AlarmGateway.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef NETWORKING_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define NETWORKING_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define NETWORKING_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define NETWORKING_API __attribute__ ((visibility ("default")))
	#else
		#define NETWORKING_API
	#endif		
#endif


/*@{*/
/** \ingroup Networking
*/
namespace External {
	namespace Groupalarm {
		/**	@param	serverName				Name of SMS-gateway server of www.Groupalarm.de */
		const std::string serverName = "www.groupalarm.de";
		/** @param	documentName			Name of weblink file on the server of www.Groupalarm.de */
		const std::string documentName = "/webin.php";
		/*	@param	successString			Text returned by the www.Groupalarm.de weblink in case of successfull SMS-sending*/
		const std::string successString = "OK";
		/*	@param	failString				Text returned by the www.Groupalarm.de weblink in case of failed SMS-sending */
		const std::string failString = "Not_OK: ";
		/*	@param	softwareVersionCode		Code for identifying the software version for www.Groupalarm.de */
		const std::string softwareVersionCode = "PersonalFME_2";

		/**	\ingroup Networking
		*	Class implementing a weblink to the SMS-gateway www.Groupalarm.de. It can be used whenever an internet connection is available.
		*/
		class CGroupalarmGateway : public CAlarmGateway
		{
		public:
			NETWORKING_API CGroupalarmGateway(void);
			NETWORKING_API virtual ~CGroupalarmGateway(void) {};
			NETWORKING_API virtual void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile = Utilities::CMediaFile() ) override;
			NETWORKING_API virtual std::unique_ptr<CAlarmGateway> Clone() const override;
		protected:
			virtual bool IsEqual( const CAlarmGateway& rhs ) const override;
			static std::string CreateOtherMessagesInfo( const Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage );
			static std::string GetAlarmReceiverID( const External::Email::CEmailMessage& message );
		};
	}
}
/*@}*/
