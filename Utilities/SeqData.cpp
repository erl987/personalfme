/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)

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

#include "SeqData.h"



/**	@brief		Standard constructor
*/
Utilities::CSeqData::CSeqData(void)
{
}



/**	@brief		Destructor
*/
Utilities::CSeqData::~CSeqData(void)
{
}



/**	@brief		Constructor
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		code					Container storing the tones of the code sequence
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
Utilities::CSeqData::CSeqData(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString)
{
	Set( startTime, code, infoString );
}



/**	@brief		Setting the sequence data
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		code					Container storing the tones of the code sequence
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Utilities::CSeqData::Set(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString)
{
	sequenceData = std::make_tuple( startTime, code, infoString );
}



/**	@brief		Getting the sequence data
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		code					Container storing the tones of the code sequence
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Utilities::CSeqData::Get(CDateTime& startTime, std::vector<int>& code, std::string& infoString) const
{
	using namespace std;

	startTime = get<0>( sequenceData );
	code = get<1>( sequenceData );
	infoString = get<2>( sequenceData );
}



/**	@brief		Getting the start time of the sequence
*	@return								Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@exception							None
*	@remarks							None
*/
const Utilities::CDateTime& Utilities::CSeqData::GetStartTime(void) const
{
	return std::get<0>( sequenceData );
}



/**	@brief		Getting the code sequence
*	@return								Container storing the tones of the code sequence
*	@exception							None
*	@remarks							None
*/
const std::vector<int>& Utilities::CSeqData::GetCode(void) const
{
	return std::get<1>( sequenceData );
}



/**	@brief		Getting the info string of the sequence
*	@return								Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
const std::string& Utilities::CSeqData::GetInfoString(void) const
{
	return std::get<2>( sequenceData );
}



namespace Utilities {
	/**	@brief		Equality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of equality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator==(const CSeqData& lhs, const CSeqData& rhs)
	{
		bool result = true;

		if ( lhs.GetStartTime() != rhs.GetStartTime() ) {
			result = false;
		}
		if ( lhs.GetCode() != rhs.GetCode() ) {
			result = false;
		}
		if ( lhs.GetInfoString() != rhs.GetInfoString() ) {
			result = false;
		}

		return result;
	}



	/**	@brief		Inequality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of ienquality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator!=(const CSeqData& lhs, const CSeqData& rhs)
	{
		return !( lhs == rhs );
	}
}