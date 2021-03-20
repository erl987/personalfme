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

#include <string>
#include <future>
#include <sstream>
#include <system_error>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "german_local_date_time.h"
#include "BoostStdTimeConverter.h"
#include "ExternalProgramMessage.h"
#include "InfoalarmMessageDecorator.h"
#include "ExternalProgramGateway.h"



/** @brief		Standard constructor.
*	@remarks								None
*/
External::ExternalProgram::CExternalProgramGateway::CExternalProgramGateway(void)
{
}


/**	@brief		Clone method
*	@return										Copy of the present instance
*	@exception									None
*	@remarks									None
*/
std::unique_ptr<External::CAlarmGateway> External::ExternalProgram::CExternalProgramGateway::Clone() const
{
	return std::make_unique<CExternalProgramGateway>();
}


/** @brief		Performs the sending of the alarm via the gateway
*	@param		code						Container with the alarm code
*	@param		alarmTime					Time of the alarm (UTC)
*	@param		isRealAlarm					Flag stating if the current alarm is a real (default) or test alarm
*	@param		loginData					"Login data"
*	@param		message						Data for the alarm message
*	@param		audioFile					Audio file of the alarm message. It will be ignored by this method.
*	@return									None
*	@exception	std::runtime_error			Thrown if the external program execution failed
*	@remarks								None
*/
void External::ExternalProgram::CExternalProgramGateway::Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile )
{
	using namespace std;
	using namespace boost::algorithm;
	using namespace Infoalarm;
	using namespace Utilities::Time;

	int returnCode;
	string command, programParams, alarmTimeStr, alarmTypeStr;
	stringstream codeStream,alarmTimeStream;
	boost::posix_time::ptime alarmTimeBoost;
	CExternalProgramMessage externalProgramMessage;
	unique_ptr<CAlarmMessage> containedMessage;

	if ( typeid( *message ) == typeid( CInfoalarmMessageDecorator ) ) {
		auto infoalarmMessage = dynamic_cast<const CInfoalarmMessageDecorator&>( *message );
		containedMessage = infoalarmMessage.GetContainedMessage();
		externalProgramMessage = dynamic_cast<const CExternalProgramMessage&>( *containedMessage );
	} else {
		externalProgramMessage = dynamic_cast<const CExternalProgramMessage&>( *message );
	}

	command = externalProgramMessage.GetCommand();
	programParams = externalProgramMessage.GetProgramArguments();

	// replace any placeholders in the program arguments
	for ( auto digit : code ) {
		codeStream << digit;
	}
	alarmTimeBoost = CBoostStdTimeConverter::ConvertToBoostTime( alarmTime );
	alarmTimeStream << german_local_date_time( alarmTimeBoost );
	if ( isRealAlarm ) {
		alarmTypeStr = "Einsatzalarmierung";
	} else {
		alarmTypeStr = "Probealarm";
	}

	replace_all( programParams, "$CODE", codeStream.str() );
	replace_all( programParams, "$TIME", alarmTimeStream.str() );
	replace_all( programParams, "$TYPE", alarmTypeStr );

	// calling the external program (the stdin / stderr streams are not handled)
	returnCode = system( ( command + " " + programParams ).c_str() );

	if ( returnCode != 0 ) {
		try {
			throw std::system_error( returnCode, system_category() );
		} catch ( std::system_error& e ) {
			throw std::runtime_error( "External program error (" + command + "). Error code: " + to_string( e.code().value() ) + " - " + e.what() );
		}
	}
}



/**	@brief		Equality comparison method
*	@param		rhs								Object to be compared with the present object
*	@return										True if the objects are equal, false otherwise
*	@exception									None
*	@remarks									This method is required for the equality and unequality operators
*/
bool External::ExternalProgram::CExternalProgramGateway::IsEqual( const CAlarmGateway& rhs ) const
{
	try {
		dynamic_cast< const CExternalProgramGateway& >( rhs );
	} catch ( std::bad_cast e ) {
		return false;
	}

	return true;
}
