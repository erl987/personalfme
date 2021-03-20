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

#include <boost/algorithm/string/trim.hpp>
#include "XMLSerializableGroupalarmLoginData.h"

const std::string USER_KEY = "user";
const std::string PASSWORD_KEY = "password";
	const std::string PASSWORD_ATTRIB_KEY = "[@type]";
	const std::string PASSWORD_ATTRIB_PLAIN = "plain";
	const std::string PASSWORD_ATTRIB_HASHED = "hashed";

const std::string PROXY_KEY = "proxy";
	const std::string PROXY_SERVER_KEY = "server";
	const std::string PROXY_SERVER_PORT = "port";

const std::string TRIALS_KEY = "trials";
const std::string WAIT_TIME_KEY = "waitTime";
const std::string CONNECTIONS_KEY = "connections";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::Groupalarm::CXMLSerializableGroupalarmLoginData::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	bool isProxy, isHashedPassword;
	unsigned short proxyServerPort;
	unsigned int numTrials, maxNumConnections;
	string userName, password, proxyServerName;
	float timeDistTrial;
	string passwordType;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	userName = boost::algorithm::trim_copy( xmlFile->getString( USER_KEY ) );
	password = boost::algorithm::trim_copy( xmlFile->getString( PASSWORD_KEY ) );
	passwordType = xmlFile->getString( PASSWORD_KEY + PASSWORD_ATTRIB_KEY );
	if ( passwordType == PASSWORD_ATTRIB_PLAIN ) {
		isHashedPassword = false;
	} else if ( passwordType == PASSWORD_ATTRIB_HASHED ) {
		isHashedPassword = true;
	}

	if ( xmlFile->hasProperty( PROXY_KEY ) ) {
		isProxy = true;
		proxyServerName = boost::algorithm::trim_copy( xmlFile->getString( PROXY_KEY + "." + PROXY_SERVER_KEY ) );
		proxyServerPort = static_cast<unsigned short>( xmlFile->getInt( PROXY_KEY + "." + PROXY_SERVER_PORT, 8080 ) ); // default port is possible
	} else {
		isProxy = false;
		proxyServerName = "";
		proxyServerPort = 0;
	}

	numTrials = xmlFile->getUInt( TRIALS_KEY, 10 ); // default value: 10 trials
	timeDistTrial = static_cast<float>( xmlFile->getDouble( WAIT_TIME_KEY, 30.0 ) ); // default value: 30.0 s
	maxNumConnections = xmlFile->getUInt( CONNECTIONS_KEY, 1 ); // default value: 1 parallel connection

	SetServerInformation( userName, isHashedPassword, password, proxyServerName, proxyServerPort );
	SetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::Groupalarm::CXMLSerializableGroupalarmLoginData::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	bool isHashedPassword;
	unsigned short proxyServerPort;
	unsigned int numTrials, maxNumConnections;
	string userName, password, proxyServerName;
	float timeDistTrial;
	string passwordType;

	if ( IsValid() ) {
		GetServerInformation( userName, isHashedPassword, password, proxyServerName, proxyServerPort );
		GetConnectionTrialInfos( numTrials, timeDistTrial, maxNumConnections );

		// write the data to the XML-file
		xmlFile->setString( USER_KEY, userName );
		xmlFile->setString( PASSWORD_KEY, password );
		if ( isHashedPassword ) {
			passwordType = PASSWORD_ATTRIB_HASHED;
		} else {
			passwordType = PASSWORD_ATTRIB_PLAIN;
		}
		xmlFile->setString( PASSWORD_KEY + PASSWORD_ATTRIB_KEY, passwordType );

		if ( !proxyServerName.empty() ) {
			xmlFile->setString( PROXY_KEY + "." + PROXY_SERVER_KEY, proxyServerName );
			xmlFile->setInt( PROXY_KEY + "." + PROXY_SERVER_PORT, proxyServerPort );
		}

		xmlFile->setUInt( TRIALS_KEY, numTrials );
		xmlFile->setDouble( WAIT_TIME_KEY, timeDistTrial );
		xmlFile->setUInt( CONNECTIONS_KEY, maxNumConnections );
	}
}