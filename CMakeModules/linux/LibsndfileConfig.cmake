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
# sndfile-shared 			- The library

# Variables set by the module:
# Libsndfile_FOUND			- TRUE if all components are found
# Libsndfile_INCLUDE_DIR		- Location of the include directory
# Libsndfile_LIBRARY			- Library (for linking)
# Libsndfile_BIN			- Binary (DLL on Windows)
# Libsndfile_BIN_NAME			- File name of the binary (DLL on Windows)
# Libsndfile_LIC_FILE_1			- License file 1
# Libsndfile_LIC_FILE_2			- License file 2
# Libsndfile_LIC_FILE_NAME_1		- License file name 1
# Libsndfile_LIC_FILE_NAME_2		- License file name 2
#
# Example Usage:
#	find_package( Libsndfile REQUIRED )
#	target_link_libraries( MyApp PRIVATE sndfile-shared )
#
# Options to locate the Libsndfile-library with this module: 
# 1. put the libary in the directory "../libraries/libsndfile"
# 2. define the Libsndfile-library root path in the evironment variable "LIBSNDFILE_ROOT"
# 3. specify the path of Libsndfile_DIR directly in CMake (the others paths are chosen automatically)
#
# Under linux the library should be usually automatically found if installed properly.

find_package( PkgConfig )

include( determine_compiler )
determine_compiler( COMPILER_ID )

find_path(
	Libsndfile_DIR
	NAMES sndfile.h
	HINTS
		/usr/include
		/usr/local/include
		ENV LIBSNDFILE_ROOT
		${PROJECT_SOURCE_DIR}/libraries/sndfile/include
)

set( Libsndfile_INCLUDE_DIR ${Libsndfile_DIR} )

# find the libraries
find_library(
	Libsndfile_LIBRARY
	NAMES libsndfile-1 sndfile
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Libsndfile_DIR}/../lib
)

get_filename_component(
	Libsndfile_LIBRARY_DIR
	${Libsndfile_LIBRARY}
	PATH
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# find the corresponding binaries
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
	find_library(
		Libsndfile_BIN
		NAMES libsndfile-1
		PATHS
			${Libsndfile_DIR}/../bin
		NO_CMAKE_SYSTEM_PATH
	)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

	get_filename_component(
		Libsndfile_BIN_NAME
		${Libsndfile_BIN}
		NAME
	)
else()
	# on linux the binaries are not required explicitly
	set( Libsndfile_BIN "not required" )
	set( Libsndfile_BIN_NAME "not required" )
endif()

# license files
include( get_GNU_license )
get_GNU_license( "LGPL" "2.1" Libsndfile_LIC_FILE_1 )

get_filename_component(
	Libsndfile_LIC_FILE_NAME_1
	${Libsndfile_LIC_FILE_1}
	NAME
)
set( Libsndfile_LIC_FILE_NAME_1 libsndfile_${Libsndfile_LIC_FILE_NAME_1} )

get_GNU_license( "LGPL" "3" Libsndfile_LIC_FILE_2 )
get_filename_component(
	Libsndfile_LIC_FILE_NAME_2
	${Libsndfile_LIC_FILE_2}
	NAME
)
set( Libsndfile_LIC_FILE_NAME_2 libsndfile_${Libsndfile_LIC_FILE_NAME_2} )

# check that all files and directories were properly found
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	Libsndfile
	DEFAULT_MSG
	Libsndfile_INCLUDE_DIR
	Libsndfile_LIBRARY
	Libsndfile_BIN
	Libsndfile_BIN_NAME
	Libsndfile_LIC_FILE_1
	Libsndfile_LIC_FILE_2
	Libsndfile_LIC_FILE_NAME_1
	Libsndfile_LIC_FILE_NAME_2
)

if (Libsndfile_FOUND AND NOT TARGET sndfile-shared)
	add_library( sndfile-shared UNKNOWN IMPORTED )
	set_target_properties( sndfile-shared PROPERTIES
			IMPORTED_LOCATION "${Libsndfile_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${Libsndfile_INCLUDE_DIR}" )
endif()

