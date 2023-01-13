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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
#define NETWORKING_API __attribute__ ((dllexport))
	#else
	// Microsoft Visual Studio
#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <boost/algorithm/string/trim.hpp>
#include "XMLSerializableEmailMessage.h"

const std::string ORGANIZATION_KEY = "organization";
const std::string UNIT_KEY = "unit";
const std::string RECIPIENT_KEY = "recipient";
	const std::string RECIPIENT_NAME_KEY = "name";
	const std::string RECIPIENT_ADDRESS_KEY = "address";
const std::string MESSAGE_KEY = "message";
const std::string ATTACH_VOICE_KEY = "attachVoice";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Email::CXMLSerializableEmailMessage::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	bool isNotAttachVoice;
	string organization, unit, message, currRecipientName, currRecipientAddress;
	vector< pair<string, string> > recipients;
	vector<string> rootTags;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	organization = boost::algorithm::trim_copy( xmlFile->getString( ORGANIZATION_KEY ) );
	unit = boost::algorithm::trim_copy( xmlFile->getString( UNIT_KEY ) );

	xmlFile->keys( "", rootTags );
	for ( auto tag : rootTags ) {
		if ( tag.find( RECIPIENT_KEY ) == 0 ) {
			if ( xmlFile->hasProperty( tag + "." + RECIPIENT_NAME_KEY ) ) {
				currRecipientName = boost::algorithm::trim_copy( xmlFile->getString( tag + "." + RECIPIENT_NAME_KEY ) );
			} else {
				currRecipientName.clear();
			}
			currRecipientAddress = boost::algorithm::trim_copy( xmlFile->getString( tag + "." + RECIPIENT_ADDRESS_KEY ) );
			recipients.push_back( make_pair( currRecipientName, currRecipientAddress ) );
		}
	}

	message = boost::algorithm::trim_copy( xmlFile->getString( MESSAGE_KEY ) );
	isNotAttachVoice = !xmlFile->getBool( ATTACH_VOICE_KEY );

	Set( organization, unit, recipients, message );
	SetRequiredState( isNotAttachVoice );
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Email::CXMLSerializableEmailMessage::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	bool isNotAttachVoice;
	string organization, unit, message;
	vector< pair<string, string> > recipients;

	if ( !IsEmpty() ) {
		Get( organization, unit, recipients, message, isNotAttachVoice );

		// write the data to the XML-file
		xmlFile->setString( ORGANIZATION_KEY, organization );
		xmlFile->setString( UNIT_KEY, unit );

		auto i = 0;
		for ( auto recipient : recipients ) {
			if ( !recipient.first.empty() ) {
				xmlFile->setString( RECIPIENT_KEY + "[" + to_string( i ) + "]." + RECIPIENT_NAME_KEY, recipient.first );
			}
			xmlFile->setString( RECIPIENT_KEY + "[" + to_string( i ) + "]." + RECIPIENT_ADDRESS_KEY, recipient.second );
			i++;
		}

		xmlFile->setString( MESSAGE_KEY, message );
		xmlFile->setBool( ATTACH_VOICE_KEY, !isNotAttachVoice );
	}
}