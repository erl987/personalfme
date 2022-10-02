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

# Variables set by the module:
# Poco_FOUND					- TRUE if all components are found
# Poco_INCLUDE_DIRS				- Locations of the include directories
# Poco_LIBRARY_DIRS				- Locations of the library directories
# Poco_LIBRARIES				- Poco (needed for linking)
# Poco_BINS_DEBUG				- Debug binaries (DLLs on Windows)
# Poco_BINS_RELEASE				- Release binaries (DLLs on Windows)
#
# Example Usage:
#	FIND_PACKAGE( Poco )
#
# Options to locate the Poco-library with this module: 
# 1. define the Poco-library root path in the evironment variable "POCO_ROOT"
# 2. specify the path of Poco_DIR directly in CMake (the others paths are chosen automatically)

find_package( PkgConfig )

include( determine_compiler )
determine_compiler( COMPILER_ID )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	find_path(
		Poco_DIR
		NAMES ${COMPILER_ID}/buildwin.cmd
		HINTS
			ENV POCO_ROOT
	)
	set( Poco_ROOT_DIR ${Poco_DIR}/${COMPILER_ID} )
else()
	find_path(
		Poco_DIR
		NAMES Poco/Poco.h
		HINTS
			/usr/local/include
			/usr/include
	)
	set( Poco_ROOT_DIR ${Poco_DIR} )
endif()
 
# find all libraries, store debug and release separately
foreach(lib ${Poco_FIND_COMPONENTS} )
	# replace all underscore characters from the SSL-library names (required due to a inconsistency of Poco)
	string( REPLACE "_" "" Poco_${lib}_FILE_NAME Poco${lib} )
	string( REPLACE "OpenSSL" "" Poco_${lib}_FILE_NAME ${Poco_${lib}_FILE_NAME} )
	
	# find the library
	find_library(
		Poco_${lib}_LIBRARY_RELEASE
		NAMES ${Poco_${lib}_FILE_NAME} ${Poco_${lib}FILE_NAME}
		PATHS 
			${Poco_ROOT_DIR}/build/lib/Release
			/usr/lib/x86_64-linux-gnu
			/usr/local/lib
			/usr/lib
	)
	find_library(
		Poco_${lib}_LIBRARY_DEBUG
		NAMES ${Poco_${lib}_FILE_NAME}d ${Poco_${lib}FILE_NAME}d
		PATHS 
			${Poco_ROOT_DIR}/build/lib/Debug
			/usr/lib/x86_64-linux-gnu
			/usr/local/lib
			/usr/lib
    	)

	if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		# find the binary
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
		get_filename_component(RELEASE_LIB_DIR ${Poco_${lib}_LIBRARY_RELEASE} PATH)
		find_library(
			Poco_${lib}_BIN_RELEASE
			NAMES ${Poco_${lib}_FILE_NAME}
			PATHS
				${RELEASE_LIB_DIR}/../bin  # for vcpkg-dependencies
				${Poco_ROOT_DIR}/build/bin/Release
			NO_CMAKE_SYSTEM_PATH
		)
		
		get_filename_component(DEBUG_LIB_DIR ${Poco_${lib}_LIBRARY_DEBUG} PATH)
		find_library(
			Poco_${lib}_BIN_DEBUG
			NAMES ${Poco_${lib}_FILE_NAME}d
			PATHS
				${DEBUG_LIB_DIR}/../bin  # for vcpkg-dependencies
				${Poco_ROOT_DIR}/build/bin/Debug
			NO_CMAKE_SYSTEM_PATH
		)	
	
		# find the OpenSSL binaries if they are linked
		if ( ${lib} STREQUAL NetSSL_OpenSSL )
			find_library(
				Poco_ssleay_BIN_RELEASE
				NAMES ssleay32MD
				PATHS
					${Poco_ROOT_DIR}/build/bin/Release
			)
		
			find_library(
				Poco_ssleay_BIN_DEBUG
				NAMES ssleay32MDd
				PATHS
					${Poco_ROOT_DIR}/build/bin/Debug
			)
		
			find_library(
				Poco_libeay32_BIN_RELEASE
				NAMES libeay32MD
				PATHS
					${Poco_ROOT_DIR}/build/bin/Release
			)
		
			find_library(
				Poco_libeay32_BIN_DEBUG
				NAMES libeay32MDd
				PATHS
					${Poco_ROOT_DIR}/build/bin/Debug
			)		
		endif()
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")	

		# find the include path
		set(Poco_${lib}_INCLUDE_DIR ${Poco_ROOT_DIR}/${lib}/include)	
	else()
		set( Poco_BINS_RELEASE "unused" )
		set( Poco_BINS_DEBUG "unused" )
		set( Poco_INCLUDE_DIRS ${Poco_ROOT_DIR} )
	endif()	
	

	# add the libraries to the variables according to the build type
	set(Poco_${lib}_LIBRARY)
	if(Poco_${lib}_LIBRARY_RELEASE)
		list(APPEND Poco_LIBRARIES optimized ${Poco_${lib}_LIBRARY_RELEASE})
		list(APPEND Poco_${lib}_LIBRARY optimized ${Poco_${lib}_LIBRARY_RELEASE})
		get_filename_component(lib_dir ${Poco_${lib}_LIBRARY_RELEASE} PATH)
		list(APPEND Poco_LIBRARY_DIRS ${lib_dir})
		
		list(APPEND Poco_BINS_RELEASE ${Poco_${lib}_BIN_RELEASE})
		
		# add the OpenSSL binaries
		if ( ${lib} STREQUAL NetSSL_OpenSSL )
			list(APPEND Poco_BINS_RELEASE ${Poco_ssleay_BIN_RELEASE})
			list(APPEND Poco_BINS_RELEASE ${Poco_libeay32_BIN_RELEASE})
		endif()
	endif()
 
	if(Poco_${lib}_LIBRARY_DEBUG)
		list(APPEND Poco_LIBRARIES debug ${Poco_${lib}_LIBRARY_DEBUG})
		list(APPEND Poco_${lib}_LIBRARY debug ${Poco_${lib}_LIBRARY_DEBUG})
		get_filename_component(lib_dir ${Poco_${lib}_LIBRARY_DEBUG} PATH)
		list(APPEND Poco_LIBRARY_DIRS ${lib_dir})
		
		list(APPEND Poco_BINS_DEBUG ${Poco_${lib}_BIN_DEBUG})
		
		# add the OpenSSL binaries
		if ( ${lib} STREQUAL NetSSL_OpenSSL )
			list(APPEND Poco_BINS_DEBUG ${Poco_ssleay_BIN_DEBUG})
			list(APPEND Poco_BINS_DEBUG ${Poco_libeay32_BIN_DEBUG})
		endif()
	endif()
	
	if(Poco_${lib}_INCLUDE_DIR)
		list(APPEND Poco_INCLUDE_DIRS ${Poco_${lib}_INCLUDE_DIR})
	endif()
 endforeach()
	
if(Poco_LIBRARY_DIRS)
	list(REMOVE_DUPLICATES Poco_LIBRARY_DIRS)
endif()
if(Poco_INCLUDE_DIRS)
	list(REMOVE_DUPLICATES Poco_INCLUDE_DIRS)
endif()
   
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Poco DEFAULT_MSG
	Poco_ROOT_DIR
	Poco_INCLUDE_DIRS	
	Poco_LIBRARY_DIRS	
	Poco_LIBRARIES
	Poco_BINS_DEBUG
	Poco_BINS_RELEASE	
)
