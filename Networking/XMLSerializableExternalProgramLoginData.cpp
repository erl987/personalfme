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
#include "XMLSerializableExternalProgramLoginData.h"

const std::string PROGRAM_SESSIONS_KEY = "maxSessions";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::ExternalProgram::CXMLSerializableExternalProgramLoginData::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	unsigned int maxProgramSessions;

	maxProgramSessions = xmlFile->getUInt( PROGRAM_SESSIONS_KEY, 10 ); // default value: 10 parallel external program calls
	SetConnectionTrialInfos( 1, 0.0f, maxProgramSessions ); // repeated connection trials are not senseful for that gateway
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::ExternalProgram::CXMLSerializableExternalProgramLoginData::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	unsigned int numTrials, maxProgramSessions;
	float timeDistTrial;

	if ( IsValid() ) {
		GetConnectionTrialInfos( numTrials, timeDistTrial, maxProgramSessions );
		xmlFile->setUInt( PROGRAM_SESSIONS_KEY, maxProgramSessions );
	}
}