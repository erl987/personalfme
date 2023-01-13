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


if ( UNIX )
	find_package( Alglib CONFIG REQUIRED )
endif()

find_package( portaudio CONFIG REQUIRED )

find_package( Boost COMPONENTS 
	system
	filesystem
	serialization
	thread
REQUIRED )

set( SOURCE
	AudioDevice.cpp
	AnalysisParam.cpp
	AudioInput.cpp
	AudioInputParam.cpp
	ErrorStreamManipulator.cpp
	FMEAnalysisParam.cpp
	FMEAudioInput.cpp
	FMEAudioInputDebug.cpp
	FMEGenerateParam.cpp
	privImplementation.cpp
	SearchTransferFunc.cpp
)

set( HEADERS
	AnalysisParam.h
	AudioDevice.h
	AudioFullDownsampler.h
	AudioInput.h
	AudioInputParam.h
	AudioSignalPreserver.h
	AudioSignalReader.h
	ErrorStreamManipulator.h
	DataProcessing.h
	FFT.h
	filter.h
	FIRfilter.h
	FME.h
	FMEAnalysisParam.h
	FMEAudioInput.h
	FMEAudioInputDebug.h
	FMEGenerateParam.h
	FMESequenceSearch.h
	FrequencySearch.h
	IIRfilter.h
	PortaudioWrapper.h
	privImplementation.h
	PrivImplementationDebug.h
	ProduceCode.h
	ProduceFMECode.h
	publicAudioSPDefinitions.h
	Search.h
	SearchTransferFunc.h
	SequencePasser.h
	SequencePasserDebug.h
	ToneSearch.h
)

add_library( Core SHARED ${SOURCE} ${HEADERS} )
target_include_directories( Core PUBLIC ${PROJECT_SOURCE_DIR}/Core )

target_link_libraries( Core PRIVATE 
	Utilities
	
	Boost::system 
	Boost::serialization
	Boost::thread
	Boost::filesystem
)
target_link_libraries( Core PUBLIC
	Alglib
	portaudio
) 

# enable SSE-optimizations for the Alglib-library on the supported platforms
if ( ${CMAKE_SYSTEM_PROCESSOR} MATCHES "(x86)|(X86)|(amd64)|(AMD64)" )
	target_compile_definitions( Core PUBLIC
		AE_CPU=AE_INTEL
	)
endif()

install( TARGETS Core
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/${CMAKE_PROJECT_NAME}
	COMPONENT ${PROJECT_NAME}
	CONFIGURATIONS Release
)
