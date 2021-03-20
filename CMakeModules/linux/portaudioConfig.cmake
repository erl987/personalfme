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

# Targets exported by the module:
# portaudio				- The library

# Variables set by the module:
# portaudio_FOUND			- TRUE if all components are found
# Portaudio_INCLUDE_DIR			- Location of the include directory
# Portaudio_LIBRARY			- Library (debug and release library for linking)
# Portaudio_LIBRARY_DEBUG		- Debug library (for linking)
# Portaudio_LIBRARY_RELEASE		- Release library (for linking)
# Portaudio_BIN_DEBUG			- Debug binary (DLL on Windows)
# Portaudio_BIN_RELEASE			- Release binary (DLL on Windows)
# Portaudio_LIC_FILE			- License file
#
# Example Usage:
#	find_package( portaudio REQUIRED )
#	target_link_libraries( MyApp PRIVATE portaudio )
#
# Options to locate the Portaudio-library with this module: 
# 1. put the libary in the directory "../libraries/portaudio"
# 2. define the Portaudio-library root path in the evironment variable "PORTAUDIO_ROOT"
# 3. specify the path of Portaudio_DIR directly in CMake (the others paths are chosen automatically)
#
# Under linux the library should be usually automatically found if installed properly.

find_package(PkgConfig)

find_path(
	Portaudio_DIR
	NAMES portaudio.h
	HINTS
		ENV PORTAUDIO_ROOT
		/usr/include
		/usr/local/include
		${PROJECT_SOURCE_DIR}/libraries/portaudio/include
)

set( Portaudio_INCLUDE_DIR ${Portaudio_DIR} )

# find the libraries
find_library(
	Portaudio_LIBRARY_DEBUG
	NAMES portaudio_x86 portaudio
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib		
		/usr/local/lib
		${Portaudio_DIR}/../build/Debug
)

find_library(
	Portaudio_LIBRARY_RELEASE
	NAMES portaudio_x86 portaudio
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Portaudio_DIR}/../build/Release
)

set( Portaudio_LIBRARY $<$<CONFIG:Debug>:${Portaudio_LIBRARY_DEBUG}> $<$<CONFIG:Release>:${Portaudio_LIBRARY_RELEASE}> )

get_filename_component(
	Portaudio_LIBRARY_DEBUG_DIR
	${Portaudio_LIBRARY_DEBUG}
	PATH
)

get_filename_component(
	Portaudio_LIBRARY_RELEASE_DIR
	${Portaudio_LIBRARY_RELEASE}
	PATH
)

set( Portaudio_LIBRARY_DIR ${Portaudio_LIBRARY_DEBUG_DIR} ${Portaudio_LIBRARY_RELEASE_DIR} )

if( WIN32 )
	# find the corresponding binaries
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
	find_library(
		Portaudio_BIN_DEBUG
		NAMES portaudio_x86
		PATHS
			${Portaudio_DIR}/../build/Debug
		NO_CMAKE_SYSTEM_PATH
	)

	find_library(
		Portaudio_BIN_RELEASE
		NAMES portaudio_x86
		PATHS
			${Portaudio_DIR}/../build/Release
		NO_CMAKE_SYSTEM_PATH			
	)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
else()
	set( Portaudio_BIN_DEBUG ${Portaudio_LIBRARY_DEBUG} )
	set( Portaudio_BIN_RELEASE ${Portaudio_LIBRARY_RELEASE_DIR} )
endif()

set( Portaudio_BIN $<$<CONFIG:Debug>:${Portaudio_BIN_DEBUG}> $<$<CONFIG:Release>:${Portaudio_BIN_RELEASE}> )

# license files
set( Portaudio_LIC_FILE ${Portaudio_DIR}/../LICENSE.txt )		

# check that all files and directories were properly found
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	portaudio DEFAULT_MSG
	Portaudio_INCLUDE_DIR
	Portaudio_LIBRARY
	Portaudio_LIBRARY_DEBUG
	Portaudio_LIBRARY_RELEASE
	Portaudio_BIN_DEBUG
	Portaudio_BIN_RELEASE
	Portaudio_LIC_FILE
)

if ( portaudio_FOUND AND NOT TARGET portaudio )
	add_library( portaudio UNKNOWN IMPORTED )
	set_target_properties( portaudio PROPERTIES
		IMPORTED_LOCATION_DEBUG "${Portaudio_LIBRARY_DEBUG}"
		IMPORTED_LOCATION_RELEASE "${Portaudio_LIBRARY_RELEASE}"
		MAP_IMPORTED_CONFIG_MINSIZEREL Release
		MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
		INTERFACE_INCLUDE_DIRECTORIES "${Portaudio_INCLUDE_DIR}"
	)
endif()


