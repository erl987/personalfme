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

#include <boost/algorithm/string/trim.hpp>
#include "XMLSerializableGroupalarm2Message.h"

const std::string TYPE_ATTRIB_KEY = "[@type]";
const std::string RESOURCES_KEY = "resources";
const std::string UNIT_KEY = "unit";
const std::string MESSAGE_TYPE = "message";
const std::string EVENT_OPEN_PERIOD_KEY = "eventOpenPeriodInHours";
const std::string MESSAGE_TEMPLATE_ATTRIB_KEY = "template";
const std::string MESSAGE_FREE_TEXT_ATTRIB_KEY = "freeText";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Groupalarm::CXMLSerializableGroupalarm2Message::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	bool allUsers = false;
	map<string, unsigned int> labels;
	vector<string> scenarios;
	vector<string> units;
	vector<string> users;
	string messageText;
	string messageTemplate;
	unsigned int eventOpenPeriodInHours;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	units = { boost::algorithm::trim_copy(xmlFile->getString(RESOURCES_KEY + "." + UNIT_KEY)) };

	if ( xmlFile->getString( MESSAGE_TYPE + TYPE_ATTRIB_KEY ) == MESSAGE_TEMPLATE_ATTRIB_KEY ) {
		messageTemplate = boost::algorithm::trim_copy(xmlFile->getString(MESSAGE_TYPE));
	} else {
		messageText = boost::algorithm::trim_copy(xmlFile->getString(MESSAGE_TYPE));
	}

	eventOpenPeriodInHours = xmlFile->getUInt(EVENT_OPEN_PERIOD_KEY);

	Set(allUsers, labels, scenarios, units, users, messageText, messageTemplate, eventOpenPeriodInHours);
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Groupalarm::CXMLSerializableGroupalarm2Message::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	if ( !IsEmpty() ) {
		// write the data to the XML-file
		xmlFile->setString(RESOURCES_KEY + "." + UNIT_KEY, GetUnits()[0]);

		if (HasMessageText()) {
			xmlFile->setString(MESSAGE_TYPE, GetMessageText());
			xmlFile->setString(MESSAGE_TYPE + TYPE_ATTRIB_KEY, MESSAGE_FREE_TEXT_ATTRIB_KEY);
		} else {
			xmlFile->setString(MESSAGE_TYPE, GetMessageTemplate());
			xmlFile->setString(MESSAGE_TYPE + TYPE_ATTRIB_KEY, MESSAGE_TEMPLATE_ATTRIB_KEY);
		}

		xmlFile->setUInt(EVENT_OPEN_PERIOD_KEY, GetEventActivePeriodInHours());
	}
}