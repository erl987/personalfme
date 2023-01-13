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

#include <string>
#include <vector>
#include <map>
#include <tuple>
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
		*	Class implementing an e-mail message.
		*/
		class CEmailMessage : public CAlarmMessage
		{
		public:
			NETWORKING_API CEmailMessage(void);
			NETWORKING_API CEmailMessage(const std::string& siteID, const std::string& alarmID, const std::vector< std::pair<std::string, std::string> >& recipients, const std::string& alarmText, const bool& requiredState);
			NETWORKING_API virtual ~CEmailMessage(void) {};
			NETWORKING_API void Set(const std::string& siteID, const std::string& alarmID, const std::vector< std::pair<std::string, std::string> >& recipients, const std::string& alarmText);
			NETWORKING_API void SetIDs(const std::string& siteID, const std::string& alarmID);
			NETWORKING_API virtual void SetEmpty(void) override;
			NETWORKING_API void Get(std::string& siteID, std::string& alarmID, std::vector< std::pair<std::string, std::string> >& recipients, std::string& alarmText, bool& requiredState) const;
			NETWORKING_API virtual bool IsEmpty(void) const override;	
			NETWORKING_API virtual std::unique_ptr< CAlarmMessage > Clone() const override;
			NETWORKING_API virtual std::type_index GetGatewayType() const override;
		protected:
			NETWORKING_API virtual bool IsEqual(const CAlarmMessage& rhs) const override;
			NETWORKING_API virtual bool IsSmaller(const CAlarmMessage& rhs) const override;
		private:
			std::tuple< std::string, std::string, std::vector< std::pair<std::string, std::string> >, std::string > data;
			bool isEmpty;
		};
	}
}
/*@}*/
