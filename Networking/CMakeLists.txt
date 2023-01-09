# PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)
#
# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
#along with this program.If not, see <http://www.gnu.org/licenses/>

# on Windows the SChannel-library is used for SSL, otherwise OpenSSL (which is a system library on Linux)
if( WIN32 )
	find_package( Poco COMPONENTS
		Foundation
		Net
		NetSSLWin
		Util
		JSON
		XML
	REQUIRED )
else()
	find_package( Poco COMPONENTS
		Foundation
		Net
		NetSSL
		Util
		JSON
		XML
	REQUIRED )
	find_package( OpenSSL REQUIRED )
endif()

find_package( Boost COMPONENTS
	filesystem
	date_time
	thread
	serialization
	locale
REQUIRED )

set( SOURCE
	AlarmGateway.cpp
	AlarmGatewaysManager.cpp
	AlarmMessage.cpp	
	AlarmMessagesDatabase.cpp	
	AlarmValidities.cpp	
	ConnectionManager.cpp
	ConnectionThread.cpp
	DefaultValidity.cpp
	EmailGateway.cpp
	EmailGatewayImpl.cpp
	EmailLoginData.cpp		
	EmailMessage.cpp
	ExternalProgramGateway.cpp
	ExternalProgramLoginData.cpp
	ExternalProgramMessage.cpp
	FMEClient.cpp
	FMEClientDebug.cpp
	FMEServer.cpp
	FMEServerDebug.cpp
	GatewayLoginData.cpp
	GatewayLoginDatabase.cpp
	Groupalarm2Gateway.cpp
	Groupalarm2GatewayImpl.cpp
	Groupalarm2LoginData.cpp	
	Groupalarm2Message.cpp
	InfoalarmMessageDecorator.cpp
	MonthlyValidity.cpp
	SingleTimeValidity.cpp
	Validity.cpp
	WeeklyValidity.cpp
	XMLSerializableAlarmMessagesDatabase.cpp	
	XMLSerializableAlarmValidities.cpp	
	XMLSerializableEmailLoginData.cpp	
	XMLSerializableEmailMessage.cpp
	XMLSerializableExternalProgramLoginData.cpp
	XMLSerializableExternalProgramMessage.cpp
	XMLSerializableGatewayLoginDatabase.cpp
	XMLSerializableGroupalarm2LoginData.cpp
	XMLSerializableGroupalarm2Message.cpp
	XMLSerializableMonthlyValidity.cpp
	XMLSerializableSingleTimeValidity.cpp
	XMLSerializableWeeklyValidity.cpp
)

set( HEADERS
	AlarmGateway.h
	AlarmGatewaysManager.h	
	AlarmMessage.h
	AlarmMessagesDatabase.h	
	AlarmValidities.h
	ConnectionManager.h
	ConnectionThread.h
	DefaultValidity.h
	EmailGateway.h
	EmailGatewayImpl.h
	EmailLoginData.h	
	EmailMessage.h
	ExternalProgramGateway.h
	ExternalProgramLoginData.h
	ExternalProgramMessage.h
	FMEClient.h
	FMEClientDebug.h
	FMEClientPrivImplDebug.h
	FMEClientPrivImplementation.h
	FMEServer.h
	FMEServerDebug.h
	FMEServerPrivImplDebug.h
	FMEServerPrivImplementation.h
	FMESession.h
	FMESessionDebug.h
	FMESessionManager.h
	FMESessionManagerDebug.h
	GatewayLoginData.h
	GatewayLoginDatabase.h
	Groupalarm2Gateway.h
	Groupalarm2GatewayImpl.h	
	Groupalarm2LoginData.h
	Groupalarm2Message.h
	InfoalarmMessageDecorator.h
	MonthlyValidity.h
	SingleTimeValidity.h
	Validity.h
	WeeklyValidity.h
	XMLSerializableAlarmMessagesDatabase.h
	XMLSerializableAlarmValidities.h	
	XMLSerializableEmailLoginData.h	
	XMLSerializableEmailMessage.h
    XMLSerializableExternalProgramLoginData.h
	XMLSerializableExternalProgramMessage.h
	XMLSerializableGatewayLoginDatabase.h
	XMLSerializableGroupalarm2LoginData.h
	XMLSerializableGroupalarm2Message.h
	XMLSerializableMonthlyValidity.h
	XMLSerializableSingleTimeValidity.h
	XMLSerializableWeeklyValidity.h
)

add_library( Networking SHARED ${SOURCE} ${HEADERS} )
target_include_directories( Networking PUBLIC ${PROJECT_SOURCE_DIR}/Networking )

target_link_libraries( Networking PRIVATE 
	Boost::filesystem 
	Boost::date_time 
	Boost::thread 
	Boost::serialization 
	Boost::locale

	Poco::Foundation
	Poco::Net
	Poco::Util
	Poco::JSON
	Poco::XML
)

if( WIN32 )
	target_link_libraries( Networking PRIVATE Poco::NetSSLWin )
else()
	target_link_libraries( Networking PRIVATE Poco::NetSSL )
endif()

target_link_libraries( Networking PUBLIC Utilities )

if ( WIN32 )
	set_target_properties( Networking PROPERTIES LINK_FLAGS "/ignore:4217")
endif()


# installer settings
install( TARGETS Networking
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/${CMAKE_PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)
