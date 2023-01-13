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
#include <vector>
#include <string>
#include <cmath>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include "FMEGenerateParam.h"
#include "ProduceFMECode.h"

/*@{*/
/** \ingroup UnitTests
*/

/*@{*/
/** \ingroup FMEdetectionTests
*/

namespace FMEdetectionTests {
	using namespace boost::accumulators;

	const unsigned int toneZero = 10;		// tone used as tone "0" is the standard TR-BOS FME
	const unsigned int toneRepetition = 11;	// tone used as repetition tone "R" in the standard TR-BOS FME

	/**	\ingroup FMEdetectionTests
	*	Class performing statistical analysis of FME detection deviations
	*/
	template <class T> class CStatisticalAnalysis
	{
	public:
		CStatisticalAnalysis() {};
		CStatisticalAnalysis( const std::string& fmeProductionParamsFileName, const T& minHistogramSNR, const T& maxHistogramSNR, const unsigned int& numBins );
		virtual ~CStatisticalAnalysis(void) {};
		void PushValues( std::vector<T> foundFreq, std::vector<T> givenFreqDev, std::vector<T> foundLength, std::vector<T> givenLengthDev, std::vector<T> foundCycle, std::vector<T> givenCycleDev, std::vector<T> absToneLevel, std::vector<int> code, T SNR );
		void GetFreqStatistics(T& minimum, T& maximum, T& meanVal);
		void GetLengthStatistics(T& minimum, T& maximum, T& meanVal);
		void GetCycleStatistics(T& minimum, T& maximum, T& meanVal);
		void GetToneLevelStatistics( std::vector< std::tuple<T, T, T, T, T> >& histogram );
	protected:
		double GetToneFreq(unsigned int toneIndex, std::vector<int> code);

		accumulator_set< T, features< tag::min, tag::max, tag::mean > > accumulatorFreq;
		accumulator_set< T, features< tag::min, tag::max, tag::mean > > accumulatorLength;
		accumulator_set< T, features< tag::min, tag::max, tag::mean > > accumulatorCycle;
		template<typename U> using LevelAccType = accumulator_set< U, features< tag::min, tag::max, tag::mean, tag::variance > >;
		std::vector< LevelAccType<T> > accumulatorsLevel;

		double toneLength;
		std::vector<double> toneFreq;
		T minHistogramSNR;
		T maxHistogramSNR;
		unsigned int numBins;
	};
}
/*@}*/
/*@}*/



/** @brief		Constructor
*/
template <class T>
FMEdetectionTests::CStatisticalAnalysis<T>::CStatisticalAnalysis( const std::string& fmeProductionParamsFileName, const T& minHistogramSNR, const T& maxHistogramSNR, const unsigned int& numBins )
	: minHistogramSNR( minHistogramSNR ),
	  maxHistogramSNR( maxHistogramSNR ),
	  numBins( numBins ),
	  accumulatorsLevel( std::vector< LevelAccType<T> >( numBins ) )
{
	using namespace std;
	double samplingFreq, tLength, pauseTime;
	int lengthFMECode;
	vector<double> biasFilterParams;
	Core::FME::CFMEGenerateParam params;
	Core::FME::CProduceFMECode<float> paramReader;

	// deserialize parameter data from file
	paramReader.LoadParameters( fmeProductionParamsFileName, params );
	params.Get( samplingFreq, tLength, toneLength, pauseTime, lengthFMECode, back_inserter( toneFreq ), back_inserter( biasFilterParams ) );

	if ( toneFreq.size() < toneRepetition ) {
		throw std::length_error( "Container with the tone frequencies is too short." );
	}
}



/** @brief		Loading the detection deviations of a new test into the class
*/
template <class T>
void FMEdetectionTests::CStatisticalAnalysis<T>::PushValues( std::vector<T> foundFreq, std::vector<T> givenFreqDev, std::vector<T> foundLength, std::vector<T> givenLengthDev, std::vector<T> foundCycle, std::vector<T> givenCycleDev, std::vector<T> absToneLevel, std::vector<int> code, T SNR )
{
	unsigned int binID;
	std::vector<T> relToneLevels;

	for ( size_t i = 0; i < foundFreq.size(); i++ ) {
		accumulatorFreq( static_cast<T>( std::abs( foundFreq[i] - ( GetToneFreq( i, code ) * ( 1 + givenFreqDev[i] / 100.0f ) ) ) ) );	// in Hz
		accumulatorLength( static_cast<T>( std::abs( 1000.0f * foundLength[i] - ( 1000.0f * toneLength + givenLengthDev[i] ) ) ) );		// in ms
		if ( foundCycle[i] < 1 ) { // values larger than 1.0s are assumed to be invalid
			accumulatorCycle( static_cast<T>( std::abs( 1000.0f * foundCycle[i] - ( 1000.0f * toneLength + givenCycleDev[i] ) ) ) );		// in ms
		}

		if ( i > 0 ) {
			if ( absToneLevel.front() != 0 ) {
				relToneLevels.push_back( absToneLevel[i] / absToneLevel.front() ); // the first tone is not considered (because the relative tone level is always 1)
			} else {
				relToneLevels.push_back( 0 );
			}
		}
	}

	binID = static_cast<int>( std::floor( numBins * ( SNR - minHistogramSNR ) / ( maxHistogramSNR - minHistogramSNR ) ) );
	for ( const auto& entry : relToneLevels ) {
		if ( binID < numBins ) {
			accumulatorsLevel[binID]( entry );
		} else {
			accumulatorsLevel.back()( entry ); // handles the special case if SNR = maxHistogramSNR
		}
	}
}



/** @brief		Obtaining the frequency deviation statistics for all tests loaded into the class [Hz]
*/
template <class T>
void FMEdetectionTests::CStatisticalAnalysis<T>::GetFreqStatistics(T& minimum, T& maximum, T& meanVal)
{
	using namespace boost::accumulators;

	// obtain statistics
	minimum = min( accumulatorFreq );
	maximum = max( accumulatorFreq );
	meanVal = mean(accumulatorFreq );
}



/** @brief		Obtaining the tone length deviation statistics for all tests loaded into the class [ms]
*/
template <class T>
void FMEdetectionTests::CStatisticalAnalysis<T>::GetLengthStatistics(T& minimum, T& maximum, T& meanVal)
{
	using namespace boost::accumulators;

	// obtain statistics
	minimum = min( accumulatorLength );
	maximum = max( accumulatorLength );
	meanVal = mean(accumulatorLength );
}



/** @brief		Obtaining the cycle length deviation statistics for all tests loaded into the class [ms]
*/
template <class T>
void FMEdetectionTests::CStatisticalAnalysis<T>::GetCycleStatistics(T& minimum, T& maximum, T& meanVal)
{
	using namespace boost::accumulators;

	// obtain statistics
	minimum = min( accumulatorCycle );
	maximum = max( accumulatorCycle );
	meanVal = mean(accumulatorCycle );
}



/** @brief		Obtaining the absolute tone level statistics for all tests loaded into the class [-]
*/
template <class T>
void FMEdetectionTests::CStatisticalAnalysis<T>::GetToneLevelStatistics( std::vector< std::tuple<T,T,T,T,T> >& histogram )
{
	using namespace boost::accumulators;
	T currBinSNR;

	// obtain statistics
	for ( size_t i = 0; i < accumulatorsLevel.size(); i++ ) {
		currBinSNR = static_cast<T>( i + 0.5 ) * ( maxHistogramSNR - minHistogramSNR ) / numBins + minHistogramSNR;
		histogram.push_back( std::make_tuple( currBinSNR, min( accumulatorsLevel[i] ), mean( accumulatorsLevel[i] ), max( accumulatorsLevel[i] ), std::sqrt( variance( accumulatorsLevel[i] ) ) ) );
	}
}



/** @brief		Obtaining the frequency of a certain tone in the given code [Hz]
*/
template <class T>
double FMEdetectionTests::CStatisticalAnalysis<T>::GetToneFreq(unsigned int toneIndex, std::vector<int> code)
{
	int currIndex;
	double currToneFreq;

	// consider tone "0"
	for (size_t i=0; i < code.size(); i++) {
		if ( code[i] == 0 ) {
			code[i] = toneZero;
		}
	}

	// consider tone "R" (repetition)
	for (size_t i=1; i < code.size(); i++) {
		if ( code[i] == code[i-1] ) {
			code[i] = toneRepetition;
		}
	}

	// obtain frequency corresponding to the tone
	if ( toneIndex >= code.size() ) {
		throw std::runtime_error( "Tone index is larger than code length." );
	}

	currIndex = code[ toneIndex ] - 1;
	if ( ( currIndex < 0 ) || ( currIndex >= static_cast<int>( toneFreq.size() ) ) ) {
		throw std::length_error( "Code contains invalid numbers (no digits)." );
	}
	currToneFreq = toneFreq[ currIndex ];

	return currToneFreq;
}