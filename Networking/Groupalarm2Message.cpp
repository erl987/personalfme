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

#include "math.h"
#include "Groupalarm2Gateway.h"
#include "Groupalarm2Message.h"


External::Groupalarm::CGroupalarm2Message::CGroupalarm2Message()
	: CAlarmMessage(true), // Groupalarm.de alarms are only allowed directly after detecting an alarm
	isEmpty(true)
{
}

External::Groupalarm::CGroupalarm2Message::CGroupalarm2Message(const bool& allUsers, const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours)
	: CAlarmMessage(true), // Groupalarm.de alarms are only allowed directly after detecting an alarm
	isEmpty(true)
{
	Set(allUsers, labels, scenarios, units, users, messageText, messageTemplate, eventOpenPeriodInHours);
}

/**	@brief		Cloning method duplicating the object
*	@return									Duplicate base class pointer of the object
*	@exception								None
*	@remarks								NOne
*/
std::unique_ptr< External::CAlarmMessage > External::Groupalarm::CGroupalarm2Message::Clone() const
{
	return std::make_unique<CGroupalarm2Message>(*this);
}

void External::Groupalarm::CGroupalarm2Message::Set(const bool& allUsers, const std::map<std::string, unsigned int>& labels, const std::vector<std::string>& scenarios, const std::vector<std::string>& units, const std::vector<std::string>& users, const std::string& messageText, const std::string& messageTemplate, const double& eventOpenPeriodInHours)
{
	if (allUsers) {
		if (!labels.empty()) {
			throw Exception::Groupalarm2FullAlarmInconsistent("Full alarm but also labels are defined");
		}
		if (!scenarios.empty()) {
			throw Exception::Groupalarm2FullAlarmInconsistent("Full alarm but also scenarios are defined");
		}
		if (!units.empty()) {
			throw Exception::Groupalarm2FullAlarmInconsistent("Full alarm but also units are defined");
		}
		if (!users.empty()) {
			throw Exception::Groupalarm2FullAlarmInconsistent("Full alarm but also users are defined");
		}
	}

	if (!messageText.empty() && !messageTemplate.empty()) {
		throw Exception::Groupalarm2MessageContentInconsistent("Both message text and template are defined");
	}

	if (messageText.empty() && messageTemplate.empty()) {
		throw Exception::Groupalarm2MessageContentInconsistent("Neither message text nor template are defined");
	}

	if (eventOpenPeriodInHours < 0) {
		throw Exception::Groupalarm2EventOpenPeriodOutOfRange("The event open period must not be negative");
	}

	CGroupalarm2Message::allUsers = allUsers;
	CGroupalarm2Message::labels = labels;
	CGroupalarm2Message::scenarios = scenarios;
	CGroupalarm2Message::units = units;
	CGroupalarm2Message::users = users;
	CGroupalarm2Message::messageText = messageText;
	CGroupalarm2Message::messageTemplate = messageTemplate;
	CGroupalarm2Message::eventOpenPeriodInHours = eventOpenPeriodInHours;
	CGroupalarm2Message::isEmpty = false;
}

/**	@brief		Determines if this object and onother CGroupalarm2 dataset are equivalent
*	@param		rhs								Dataset to be compared
*	@return										True if the datasets are equal, false otherwise.
*	@exception									None
*	@remarks									Comparing with any other object than CGroupalarm2Message will return false
*/
bool External::Groupalarm::CGroupalarm2Message::IsEqual(const CAlarmMessage& rhs) const
{
	try {
		const CGroupalarm2Message& derivRhs = dynamic_cast<const CGroupalarm2Message&>(rhs);

		if ((IsEmpty() && !derivRhs.IsEmpty()) || (!IsEmpty() && derivRhs.IsEmpty())) {
			return false;
		}

		// checks for equality of all components
		if (!IsEmpty() && !derivRhs.IsEmpty()) {
			if (allUsers != derivRhs.allUsers) {
				return false;
			}
			if (labels != derivRhs.labels) {
				return false;
			}
			if (scenarios != derivRhs.scenarios) {
				return false;
			}
			if (units != derivRhs.units) {
				return false;
			}
			if (users != derivRhs.users) {
				return false;
			}
			if (messageText != derivRhs.messageText) {
				return false;
			}
			if (messageTemplate != derivRhs.messageTemplate) {
				return false;
			}
			if (eventOpenPeriodInHours != derivRhs.eventOpenPeriodInHours) {
				return false;
			}
		}
	}
	catch (std::bad_cast e) {
		return false;
	}

	return true;
}


/**	@brief		Determines if this object is smaller than another CGroupalarm2 object
*	@param		rhs								Dataset to be compared
*	@return										True if this dataset is smaller, false otherwise
*	@exception									None
*	@remarks									Comparing with any other object than CGroupalarm2Message will return false
*/
bool External::Groupalarm::CGroupalarm2Message::IsSmaller(const CAlarmMessage& rhs) const
{
	try {
		const CGroupalarm2Message& derivRhs = dynamic_cast<const CGroupalarm2Message&>(rhs);

		// check for is-smaller for all components
		if (!IsEmpty() && !derivRhs.IsEmpty()) {
			if (allUsers >= derivRhs.allUsers) {
				return false;
			}
			if (labels >= derivRhs.labels) {
				return false;
			}
			if (scenarios >= derivRhs.scenarios) {
				return false;
			}
			if (units >= derivRhs.units) {
				return false;
			}
			if (users >= derivRhs.users) {
				return false;
			}
			if (messageText >= derivRhs.messageText) {
				return false;
			}
			if (messageTemplate >= derivRhs.messageTemplate) {
				return false;
			}
			if (eventOpenPeriodInHours >= derivRhs.eventOpenPeriodInHours) {
				return false;
			}
		}
		else {
			return false;
		}
	}
	catch (std::bad_cast e) {
		return false;
	}

	return true;
}


/**	@brief		Checking if the dataset is empty
*	@return										True if the dataset is empty, if it contains valid data it is false
*	@exception									None
*	@remarks									An empty dataset is interpreted as the suppression of the alarm weblink call
*/
bool External::Groupalarm::CGroupalarm2Message::IsEmpty(void) const
{
	return isEmpty;
}


/**	@brief		Setting the dataset empty
*	@return										None
*	@exception									None
*	@remarks									An empty dataset is interpreted as the suppression of the alarm weblink call
*/
void External::Groupalarm::CGroupalarm2Message::SetEmpty(void)
{
	isEmpty = true;
}


bool External::Groupalarm::CGroupalarm2Message::ToAllUsers() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return allUsers;
}

bool External::Groupalarm::CGroupalarm2Message::ToLabels() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return !labels.empty();
}

bool External::Groupalarm::CGroupalarm2Message::ToUnits() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return !units.empty();
}

bool External::Groupalarm::CGroupalarm2Message::ToUsers() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return !users.empty();
}

bool External::Groupalarm::CGroupalarm2Message::ToScenarios() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return !scenarios.empty();
}

std::map<std::string, unsigned int> External::Groupalarm::CGroupalarm2Message::GetLabels() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return labels;
}

std::vector<std::string> External::Groupalarm::CGroupalarm2Message::GetUnits() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return units;
}

std::vector<std::string> External::Groupalarm::CGroupalarm2Message::GetUsers() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return users;
}

std::vector<std::string> External::Groupalarm::CGroupalarm2Message::GetScenarios() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return scenarios;
}

double External::Groupalarm::CGroupalarm2Message::GetEventOpenPeriodInHours() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return eventOpenPeriodInHours;
}

unsigned int External::Groupalarm::CGroupalarm2Message::GetEventOpenPeriodHour() const
{
	return static_cast<unsigned int>(floor(eventOpenPeriodInHours));
}

unsigned int External::Groupalarm::CGroupalarm2Message::GetEventOpenPeriodMinute() const
{
	return static_cast<unsigned int>(floor((eventOpenPeriodInHours - GetEventOpenPeriodHour()) * 60));
}

unsigned int External::Groupalarm::CGroupalarm2Message::GetEventOpenPeriodSecond() const
{
	double residualMinutes = (eventOpenPeriodInHours - GetEventOpenPeriodHour()) * 60;
	double residualSeconds = (residualMinutes - GetEventOpenPeriodMinute()) * 60;
	return static_cast<unsigned int>(floor(residualSeconds));
}

bool External::Groupalarm::CGroupalarm2Message::HasMessageText() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return !messageText.empty();
}

std::string External::Groupalarm::CGroupalarm2Message::GetMessageText() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return messageText;
}

std::string External::Groupalarm::CGroupalarm2Message::GetMessageTemplate() const
{
	if (IsEmpty()) {
		throw std::domain_error("The groupalarm.de dataset is empty.");
	}

	return messageTemplate;
}

/**	@brief		Obtains the type index of the gateway class corresponding to the present message class.
*	@return										Type index of the gateway class corresponding to the present message class
*	@exception									None
*	@remarks									None
*/
std::type_index External::Groupalarm::CGroupalarm2Message::GetGatewayType() const
{
	return std::type_index(typeid(CGroupalarm2Gateway));
}