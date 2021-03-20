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
# Alglib			- The library

# Variables set by the module:
# Alglib_FOUND			- TRUE if all components are found
# Alglib_INCLUDE_DIR		- Location of the include directory
# Alglib_LIBRARY		- Library (debug and release library for linking)
# Alglib_LIBRARY_DEBUG		- Debug library (for linking)
# Alglib_LIBRARY_RELEASE	- Release library (for linking)
#
# Example Usage:
#	find_package( Alglib CONFIG REQUIRED )
#	target_link_libraries( MyApp PRIVATE Alglib )
#
# Options to locate the Alglib-library with this module: 
# 1. put the libary in the directory "../libraries/alglib"
# 2. define the Alglib-library root path in the evironment variable "ALGLIB_ROOT"
# 3. specify the path of Alglib_DIR directly in CMake (the others paths are chosen automatically)
#
# Under linux the library should be usually automatically found if installed properly.

find_package( PkgConfig )

find_path(
	Alglib_DIR
	NAMES libalglib/alglibinternal.h
	HINTS
		/usr/include
		/usr/local/include
		ENV ALGLIB_ROOT
		${PROJECT_SOURCE_DIR}/libraries/alglib/AlgLib
)

set( Alglib_INCLUDE_DIR ${Alglib_DIR} )

# find the libraries
find_library(
	Alglib_LIBRARY_DEBUG
	NAMES alglib libalglib
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Alglib_DIR}/../build/Debug
)

find_library(
	Alglib_LIBRARY_RELEASE
	NAMES alglib libalglib
	PATHS 
		/usr/lib/x86_64-linux-gnu
		/usr/lib
		/usr/local/lib
		${Alglib_DIR}/../build/Release
)

set( Alglib_LIBRARY 
	$<$<CONFIG:Debug>:${Alglib_LIBRARY_DEBUG}>
	$<$<CONFIG:Release>:${Alglib_LIBRARY_RELEASE}>
)

# check that all files and directories were properly found
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( 
	Alglib
	DEFAULT_MSG
	Alglib_INCLUDE_DIR
	Alglib_LIBRARY
	Alglib_LIBRARY_DEBUG	
	Alglib_LIBRARY_RELEASE
)

if (Alglib_FOUND AND NOT TARGET Alglib )
	add_library( Alglib UNKNOWN IMPORTED )
	set_target_properties( Alglib PROPERTIES
			IMPORTED_LOCATION_DEBUG "${Alglib_LIBRARY_DEBUG}"
			IMPORTED_LOCATION_RELEASE "${Alglib_LIBRARY_RELEASE}"
			MAP_IMPORTED_CONFIG_MINSIZEREL Release
			MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
			INTERFACE_INCLUDE_DIRECTORIES "${Alglib_INCLUDE_DIR}"
	)
endif()


