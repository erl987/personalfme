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
#include <tuple>
#include <vector>
#include "SerializableDateTime.h"
#include "SerializableCodeData.h"
#include "SeqDataComplete.h"

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
	*	Serializable class representing the full data of a code sequence including all tone parameters
	*/
	template <typename T>
	class CSerializableSeqDataComplete : public CSeqDataComplete<T>
	{
	public:	
		CSerializableSeqDataComplete(void) {};
		CSerializableSeqDataComplete(const CSeqDataComplete<T>& data);
		CSerializableSeqDataComplete(const CDateTime& startTime, const CCodeData<T>& codeData, const std::string& infoString);
		virtual ~CSerializableSeqDataComplete(void) {};
		template <typename Archive> void serialize(Archive & ar, const unsigned int version);
	};
}
/*@}*/



/**	@brief		Serialization using boost::serialize
*	@param		ar						See Boost Serialize documentation for details
*	@param		version					See Boost Serialize documentation for details
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <typename T>
template <typename Archive>
void Utilities::CSerializableSeqDataComplete<T>::serialize(Archive& ar, const unsigned int version) {
	using namespace std;

	// convert to serializable data structures
	CSerializableDateTime serializableTime = get<0>( CSeqDataComplete<T>::sequenceDataComplete );
	CSerializableCodeData<T> serializableCode( get<1>( CSeqDataComplete<T>::sequenceDataComplete ) );

	ar & serializableTime;
	ar & serializableCode;
	ar & get<2>( CSeqDataComplete<T>::sequenceDataComplete );
	
	// convert back to original data structure
	CSeqDataComplete<T>::sequenceDataComplete = make_tuple( serializableTime, serializableCode, get<2>( CSeqDataComplete<T>::sequenceDataComplete ) );
}



/**	@brief		Constructor
*	@param		startTime				Start time of sequence (DD, MM, YYYY, HH, MM, SS, MMM). Usually UTC-time is stored.
*	@param		codeData				Container storing the full infomration of the tones of the code sequence (tone indices, tone lengths [s], tone periods [s], tone frequencies [Hz])
*	@param		infoString				Storing the additional information for the sequence. It is not used for FME-code sequences.
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CSerializableSeqDataComplete<T>::CSerializableSeqDataComplete(const CDateTime& startTime, const CCodeData<T>& codeData, const std::string& infoString)
{
	this->Set( startTime, codeData, infoString );
}



/**	@brief		Constructor using the non-serializable class format
*	@param		data					Data in non-serializable format
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CSerializableSeqDataComplete<T>::CSerializableSeqDataComplete(const CSeqDataComplete<T>& data)
{
	this->Set( data.GetStartTime(), data.GetCodeData(), data.GetInfoString() );
}
