/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)

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

#include "FMEServerDebug.h"
#include "FMEServerPrivImplementation.h"

/*@{*/
/** \ingroup Networking
*/

/**	\ingroup Networking
*	Pimple idiom for hiding the private implementation details of CFMEServerDebug
*/
class Networking::CFMEServerDebug::FMEServerPrivImplDebug : 
	public Networking::CFMEServer::FMEServerPrivImplementation
{
public:
	FMEServerPrivImplDebug(std::function< void(const Utilities::CSeqDataComplete<float>&) > foundCallback);
	virtual ~FMEServerPrivImplDebug(void){};
	virtual void ResetConnection(void);

	std::function< void(const Utilities::CSeqDataComplete<float>&) > foundCallbackDebug;
};

/*@}*/