/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)

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

#include <memory>
#include <vector>
#include "GroupalarmGateway.h"
#include "EmailGateway.h"
#include "ExternalProgramGateway.h"
#include "XMLSerializableGroupalarmLoginData.h"
#include "XMLSerializableEmailLoginData.h"
#include "XMLSerializableExternalProgramLoginData.h"
#include "XMLSerializableGatewayLoginDatabase.h"

const std::string EMAIL_KEY = "email";
const std::string GROUPALARM_KEY = "groupalarm";
const std::string EXTERNAL_PROGRAM_KEY = "external";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::CXMLSerializableGatewayLoginDatabase::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;
	using namespace Poco::Util;

	vector<string> availableLogins;
	External::Groupalarm::CXMLSerializableGroupalarmLoginData groupalarmLogin;
	External::Email::CXMLSerializableEmailLoginData emailLogin;
	External::ExternalProgram::CXMLSerializableExternalProgramLoginData externalProgramLoginData;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	xmlFile->keys( "", availableLogins );
	for ( auto login : availableLogins ) {
		Poco::AutoPtr<AbstractConfiguration> gatewayView( xmlFile->createView( login ) );

		if ( login == GROUPALARM_KEY ) {
			groupalarmLogin.SetFromXML( gatewayView );
			Add( make_unique<External::Groupalarm::CGroupalarmGateway>(), make_unique<External::Groupalarm::CGroupalarmLoginData>( groupalarmLogin ) );
		} else if ( login == EMAIL_KEY ) {
			emailLogin.SetFromXML( gatewayView );
			Add( make_unique<External::Email::CEmailGateway>(), make_unique<External::Email::CEmailLoginData>( emailLogin ) );
		} else if ( login == EXTERNAL_PROGRAM_KEY ) {
			externalProgramLoginData.SetFromXML( gatewayView );
			Add( make_unique<External::ExternalProgram::CExternalProgramGateway>(), make_unique<External::ExternalProgram::CExternalProgramLoginData>( externalProgramLoginData ) );
		}
	}
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the database is empty, nothing will be written to the XML-file
*/
void External::CXMLSerializableGatewayLoginDatabase::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;
	using namespace Poco::Util;
	using namespace External::Email;
	using namespace External::Groupalarm;
	using namespace External::ExternalProgram;

	std::vector< External::LoginDatasetType > loginsList;

	// write the data to the XML-file
	loginsList = GetAllEntries();
	for ( auto&& login : loginsList ) {
		if ( login.first == typeid( External::Email::CEmailGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( xmlFile->createView( EMAIL_KEY ) );
			auto emailLoginData = CXMLSerializableEmailLoginData( *dynamic_cast<CEmailLoginData*>( login.second.get() ) );
			emailLoginData.GenerateXML( gatewayView );			
		} else if ( login.first == typeid( External::Groupalarm::CGroupalarmGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( xmlFile->createView( GROUPALARM_KEY ) );
			auto groupalarmLoginData = CXMLSerializableGroupalarmLoginData( *dynamic_cast<CGroupalarmLoginData*>( login.second.get() ) );
			groupalarmLoginData.GenerateXML( gatewayView );
		} else if ( login.first == typeid( External::ExternalProgram::CExternalProgramGateway ) ) {
			Poco::AutoPtr<AbstractConfiguration> gatewayView( xmlFile->createView( EXTERNAL_PROGRAM_KEY ) );
			auto externalProgramLoginData = CXMLSerializableExternalProgramLoginData( *dynamic_cast<CExternalProgramLoginData*>( login.second.get() ) );
			externalProgramLoginData.GenerateXML( gatewayView );
		}
	}
}