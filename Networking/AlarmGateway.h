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
#include <vector>
#include <memory>
#include "MediaFile.h"
#include "DateTime.h"
#include "GatewayLoginData.h"
#include "AlarmMessage.h"

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
	/**	\ingroup Networking
	*	Class implementing an abstract interface for alarm gateways
	*/
	class CAlarmGateway
	{
	public:
		NETWORKING_API CAlarmGateway();
		NETWORKING_API virtual ~CAlarmGateway();
		NETWORKING_API friend bool operator==( const CAlarmGateway& lhs, const CAlarmGateway& rhs );
		NETWORKING_API friend bool operator!=( const CAlarmGateway& lhs, const CAlarmGateway& rhs );

		/** @brief		Performs the sending of the alarm via the gateway
		*	@param		code						Container with the alarm code
		*	@param		alarmTime					Time of the alarm (UTC)
		*	@param		isRealAlarm					Flag stating if the current alarm is a real (default) or test alarm
		*	@param		loginData					Data for the login to the server
		*	@param		message						Message dataset
		*	@param		audioFile					Audio file of the alarm message. If the "state" is directly after detection, the value will be ignored.
		*	@return									None
		*	@exception	std::domain_error			Thrown if the a non-fatal error occured (for example: internet connection was missing, or if the host is unknown)
		*	@exception	std::runtime_error			Thrown if a fatal error occured (for example: SMTP-connection error or the SSL-certificate could not be accepted)
		*	@remarks								None
		*/
		NETWORKING_API virtual void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile = Utilities::CMediaFile() ) = 0;
		
		/** @brief		Clone method
		*	@return									Copy of the current gateway instance
		*	@remarks								None
		*/
		NETWORKING_API virtual std::unique_ptr<CAlarmGateway> Clone() const = 0;
	protected:
		NETWORKING_API virtual bool IsEqual( const CAlarmGateway& rhs ) const = 0;
	};

	NETWORKING_API bool operator==( const CAlarmGateway& lhs, const CAlarmGateway& rhs );
	NETWORKING_API bool operator!=( const CAlarmGateway& lhs, const CAlarmGateway& rhs );
}
/*@}*/
