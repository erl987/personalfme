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
#include <vector>
#include <string>
#include <map>
#include <thread>
#include "GatewayLoginDatabase.h"
#include "AlarmMessagesDatabase.h"
#include "StatusMessage.h"
#include "ConnectionManager.h"

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
	*	Class implementing the access to the alarm gateways (for example: SMS-gateway, e-mail ...).
	*/
	class CAlarmGatewaysManager
	{
	public:
		NETWORKING_API CAlarmGatewaysManager( std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback, std::function<void( const std::exception_ptr& )> exceptionCallback );
		NETWORKING_API virtual ~CAlarmGatewaysManager();
		NETWORKING_API virtual void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const Utilities::CMediaFile& audioFile, const bool& state );
		NETWORKING_API virtual void ResetGatewayLoginDatabase( const CGatewayLoginDatabase& newDatabase );
		NETWORKING_API virtual void GetGatewayLoginDatabase( CGatewayLoginDatabase& database );
		NETWORKING_API virtual void ResetAlarmMessagesDatabase( const CAlarmMessageDatabase& newDatabase );
		NETWORKING_API virtual void GetAlarmMessagesDatabase( CAlarmMessageDatabase& database );
	private:
		std::mutex databaseMutex;
		CGatewayLoginDatabase loginDatabase;
		CAlarmMessageDatabase alarmMessagesDatabase;
		std::map< std::type_index, std::unique_ptr<CConnectionManager> > connectionManagers;
		bool isLoginDatabaseLoaded;
		bool isAlarmMessagesDatabaseLoaded;
		std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback;
		std::function<void( const std::exception_ptr& )> exceptionCallback;
	};
}
/*@}*/
