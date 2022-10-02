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

find_package( Boost COMPONENTS 
	date_time
	filesystem
REQUIRED )

set( SOURCE
	Dir.cpp
	ExecutionDetectorRuntime.cpp
	ExecutionRuntime.cpp
	SettingsParam.cpp
	XMLSerializableSettingsParam.cpp
)

set( HEADERS
	Dir.h
	ExecutionDetectorRuntime.h
	ExecutionRuntime.h
	AudioSettings.h
	FileSettings.h
	SettingsParam.h
	XMLSerializableSettingsParam.h
)

add_library( Middleware SHARED ${SOURCE} ${HEADERS} )
target_include_directories( Middleware PUBLIC ${PROJECT_SOURCE_DIR}/Middleware )

target_link_libraries( Middleware PRIVATE 
	Utilities
	
	Boost::date_time
	Boost::filesystem

	Poco::Foundation
	Poco::Util
	Poco::XML
)
target_link_libraries( Middleware PUBLIC
	Core
	Networking
)

target_include_directories( Middleware PRIVATE ${Poco_INCLUDE_DIRS} )

# installer settings
install( TARGETS Middleware
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/${CMAKE_PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)
