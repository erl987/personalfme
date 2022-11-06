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
# along with this program.If not, see <http://www.gnu.org/licenses/>

find_package( Boost COMPONENTS 
	system
	filesystem
REQUIRED )

if ( UNIX AND NOT APPLE )
	find_package( Curses REQUIRED )
endif()

if( UNIX AND NOT APPLE )
	# set the RPATH for the private libraries of the project (but only for installation)
	set( CMAKE_SKIP_BUILD_RPATH TRUE )
	set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE ) 
	set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_PROJECT_NAME}" )
endif()

set( SOURCE
	BasicFunctionality.cpp
	main.cpp
	sync_cout.cpp
	Logger.cpp
)

set( HEADERS
	BasicFunctionality.h
	Logger.h
	sync_cout.h
)

add_executable( ${PROJECT_NAME} ${SOURCE} ${HEADERS} )

target_link_libraries( ${PROJECT_NAME} PRIVATE 
	Middleware
	Boost::system 
	Boost::filesystem
)
if( UNIX AND NOT APPLE )
	target_link_libraries( ${PROJECT_NAME} PRIVATE ncurses )
endif()

# installer settings
install( TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)

if( WIN32 )
	install( FILES "${PROJECT_SOURCE_DIR}/Console/${PROJECT_NAME}.bat"
		DESTINATION bin
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)

	install( FILES "${PROJECT_SOURCE_DIR}/COPYRIGHT.txt"
		DESTINATION bin
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)
endif()
