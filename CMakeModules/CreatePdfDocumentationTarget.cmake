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

# Creates the a target for generating the PDF-documentation for PersonalFME

macro( CreatePdfDocumentationTarget )
	# generate the DocBook documentation	
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/chap2.template"
		"${PROJECT_SOURCE_DIR}/documentation/chap2.xml"
	)
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/chap4.template"
		"${PROJECT_SOURCE_DIR}/documentation/chap4.xml"
	)	
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch.template"
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch.xml"
	)	
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch_xsl.template"
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch.xsl"
	)
	configure_file ( 
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch_html_xsl.template"
		"${PROJECT_SOURCE_DIR}/documentation/benutzerhandbuch_html.xsl"
	)
	
	FILE(GLOB DOC_XML_FILES ${PROJECT_SOURCE_DIR}/documentation/*.xml)
	FILE(GLOB DOC_XSL_FILES ${PROJECT_SOURCE_DIR}/documentation/*.xsl)
	add_custom_target( PdfDocumentation DEPENDS ${DOC_XML_FILES} ${DOC_XSL_FILES} )
	set_target_properties( PdfDocumentation PROPERTIES EXCLUDE_FROM_ALL ON EXCLUDE_FROM_DEFAULT_BUILD ON )
	
	install( CODE
		"execute_process(
			COMMAND \"${CMAKE_COMMAND}\" 
				--build \"${CMAKE_CURRENT_BINARY_DIR}\"
				--target PdfDocumentation
		)"
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)
		
	if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows" )
		add_custom_command( TARGET PdfDocumentation 
							COMMAND ${PROJECT_SOURCE_DIR}/documentation/make_pdf_doc.bat
							WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation )	
	elseif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
		# workaround because in the ZIP-source file no permissions are maintained
		add_custom_command( TARGET PdfDocumentation 
							COMMAND chmod +x ${PROJECT_SOURCE_DIR}/documentation/make_pdf_doc.sh
							WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation )

		add_custom_command( TARGET PdfDocumentation 
							COMMAND ${PROJECT_SOURCE_DIR}/documentation/make_pdf_doc.sh 
							WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/documentation )				
	endif()
endmacro()
