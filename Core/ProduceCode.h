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
#pragma once

#include <boost/math/constants/constants.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include "math.h"
#include "FIRfilter.h"


/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace General {
		/** \ingroup Core
		*	Base class for generation of tone codes.
		*/
		template <class T> class CProduceCode
		{
		public:
			CProduceCode(void);
			CProduceCode(double samplingFreq);
			~CProduceCode(void);
		protected:
			CProduceCode(const CProduceCode &);					// prevent copying
    		CProduceCode & operator= (const CProduceCode &);	// prevent assignment
			template <class Out_It> Out_It GenerateSinusTone(double amplitude, double freq, double toneLength, Out_It signalBegin);
			template <class Out_It> Out_It GenerateWhiteNoise(T SNR, bool isWhiteNoise, T refAmpl, double tLength, T maxToneAmp, double &amplSignal, Out_It noiseSignalBegin);
			template <class In_It, class Out_It> Out_It FrequencyBias(In_It inputSignalBegin, In_It inputSignalEnd, Out_It filteredSignalBegin);

			double samplingFreq;
			std::vector<double> biasFilterParams;
		};
	}
}
/*@}*/



/** @brief	Standard constructor.
*/
template <class T> Core::General::CProduceCode<T>::~CProduceCode()
{
}



/** @brief	Standard destructor.
*/
template <class T> Core::General::CProduceCode<T>::CProduceCode()
{
}



/**	@brief		Constructor for initialization of all parameters.
*	@param		samplingFreq						Sampling frequency [Hz]
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T>  Core::General::CProduceCode<T>::CProduceCode(double samplingFreq)
{
	CProduceCode<T>::samplingFreq = samplingFreq;
}



/**	@brief		Generator for white noise.
*	@param		SNR									Signal-to-noise ratio (power ratio) requested [dB]
*	@param		isWhiteNoise						Flag stating if white noise is required. If not, only the signal amplitude "amplSignal" will be calculated and zeros will be returned in the container.
*	@param		refAmpl								Maximum amplitude possible in the given audio device (for example 1.0 for Portaudio with datatype "float")
*	@param		tLength								Overall length of the white noise signal [s]
*	@param		maxToneAmp							Maximum deviation from the reference amplitude [%]
*	@param		amplSignal							Amplitude of the sinus signals of the code required for obtaining the requested SNR. The noise and the sinus amplitude can be added and will give the reference amplitude as a maximum.
*	@param		noiseSignalBegin					Iterator to the beginning of the container for the white noise. The container must have the size ( samplingFreq * tLength + 1 ) or std::back_inserter should be used.
*	@return 										Iterator to one element after the end of the container for the white noise
*	@exception 										None
*	@remarks 										This function generates a white noise signal with an amplitude that allows in combination with the returned signal amplitude to generate a sinus (or similar) signal with white noise at a given SNR and a given total amplitude.
*/
template <class T> template <class Out_It> Out_It Core::General::CProduceCode<T>::GenerateWhiteNoise(T SNR, bool isWhiteNoise, T refAmpl, double tLength, T maxToneAmp, double &amplSignal, Out_It noiseSignalBegin)
{
	using namespace std;

	vector<T> time, noiseSignal;
	double amplNoise;

	// convert from dB
	if ( SNR > 10 * log10( numeric_limits<T>::max() ) ) {
		isWhiteNoise = false;
	}
	SNR = static_cast<T>( pow( 10.0, SNR / 10.0 ) );

	// initialize random number generator
	mt19937 eng( static_cast<unsigned long>( std::time( nullptr ) ) );
	uniform_real_distribution<T> dist( static_cast<T>( -1.0 ), static_cast<T>( 1.0 ) );
	auto random = bind( dist, eng );

	// calculate the required amplitudes of noise and signal - using the equation system (I) aS + aN = refAmpl, (II) ( aS^2 * 1 / sqrt(2) ) / ( aN^2 * 1 / sqrt(3) ) = SNR
	amplNoise = refAmpl * sqrt(2.0) * ( sqrt( sqrt( 6.0 ) ) * sqrt( SNR ) - sqrt(3.0) ) / ( 2.0 * ( SNR - sqrt(6.0) / 2.0 ) );
	if ( !isWhiteNoise ) {
		amplSignal = refAmpl;
	} else {
		amplSignal = refAmpl - amplNoise;
	}

	// generate white noise
	time.resize( static_cast<int>( samplingFreq * tLength + 1  ) );
	noiseSignal.resize( static_cast<int>( samplingFreq * tLength + 1 ) );
	if ( isWhiteNoise ) {
		for (size_t i=0; i < noiseSignal.size(); i++) {
			time[i] = static_cast<T>( i / samplingFreq );
			noiseSignal[i] = static_cast<T>( ( 1 + maxToneAmp / 100.0f ) * amplNoise * random() );
		}
	}

	// set output values
	return std::move( noiseSignal.begin(), noiseSignal.end(), noiseSignalBegin );
}



/**	@brief		Generator for a sinus tone.
*	@param		amplitude							Amplitude of the sinus tone
*	@param		freq								Frequency of the sinus tone [Hz]
*	@param		toneLength							Length of the sinus tone [s]
*	@param		signalBegin							Iterator to the beginning of the container with the sinus tone
*	@return 										Iterator to one element after the end of the container for the sinus tone
*	@exception 										None
*	@remarks 										None
*/
template <class T> template <class Out_It> Out_It Core::General::CProduceCode<T>::GenerateSinusTone(double amplitude, double freq, double toneLength, Out_It signalBegin)
{
	using namespace std;

	vector<T> signal;

	// generate sinus signal
	signal.resize( static_cast<int>( samplingFreq * toneLength ) );
	for (size_t i=0; i < signal.size(); i++) {
		signal[i] = static_cast<T>( amplitude * sin( 2 * boost::math::constants::pi<T>() * freq * i / samplingFreq ) );
	}

	// set output values
	return std::move( signal.begin(), signal.end(), signalBegin );
}



/**	@brief		Filtering of a signal in order to simulate a frequency bias of an audio input device.
*	@param		inputSignalBegin					Iterator to the beginning of a container with the input signal
*	@param		inputSignalEnd						Iterator to one element after the end of the container with the input signal	
*	@param		filteredSignalBegin					Iterator to the beginning of the container for the signal with the frequency bias
*	@return 										Iterator to one element after the end of the container for the signal with the frequency bias
*	@exception 										None
*	@remarks 										With this function the influece of a non-perfect audio device can be simulated.
*/
template <class T> template <class In_It, class Out_It> Out_It Core::General::CProduceCode<T>::FrequencyBias(In_It inputSignalBegin, In_It inputSignalEnd, Out_It filteredSignalBegin)
{
	using namespace std;

	vector<T> filteredSignal, inputSignal, biasFilterParams;
	Processing::Filter::CFIRfilter<T> biasFilter;

	// read input data
	inputSignal.reserve( distance( inputSignalBegin, inputSignalEnd ) );
	while ( inputSignalBegin != inputSignalEnd ) {
		inputSignal.push_back( *(inputSignalBegin++) );
	}
	biasFilterParams.resize( CProduceCode<T>::biasFilterParams.size() );
	for (size_t i=0; i < biasFilterParams.size(); i++) {
		biasFilterParams[i] = static_cast<T>( CProduceCode<T>::biasFilterParams[i] );
	}

	// filter signal
	filteredSignal.resize( inputSignal.size() );
	biasFilter.SetParams( biasFilterParams.begin(), biasFilterParams.end() );
	biasFilter.Processing( inputSignal.begin(), inputSignal.end(), filteredSignal.begin() );
	
	// write output data
	for (size_t i=0; i < filteredSignal.size(); i++) {
		*(filteredSignalBegin++) = filteredSignal[i];
	}

	return filteredSignalBegin;
}