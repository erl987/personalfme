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

#include <algorithm>
#include <numeric>
#include <complex>
#include <vector>
#include <type_traits>
#include <boost/math/special_functions/pow.hpp>
#include "libalglib/fasttransforms.h"
#include "DataProcessing.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Processing {
		/**	\ingroup Core
		*	Class for calculation of FFT-transformations.
		*	The AlgLib-library is used for the Fast Fourier Transformations.
		*/
		template <class T> class CFFT
		{
		public:
			CFFT(void);
			CFFT(const int& numSamplesPerBuf);
			template <class InIt, class OutIt1, class OutIt2> void AmplitudeFFT(OutIt1 fFirst, OutIt2 spectrumFirst, InIt signalFirst, InIt signalLast, const double& samplingFreq);
			template <class InIt, class OutIt1, class OutIt2> void ComplexFFT(OutIt1 fFirst, OutIt2 fourierFirst, InIt signalFirst, InIt signalLast, const double& samplingFreq);
			template <class InIt, class OutIt1, class OutIt2> void InverseFFT(InIt fourierFirst, InIt fourierLast, OutIt1 timeFirst, OutIt2 signalFirst, const double& samplingFreq);
			template <class InIt, class OutIt1, class OutIt2, class OutIt3> void Spectrogram(OutIt1 spectrumFirst, OutIt2 freqFirst, OutIt3 timeFirst, InIt signalFirst, InIt signalLast, const int& stepLength, const double& overlap, const double& samplingFreq);
			static int GetNumSpectrogramTimesteps(const int& lengthData, const double& overlap, const int& stepLength);
			void Init(const int& numSamples);
		private:
			int numSamples;
			bool isInit;

			CFFT (const CFFT &); // prevent copying
    		CFFT & operator= (const CFFT &);
			T CalcPDSConversionFactor(const int& windowLength, const double& samplingFreq);
		};
	}
}

/*@}*/


/**
* 	@brief Default constructor.
*/
template <class T> Core::Processing::CFFT<T>::CFFT(void)
	: numSamples(0)
	, isInit(false)
{
}


/**
* 	@brief Constructor.
* 	Initializes the class for use with a certain sample length
*	@param 		numSamplesPerBuf		Length of samples to be processed.
*	@exception 	None
*/
template <class T> Core::Processing::CFFT<T>::CFFT(const int& numSamplesPerBuf)
	: numSamples(0)
	, isInit(false)
{
	Init( numSamplesPerBuf );
}



/**	@brief Perform FFT-transformation.
*	The one-sided amplitude spectrum is calculated.
*	@param 		fFirst 				Iterator to beginning of container for frequencies of the spectrum [Hz]. The length must be CFFT::numSamples.
*	@param 		spectrumFirst 		Iterator to beginning of container for one-sided amplitude spectrum. The length must be CFFT::numSamples. The upper half of the spectrum is set to zero.
*	@param 		signalFirst 		Iterator to beginning of input signal, only the first CFFT::numSamples are processed, all other values are ignored
*	@param		signalLast			Iterator to end of input signal
*	@param 		samplingFreq 		Sampling frequency [Hz]
*	@return 						None
*	@exception 	std::runtime_error	Thrown when CFFT::Init was not called in advance
*	@remarks 						If the input queue is shorter, the rest of the data to be processed will be zero-padded. If it is longer, 
*									only CFFT::numSamples will be processed. The basic type of the containers must be equivalent to the template parameter of the class
*									Use of datatype T=float might be inaccurate if the Fourier transform is very small. Use T=double instead and avoid any conversion of input data to datatype float.
*/
template <class T> template <class InIt, class OutIt1, class OutIt2> void Core::Processing::CFFT<T>::AmplitudeFFT(OutIt1 fFirst, OutIt2 spectrumFirst, InIt signalFirst, InIt signalLast, const double& samplingFreq)
{
	using namespace std;
	using namespace alglib;

	vector<T> f, spectrum, signalIn;
	complex_1d_array out;
	real_1d_array in;

	if ( !isInit ) {
		throw std::runtime_error("Object was not initialized before use!");
	}

	// prepare fourier transformation
	in.setlength( numSamples );
	out.setlength( numSamples );
	signalIn.assign( signalFirst, signalLast );
	signalIn.resize( numSamples );
	for (int i=0; i < numSamples; i++) {
		in[i] = static_cast<double>( signalIn[i] ); // converts all values from desired type to double (for internal purposes)
	}

	fftr1d( in, out );

	// calculate frequencies
	f.resize( numSamples );
	for (int i=0; i < numSamples; i++) {
		f[i] = static_cast<T>( samplingFreq * i / static_cast<double>( numSamples-1 ) );
	}

	// calculate one-sided amplitude spectrum - only for the lower half of the spectrum, the rest is zero
	spectrum.resize( numSamples );
	for (int i=0; i < numSamples / 2; i++) {
		spectrum[i] = static_cast<T>( 2 * abscomplex( out[i] ) / numSamples ); // converts back to datatype T
	}

	// set output iterators
	std::move( f.begin(), f.end(), fFirst );
	std::move( spectrum.begin(), spectrum.end(), spectrumFirst );
}



/**	@brief Perform FFT-transformation.
*	The complex DFT spectrum is calculated.
*	@param 		fFirst 				Iterator to beginning of container for frequencies of the spectrum [Hz]. The length must be CFFT::numSamples.
*	@param 		fourierFirst 		Iterator to beginning of container for the complex output. The length must be CFFT::numSamples, the underlying datatype must be std::complex<T>
*	@param 		signalFirst 		Iterator to beginning of input signal, only the first CFFT::numSamples are processed, all other values are ignored
*	@param		signalLast			Iterator to end of input signal
*	@param 		samplingFreq 		Sampling frequency [Hz]
*	@return 						None
*	@exception 	std::runtime_error	Thrown when CFFT::Init was not called in advance
*	@remarks 						If the input queue is shorter, the rest of the data to be processed will be zero-padded. If it is longer, 
*									only CFFT::numSamples will be processed. The basic type of the containers must be equivalent to the template parameter of the class.
*									Use of datatype T=float might be inaccurate if the Fourier transform is very small. Use T=double instead and avoid any conversion of input data to datatype float.
*/
template <class T> template <class InIt, class OutIt1, class OutIt2> void Core::Processing::CFFT<T>::ComplexFFT(OutIt1 fFirst, OutIt2 fourierFirst, InIt signalFirst, InIt signalLast, const double& samplingFreq)
{
	using namespace std;
	using namespace alglib;

	vector<T> f, spectrum;
	vector< std::complex<T> > fourier;
	complex_1d_array out;
	real_1d_array in;

	if ( !isInit ) {
		throw std::runtime_error("Object was not initialized before use!");
	}

	// prepare fourier transformation
	in.setlength( numSamples );
	out.setlength( numSamples );
	for (int i=0; i < numSamples; i++) {
		if ( signalFirst != signalLast ) {
			in[i] = static_cast<double>( *(signalFirst++) ); // converts all values from desired type to double (for internal purposes)
		} else {
			in[i] = 0;
		}
	}

	fftr1d( in, out );
		
	// calculate frequencies
	f.resize( numSamples );
	for (int i=0; i < numSamples; i++) {
		f[i] = static_cast<T>( samplingFreq * i / static_cast<double>( numSamples-1 ) );
	}

	// convert back using datatype T
	fourier.resize( numSamples );
	for (int i=0; i < numSamples; i++) {
		fourier[i] = std::complex<T>( static_cast<T>( out[i].x ), static_cast<T>( out[i].y ) );
	}

	// set output iterators
	std::move( f.begin(), f.end(), fFirst );
	std::move( fourier.begin(), fourier.end(), fourierFirst );
}



/**	@brief Initializiation.
*	@param 		numSamples 			Length of the samples that will be processed.
*	@return 	None
*	@remarks 						The function is called automatically on construction. Multiple calls of CFFT::Init are possible.
*/
template <class T> void Core::Processing::CFFT<T>::Init(const int& numSamples)
{
	CFFT::numSamples = numSamples;
	isInit = true;
}



/**	@brief	Perform inverse FFT transformation.
*	@param		fourierFirst	Iterator to the beginning of the container with the complex DFT-input data. It must have the datatype std::complex<T>.
*	@param		fourierLast		Iterator to one element after the end of the container with the complex DFT-input data.
*	@param		timeFirst		Iterator the beginning of the time data, which will be obtained from the transformation. Must have the same size as the input data.
*	@param		signalFirst		Iterator the beginning of the obtained signal from the transformation. Must be of same size as the input data.
*	@param		samplingFreq	Sampling frequency [Hz]
*	@return						None
*	@exception					None
*	@remarks					If the fourier container has a different underlying datatype than std::complex<T>, the function will not compile.
*								Exactly CFFT::numSamples will be processed. If less data is given, the rest will be zero-padded. Data of longer containers will be ignored.
*/
template <class T> template <class InIt, class OutIt1, class OutIt2> void Core::Processing::CFFT<T>::InverseFFT(InIt fourierFirst, InIt fourierLast, OutIt1 timeFirst, OutIt2 signalFirst, const double& samplingFreq)
{
	using namespace std;
	using namespace alglib;

	std::complex<T> value;
	std::vector< std::complex<T> > fourier;
	std::vector<T> time, signal;
	complex_1d_array in;
	real_1d_array out;

	if ( !isInit ) {
		throw std::runtime_error("Object was not initialized before use!");
	}

	// ensure std::complex<T> as underlying input datatype
	static_assert( std::is_same< typename iterator_traits<InIt>::value_type, std::complex<T> >::value, "The datatype of the input data is not std::complex<T>." );

	// prepare fourier transformation
	fourier.assign( fourierFirst, fourierLast );
	fourier.resize( numSamples );
	in.setlength( numSamples );
	out.setlength( numSamples );
	for (int i=0; i < numSamples; i++) {
		// converts all values from desired type to double (for internal purposes)
		in[i] = alglib::complex( static_cast<double>( real( fourier[i] ) ), static_cast<double>( imag( fourier[i] ) ) );
	}

	fftr1dinv( in, out );
		
	// calculate times
	time.resize( numSamples );
	for (int i=0; i < numSamples; i++) {
		time[i] = static_cast<T>( i / samplingFreq ); // converts back to datatype T
	}
	signal.resize( out.length() );
	for (int i=0; i < out.length(); i++ ) {
		signal[i] = static_cast<T>( out[i] ); // converts back to datatype T
	}

	// set output iterators
	std::move( time.begin(), time.end(), timeFirst );
	std::move( signal.begin(), signal.end(), signalFirst );
}



/**	@brief	Calculates a three-dimensional spectrogram (time-frequency-power density spectrum) of a signal using Short Time Fourier Transformation (STFT).
*	@param		spectrumFirst		Iterator to the beginning of the two dimensional container with the one-sided power-density spectrum, which will be obtained from the transformation. It must have the same size as the input data. First dimension refers to the time and the second to the frequency. Set the required container sizes manually, as std::back_inserter cannot be used here.
*	@param		freqFirst			Iterator to the beginning of the container with the obtained frequencies. It must have the same size as the signal container, but std::back_inserter can be used.
*	@param		timeFirst			Iterator to the beginning of the time data [s], will be obtained from the transformation. Must have the same size as the signal container, but std::back_inserter can be used. The time is the central time of each page.
*	@param		signalFirst			Iterator to the beginning of the input signal.
*	@param		signalLast			Iterator to the one element after the end of the input signal container.
*	@param		stepLength			Length of the window for the STFT-transformation given in number of samples.
*	@param		overlap				Overlap of the windows for the STFT-transformation given as fraction of the overall window length. 0 <= overlap < 1. Set to 0 if no overlap is required.
*	@param		samplingFreq		Sampling frequency [Hz]
*	@return							None
*	@exception	std::out_of_range	Overlap is not within range (0 <= overlap < 1).
*	@remarks						The number of samples used for the FFT-calculations is controlled by the parameters "numSamples" given on initialization of the class. All output containers except spectrumFirst should be used with std::back_inserter for convenience. 
*									The resulting spectrum is two-dimensional. It contains the power density for all timesteps, i.e. spectrum[5][0..(stepLength-1)] is the spectrum at timestep 5.
*/
template <class T> template <class InIt, class OutIt1, class OutIt2, class OutIt3> void Core::Processing::CFFT<T>::Spectrogram(OutIt1 spectrumFirst, OutIt2 freqFirst, OutIt3 timeFirst, InIt signalFirst, InIt signalLast, const int& stepLength, const double& overlap, const double& samplingFreq)
{
	using namespace std;
	int i, j;
	T k;
	std::vector<T> input, pageInput, f, time, pageSpectrum;
	std::vector< std::vector<T> > spectrum;
	typename std::vector<T>::iterator it;

	// check if overlap is within range
	if ( ( overlap < 0 ) || ( overlap >= 1 ) ) {
		throw std::out_of_range("Variable 'overlap' is out of range");
	}

	// obtain input data
	input.assign( signalFirst, signalLast );

	// prepare calculation of power density spectra
	k = CalcPDSConversionFactor( stepLength, samplingFreq );

	// perform Short Time Fourier Transformation (STFT)
	time.reserve( GetNumSpectrogramTimesteps( static_cast<int>( input.size() ), overlap, stepLength ) );
	spectrum.reserve( GetNumSpectrogramTimesteps( static_cast<int>( input.size() ), overlap, stepLength ) );
	i = 0;
	while ( input.size() > 0 ) {
		// obtain the new page
		if ( static_cast<int>( input.size() ) >= stepLength ) {
			pageInput.assign( input.begin(), input.begin() + stepLength );
			input.erase( input.begin(), input.begin() + static_cast<int>( ( 1 - overlap ) * stepLength ) );
		} else {
			pageInput.assign( input.begin(), input.end() );
			pageInput.resize( stepLength );
			input.clear();
		}
	
		// apply Hamming-window
		CDataProcessing<T>::HammingWindow( pageInput.begin(), pageInput.end(), pageInput.begin() );		

		// resize page for correct frequency resolution as requested
		pageInput.resize( numSamples );

		// calculate Fourier transformation for the page
		f.resize( numSamples );
		pageSpectrum.resize( numSamples );
		AmplitudeFFT( f.begin(), pageSpectrum.begin(), pageInput.begin(), pageInput.end(), samplingFreq );

		// convert one-sided amplitude to DFT and then to one-sided power density spectrum [power/Hz]
		transform( pageSpectrum.begin(), pageSpectrum.end(), pageSpectrum.begin(), [=](T val){ return ( boost::math::pow<2>( val * numSamples / 2.0 ) * k ); } );
		pageSpectrum.front() /= 2;
		it = find_if( f.begin(), f.end(), [=](T val){ return ( val >= samplingFreq / 2 ); } );
		pageSpectrum[ distance( f.begin(), it ) ] /= 2;
		pageSpectrum.back() /= 2;

		// add the spectrum of the page to the overall result data container
		spectrum.push_back( pageSpectrum );

		// calculate time
		time.push_back( static_cast<T>( ( 0.5 + i * ( 1 - overlap ) ) * stepLength / samplingFreq ) );	// converts back to datatype T
		i++;
	}

	// set output containers
	for (size_t i = 0; i < time.size(); i++) {
		j = 0;
		for ( typename std::iterator_traits<OutIt1>::value_type::iterator it = spectrumFirst->begin(); it != spectrumFirst->end(); it++ ) {
			*it = spectrum[i][j];
			j++;
		}
		spectrumFirst++;
	}
	std::move( time.begin(), time.end(), timeFirst );
	std::move( f.begin(), f.end(), freqFirst );
}



/**	@brief	Calculates the number of datapoints, which will be returned by the function CFFT<T>::Spectrogram in a convenient and fast way. It can be used to preset the length of the power density spectrum container.
*	@param		lengthData			Length of the data container (number of samples).
*	@param		overlap				Overlap of the windows for the STFT-transformation given as fraction of the overall window length. See CFFT<T>::Spectrogram for details.
*	@param		stepLength			Length of the window for the STFT-transformation given in number of samples.
*	@return							Number of timesteps, which will be returned by CFFT<T>::Spectrogram and can be used to preset the length of the power density spectrum container.
*	@exception						None
*	@remarks						None
*/
template <class T> int Core::Processing::CFFT<T>::GetNumSpectrogramTimesteps(const int& lengthData, const double& overlap, const int& stepLength)
{
	int numTimesteps = 0;
	int lengthDataResidual = lengthData;

	// determine number of timesteps by iteration
	while ( lengthDataResidual > 0 ) {
		numTimesteps++;		
		if ( lengthDataResidual >= stepLength ) {
			lengthDataResidual -= static_cast<int>( ( 1 - overlap ) * stepLength );
		} else {
			break;
		}
	}

	return numTimesteps;
}



/**	@brief	Calculates the factor k required for conversion of amplitude spectrum to power density spectrum.
*	@param		windowLength		Number of samples of the used window.
*	@param		samplingFreq		Sampling frequency [Hz].
*	@return							Conversion factor.
*	@exception						None
*	@remarks						The conversion factor is valid for: PSD = abs(DFT)^2 * k (both one-sided). It must be divided by a factor of two for zero frequency and the Nyquist frequencies manually.
*/
template <class T> T Core::Processing::CFFT<T>::CalcPDSConversionFactor(const int& windowLength, const double& samplingFreq)
{
	using namespace std;
	T k;
	std::vector<T> hamming( windowLength, 1 );

	// calculate factor k for transformation to one-sided PSD
	CDataProcessing<T>::HammingWindow( hamming.begin(), hamming.end(), hamming.begin() );
	transform( hamming.begin(), hamming.end(), hamming.begin(), [](T val){ return ( boost::math::pow<2>( val ) ); } );
	k = static_cast<T>( 2 / accumulate( hamming.begin(), hamming.end(), static_cast<T>( 0 ) ) / samplingFreq );

	return k;
}
