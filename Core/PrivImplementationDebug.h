/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>
*/
#pragma once

#include "FMEAudioInputDebug.h"
#include "privImplementation.h"

/*@{*/
/** \ingroup Core
*/

/**	\ingroup Core
*	Pimple idiom for hiding the private implementation details of the class for getting detailed debug information with the FME sequences
*/
class Core::CFMEAudioInputDebug::PrivImplementationDebug :
	public Core::CAudioInput::PrivImplementation
{
public:
	PrivImplementationDebug(void (*foundCallback)(std::tuple< std::tuple<int,int,int,int,int,int,int>, std::vector< std::tuple<int,float,float,float> > >));
	~PrivImplementationDebug(void){};
	void SignalNewSequence(std::tuple< std::tuple<int,int,int,int,int,int,int>, std::vector< std::tuple<int,float,float,float> > > sequenceData, std::string infoString);
	boost::signal< void( std::tuple< std::tuple<int,int,int,int,int,int,int>, std::vector< std::tuple<int,float,float,float> > > ) > debugFoundSequenceSignal;
};

/*@}*/