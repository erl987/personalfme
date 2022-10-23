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
#include "XMLSerializableExternalProgramMessage.h"

const std::string COMMAND_KEY = "command";
const std::string PROGRAM_ARGUMENTS_KEY = "arguments";


/** @brief		Unmarshalling the class object from the XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								The object is reset. It is assumed that the XML-file is well-formed and valid. Validate it before calling this method using the Utilities::XML::CXMLValidator class.
*/
void External::ExternalProgram::CXMLSerializableExternalProgramMessage::SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile )
{
	using namespace std;

	string command, programArguments;

	// read the data from the XML-file (it is assumed that the XML-file is well-formed and valid)
	command = boost::algorithm::trim_copy( xmlFile->getString( COMMAND_KEY ) );
	programArguments = boost::algorithm::trim_copy( xmlFile->getString( PROGRAM_ARGUMENTS_KEY ) );

	Set( command, programArguments );
}


/** @brief		Marshalling the present object data to an XML-file
*	@param		xmlFile						Pointer to the Poco AbstractConfiguration typically representing a XML-file
*	@return									None
*	@exception								None
*	@remarks								If the dataset is empty, nothing will be written to the XML-file
*/
void External::ExternalProgram::CXMLSerializableExternalProgramMessage::GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const
{
	using namespace std;

	string command, programArguments;
 
	if ( !IsEmpty() ) {
		command = GetCommand();
		programArguments = GetProgramArguments();

		xmlFile->setString( COMMAND_KEY, command );
		xmlFile->setString( PROGRAM_ARGUMENTS_KEY, programArguments );
	}
}