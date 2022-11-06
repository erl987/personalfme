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

# Macro defining the Debian/Ubuntu-packager for PersonalFME

macro( DebianPackager )
	
	include( GetDebianPackageName )
	GetDebianPackageName( ${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} ${CPACK_DEBIAN_PACKAGE_REVISION} CPACK_PACKAGE_FILE_NAME )

	set( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA 
		${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/preinst
		${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/postinst
		${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/prerm 
		${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/postrm
		${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/conffiles
	)

	# XML-schema files
	install( FILES
			${CONFIG_XSD_FILE_NAME}
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}/schema
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}			 
	)

	install( DIRECTORY
			${PROJECT_SOURCE_DIR}/Networking
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}/schema
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}			 
		 FILES_MATCHING PATTERN "*.xsd"
	)

	install( DIRECTORY 
			${PROJECT_SOURCE_DIR}/Middleware
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}/schema
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
		 FILES_MATCHING PATTERN "*.xsd"
	)

	# configuration example files
	install( FILES 
			${BASIC_CONFIG_FILE_NAME}
			${COMPLETE_CONFIG_FILE_NAME}
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}/examples
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
	)

	# default configuration file (which is marked as conffile in the Debian-package)
	install( FILES 
			${BASIC_CONFIG_FILE_NAME}
		 RENAME
			config.xml
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
	)

	# basic configuration files
	install( FILES 
			${AUDIO_SETTINGS_FILE_NAME}
			${FME_PARAMS_FILE_NAME}
			${PARAMS_FILE_NAME}
		 DESTINATION /etc/${CMAKE_PROJECT_NAME}/basicsettings
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
	)

	# changelog of the software
	install( CODE
		"execute_process( COMMAND gzip -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/CHANGES.txt
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			OUTPUT_FILE ${CMAKE_BINARY_DIR}/changelog.gz
		)"
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)			

	# changelog of the Debian package
	install( CODE
		"execute_process( COMMAND gzip -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/changelog
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			OUTPUT_FILE ${CMAKE_BINARY_DIR}/changelog.Debian.gz
		)"
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)	

	install( FILES 
			${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/copyright
			${CMAKE_BINARY_DIR}/changelog.gz
			${CMAKE_BINARY_DIR}/changelog.Debian.gz
			${DOC_FILE_NAME}
		 DESTINATION /usr/share/doc/${CMAKE_PROJECT_NAME}
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
	)

	# manpage
	install( CODE
		"execute_process( COMMAND gzip -9 -n -c ${CMAKE_CURRENT_SOURCE_DIR}/documentation/personalfme.1
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
			OUTPUT_FILE ${CMAKE_BINARY_DIR}/personalfme.1.gz
		)"
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)	
	
	install( FILES
			${CMAKE_BINARY_DIR}/personalfme.1.gz
		 DESTINATION /usr/share/man/man1
		 CONFIGURATIONS Release
		 COMPONENT ${PROJECT_NAME}
	)

	# Systemd daemon starter
	install( FILES 
			${CMAKE_CURRENT_SOURCE_DIR}/linux/debian/${CMAKE_PROJECT_NAME}.service
		 DESTINATION /lib/systemd/system
		 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
		 COMPONENT ${PROJECT_NAME}
		 CONFIGURATIONS Release
	)
endmacro()
