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

# Creates the a target for generating the Linux manpage-documentation for PersonalFME

macro( CreateManpageDocumentationTarget )
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/manpage.template"
		"${PROJECT_SOURCE_DIR}/documentation/manpage.xml"
	)
	
	FILE(GLOB DOC_XML_FILES ${PROJECT_SOURCE_DIR}/documentation/*.xml)
	add_custom_target( ManpageDocumentation DEPENDS ${DOC_XML_FILES} )
	set_target_properties( ManpageDocumentation PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON )
	
	install( CODE
		"execute_process(
			COMMAND \"${CMAKE_COMMAND}\" 
				--build \"${CMAKE_CURRENT_BINARY_DIR}\"
				--target ManpageDocumentation
		)"
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)
	# workaround because in the ZIP-source file no permissions are maintained
	add_custom_command( TARGET ManpageDocumentation 
						COMMAND chmod +x ${PROJECT_SOURCE_DIR}/documentation/make_manpage_doc.sh
						WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation )

	add_custom_command( TARGET ManpageDocumentation 
						COMMAND ${PROJECT_SOURCE_DIR}/documentation/make_manpage_doc.sh 
						WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation )				
endmacro()
