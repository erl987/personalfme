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
#include "XMLSerializableGroupalarm2LoginData.h"

const std::string ORGANIZATION_ID_KEY = "organizationid";
const std::string API_TOKEN_KEY = "apitoken";

const std::string PROXY_KEY = "proxy";
	const std::string PROXY_ADDRESS_KEY = "address";
	const std::string PROXY_PORT = "port";
	const std::string PROXY_USER_NAME = "username";
	const std::string PROXY_PASSWORD = "password";

const std::string TRIALS_KEY = "trials";
const std::string WAIT_TIME_KEY = "waitTime";
const std::string CONNECTIONS_KEY = "connections";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Groupalarm::CXMLSerializableGroupalarm2LoginData::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	unsigned short proxyPort;
	unsigned int numTrials, maxNumConnections, organizationId;
	string proxyAddress, apiToken, proxyUserName, proxyPassword;
	float timeDistTrial;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	organizationId = xmlFile->getUInt( ORGANIZATION_ID_KEY );
	apiToken = boost::algorithm::trim_copy( xmlFile->getString( API_TOKEN_KEY ) );

	if ( xmlFile->hasProperty( PROXY_KEY ) ) {
		proxyAddress = boost::algorithm::trim_copy( xmlFile->getString( PROXY_KEY + "." + PROXY_ADDRESS_KEY) );
		proxyPort = static_cast<unsigned short>( xmlFile->getInt( PROXY_KEY + "." + PROXY_PORT, 8080 ) ); // default port is possible
		proxyUserName = boost::algorithm::trim_copy(xmlFile->getString(PROXY_KEY + "." + PROXY_USER_NAME, "")); // empty user name is possible
		proxyPassword = boost::algorithm::trim_copy(xmlFile->getString(PROXY_KEY + "." + PROXY_PASSWORD, "")); // empty password is possible
	} else {
		proxyAddress = "";
		proxyPort = 0;
		proxyUserName = "";
		proxyPassword = "";

	}

	numTrials = xmlFile->getUInt( TRIALS_KEY, 10 ); // default value: 10 trials
	timeDistTrial = static_cast<float>( xmlFile->getDouble( WAIT_TIME_KEY, 30.0 ) ); // default value: 30.0 s
	maxNumConnections = xmlFile->getUInt( CONNECTIONS_KEY, 1 ); // default value: 1 parallel connection

	Set(organizationId, apiToken, proxyAddress, proxyPort, proxyUserName, proxyPassword);
	SetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Groupalarm::CXMLSerializableGroupalarm2LoginData::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	unsigned short proxyPort;
	unsigned int numTrials, maxNumConnections, organizationId;
	string proxyAddress, apiToken, proxyUserName, proxyPassword;
	float timeDistTrial;

	if ( IsValid() ) {
		Get(organizationId, apiToken, proxyAddress, proxyPort, proxyUserName, proxyPassword);
		GetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );

		// write the data to the XML-file
		xmlFile->setUInt( ORGANIZATION_ID_KEY, organizationId );
		xmlFile->setString( API_TOKEN_KEY, apiToken );

		if ( !proxyAddress.empty() ) {
			xmlFile->setString( PROXY_KEY + "." + PROXY_ADDRESS_KEY, proxyAddress );
			xmlFile->setInt( PROXY_KEY + "." + PROXY_PORT, proxyPort );
			if (!proxyUserName.empty()) {
				xmlFile->setString(PROXY_KEY + "." + PROXY_USER_NAME, proxyUserName );
			}
			if (!proxyPassword.empty()) {
				xmlFile->setString(PROXY_KEY + "." + PROXY_PASSWORD, proxyPassword);
			}
		}

		xmlFile->setUInt( TRIALS_KEY, numTrials );
		xmlFile->setDouble( WAIT_TIME_KEY, timeDistTrial );
		xmlFile->setUInt( CONNECTIONS_KEY, maxNumConnections );
	}
}