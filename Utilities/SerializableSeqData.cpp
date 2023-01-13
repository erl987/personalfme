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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "SerializableSeqData.h"



/**	@brief		Standard constructor
*/
Utilities::CSerializableSeqData::CSerializableSeqData(void)
{
}



/**	@brief		Destructor
*/
Utilities::CSerializableSeqData::~CSerializableSeqData(void)
{
}



/**	@brief		Constructor
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		code					Container storing the tones of the code sequence
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
Utilities::CSerializableSeqData::CSerializableSeqData(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString)
{
	Set( startTime, code, infoString );
}



/**	@brief		Constructor using the non-serializable class format
*	@param		data					Data in non-serializable format
*	@exception							None
*	@remarks							None
*/
Utilities::CSerializableSeqData::CSerializableSeqData(const CSeqData& data)
{
	Set( data.GetStartTime(), data.GetCode(), data.GetInfoString() );
}