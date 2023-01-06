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
			NETWORKING_API CGroupalarm2Message();
			NETWORKING_API ~CGroupalarm2Message() {};
			NETWORKING_API CGroupalarm2Message(const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API CGroupalarm2Message(const std::string& alarmTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API CGroupalarm2Message(const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API std::unique_ptr< External::CAlarmMessage > Clone() const override;
			NETWORKING_API virtual void SetEmpty() override;
			NETWORKING_API virtual bool IsEmpty() const override;
			NETWORKING_API virtual void SetAlarmToAllUsers(const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API virtual void SetAlarmTemplate(const std::string& alarmTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API virtual void SetAlarmToDefinedUsers(const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours);
			NETWORKING_API bool ToAllUsers() const;
			NETWORKING_API bool ToAlarmTemplate() const;
			NETWORKING_API bool ToLabels() const;
			NETWORKING_API bool ToUnits() const;
			NETWORKING_API bool ToUsers() const;
			NETWORKING_API bool ToScenarios() const;
			NETWORKING_API std::string GetAlarmTemplate() const;
			NETWORKING_API std::map<std::string, unsigned int> GetLabels() const;
			NETWORKING_API std::vector<std::string> GetUnits() const;
			NETWORKING_API std::vector<std::string> GetUsers() const;
			NETWORKING_API std::vector<std::string> GetScenarios() const;
			NETWORKING_API double GetEventOpenPeriodInHours() const;
			NETWORKING_API unsigned int GetEventOpenPeriodHour() const;
			NETWORKING_API unsigned int GetEventOpenPeriodMinute() const;
			NETWORKING_API unsigned int GetEventOpenPeriodSecond() const;
			NETWORKING_API bool HasMessageText() const;
			NETWORKING_API std::string GetMessageText() const;
			NETWORKING_API std::string GetMessageTemplate() const;
			NETWORKING_API virtual std::type_index GetGatewayType() const override;
		protected:
			NETWORKING_API virtual bool IsEqual(const CAlarmMessage& rhs) const override;
			NETWORKING_API virtual bool IsSmaller(const CAlarmMessage& rhs) const override;
		private:
			bool allUsers;
			std::string alarmTemplate;
			std::map<std::string, unsigned int> labels;
			std::vector<std::string> scenarios;
			std::vector<std::string> units;
			std::vector<std::string> users;
			std::string messageText;
			std::string messageTemplate;
			double eventOpenPeriodInHours;
			bool isEmpty;
		};

		namespace Exception {
			/**	This class provides an exception for Groupalarm.de if a message text and message template are used at the same time
			*/
			class Groupalarm2MessageContentInconsistent : public virtual std::logic_error
			{
			public:
				Groupalarm2MessageContentInconsistent(const char* errorMessage)
					: logic_error(errorMessage)
				{}
			};

			/**	This class provides an exception for Groupalarm.de if the event open period is invalid
*/
			class Groupalarm2EventOpenPeriodOutOfRange : public virtual std::logic_error
			{
			public:
				Groupalarm2EventOpenPeriodOutOfRange(const char* errorMessage)
					: logic_error(errorMessage)
				{}
			};
		}
	}
}
/*@}*/
