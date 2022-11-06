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

# Macro defining the installer for PersonalFME

macro( Installer )
	# Linux-specific settings (ignore them on Windows)
	set( CPACK_DEBIAN_PACKAGE_REVISION "1" )
	set( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )
	set( CPACK_DEBIAN_PACKAGE_SECTION "net" )

	if( UNIX AND NOT APPLE )
		set( CMAKE_INSTALL_PREFIX "/usr" )
	
		# Debian-package for Debian-derived Linux-systems
		set( CPACK_GENERATOR "DEB" )
		set( CPACK_SET_DESTDIR ON )
		set( CPACK_STRIP_FILES ON )

		# auto-generate dependency information
		set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
		set( CPACK_DEBIAN_PACKAGE_DEPENDS "pulseaudio, adduser" )

		set( DOCBOOK_ROOT /usr/share/xml/docbook/stylesheet/docbook-xsl/ )
	elseif( WIN32 )
		set( DOCBOOK_ROOT $ENV{DOCBOOK_ROOT} )
		
		# include all dependency DLLs (mostly provided by vcpkg)
		install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Release/
			TYPE BIN
			CONFIGURATIONS Release
			FILES_MATCHING PATTERN "*.dll"
		)
		
		# include required license files for all dependencies
		install( DIRECTORY ${PROJECT_SOURCE_DIR}/libraries/licenses/
			TYPE BIN
			CONFIGURATIONS Release
		)
		
		
	endif()

	# prepare the installation
	set( CPACK_PACKAGE_NAME ${PROJECT_SOFTWARE_NAME} )
	set( CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR_NAME} )
	set( CPACK_PACKAGE_CONTACT ${PROJECT_MAINTAINER_CONTACT} )
	set( CPACK_PACKAGE_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )
	if ( NOT ${PROJECT_VERSION_TYPE} STREQUAL "" )
		set( CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}-${PROJECT_VERSION_TYPE}" )
	endif()
	set( CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_SOFTWARE_NAME} )
	set( CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME ${PROJECT_SOFTWARE_NAME} )
	
	# for the documentation always the lowercase name is required	
	string( TOLOWER ${PROJECT_SOFTWARE_NAME} LINUX_PROJECT_SOFTWARE_NAME )
	
	if ( ${Option_CREATE_DOCUMENTATION} )
		# create a target for generating the PDF-documentation
		include( CreatePdfDocumentationTarget )
		CreatePdfDocumentationTarget()

		if ( UNIX AND NOT APPLE )
			# create a target for generating the Linux manpage-docuemtation
			include ( CreateManpageDocumentationTarget )
			CreateManpageDocumentationTarget()
		endif()
	endif()
		
	# obtain the binaries for the audio plugins
	find_package( SndFile )
	
	# define the required files for the installation	
	set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYRIGHT.txt" )
	set( AUDIO_SETTINGS_FILE_NAME "${PROJECT_SOURCE_DIR}/audioSettings.dat" )
	set( FME_PARAMS_FILE_NAME "${PROJECT_SOURCE_DIR}/fmeParams.dat" )
	set( PARAMS_FILE_NAME "${PROJECT_SOURCE_DIR}/params.dat" )
	set( BASIC_CONFIG_FILE_NAME "${PROJECT_SOURCE_DIR}/documentation/examples/basic_config.xml" )
	set( COMPLETE_CONFIG_FILE_NAME "${PROJECT_SOURCE_DIR}/documentation/examples/complete_config.xml" )
	set( CONFIG_FILE_NAME "config.xml" )
	set( CONFIG_XSD_FILE_NAME "${PROJECT_SOURCE_DIR}/config.xsd" )
	set( OGG_PLUGIN_FILE_NAME "${PROJECT_SOURCE_DIR}/build/Release/OGGAudioPlugin.dll" )
	set( WAV_PLUGIN_FILE_NAME "${PROJECT_SOURCE_DIR}/build/Release/WAVAudioPlugin.dll" )
	set( SNDFILE_BIN "${SndFile_BIN}" )
	set( SNDFILE_LIC_1_FILE "${SndFile_LIC_FILE_1}" )
	set( SNDFILE_LIC_2_FILE "${SndFile_LIC_FILE_2}" )
	set( DOC_FILE_NAME "${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch.pdf" )

	# generate the installer
	if( WIN32 )
		include( WindowsInstaller )
		WindowsInstaller()
	else( UNIX AND NOT APPLE )
		include( DebianPackager )
		DebianPackager()
	endif()
endmacro()
