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
#include <tuple>
#include <vector>
#include <string>
#include "DateTime.h"
#include "CodeData.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef UTILITY_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define UTILITY_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define UTILITY_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define UTILITY_API __attribute__ ((visibility ("default")))
	#else
		#define UTILITY_API
	#endif		
#endif

/*@{*/
/** \ingroup Utilities
*/


namespace Utilities {
	/**	\ingroup Utilities
	*	Class representing the full data of a code sequence including all tone parameters
	*/
	template <typename T>
	class CSeqDataComplete
	{
	public:	
		CSeqDataComplete(void);
		CSeqDataComplete(const CDateTime& startTime, const CCodeData<T>& codeData, const std::string& infoString);
		virtual ~CSeqDataComplete(void);
		void Set(const CDateTime& startTime, const CCodeData<T>& code, const std::string& infoString);
		void Get(CDateTime& startTime, CCodeData<T>& code, std::string& infoString) const;
		CDateTime GetStartTime(void) const;
		CCodeData<T> GetCodeData(void) const;
		std::string GetInfoString(void) const;
		template <typename U> friend bool operator==(const CCodeData<U>& lhs, const CCodeData<U>& rhs);
		template <typename U> friend bool operator!=(const CCodeData<U>& lhs, const CCodeData<U>& rhs);
	protected:
		std::tuple< CDateTime, CCodeData<T>, std::string > sequenceDataComplete;
	};
}
/*@}*/



/**	@brief		Standard constructor
*/
template <typename T>
Utilities::CSeqDataComplete<T>::CSeqDataComplete(void)
{
}



/**	@brief		Destructor
*/
template <typename T>
Utilities::CSeqDataComplete<T>::~CSeqDataComplete(void)
{
}



/**	@brief		Constructor
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		codeData				Container storing the full infomration of the tones of the code sequence (tone indices, tone lengths [s], tone periods [s], tone frequencies [Hz], absolute tone levels (PDS) [-])
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CSeqDataComplete<T>::CSeqDataComplete(const CDateTime& startTime, const CCodeData<T>& codeData, const std::string& infoString)
{
	Set( startTime, codeData, infoString );
}



/**	@brief		Setting the sequence data
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		codeData				Container storing the full infomration of the tones of the code sequence (tone indices, tone lengths [s], tone periods [s], tone frequencies [Hz], absolute tone levels (PDS)[-])
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@return								None
*	@exception							None
*	@remarks							This function can be used for resetting the sequence data
*/
template <typename T>
void Utilities::CSeqDataComplete<T>::Set(const CDateTime& startTime, const CCodeData<T>& codeData, const std::string& infoString)
{
	sequenceDataComplete = std::make_tuple( startTime, codeData, infoString );
}



/**	@brief		Getting the sequence data
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		codeData				Container storing the full infomration of the tones of the code sequence (tone indices, tone lengths [s], tone periods [s], tone frequencies [Hz], absolute tone levels (PDS) [-])
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <typename T>
void Utilities::CSeqDataComplete<T>::Get(CDateTime& startTime, CCodeData<T>& codeData, std::string& infoString) const
{
	using namespace std;

	startTime = get<0>( sequenceDataComplete );
	codeData = get<1>( sequenceDataComplete );
	infoString = get<2>( sequenceDataComplete );
}



/**	@brief		Getting the start time of the sequence
*	@return								Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CDateTime Utilities::CSeqDataComplete<T>::GetStartTime(void) const
{
	return std::get<0>( sequenceDataComplete );
}



/**	@brief		Getting the full information on the tones of the code sequence
*	@return								Container storing the full infomration of the tones of the code sequence (tone indices, tone lengths [s], tone periods [s], tone frequencies [Hz])
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CCodeData<T> Utilities::CSeqDataComplete<T>::GetCodeData(void) const
{
	return std::get<1>( sequenceDataComplete );
}



/**	@brief		Getting the info string of the sequence
*	@return								Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::string Utilities::CSeqDataComplete<T>::GetInfoString(void) const
{
	return std::get<2>( sequenceDataComplete );
}



namespace Utilities {
	/**	@brief		Equality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of equality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	template <typename T>
	bool operator==(const CSeqDataComplete<T>& lhs, const CSeqDataComplete<T>& rhs)
	{
		bool result = true;

		if ( lhs.GetStartTime() != rhs.GetStartTime() ) {
			result = false;
		}
		if ( lhs.GetCodeData() != rhs.GetCodeData() ) {
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
	template <typename T>
	bool operator!=(const CSeqDataComplete<T>& lhs, const CSeqDataComplete<T>& rhs)
	{
		return !( lhs == rhs );
	}
}