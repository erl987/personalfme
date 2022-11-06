#	Ogg Vorbis audio file handler plugin for the software PersonalFME
#	Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)
#
#	This program is free software: you can redistribute it and / or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with this program.If not, see <http://www.gnu.org/licenses/>

find_package( Poco COMPONENTS
	Foundation
REQUIRED )

find_package( SndFile CONFIG REQUIRED )

set( SOURCE
	OGGAudioPlugin.cpp
	OGGHandler.cpp
)

set( HEADERS
	OGGAudioPlugin.h
	OGGHandler.h
)

add_library( OGGAudioPlugin SHARED ${SOURCE} ${HEADERS} )

target_link_libraries( OGGAudioPlugin PRIVATE
	Utilities

	SndFile::sndfile
	Poco::Foundation
)

target_include_directories( OGGAudioPlugin PRIVATE ${Poco_INCLUDE_DIRS} )

# installer settings (on Windows the required filesystem structure is not available here)
if( UNIX AND NOT APPLE )
	install( TARGETS OGGAudioPlugin
		LIBRARY DESTINATION /usr/lib/${CMAKE_PROJECT_NAME}/plugins
		COMPONENT ${PROJECT_NAME}
		CONFIGURATIONS Release
	)
endif()
