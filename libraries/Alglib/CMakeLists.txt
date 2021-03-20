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

set( ${PROJECT_NAME}_VERSION_MAJOR 3 ) 		# Adjust here the version number manually
set( ${PROJECT_NAME}_VERSION_MINOR 17 ) 	# Adjust here the version number manually
set( ${PROJECT_NAME}_VERSION_PATCH 0 ) 		# Adjust here the version number manually
set( ${PROJECT_NAME}_DATE "Dec 29 2020" )

add_definitions( -DUNICODE -D_UNICODE )
add_definitions( /DAE_CPU=AE_INTEL )

# generate the version info header file
configure_file ( 
	"${PROJECT_SOURCE_DIR}/libraries/Alglib/libalglib/alglibversion.template"
	"${PROJECT_SOURCE_DIR}/libraries/Alglib/libalglib/alglibversion.h"
)

set( HEADERS
	libalglib/alglibinternal.h
	libalglib/alglibmisc.h
	libalglib/ap.h
	libalglib/dataanalysis.h
	libalglib/diffequations.h
	libalglib/fasttransforms.h
	libalglib/integration.h
	libalglib/interpolation.h
	libalglib/linalg.h
	libalglib/optimization.h
	libalglib/solvers.h
	libalglib/specialfunctions.h
	libalglib/statistics.h
	libalglib/alglibversion.h
	libalglib/stdafx.h	
)

set( SOURCE
	libalglib/alglibinternal.cpp
	libalglib/alglibmisc.cpp
	libalglib/ap.cpp
	libalglib/dataanalysis.cpp
	libalglib/diffequations.cpp
	libalglib/fasttransforms.cpp
	libalglib/integration.cpp
	libalglib/interpolation.cpp
	libalglib/linalg.cpp
	libalglib/optimization.cpp
	libalglib/solvers.cpp
	libalglib/specialfunctions.cpp
	libalglib/statistics.cpp
)

add_library( Alglib STATIC ${SOURCE} ${HEADERS} )
target_include_directories( Alglib PUBLIC ${PROJECT_SOURCE_DIR}/libraries/Alglib )