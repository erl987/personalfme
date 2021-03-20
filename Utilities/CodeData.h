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
#include <memory>
#include <stdexcept>

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
	*	Class representing the full parameter set of a sequence of tones (tone number, tone lengths [s], tone periods [s], tone frequencies [Hz], absolute tone levels [-])
	*/
	template <typename T>
	class CCodeData
	{
	public:	
		CCodeData(void);
		CCodeData( const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels );
		virtual ~CCodeData(void);
		void Set( const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels );
		void AddOneTone( const int& tone, const T& toneLength, const T& tonePeriod, const T& toneFrequency, const T& absToneLevel );
		size_t GetLength(void) const;
		void SetTones( const std::vector<int>& tones );
		void Clear(void);
		void Get( std::vector<int>& tones, std::vector<T>& toneLengths, std::vector<T>& tonePeriods, std::vector<T>& toneFrequencies, std::vector<T>& absToneLevels ) const;
		std::vector<int> GetTones() const;
		std::vector<T> GetToneLengths() const;
		std::vector<T> GetTonePeriods() const;
		std::vector<T> GetToneFrequencies() const;
		std::vector<T> GetAbsToneLevels() const;
		template <typename U> friend bool operator==(const CCodeData<U>& lhs, const CCodeData<U>& rhs);
		template <typename U> friend bool operator!=(const CCodeData<U>& lhs, const CCodeData<U>& rhs);
	protected:
		typedef std::tuple<int,T,T,T,T> CodeDataType;
		std::vector<CodeDataType> codeData;
	};
}
/*@}*/



/**	@brief		Standard constructor
*/
template <typename T>
Utilities::CCodeData<T>::CCodeData(void)
{
}



/**	@brief		Destructor
*/
template <typename T>
Utilities::CCodeData<T>::~CCodeData(void)
{
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
Utilities::CCodeData<T>::CCodeData( const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels )
{
	Set( tones, toneLengths, tonePeriods, toneFrequencies, absToneLevels );
}



/**	@brief		Setting all tones in one function call
*	@param		tones					Vector containing the tone indices of the sequence
*	@param		toneLengths				Vector containing the length of the tones [s]
*	@param		tonePeriods				Vector containing the periods between the tones (cycle time) [s]
*	@param		toneFrequencies			Vector containing the frequencies of the tones [Hz]
*	@param		absToneLevels			Vector containing the absolute levels of the tones [-]
*	@return								None
*	@exception	std::length_error		Thrown if the length of the input data is identical for all containers
*	@remarks							Any dataset already stored will be overwritten
*/
template <typename T>
void Utilities::CCodeData<T>::Set( const std::vector<int>& tones, const std::vector<T>& toneLengths, const std::vector<T>& tonePeriods, const std::vector<T>& toneFrequencies, const std::vector<T>& absToneLevels )
{
	using namespace std;
	vector<CodeDataType> newCodeData( tones.size() );

	// check for identical size of input data
	if ( ( tones.size() != toneLengths.size() ) || ( tones.size() != tonePeriods.size() ) || ( tones.size() != toneFrequencies.size() ) || ( tones.size() != absToneLevels.size() ) ) {
		throw std::length_error( "The containers do not have identical size." );
	}

	for (size_t i=0; i < tones.size(); i++) {
		newCodeData[i] = make_tuple( tones[i], toneLengths[i], tonePeriods[i], toneFrequencies[i], absToneLevels[i] );
	}

	CCodeData::codeData = newCodeData;
}



/**	@brief		Resetting only the tone indices
*	@param		tones					Vector containing the tone indices. It must have the size of the stored dataset, which can be obtained with the function CCodeData<T>::GetLength().
*	@return								None
*	@exception	std::length_error		Thrown if the size of "tones" differs from the size of the stored dataset.
*	@remarks							None
*/
template <typename T>
void Utilities::CCodeData<T>::SetTones(const std::vector<int>& tones)
{
	using namespace std;
	vector<T> toneLengths, tonePeriods, toneFrequencies, toneLevels;
	vector<CodeDataType> newCodeData( tones.size() );

	// check for identical size of input data
	if ( tones.size() != codeData.size() ) {
		throw std::length_error( "The container does not have the correct size." );
	}

	// obtain unchanged data
	toneLengths = GetToneLengths();
	tonePeriods = GetTonePeriods();
	toneFrequencies = GetToneFrequencies();
	toneLevels = GetAbsToneLevels();

	// set new tones
	for (size_t i=0; i < tones.size(); i++) {
		newCodeData[i] = make_tuple( tones[i], toneLengths[i], tonePeriods[i], toneFrequencies[i], toneLevels[i] );
	}

	CCodeData::codeData = newCodeData;
}



/**	@brief		Obtaining the full stored dataset
*	@param		tones					Vector containing the tone indices of the sequence
*	@param		toneLengths				Vector containing the length of the tones [s]
*	@param		tonePeriods				Vector containing the periods between the tones (cycle time) [s]
*	@param		toneFrequencies			Vector containing the frequencies of the tones [Hz]
*	@param		absToneLevels			Vector containing the absolute levels of the tones [-]
*	@return								None
*	@exception							None
*	@remarks							The length of the containers can be obtained by CCodeData<T>::GetLength() in advance if required
*/
template <typename T>
void Utilities::CCodeData<T>::Get( std::vector<int>& tones, std::vector<T>& toneLengths, std::vector<T>& tonePeriods, std::vector<T>& toneFrequencies, std::vector<T>& absToneLevels ) const
{
	using namespace std;
	
	tones.resize( codeData.size() );
	toneLengths.resize( codeData.size() );
	tonePeriods.resize( codeData.size() );
	toneFrequencies.resize( codeData.size() );
	absToneLevels.resize( codeData.size() );
	for (size_t i=0; i < codeData.size(); i++) {
		tones[i] = get<0>( codeData[i] );
		toneLengths[i] = get<1>( codeData[i] );
		tonePeriods[i] = get<2>( codeData[i] );
		toneFrequencies[i] = get<3>( codeData[i] );
		absToneLevels[i] = get<4>( codeData[i] );
	}
}



/**	@brief		Obtaining the tone indices of the stored dataset
*	@return								Container storing all tone indices
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::vector<int> Utilities::CCodeData<T>::GetTones(void) const
{
	using namespace std;
	vector<int> outTones( codeData.size() );

	for (size_t i=0; i < codeData.size(); i++) {
		outTones[i] = get<0>( codeData[i] ); 
	}

	return outTones;
}



/**	@brief		Obtaining the tone lengths of the stored dataset
*	@return								Container storing all tone lengths [s]
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::vector<T> Utilities::CCodeData<T>::GetToneLengths(void) const
{
	using namespace std;
	vector<T> outToneLengths( codeData.size() );

	for (size_t i=0; i < codeData.size(); i++) {
		outToneLengths[i] = get<1>( codeData[i] ); 
	}

	return outToneLengths;
}



/**	@brief		Obtaining the tone periods of the stored dataset
*	@return								Container storing all tone periods [s]
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::vector<T> Utilities::CCodeData<T>::GetTonePeriods(void) const
{
	using namespace std;
	std::vector<T> outTonePeriods( codeData.size() );

	for (size_t i=0; i < codeData.size(); i++) {
		outTonePeriods[i] = get<2>( codeData[i] ); 
	}

	return outTonePeriods;
}



/**	@brief		Obtaining the tone frequencies of the stored dataset
*	@return								Container storing all tone frequencies [Hz]
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::vector<T> Utilities::CCodeData<T>::GetToneFrequencies(void) const
{
	using namespace std;
	vector<T> outToneFrequencies( codeData.size() );

	for (size_t i=0; i < codeData.size(); i++) {
		outToneFrequencies[i] = get<3>( codeData[i] ); 
	}

	return outToneFrequencies;
}



/**	@brief		Obtaining the absolute tone levels of the stored dataset
*	@return								Container storing all absolute tone levels [-]
*	@exception							None
*	@remarks							None
*/
template <typename T>
std::vector<T> Utilities::CCodeData<T>::GetAbsToneLevels( void ) const
{
	using namespace std;
	vector<T> outToneLevels( codeData.size() );

	for ( size_t i = 0; i < codeData.size(); i++ ) {
		outToneLevels[i] = get<4>( codeData[i] );
	}

	return outToneLevels;
}


/**	@brief		Adding a single dataset to the existing data
*	@param		tone					Tone index of the new dataset
*	@param		toneLength				Length of the new tone [s]
*	@param		tonePeriod				Period of the new tone to the previous one [s]
*	@param		toneFrequency			Frequency of the new tone [Hz]
*	@param		absToneLevel			Absolute signal level of the new tone [-]
*	@return								None
*	@exception							None
*	@remarks							This function will add the datset to the existing data. The size of the stored dataset does not matter.
*/
template <typename T>
void Utilities::CCodeData<T>::AddOneTone(const int& tone, const T& toneLength, const T& tonePeriod, const T& toneFrequency, const T& absToneLevel)
{
	CodeDataType newTone;

	newTone = std::make_tuple( tone, toneLength, tonePeriod, toneFrequency, absToneLevel );
	codeData.push_back( newTone );
}




/**	@brief		Obtaining the length of the stored dataset
*	@return								Length of the stored dataset (i.e. number of entries)
*	@exception							None
*	@remarks							None
*/
template <typename T>
size_t Utilities::CCodeData<T>::GetLength(void) const
{
	return codeData.size();
}



/**	@brief		Delete all data stored
*	@return								None
*	@exception							None
*	@remarks							The dataset will be empty after the call
*/
template <typename T>
void Utilities::CCodeData<T>::Clear(void)
{
	codeData.clear();
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
	bool operator==(const CCodeData<T>& lhs, const CCodeData<T>& rhs)
	{
		bool result = true;

		if ( lhs.GetTones() != rhs.GetTones() ) {
			result = false;
		}
		if ( lhs.GetToneLengths() != rhs.GetToneLengths() ) {
			result = false;
		}
		if ( lhs.GetTonePeriods() != rhs.GetTonePeriods() ) {
			result = false;
		}
		if ( lhs.GetToneFrequencies() != rhs.GetToneFrequencies() ) {
			result = false;
		}
		if ( lhs.GetAbsToneLevels() != rhs.GetAbsToneLevels() ) {
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
	bool operator!=(const CCodeData<T>& lhs, const CCodeData<T>& rhs)
	{
		return !( lhs == rhs );
	}
}