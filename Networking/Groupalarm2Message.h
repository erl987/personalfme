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
#include <exception>
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
	namespace Groupalarm {
		/**	\ingroup Networking
		*	Class implementing a dataset for the www.Groupalarm.de calling parameters.
		*/
		class CGroupalarm2Message : public CAlarmMessage
		{
		public:
			CGroupalarm2Message();
			NETWORKING_API CGroupalarm2Message(const bool& allUsers, const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const unsigned int& closeEventInHours);
			~CGroupalarm2Message() {};
			NETWORKING_API std::unique_ptr< External::CAlarmMessage > Clone() const override;
			NETWORKING_API virtual void SetEmpty() override;
			NETWORKING_API virtual bool IsEmpty() const override;
			NETWORKING_API virtual void Set(const bool& allUsers, const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const unsigned int& closeEventInHours);
			bool ToAllUsers() const;
			bool ToLabels() const;
			bool ToUnits() const;
			bool ToUsers() const;
			bool ToScenarios() const;
			std::map<std::string, unsigned int> GetLabels() const;
			std::vector<std::string> GetUnits() const;
			std::vector<std::string> GetUsers() const;
			std::vector<std::string> GetScenarios() const;
			unsigned int GetEventActivePeriodInHours() const;
			bool HasMessageText() const;
			std::string GetMessageText() const;
			std::string GetMessageTemplate() const;
			NETWORKING_API virtual std::type_index GetGatewayType() const override;
		protected:
			NETWORKING_API virtual bool IsEqual(const CAlarmMessage& rhs) const override;
			NETWORKING_API virtual bool IsSmaller(const CAlarmMessage& rhs) const override;
		private:
			bool allUsers;
			std::map<std::string, unsigned int> labels;
			std::vector<std::string> scenarios;
			std::vector<std::string> units;
			std::vector<std::string> users;
			std::string messageText;
			std::string messageTemplate;
			unsigned int closeEventInHours;
			bool isEmpty;
		};
	}
}
/*@}*/
