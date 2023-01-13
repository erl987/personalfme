# PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)
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

# Determines the complete file name of a Debian package without .deb
# Parameters:
# CPACK_PACKAGE_NAME				- Name of the package
# CPACK_PACKAGE_VERSION				- Software version
# CPACK_DEBIAN_PACKAGE_REVISION		- Debian package version
#
# Returned variable:
# DEBIAN_PACKAGE_NAME				- Name of the debian package

function( GetDebianPackageName
	CPACK_PACKAGE_NAME
	CPACK_PACKAGE_VERSION
	CPACK_DEBIAN_PACKAGE_REVISION
	DEBIAN_PACKAGE_NAME
)
	string( TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME_LOWERCASE )
	find_program( DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems" )
	if( DPKG_PROGRAM )
		execute_process( COMMAND ${DPKG_PROGRAM} --print-architecture
				 OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
				 OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		set( _DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_REVISION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}" )
	else( DPKG_PROGRAM )
		set( _DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME_LOWERCASE}_${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_REVISION}_${CMAKE_SYSTEM_NAME}" )
	endif( DPKG_PROGRAM )

	set( ${DEBIAN_PACKAGE_NAME} ${_DEBIAN_PACKAGE_NAME} PARENT_SCOPE )
endfunction()
