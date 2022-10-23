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

# Determines the current compiler version
# Returned variable:
# COMPILER_ID					- ID of the compiler

function( determine_compiler
	COMPILER_ID
)
	if( MSVC )
		if( MSVC14 )
			set(COMPILER_ID "MSVC-14.0")	
		elseif( MSVC13 )
			set(COMPILER_ID "MSVC-13.0")
		elseif( MSVC12 )
			set(COMPILER_ID "MSVC-12.0")
		elseif( MSVC11 )
			set(COMPILER_ID "MSVC-11.0")
		elseif( MSVC10 )
			set(COMPILER_ID "MSVC-10.0")
		elseif( MSVC90 )
			set(COMPILER_ID "MSVC-9.0")
		endif()
	else()
		set(COMPILER_ID "gcc")
	endif()

set( COMPILER_ID ${COMPILER_ID} PARENT_SCOPE )
	
endfunction()