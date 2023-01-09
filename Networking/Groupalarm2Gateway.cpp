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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#if defined _WIN32
	#include "stdafx.h"
#endif
#include "Groupalarm2GatewayImpl.h"
#include "Groupalarm2Gateway.h"


/** @brief		Standard constructor
*	@remarks								None
*/
External::Groupalarm::CGroupalarm2Gateway::CGroupalarm2Gateway()
	: CAlarmGateway(),
	privImpl(new CGroupalarm2GatewayImpl)
{
}

/** @brief		Destructor
*	@remarks								None
*/
External::Groupalarm::CGroupalarm2Gateway::~CGroupalarm2Gateway(void)
{
}


/**	@brief		Clone method
*	@return										Copy of the present instance
*	@exception									None
*	@remarks									None
*/
std::unique_ptr<External::CAlarmGateway> External::Groupalarm::CGroupalarm2Gateway::Clone() const
{
	return std::make_unique<CGroupalarm2Gateway>();
}


/** @brief		Performs the sending of the alarm via the gateway
*	@param		code						Container with the alarm code
*	@param		alarmTime					Time of the alarm (UTC)
*	@param		isRealAlarm					Flag stating if the current alarm is a real (default) or test alarm
*	@param		loginData					Login data for the e-mail server
*	@param		message						Data for the alarm message
*	@param		audioFile					Audio file of the alarm message. If it is empty, no file will be attached. It is empty by default.
*	@return									None
*	@exception	std::domain_error			Thrown if the internet connection was missing (or if the host is unknown)
*	@exception	std::runtime_error			Thrown if the e-mail-sending failed due to a SMTP-connection error or the SSL-certificate could not be accepted
*	@remarks								None
*/
void External::Groupalarm::CGroupalarm2Gateway::Send(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile)
{
	privImpl->Send(code, alarmTime, isRealAlarm, std::move(loginData), std::move(message), audioFile);
}



/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::Groupalarm::CGroupalarm2Gateway::IsEqual(const CAlarmGateway& rhs) const
{
	try {
		dynamic_cast<const CGroupalarm2Gateway&>(rhs);
	}
	catch (std::bad_cast e) {
		return false;
	}

	return true;
}
