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
#pragma once

#include <memory>
#include <vector>
#include "AlarmGateway.h"
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
	namespace Email {
		/**	\ingroup Networking
		*	Class implementing an e-mail alarm sending gateway via the SMTP-protocol. It can be used whenever an internet connection is available.
		*/
		class CEmailGateway : public External::CAlarmGateway
		{
		public:
			NETWORKING_API CEmailGateway();
			NETWORKING_API virtual void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile = Utilities::CMediaFile() );
			NETWORKING_API virtual ~CEmailGateway();
			NETWORKING_API virtual std::unique_ptr<CAlarmGateway> Clone() const;
		protected:
			NETWORKING_API virtual bool IsEqual( const CAlarmGateway& rhs ) const;
		private:
			class CEmailGatewayImpl;
			std::unique_ptr<CEmailGatewayImpl> privImpl;
		};
	}
}
/*@}*/
