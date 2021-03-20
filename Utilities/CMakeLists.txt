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
# along with this program.If not, see <http://www.gnu.org/licenses/>

find_package( Poco COMPONENTS
	Foundation
	Util
	XML
REQUIRED )

find_package( XercesC CONFIG REQUIRED )

find_package( Boost COMPONENTS
	system
	filesystem
	locale
	date_time
REQUIRED )

set( SOURCE
	BoostStdTimeConverter.cpp
	FileUtils.cpp
	german_local_date_time.cpp
	SeqData.cpp
	SerializableSeqData.cpp
	SerializableDateTime.cpp
	SerializableTime.cpp
	DateTime.cpp
	CTime.cpp
	DetectorStatusMessage.cpp	
	GeneralStatusMessage.cpp
	MediaFile.cpp
	ParserErrorHandler.cpp
	StatusMessage.cpp
	StringUtils.cpp
	VersionInfo.cpp
	XercesString.cpp
	XMLUtilities.cpp	
	XMLValidator.cpp
)

set( HEADERS
	AudioPlugin.h
	BoostStdTimeConverter.h
	CodeData.h
	FileUtils.h
	german_local_date_time.h
	MediaFile.h
	SeqData.h
	SeqDataComplete.h
	SerializableCodeData.h
	SerializableSeqData.h
	SerializableSeqDataComplete.h
	SerializableDateTime.h
	SerializableTime.h
	DateTime.h
	CTime.h
	TimeDatatypes.h
	StatusMessage.h
	DetectorStatusMessage.h	
	GeneralStatusMessage.h
	ParserErrorHandler.h
	PluginLoader.h
	SendStatusMessage.h
	StringUtils.h
	VersionInfo.h
	XercesString.h
	XMLException.h
	XMLSerializable.h
	XMLUtilities.h	
	XMLValidator.h
)

add_library( Utilities SHARED ${SOURCE} ${HEADERS} )
target_include_directories( Utilities PUBLIC ${PROJECT_SOURCE_DIR}/Utilities )
	
target_link_libraries( Utilities PRIVATE 
	Boost::system
	Boost::filesystem
	Boost::locale
	XercesC::XercesC
)	
target_link_libraries( Utilities PUBLIC 
	${Poco_LIBRARIES}
	Boost::date_time
)

target_include_directories( Utilities PUBLIC ${Poco_INCLUDE_DIRS} )

install( 
	TARGETS
		Utilities
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/${CMAKE_PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)
