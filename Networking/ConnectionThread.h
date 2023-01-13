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
#include <string>
#include <thread>
#include <mutex>
#include <future>
#include <vector>
#include <condition_variable>
#include <functional>
#include <exception>
#include "DateTime.h"
#include "AlarmMessage.h"
#include "SendStatusMessage.h"
#include "AlarmGateway.h"
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
	*	@brief	Structure containing all data required to send an alarm message
	*/
	struct Message {
		std::vector<int> sequence;
		Utilities::CDateTime time;
		bool isRealAlarm;
		std::unique_ptr< CAlarmMessage > message;
		std::unique_ptr< CGatewayLoginData > login;
		Utilities::CMediaFile audioFile;
	};


	/**	\ingroup Networking
	*	Class implementing a thread containing a single connection to a gateway resource
	*/
	class CConnectionThread
	{
	public:
		NETWORKING_API CConnectionThread( std::unique_ptr<CAlarmGateway> connection, std::function<void()> finishedSendingCallback, std::function<void(const std::exception_ptr&)> exceptionCallback );
		NETWORKING_API virtual ~CConnectionThread();
		NETWORKING_API virtual void SendMessage( std::unique_ptr<Message> message, const unsigned int& numTrials );
		NETWORKING_API virtual bool GetStatus( Utilities::Message::SendStatus& status, Utilities::CDateTime& time, std::vector<int>& sequence, unsigned int& numTrials, std::unique_ptr<Message>& currMessage );
	private:
		virtual void ConnectionThread();

		std::thread connectionThread;
		std::unique_ptr<CAlarmGateway> connection;
		std::vector< std::future<void> > callbackFutures;
		std::unique_ptr<Message> message;
		Utilities::CDateTime time;
		std::vector<int> sequence;
		bool isRealAlarm;
		std::unique_ptr< CAlarmMessage > messageData;
		std::unique_ptr< CGatewayLoginData > login;
		Utilities::CMediaFile audioFile;
		unsigned int numTrials;
		Utilities::Message::SendStatus status;
		std::mutex dataMutex;
		std::condition_variable newMessageReceived;
		bool isNewMessageReceived;
		std::mutex messageProcessingMutex;
		bool isTerminateThread;
		std::function<void()> finishedSendingCallback;
		std::function<void(const std::exception_ptr&)> exceptionCallback;
	};
}
/*@}*/
