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

#include <vector>
#include <memory>
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
	namespace Infoalarm {
		/**	\ingroup Networking
		*	Class implementing an infoalarm decorator for any gateway message. This is used to send an info message to another user if a message has been sent.
		*/
		class CInfoalarmMessageDecorator : public External::CAlarmMessage
		{
		public:
			NETWORKING_API CInfoalarmMessageDecorator();
			NETWORKING_API CInfoalarmMessageDecorator( std::unique_ptr<External::CAlarmMessage> gateway );
			NETWORKING_API CInfoalarmMessageDecorator( const External::Infoalarm::CInfoalarmMessageDecorator& src );
			NETWORKING_API CInfoalarmMessageDecorator& operator=( const CInfoalarmMessageDecorator& rhs );
			NETWORKING_API virtual ~CInfoalarmMessageDecorator() {};
			NETWORKING_API virtual bool IsEmpty( void ) const override;
			NETWORKING_API virtual void SetEmpty( void ) override;
			NETWORKING_API virtual std::unique_ptr<CAlarmMessage> Clone() const override;
			NETWORKING_API virtual std::type_index GetGatewayType() const override;
			NETWORKING_API virtual void SetContainedMessage( std::unique_ptr<External::CAlarmMessage> containedMessage );
			NETWORKING_API virtual std::unique_ptr<CAlarmMessage> GetContainedMessage() const;
			NETWORKING_API virtual void SetOtherMessages( const std::vector< std::shared_ptr<CAlarmMessage> >& otherMessages );
			NETWORKING_API virtual std::vector< std::shared_ptr<CAlarmMessage> > GetOtherMessages() const;
		protected:
			NETWORKING_API virtual bool IsEqual( const CAlarmMessage& rhs ) const override;
			NETWORKING_API virtual bool IsSmaller( const CAlarmMessage& rhs ) const override;
		private:
			std::unique_ptr<External::CAlarmMessage> containedMessage;
			std::vector< std::unique_ptr<CAlarmMessage> > otherMessages;
			bool isEmpty;
		};
	}
}
/*@}*/
