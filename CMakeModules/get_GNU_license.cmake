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

# Returns a path to the required license file
# Parameters:
# LIC_TYPE						- Required type of license (available: 'LGPL')
# LIC_VERSION					- Required version of the license (available: '2.1', '3')
# GNU_LIC_FILE					- Requested GNU license file

function( get_GNU_license
	LIC_TYPE
	LIC_VERSION
	GNU_LIC_FILE
)

	string( TOLOWER ${LIC_TYPE} LIC_TYPE_LOWERCASE )
	
	unset(LIC_FILE CACHE)
	set( BACKUP_LIBRARY_SUFFIX ${CMAKE_FIND_LIBRARY_SUFFIXES} )
	set( CMAKE_FIND_LIBRARY_SUFFIXES ".txt" )
	find_library(
		LIC_FILE
		NAMES ${LIC_TYPE_LOWERCASE}_v${LIC_VERSION}.txt
		PATHS
			${PROJECT_SOURCE_DIR}/CMakeModules
	)
	set( CMAKE_FIND_LIBRARY_SUFFIXES ${BACKUP_LIBRARY_SUFFIX} )
	
	set( ${GNU_LIC_FILE} ${LIC_FILE} PARENT_SCOPE )
	unset(LIC_FILE CACHE)
endfunction()