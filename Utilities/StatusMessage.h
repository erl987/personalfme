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
#include <string>
#include <tuple>
#include <boost/date_time/posix_time/posix_time.hpp>

#if defined _WIN32 || defined __CYGWIN__
	#ifdef UTILITY_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define UTILITY_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define UTILITY_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define UTILITY_API __attribute__ ((visibility ("default")))
	#else
		#define UTILITY_API
	#endif		
#endif

/*@{*/
/** \ingroup Utilities
*/


namespace Utilities {
	namespace Message {
		/**	@param MessageType		Type of the message */
		enum MessageType { MESSAGE_SUCCESS, MESSAGE_ERROR };

		/**	\ingroup Utilities
		*	Base class representing a status message emitted by an object
		*/
		class CStatusMessage
		{
		public:
			UTILITY_API CStatusMessage( const MessageType& messageType, const boost::posix_time::ptime& time );
			UTILITY_API virtual ~CStatusMessage();
			UTILITY_API MessageType GetType( void ) const;
			UTILITY_API boost::posix_time::ptime GetTimestamp() const;
		protected:
			UTILITY_API CStatusMessage();
			UTILITY_API void Reset( const MessageType& messageType, const boost::posix_time::ptime& time );
			std::tuple<MessageType, boost::posix_time::ptime> message;
			bool isInit;
		};
	}
}
/*@}*/
