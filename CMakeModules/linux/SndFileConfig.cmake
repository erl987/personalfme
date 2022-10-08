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
# SndFile::sndfile 		- The library

# Variables set by the module:
# SndFile_FOUND			- TRUE if all components are found
# SndFile_INCLUDE_DIR		- Location of the include directory
# SndFile_LIBRARY		- Library (for linking)
# SndFile_BIN			- Binary (DLL on Windows)
# SndFile_BIN_NAME		- File name of the binary (DLL on Windows)
# SndFile_LIC_FILE_1		- License file 1
# SndFile_LIC_FILE_2		- License file 2
# SndFile_LIC_FILE_NAME_1	- License file name 1
# SndFile_LIC_FILE_NAME_2	- License file name 2
#
# Example Usage:
#	find_package( SndFile REQUIRED )
#	target_link_libraries( MyApp PRIVATE SndFile::sndfile )
#
# Options to locate the Sndfile-library with this module: 
# 1. put the libary in the directory "../libraries/libsndfile"
# 2. define the SndFile-library root path in the evironment variable "SNDFILE_ROOT"
# 3. specify the path of SndFile_DIR directly in CMake (the others paths are chosen automatically)
#
# Under Linux the library should be usually automatically found if installed properly.

find_package( PkgConfig )

include( determine_compiler )
determine_compiler( COMPILER_ID )

find_path(
	Sndfile_DIR
	NAMES sndfile.h
	HINTS
		/usr/include
		/usr/local/include
		ENV SNDFILE_ROOT
		${PROJECT_SOURCE_DIR}/libraries/sndfile/include
)

set( SndFile_INCLUDE_DIR ${SndFile_DIR} )

# find the libraries
find_library(
	SndFile_LIBRARY
	NAMES libsndfile-1 sndfile
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${SndFile_DIR}/../lib
)

get_filename_component(
	SndFile_LIBRARY_DIR
	${SndFile_LIBRARY}
	PATH
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# find the corresponding binaries
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
	find_library(
		SndFile_BIN
		NAMES libsndfile-1
		PATHS
			${SndFile_DIR}/../bin
		NO_CMAKE_SYSTEM_PATH
	)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

	get_filename_component(
		SndFile_BIN_NAME
		${SndFile_BIN}
		NAME
	)
else()
	# on linux the binaries are not required explicitly
	set( SndFile_BIN "not required" )
	set( SndFile_BIN_NAME "not required" )
endif()

# license files
include( get_GNU_license )
get_GNU_license( "LGPL" "2.1" SndFile_LIC_FILE_1 )

get_filename_component(
	SndFile_LIC_FILE_NAME_1
	${SndFile_LIC_FILE_1}
	NAME
)
set( SndFile_LIC_FILE_NAME_1 libsndfile_${Sndfile_LIC_FILE_NAME_1} )

get_GNU_license( "LGPL" "3" SndFile_LIC_FILE_2 )
get_filename_component(
	SndFile_LIC_FILE_NAME_2
	${SndFile_LIC_FILE_2}
	NAME
)
set( SndFile_LIC_FILE_NAME_2 libsndfile_${Sndfile_LIC_FILE_NAME_2} )

# check that all files and directories were properly found
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	SndFile
	DEFAULT_MSG
	SndFile_INCLUDE_DIR
	SndFile_LIBRARY
	SndFile_BIN
	SndFile_BIN_NAME
	SndFile_LIC_FILE_1
	SndFile_LIC_FILE_2
	SndFile_LIC_FILE_NAME_1
	SndFile_LIC_FILE_NAME_2
)

if (SndFile_FOUND AND NOT TARGET SndFile::sndfile)
	add_library( SndFile::sndfile UNKNOWN IMPORTED )
	set_target_properties( SndFile::sndfile PROPERTIES
			IMPORTED_LOCATION "${SndFile_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${SndFile_INCLUDE_DIR}" )
endif()
