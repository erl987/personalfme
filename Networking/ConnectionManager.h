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
#include <string>
#include <map>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <exception>
#include "ConnectionThread.h"
#include "StatusMessage.h"
#include "AlarmMessage.h"
#include "GatewayLoginData.h"


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
	*	Class implementing a timed connection manager for ensuring proper access to network ressources via repeated trials
	*/
	class CConnectionManager
	{
	public:
		NETWORKING_API CConnectionManager( std::unique_ptr<CAlarmGateway> gateway, std::unique_ptr<CGatewayLoginData> login, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback, std::function<void( const std::exception_ptr& )> exceptionCallback );
		NETWORKING_API virtual ~CConnectionManager( void );
		NETWORKING_API virtual void AddMessage( const std::vector<int>& newSequence, const Utilities::CDateTime& time, const bool& isRealAlarm, std::unique_ptr< CAlarmMessage > alarmMessageDataset, const Utilities::CMediaFile& audioFile );
	private:
		CConnectionManager( const CConnectionManager& ) = delete;
		CConnectionManager& operator= ( const CConnectionManager& ) = delete;
		virtual void ManagerThread();
		virtual void RegainFinishedConnections( const unsigned int& maxNumTrials, const std::chrono::milliseconds& timeDistTrials );
		virtual bool SendNextDueMessage();
		virtual void OnConnectionFinished();
		
		std::unique_ptr<CGatewayLoginData> login;
		std::map< std::chrono::time_point< std::chrono::steady_clock >, std::pair< std::unique_ptr<Message>, unsigned int > > messageQueue;
		std::thread managerThread;
		std::condition_variable managerThreadWaitCondition;
		std::mutex managerThreadMutex;
		bool isResumeThread;
		bool isTerminateThread;
		std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback;
		std::function<void( const std::exception_ptr& )> exceptionCallback;
		std::deque< std::unique_ptr< CConnectionThread > > availableConnections;
		std::deque< std::unique_ptr< CConnectionThread > > unavailableConnections;
	};
}
/*@}*/
