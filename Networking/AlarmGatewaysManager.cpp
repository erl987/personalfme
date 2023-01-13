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

#if defined _WIN32
	#include "stdafx.h"
#endif
#include <sstream>
#include "InfoalarmMessageDecorator.h"
#include "AlarmGatewaysManager.h"



/** @brief		Standard constructor
*	@param		statusCallback						Callback function for sending the message status change informations from the message sending gateways. Ensure that this function returns fast not to block the thread.
*	@param		exceptionCallback					Callback function which is called when an exception has occured in the message sending system (and their threads have finished silently)
*	@remarks										None
*/
External::CAlarmGatewaysManager::CAlarmGatewaysManager( std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> statusCallback, std::function<void( const std::exception_ptr& )> exceptionCallback )
	: isLoginDatabaseLoaded( false ),
	  isAlarmMessagesDatabaseLoaded( false ),
	  statusCallback( statusCallback ),
	  exceptionCallback( exceptionCallback )
{
}


/** @brief		Standard destructor
*	@remarks										None
*/
External::CAlarmGatewaysManager::~CAlarmGatewaysManager(void)
{
}



/** @brief		Triggering the respective alarm gateways for a certain FME-sequence
*	@param		code								FME-sequence for which an alarm should be sent. The required parameters are taken from the database.
*	@param		alarmTime							Time of alarm (UTC-time)
*	@param		audioFile							Audio file of the alarm message. If the "state" is directly after detection, the value will be ignored.
*	@param		state								Flag stating if this is a call immediately after the detection (true) or after the recording of the audio sequence (false)
*	@return											None
*	@exception	std::logic_error					Thrown if the given code is not existing in the database
*	@exception	std::range_error					Thrown if there are no message for the given code (at the given time)
*	@exception	std::runtime_error					Thrown if the alarm messages or the login database have not been loaded
*	@remarks										None
*/
void External::CAlarmGatewaysManager::Send(const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const Utilities::CMediaFile& audioFile, const bool& state)
{
	using namespace std;
	using namespace Infoalarm;

	bool isRealAlarm;
	stringstream threadID;
	unique_ptr<CGatewayLoginData> loginData;
	vector< shared_ptr< External::CAlarmMessage > > messageList;

	std::lock_guard<std::mutex> lockDatabase( databaseMutex );
	if ( !isLoginDatabaseLoaded || !isAlarmMessagesDatabaseLoaded ) {
		throw std::runtime_error( "Not all settings required for sending an alarm via a gateway have been set." );
	}

	messageList = alarmMessagesDatabase.Search( code, alarmTime, isRealAlarm ); // the method can throw exceptions (std::logic_error in case of not existing code), it is guaranteed that all alarm datasets are not empty

	if ( messageList.empty() ) {
		throw std::range_error( "There are no messages for this alarm code (at the given time)." );
	}

	for (auto const& message : messageList) {
		if ( message->RequiredState() == state ) {
			auto thisMessage = message->Clone();

			// the infoalarm needs to know about all other messages that are sent out
			if ( typeid( *thisMessage ) == typeid( CInfoalarmMessageDecorator ) ) {
				dynamic_cast<CInfoalarmMessageDecorator&>( *thisMessage ).SetOtherMessages( messageList );
			}

			loginData = loginDatabase.Search( thisMessage->GetGatewayType() ); // the method can throw exceptions (std::runtime_error if the gateway type has not been loaded in the database)

			// send the message asynchronously
			connectionManagers.at( thisMessage->GetGatewayType() )->AddMessage( code, alarmTime, isRealAlarm, move( thisMessage ), audioFile );
		}
	}
}



/** @brief		Resetting the database storing the gateway login informations
*	@param		newDatabase					New database storing the gateway login informations
*	@return									None
*	@exception								None
*	@remarks								None
*/
void External::CAlarmGatewaysManager::ResetGatewayLoginDatabase(const CGatewayLoginDatabase& newDatabase)
{
	using namespace std;

	vector<GatewayDatasetType> requiredGateways;

	std::lock_guard<std::mutex> lockDatabase( databaseMutex );
	loginDatabase = newDatabase;	
	isLoginDatabaseLoaded = true;

	connectionManagers.clear();
	requiredGateways = newDatabase.GetAllGateways();
	for ( auto& gateway : requiredGateways ) {
		auto type = gateway.first;
		auto login = loginDatabase.Search( type )->Clone();
		connectionManagers[ type ] = make_unique<CConnectionManager>( move( gateway.second ), move( login ), statusCallback, exceptionCallback );
	}
}



/** @brief		Returning the current database for the gateway login informations
*	@param		database					Current database storing the gateway login informations
*	@return									None
*	@exception	std::logic_error			Thrown if no login database has been loaded
*	@remarks								None
*/
void External::CAlarmGatewaysManager::GetGatewayLoginDatabase(CGatewayLoginDatabase& database)
{
	std::lock_guard<std::mutex> lockDatabase( databaseMutex );

	// check if a database was loaded
	if ( !isLoginDatabaseLoaded ) {
		throw std::logic_error( "Login database has not been loaded." );
	}

	database = loginDatabase;
}



/** @brief		Resetting the database storing the alarm message parameters for the FME-codes
*	@param		newDatabase					New database storing the codes and parameters
*	@return									None
*	@exception								None
*	@remarks								None
*/
void External::CAlarmGatewaysManager::ResetAlarmMessagesDatabase(const CAlarmMessageDatabase& newDatabase)
{
	std::lock_guard<std::mutex> lockDatabase( databaseMutex );
	alarmMessagesDatabase = newDatabase;	
	isAlarmMessagesDatabaseLoaded = true;
}



/** @brief		Returning the current database for the alarm message parameters
*	@param		database					Current database storing codes and parameters
*	@return									None
*	@exception	std::logic_error			Thrown if no alarm message database has been loaded
*	@remarks								None
*/
void External::CAlarmGatewaysManager::GetAlarmMessagesDatabase(CAlarmMessageDatabase& database)
{
	std::lock_guard<std::mutex> lockDatabase( databaseMutex );

	// check if a database was loaded
	if ( !isAlarmMessagesDatabaseLoaded ) {
		throw std::logic_error( "Alarm messages database has not been loaded." );
	}

	database = alarmMessagesDatabase;
}
