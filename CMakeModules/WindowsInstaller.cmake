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

# Macro defining the Windows-installer for PersonalFME

macro( WindowsInstaller )
	
	# this workaround is required for the NSIS-installer
	string( REPLACE "/" "\\\\" AUDIO_SETTINGS_FILE_NAME ${AUDIO_SETTINGS_FILE_NAME} )
	string( REPLACE "/" "\\\\" FME_PARAMS_FILE_NAME ${FME_PARAMS_FILE_NAME} )
	string( REPLACE "/" "\\\\" PARAMS_FILE_NAME ${PARAMS_FILE_NAME} )
	string( REPLACE "/" "\\\\" BASIC_CONFIG_FILE_NAME ${BASIC_CONFIG_FILE_NAME} )
	string( REPLACE "/" "\\\\" COMPLETE_CONFIG_FILE_NAME ${COMPLETE_CONFIG_FILE_NAME} )
	string( REPLACE "/" "\\\\" CONFIG_XSD_FILE_NAME ${CONFIG_XSD_FILE_NAME} )		
	string( REPLACE "/" "\\\\" OGG_PLUGIN_FILE_NAME ${OGG_PLUGIN_FILE_NAME} )
	string( REPLACE "/" "\\\\" WAV_PLUGIN_FILE_NAME ${WAV_PLUGIN_FILE_NAME} )
	string( REPLACE "/" "\\\\" DOC_FILE_NAME ${DOC_FILE_NAME} )
	
	if( ${IS_NEW_CONFIG_FILE_SYNTAX} )
		set( WARN_TEXT "Die Konfigurationsdatei config.xml existiert bereits und wurde nicht überschrieben. Passen Sie die Datei bei Bedarf an die neuen Features an (siehe Dokumentation)." )
	else()
		set( WARN_TEXT "Die Konfigurationsdatei config.xml existiert bereits und wurde nicht überschrieben. Sie erfordert keine Anpassung, da die aktuelle Version keine neue Syntax enthält." )
	endif()

	set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS  "
		SetShellVarContext current
		CreateDirectory '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\'
		SetOutPath '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\'
		File '${AUDIO_SETTINGS_FILE_NAME}' 
		File '${FME_PARAMS_FILE_NAME}' 
		File '${PARAMS_FILE_NAME}'  

		CreateDirectory '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\plugins\\\\'
		SetOutPath '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\plugins\\\\'
		File '${OGG_PLUGIN_FILE_NAME}'
		File '${WAV_PLUGIN_FILE_NAME}'
	
		CreateDirectory '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\'
		SetOutPath '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\'
		File '${CONFIG_XSD_FILE_NAME}'
	
		CreateDirectory '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Networking\\\\'
		SetOutPath '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Networking\\\\'
		File '${PROJECT_SOURCE_DIR}\\\\Networking\\\\*.xsd'
	
		CreateDirectory '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Middleware\\\\'
		SetOutPath '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Middleware\\\\'
		File '${PROJECT_SOURCE_DIR}\\\\Middleware\\\\*.xsd'
	
		CreateDirectory '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\'
		SetOutPath '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\'
		File '${DOC_FILE_NAME}'
		IfFileExists '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\${CONFIG_FILE_NAME}' 0 file_not_found
			MessageBox MB_ICONINFORMATION '${WARN_TEXT}'
			goto end_of_config_file ;
		file_not_found:
			File /oname=${CONFIG_FILE_NAME} '${BASIC_CONFIG_FILE_NAME}'
		end_of_config_file:
			
		CreateDirectory '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\beispiele\\\\'
		SetOutPath '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\beispiele\\\\'
		File '${BASIC_CONFIG_FILE_NAME}'
		File '${COMPLETE_CONFIG_FILE_NAME}'
	
		SetOutPath '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\'
		CreateDirectory '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\'
		CreateShortcut '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\${PROJECT_SOFTWARE_NAME} Handbuch.lnk' '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\benutzerhandbuch.pdf'	
		CreateShortcut '$DESKTOP\\\\${PROJECT_SOFTWARE_NAME} Handbuch.lnk' '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\benutzerhandbuch.pdf'		
	
		SetOutPath '$INSTDIR\\\\bin\\\\'
		CreateShortcut '$SMPROGRAMS\\\\Startup\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\bin\\\\${CPACK_PACKAGE_NAME}.exe' '-r $DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\config.xml'
		CreateShortcut '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\bin\\\\${CPACK_PACKAGE_NAME}.bat'
		CreateShortcut '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk' '$INSTDIR\\\\bin\\\\${CPACK_PACKAGE_NAME}.bat'
		CreateShortcut '$DESKTOP\\\\${CPACK_PACKAGE_NAME} Kommandozeile.lnk' 'cmd.exe'
	")
	
	set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS  "
		SetShellVarContext current

		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Networking\\\\*.xsd'
		RMDir '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Networking\\\\'
	
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Middleware\\\\*.xsd'
		RMDir '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\Middleware\\\\'
	
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\config.xsd'
		RMDir '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\schema\\\\'
	
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\plugins\\\\OGGAudioPlugin.dll'
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\plugins\\\\WAVAudioPlugin.dll'
		RMDir '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\plugins\\\\'
	
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\audioSettings.dat'
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\fmeParams.dat'
		Delete '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\params.dat'
		RMDir '$APPDATA\\\\${CPACK_PACKAGE_NAME}\\\\'		
	
		Delete '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\beispiele\\\\basic_config.xml'
		Delete '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\beispiele\\\\complete_config.xml'
		RMDir '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\beispiele\\\\'
	
		Delete '$DOCUMENTS\\\\${CPACK_PACKAGE_NAME}\\\\benutzerhandbuch.pdf'
	
		Delete '$DESKTOP\\\\${CPACK_PACKAGE_NAME}.lnk'	
		Delete '$DESKTOP\\\\${PROJECT_SOFTWARE_NAME} Handbuch.lnk'
		Delete '$DESKTOP\\\\${CPACK_PACKAGE_NAME} Kommandozeile.lnk'	
		Delete '$SMPROGRAMS\\\\Startup\\\\${CPACK_PACKAGE_NAME}.lnk'
		Delete '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\${PROJECT_SOFTWARE_NAME} Handbuch.lnk'	
		Delete '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_NAME}.lnk'	
		RMDir '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\'
	
		SetShellVarContext all
		Delete '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\${CPACK_PACKAGE_NAME}.lnk'
		Delete '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\Uninstall.lnk'
		RMDir '$SMPROGRAMS\\\\${CPACK_PACKAGE_NAME}\\\\'
	")
endmacro()
