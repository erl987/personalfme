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

# Targets exported by the module:
# Xerces::xerces		- The library

# Variables set by the module:
# Xerces_FOUND			- TRUE if all components are found
# Xerces_INCLUDE_DIR		- Location of the include directory
# Xerces_LIBRARY_DEBUG		- Debug library (for linking)
# Xerces_LIBRARY_RELEASE	- Release library (for linking)
# Xerces_LIBRARY		- Library (debug and release library for linking)
# Xerces_BIN_DEBUG		- Debug binary (DLL on Windows)
# Xerces_BIN_RELEASE		- Release binary (DLL on Windows)
# Xerces_LIC_FILE		- License file
# Xerces_NOTICE_FILE		- Notice file
#
# Example Usage:
#	find_package( XercesC )
#	target_link_libraries( MyApp PRIVATE XercesC::XercesC )
#
# Options to locate the Xerces-library with this module: 
# 1. put the libary in the directory "../libraries/xerces"
# 2. define the Xerces-library root path in the evironment variable "XERCES_ROOT"
# 3. specify the path of Xerces_DIR directly in CMake (the others paths are chosen automatically)
#
# Under linux the library should be usually automatically found if installed properly.

include( determine_compiler )
determine_compiler( COMPILER_ID )

find_path(
	Xerces_DIR
	NAMES xercesc/util/XercesVersion.hpp
	HINTS
		/usr/include
		/usr/local/include		
		ENV XERCES_ROOT
		${PROJECT_SOURCE_DIR}/libraries/xerces/src
)

set( Xerces_INCLUDE_DIR ${Xerces_DIR} )

# find the libraries
find_library(
	Xerces_LIBRARY_DEBUG
	NAMES xerces-c_3D xerces-c
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Xerces_DIR}/../Build/Win32/${COMPILER_ID}/Debug
)

find_library(
	Xerces_LIBRARY_RELEASE
	NAMES xerces-c_3 xerces-c
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Xerces_DIR}/../Build/Win32/${COMPILER_ID}/Release
)

set( Xerces_LIBRARY debug ${Xerces_LIBRARY_DEBUG} optimized ${Xerces_LIBRARY_RELEASE} )

get_filename_component(
	Xerces_LIBRARY_DEBUG_DIR
	${Xerces_LIBRARY_DEBUG}
	PATH
)

get_filename_component(
	Xerces_LIBRARY_RELEASE_DIR
	${Xerces_LIBRARY_RELEASE}
	PATH
)

set( Xerces_LIBRARY_DIR ${Xerces_LIBRARY_DEBUG_DIR} ${Xerces_LIBRARY_RELEASE_DIR} )

# find the corresponding binaries
if( WIN32 )
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
	find_library(
		Xerces_BIN_DEBUG
		NAMES xerces-c_3_1D
		PATHS
			${Xerces_DIR}/../Build/Win32/${COMPILER_ID}/Debug
		NO_CMAKE_SYSTEM_PATH
	)

	find_library(
		Xerces_BIN_RELEASE
		NAMES xerces-c_3_1
		PATHS
			${Xerces_DIR}/../Build/Win32/${COMPILER_ID}/Release
		NO_CMAKE_SYSTEM_PATH
	)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
else()
	# on linux the binaries are not required explicitly
	set( Xerces_BIN_DEBUG "not required" )
	set( Xerces_BIN_RELEASE "not required" )
endif()
	
# license files
set( Xerces_LIC_FILE ${Xerces_DIR}/../LICENSE )
set( Xerces_NOTICE_FILE ${Xerces_DIR}/../NOTICE )

# check that all files and directories were properly found
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	XercesC
	DEFAULT_MSG
	Xerces_INCLUDE_DIR
	Xerces_LIBRARY_DEBUG
	Xerces_LIBRARY_RELEASE
	Xerces_LIBRARY
	Xerces_BIN_DEBUG
	Xerces_BIN_RELEASE
	Xerces_LIC_FILE
	Xerces_NOTICE_FILE
)

if ( XercesC_FOUND AND NOT TARGET XercesC::XercesC )
	add_library( XercesC::XercesC UNKNOWN IMPORTED )
	set_target_properties( XercesC::XercesC PROPERTIES
			IMPORTED_LOCATION_DEBUG "${Xerces_LIBRARY_DEBUG}"
			IMPORTED_LOCATION_RELEASE "${Xerces_LIBRARY_RELEASE}"
			MAP_IMPORTED_CONFIG_MINSIZEREL Release
			MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
			INTERFACE_INCLUDE_DIRECTORIES "${Xerces_INCLUDE_DIR}"
		)
endif()

