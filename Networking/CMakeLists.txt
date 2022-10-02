# PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)
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
find_package( Poco COMPONENTS
	Foundation
	Net
	NetSSL
	Util
	JSON  # only required on Windows
	XML
REQUIRED )

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
	GroupalarmGateway.cpp
	GroupalarmLoginData.cpp	
	GroupalarmMessage.cpp
	InfoalarmMessageDecorator.cpp
	MonthlyValidity.cpp
	SimpleHTTPClient.cpp
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
	XMLSerializableGroupalarmLoginData.cpp
	XMLSerializableGroupalarmMessage.cpp
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
	GroupalarmGateway.h
	GroupalarmLoginData.h	
	GroupalarmMessage.h
	InfoalarmMessageDecorator.h
	MonthlyValidity.h
	SimpleHTTPClient.h
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
	XMLSerializableGroupalarmLoginData.h
	XMLSerializableGroupalarmMessage.h
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
	${Poco_LIBRARIES}
)
target_link_libraries( Networking PUBLIC Utilities )

target_include_directories( Networking PRIVATE ${Poco_INCLUDE_DIRS} )

# copy the required external third-party library files to the binary directory (only required for Windows)
if( WIN32 )
	if( Poco_BINS_DEBUG AND Poco_BINS_RELEASE )
		list( LENGTH Poco_BINS_DEBUG len1 )
		math( EXPR len2 "${len1} - 1" )
		foreach( val RANGE ${len2} )
			list( GET Poco_BINS_DEBUG ${val} Poco_BIN_DEBUG )
			list( GET Poco_BINS_RELEASE ${val} Poco_BIN_RELEASE )
			
			add_custom_command( TARGET Networking POST_BUILD
			   COMMAND ${CMAKE_COMMAND} -E copy
					$<$<CONFIG:debug>:${Poco_BIN_DEBUG}>$<$<CONFIG:release>:${Poco_BIN_RELEASE}>
				   $<TARGET_FILE_DIR:Networking>
			)
		endforeach()
	endif()
endif()

# installer settings
install( TARGETS Networking
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/${CMAKE_PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)

if( WIN32 )
	# DLL installation is only required for Windows
	install(
		FILES 
			${Poco_BINS_RELEASE}
		DESTINATION bin
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)
endif()
