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
/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

#include <iostream>
#include <fstream>
#include <math.h>
#include <array>
#include <vector>
#include <complex>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "basicFunctions.h"
#include "FFT.h"

using boost::unit_test::label;


/**	\defgroup	fftTests	Unit tests for the FFT-library.
*/

/*@{*/
/** \ingroup fftTests
*/
namespace FFTTests {
	const float maxErrorAllowed = 1e-6f;					// acceptable for float precision
	const double samplingFreq = 44100;						// in Hz
	const int length = 256;									// number of samples
	const float maxTestSignalAmpl = 10.0f;					// maximum amplitude of generated test signals
	const int numRepetitions = 100000;						// number of repetitions of FFT-calculation for better statistics of fast calculations
	const std::string fftResultFileName = "result.txt";		// file name of the FFT results
	const std::string fftSignalFileName = "fft_signal.txt";	// file name of the signal used for the FFT test
	


	// Test section
	BOOST_AUTO_TEST_SUITE( fft_test_suite, *label("basic") );

	// Amplitude FFT calculation
	BOOST_AUTO_TEST_CASE( amplitude_fft_test_case )
	{
		using namespace std;

		double time_Alglib;
		vector< boost::posix_time::ptime > time;
		vector<float> f_Alglib, spectrum_Alglib, signal;
		Core::Processing::CFFT<float> fft;
		boost::posix_time::ptime startTime;
		boost::posix_time::time_duration executionTime;

		// generate test data
		GenerateTestData( length, maxTestSignalAmpl, samplingFreq, back_inserter( time ), back_inserter( signal ) );

		// perform Alglib-FFT calculation
		f_Alglib.resize( signal.size() );
		spectrum_Alglib.resize( signal.size() );
		fft.Init( static_cast<int>( signal.size() ) );
		startTime = boost::posix_time::ptime( boost::posix_time::microsec_clock::local_time() );
		for (int i=0; i < numRepetitions; i++) {
			fft.AmplitudeFFT( f_Alglib.begin(), spectrum_Alglib.begin(), signal.begin(), signal.end(), samplingFreq );
		}
		executionTime = boost::posix_time::ptime( boost::posix_time::microsec_clock::local_time() ) - startTime;
		time_Alglib = executionTime.total_milliseconds() / static_cast<double>( numRepetitions );
		
		// save results
		ofstream out( fftResultFileName );
		for (size_t i=0; i < spectrum_Alglib.size(); i++) {
			out << f_Alglib[i] << "\t" << spectrum_Alglib[i] << "\n";
		}
		out.close();

		ofstream out2( fftSignalFileName );
		for (size_t i=0; i < signal.size(); i++) {
			out2 << signal[i] << "\n";
		}
		out2.close();
	}

	

	BOOST_AUTO_TEST_CASE( inverse_fft_test_case )
	{
		using namespace std;

		vector< boost::posix_time::ptime > time;
		vector<float> f, time2, signal2, diff, signal;
		vector< complex<float> > fourier;
		Core::Processing::CFFT<float> fft;
		
		// generate test data
		GenerateTestData( length, maxTestSignalAmpl, samplingFreq, back_inserter( time ), back_inserter( signal ) );
	
		// perform FFT calculation
		fft.Init( static_cast<int>( signal.size() ) );
		f.resize( signal.size() );
		fourier.resize( signal.size() );
		fft.ComplexFFT( f.begin(), fourier.begin(), signal.begin(), signal.end(), samplingFreq );

		// perform inverse FFT calculation
		time2.resize( fourier.size() );
		signal2.resize( fourier.size() );
		fft.InverseFFT( fourier.begin(), fourier.end(), time2.begin(), signal2.begin(), samplingFreq );
		transform( signal2.begin(), signal2.end(), signal.begin(), back_inserter( diff ), [](float val1, float val2){ return ( std::abs( val1 - val2 ) ); } );

		BOOST_REQUIRE( *max_element( diff.begin(), diff.end() ) < maxErrorAllowed );
	}



	BOOST_AUTO_TEST_CASE( stft_test_case )
	{
		using namespace std;

		int stepLength = 50;
		const int numFreqs = 401;
		float overlap = 0.0;
		vector< boost::posix_time::ptime > time;
		vector<float> f_STFT, time_STFT, signal;
		Core::Processing::CFFT<float> fft;
		std::vector< array<float, numFreqs> > spectrum_STFT;
	
		// generate test data
		GenerateTestData( length, maxTestSignalAmpl, samplingFreq, back_inserter( time ), back_inserter( signal ) );

		// perform STFT-transformation (Short Time Fourier Transformation)
		spectrum_STFT.resize( Core::Processing::CFFT<float>::GetNumSpectrogramTimesteps( static_cast<int>( signal.size() ), overlap, stepLength) );
		fft.Init( numFreqs );
		fft.Spectrogram( spectrum_STFT.begin(), back_inserter(f_STFT), back_inserter(time_STFT), signal.begin(), signal.end(), stepLength, overlap, samplingFreq );
		
		// save results
		ofstream out("STFT_time.txt");
		for (size_t i=0; i < time_STFT.size(); i++) {
			out << time_STFT[i] << "\n";
		}
		out.close();

		ofstream out2("STFT_freq.txt");
		for (size_t i=0; i < f_STFT.size() / 2; i++) {
			out2 << f_STFT[i] << "\n";
		}
		out2.close();

		ofstream out3("STFT_data.txt");
		for (size_t i=0; i < spectrum_STFT.size(); i++) {
			for (size_t j=0; j < spectrum_STFT[i].size() / 2; j++) {
				out3 << spectrum_STFT[i][j] << "\t";
			}
			out3 << "\n";
		}
		out3.close();	
	}
	
	BOOST_AUTO_TEST_SUITE_END();
}

/*@}*/
/*@}*/