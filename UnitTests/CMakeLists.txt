# PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)
#
# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
#( at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.If not, see <http://www.gnu.org/licenses/>

find_package( Boost COMPONENTS
	unit_test_framework
	system
	serialization
	filesystem
	thread
REQUIRED )

set( SOURCE
	fmeDetectionTester.cpp
	RandomFMEParams.cpp
	runner.cpp
	XMLAlarmMessagesDatabaseTest.cpp
	XMLAlarmValiditiesTest.cpp	
	XMLEmailLoginDataTest.cpp	
	XMLEmailMessageTest.cpp
	XMLExternalProgramLoginDataTest.cpp
	XMLExternalProgramMessageTest.cpp
	XMLGatewayLoginDatabaseTest.cpp	
	XMLGroupalarmLoginDataTest.cpp
	XMLGroupalarmMessageTest.cpp
	XMLSettingsParamTest.cpp
	XMLMonthlyValidityTest.cpp	
	XMLSingleTimeValidityTest.cpp
	XMLWeeklyValidityTest.cpp
)

set( HEADERS
	AlarmGatewaysManagerTest.h
	audioFullDownsamplerTest.h
	AlarmMessageDatabaseTest.h	
	audioSignalPreserverTest.h
	audioSignalReaderTest.h
	AlarmValiditiesTest.h	
	basicFunctions.h
	BoostStdTimeConverterTest.h
	ConnectionManagerTest.h
	ConnectionMocks.h
	ConnectionThreadTest.h
	CodeDataTest.h
	dataProcessingTest.h
	DefaultValidityTest.h
	EmailGatewayTest.h
	EmailLoginDataTest.h
	EmailMessageTest.h
	ExternalProgramGatewayTest.h
	ExternalProgramLoginDataTest.h
	ExternalProgramMessageTest.h
	fftTest.h
	FileUtilsTest.h
	filterTest.h
	fmeDetectionTest.h
	fmeDetectionTester.h
	GeneralStatusMessageTest.h
	DateTimeTest.h	
	DetectorStatusMessageTest.h
	SendStatusMessageTest.h
	germanLocalDateTimeTest.h
	GatewayLoginDatabaseTest.h
	GroupalarmGatewayTest.h		
	GroupalarmLoginDataTest.h	
	GroupalarmMessageTest.h
	InfoalarmMessageDecoratorTest.h
	MonthlyValidityTest.h
	OGGHandlerTest.h
	portaudioTest.h
	RandomFMEParams.h	
	SeqDataCompleteTest.h
	SeqDataTest.h
	sequencePasserDebugTest.h
	sequencePasserTest.h
	SerializableCodeDataTest.h
	SerializableDateTimeTest.h	
	SerializableSeqDataCompleteTest.h
	SerializableSeqDataTest.h
	SerializableTimeTest.h
	SettingsParamTest.h
	SingleTimeValidityTest.h
	StatisticalAnalysis.h
	TimeTest.h
	WeeklyValidityTest.h
	XMLAlarmMessagesDatabaseTest.h
	XMLAlarmValiditiesTest.h	
	XMLEmailLoginDataTest.h
	XMLEmailMessageTest.h
	XMLExternalProgramLoginDataTest.h
	XMLExternalProgramMessageTest.h
	XMLGatewayLoginDatabaseTest.h
	XMLGroupalarmLoginDataTest.h
	XMLGroupalarmMessageTest.h
	XMLSettingsParamTest.h
	XMLTests.h
	XMLMonthlyValidityTest.h
	XMLSingleTimeValidityTest.h
	XMLWeeklyValidityTest.h
)

add_executable( Unittests ${SOURCE} ${HEADERS} )
add_test( Unittests Unittests )

target_link_libraries( Unittests PRIVATE 
	Utilities
	Networking
	Core
	Middleware
	
	Boost::unit_test_framework
	Boost::system 
	Boost::serialization 
	Boost::filesystem 
	Boost::thread
)

if ( UNIX )
	add_definitions( -DBOOST_TEST_DYN_LINK ) 
endif()

# copy the required configuration files
add_custom_command( TARGET Unittests
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/fmeProductionParams.dat"
	"${CMAKE_BINARY_DIR}/UnitTests"
)

add_custom_command( TARGET Unittests
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/biasFilterParams.txt"
	"${CMAKE_BINARY_DIR}/UnitTests"
)

add_custom_command( TARGET Unittests
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/signal.txt"
		"${CMAKE_BINARY_DIR}/UnitTests/fmeDetectionTests/signal.txt"
)

add_custom_command( TARGET Unittests
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/shortSignal.txt"
		"${CMAKE_BINARY_DIR}/UnitTests/signal.txt"
)

add_custom_command( TARGET Unittests POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory
       "${CMAKE_CURRENT_SOURCE_DIR}/fmeDetectionTests"
       "${CMAKE_BINARY_DIR}/UnitTests/fmeDetectionTests"
)

file( GLOB ConfigFiles ${CMAKE_SOURCE_DIR}/UnitTests/*.m )
foreach( ConfigFile ${ConfigFiles} )
	add_custom_command(TARGET Unittests POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${ConfigFile} "${CMAKE_BINARY_DIR}/UnitTests"
	)
endforeach()

set( DST "${CMAKE_BINARY_DIR}" )
set( SRC "${PROJECT_SOURCE_DIR}/audioSettings.dat" )
add_custom_command( TARGET Unittests
   COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
)

set( SRC "${PROJECT_SOURCE_DIR}/fmeParams.dat" )
add_custom_command( TARGET Unittests
   COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
)

set( SRC "${PROJECT_SOURCE_DIR}/params.dat" )
add_custom_command( TARGET Unittests
   COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
)
