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

find_package( Poco COMPONENTS
	JSON
	NetSSLWin
REQUIRED )

find_package( Boost COMPONENTS
	date_time
REQUIRED )

set( SOURCE
	main.cpp
)

set( HEADERS
)

add_executable( Groupalarm ${SOURCE} ${HEADERS} )
target_include_directories( Groupalarm PUBLIC ${PROJECT_SOURCE_DIR}/groupalarmDemo )

target_link_libraries( Groupalarm PRIVATE 
	Networking	

	Poco::JSON
	Poco::NetSSLWin

	Boost::date_time
)
