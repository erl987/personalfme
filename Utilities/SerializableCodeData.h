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
	*	Serializable class representing the full parameter set of a sequence of tones (tone number, tone lengths [s], tone periods [s], tone frequencies [Hz], absolute tone levels [-])
	*/
	template <typename T>
	class CSerializableCodeData : public CCodeData<T>
	{
	public:	
		CSerializableCodeData(void) {};
		CSerializableCodeData(const CCodeData<T>& code);
		CSerializableCodeData(const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels);
		virtual ~CSerializableCodeData(void) {};
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
void Utilities::CSerializableCodeData<T>::serialize(Archive& ar, const unsigned int version) {
	using namespace std;
	int length;

	length = static_cast<int>( CCodeData<T>::codeData.size() );
	ar & length;
	CCodeData<T>::codeData.resize( length );

	for (int i=0; i < length; i++) {
		ar & get<0>( CCodeData<T>::codeData[i] );
		ar & get<1>( CCodeData<T>::codeData[i] );
		ar & get<2>( CCodeData<T>::codeData[i] );
		ar & get<3>( CCodeData<T>::codeData[i] );
		ar & get<4>( CCodeData<T>::codeData[i] );
	}
}



/**	@brief		Constructor
*	@param		tones					Vector containing the tone indices of the sequence
*	@param		toneLengths				Vector containing the length of the tones [s]
*	@param		tonePeriods				Vector containing the periods between the tones (cycle time) [s]
*	@param		toneFrequencies			Vector containing the frequencies of the tones [Hz]
*	@param		absToneLevels			Vector containing the absolute levels of the tones [-]
*	@exception	std::length_error		Thrown if the length of the input data is identical for all containers
*	@remarks							The length of the datset is not limited, but should in general be consistent to the length of a code sequence
*/
template <typename T>
Utilities::CSerializableCodeData<T>::CSerializableCodeData( const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels )
{
	this->Set( tones, toneLengths, tonePeriods, toneFrequencies, absToneLevels );
}



/**	@brief		Constructor using the non-serializable class format
*	@param		code					Code data in non-serializable format
*	@exception							None
*	@remarks							None
*/
template <typename T>
Utilities::CSerializableCodeData<T>::CSerializableCodeData(const CCodeData<T>& code)
{
	this->Set( code.GetTones(), code.GetToneLengths(), code.GetTonePeriods(), code.GetToneFrequencies(), code.GetAbsToneLevels() );
}
