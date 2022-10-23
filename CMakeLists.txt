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

# PersonalFME-project make file

# This project is using vcpkg (on Windows only), its toolchain file needs to be provided via CMake: [vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake_minimum_required ( VERSION 3.5 )
project( PersonalFME )	
set( PROJECT_NAME PersonalFME )
if( UNIX )
	# on Linux only lowercase characters are used
	string( TOLOWER ${PROJECT_NAME} PROJECT_NAME )
endif()
set( CMAKE_PROJECT_NAME ${PROJECT_NAME} )

set( PROJECT_VERSION_MAJOR 1 ) 		 # Adjust here the version number manually
set( PROJECT_VERSION_MINOR 1 ) 		 # Adjust here the version number manually
set( PROJECT_VERSION_PATCH 0 ) 		 # Adjust here the version number manually
set( PROJECT_VERSION_TYPE "pre-release" )	 	 # Adjust here the version number manually (i.e. "" = Release to Manufacturing, "RC1" = Release candidate 1, "pre-release" = Development version)
set( IS_NEW_CONFIG_FILE_SYNTAX OFF ) # Set this flag to ON in order to warn the user that the syntax of the configuration file config.xml has been changed and his file may require updating
set( PROJECT_VENDOR_NAME "Ralf Rettig" )
set( PROJECT_MAINTAINER_CONTACT "Ralf Rettig <info@${PROJECT_NAME}.de>" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gateway for forwarding selcalls to internet services\\n Forwarding to e-mail and SMS-services (www.groupalarm.de) is possible.\\n The program is suitable for the analog radio network of the German\\n security services (BOS).\\n It requires a radio device connected to the sound card.\\n It is running 24/7 as a daemon in the background.\\n WARNING: YOU MAY ONLY DECODE SELCALLS FROM ANY OFFICIAL RADIO NETWORK IF\\n YOU ARE AUTHORIZED TO DO SO. OTHERWISE YOU MAY BE SUBJECT TO\\n CRIMINAL PROSECUTION." )

# compiler configuration
add_definitions( -DUNICODE -D_UNICODE )
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# all binaries are stored in the Debug / Release directory
set( PROJECT_SOFTWARE_NAME ${PROJECT_NAME} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
	
if ( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" )
	# expects a Raspberry Pi 3 Model B
	add_definitions( -mfpu=neon-fp-armv8 )
	add_definitions( -funsafe-math-optimizations )
	add_definitions( -mtune=cortex-a53 )
	add_definitions( -mfloat-abi=hard )
	add_definitions( -Wno-psabi )  # disables irrelevant warnings because all code is compiled with the same compiler version
endif()

if ( UNIX )
	set( CMAKE_INSTALL_PREFIX "/usr" )
endif()

# options
include(CMakeDependentOption)
option( Option_BUILD_UNITTESTS
		"Set to ON to build the unittest suite."
		OFF )
option( Option_USE_GIT
		"Use Git to determine the current revision."
		OFF )
option( Option_CREATE_INSTALLER
		"Generate an installer."
		ON )
cmake_dependent_option( Option_CREATE_DOCUMENTATION
						"Regenerate the PDF- (and on Linux the manpage) documentation, this requires DocBook." 
						OFF
						Option_USE_GIT
						OFF )

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules" "${PROJECT_SOURCE_DIR}/CMakeModules/externals/CMake-gitinfo/cmake" ${CMAKE_MODULE_PATH} )
if ( UNIX )
	set( CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/linux" ${CMAKE_PREFIX_PATH} )
endif()

add_subdirectory( Utilities )
add_subdirectory( WAVAudioPlugin )
add_subdirectory( OGGAudioPlugin )
add_subdirectory( Networking )
add_subdirectory( Core )
add_subdirectory( Middleware )
add_subdirectory( Console )
add_subdirectory( CoreTest )
if (${Option_BUILD_UNITTESTS})
	add_subdirectory( UnitTests )
endif()

if ( WIN32 )
	add_subdirectory( libraries/Alglib )
endif()

# generate the version information header of the project
if ( ${Option_USE_GIT} )
	find_package( GitInfo REQUIRED )
	
	# Get the Git revision and append that to the patch version.
	git_wc_info( ${PROJECT_SOURCE_DIR} Project )
	message( "Current revision is ${Project_WC_REVISION}" )
	message( "Date of revision: ${Project_WC_LAST_CHANGED_DATE}" )
	
	set( PROJECT_REVISION ${Project_WC_REVISION} )
	set( PROJECT_DATE ${Project_WC_LAST_CHANGED_DATE} )
	string( SUBSTRING ${PROJECT_DATE} 1 4 REVISION_YEAR )	

	configure_file ( 
		"${PROJECT_SOURCE_DIR}/Utilities/version.template"
		"${PROJECT_SOURCE_DIR}/Utilities/version.h"
	)
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/license.template"
		"${PROJECT_SOURCE_DIR}/LICENSE.txt"
	)
endif()

if( NOT EXISTS "${PROJECT_SOURCE_DIR}/Utilities/version.h" )
	message( FATAL_ERROR "Necessary source file \"Utilities/version.h\" is missing. Set the option USE_GIT to ON or provide it manually!" )
endif()
if( NOT EXISTS "${PROJECT_SOURCE_DIR}/LICENSE.txt" )
	message( FATAL_ERROR "Necessary source file \"LICENSE.txt\" is missing. Set the option USE_GIT to ON or provide it manually!" )
endif()

if ( ${Option_CREATE_INSTALLER} )
	include( Installer )
	Installer()
	include( CPack )
endif()
