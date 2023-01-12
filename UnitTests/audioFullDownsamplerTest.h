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
#include <vector>
#include <fstream>
#include <random>
#include <chrono>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "FIRfilter.h"
#include "AudioFullDownsampler.h"

using boost::unit_test::label;


/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

/**	\defgroup	AudioFullDownsampler	Unit test for the class CAudioFullDownsampler<T>
*/

namespace Audio {
	/*@{*/
	/** \ingroup AudioFullDownsampler
	*/
	namespace CAudioFullDownsampler {
		const double samplingFreq = 96000; // Hz		
		const unsigned int downsamplingFactorProc = 3;
		const double cutoffFreqProc = 1.0 / downsamplingFactorProc * samplingFreq / 2.0;
		const double transWidthProc =350; // Hz
		const unsigned int downsamplingFactorRec = 7;
		const double cutoffFreqRec = 1.0 / downsamplingFactorRec * samplingFreq / 2.0;
		const double transWidthRec = 1000; // Hz

		// test section
		BOOST_AUTO_TEST_SUITE( CAudioFullDownsampler_test_suite, *label("default") );

		/**	@brief		Test of set and get functions
		*/
		BOOST_AUTO_TEST_CASE( set_test_case )
		{
			unsigned int receivedDownsamplingFactorProc, receivedDownsamplingFactorRec;
			double receivedCutoffFreqProc, receivedCutoffFreqRec, receivedTransWidthRec, receivedTransWidthProc, receivedSamplingFreq;

			// test constructor
			Core::Audio::CAudioFullDownsampler<float> downsampler( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
			downsampler.GetParameters( receivedDownsamplingFactorProc, receivedCutoffFreqProc, receivedTransWidthProc, receivedDownsamplingFactorRec, receivedCutoffFreqRec, receivedTransWidthRec, receivedSamplingFreq );
			BOOST_REQUIRE( receivedDownsamplingFactorProc == downsamplingFactorProc );
			BOOST_REQUIRE( receivedCutoffFreqProc == cutoffFreqProc );
			BOOST_REQUIRE( receivedTransWidthProc == transWidthProc );			
			BOOST_REQUIRE( receivedDownsamplingFactorRec == downsamplingFactorRec );
			BOOST_REQUIRE( receivedCutoffFreqRec == cutoffFreqRec );
			BOOST_REQUIRE( receivedTransWidthRec == transWidthRec );
			BOOST_REQUIRE( receivedSamplingFreq == samplingFreq );

			// test parameter resetting
			downsampler.SetParameters( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
			downsampler.GetParameters( receivedDownsamplingFactorProc, receivedCutoffFreqProc, receivedTransWidthProc, receivedDownsamplingFactorRec, receivedCutoffFreqRec, receivedTransWidthRec, receivedSamplingFreq );
			BOOST_REQUIRE( receivedDownsamplingFactorProc == downsamplingFactorProc );
			BOOST_REQUIRE( receivedCutoffFreqProc == cutoffFreqProc );
			BOOST_REQUIRE( receivedTransWidthProc == transWidthProc );
			BOOST_REQUIRE( receivedDownsamplingFactorRec == downsamplingFactorRec );
			BOOST_REQUIRE( receivedCutoffFreqRec == cutoffFreqRec );
			BOOST_REQUIRE( receivedTransWidthRec == transWidthRec );
			BOOST_REQUIRE( receivedSamplingFreq == samplingFreq );
		}



		/**	@brief		Test of processing
		*/
		BOOST_AUTO_TEST_CASE( processing_test_case )
		{
			using namespace std;
			using namespace boost::posix_time;
			typedef mt19937::result_type seed_type;

			size_t processedLengthProc, processedLengthRec;
			ptime startTime;
			vector<ptime> time, downsampledTimeProc, downsampledTimeRec, refDownsampledTimeProc, refDownsampledTimeRec;
			vector<float> signal, downsampledSignalProc, downsampledSignalRec, refDownsampledSignalProc, refDownsampledSignalRec, filterParamsProc, filterParamsRec;
			float maxSignal = 0.5f;
			int testSignalLength = 10000;

			// prepare random number generation
			mt19937 generator( static_cast<seed_type>( std::chrono::high_resolution_clock::now().time_since_epoch().count() ) );
			auto random = bind( uniform_real_distribution<float>( -maxSignal, maxSignal ), generator );

			// generate test data
			time.resize( testSignalLength );
			signal.resize( testSignalLength );
			startTime = ptime( microsec_clock::universal_time() );
			for (size_t i=0; i < time.size(); i++) {
				time[i] = startTime + microseconds( static_cast<long>( i / samplingFreq * 1.0e6 ) );
				signal[i] = random();
			}

			// perform processing
			Core::Audio::CAudioFullDownsampler<float> downsampler( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
			downsampler.GetProcessedLengths( time.size(), processedLengthProc, processedLengthRec );
			downsampler.PerformDownsampling( time.begin(), time.end(), signal.begin(), back_inserter( downsampledTimeProc ), back_inserter( downsampledSignalProc ), back_inserter( downsampledTimeRec ), back_inserter( downsampledSignalRec ) );

			// perform reference calculations
			Core::Processing::Filter::CFIRfilter<float>::DesignLowPassFilter( static_cast<float>( transWidthProc ), static_cast<float>( cutoffFreqProc ), static_cast<float>( samplingFreq ), back_inserter( filterParamsProc ) );
			Core::Processing::Filter::CFIRfilter<float> filterProc( filterParamsProc.begin(), filterParamsProc.end(), downsamplingFactorProc, 1, 1e-7f );
			filterProc.Processing( time.begin(), time.end(), signal.begin(), back_inserter( refDownsampledTimeProc ), back_inserter( refDownsampledSignalProc ) );

			Core::Processing::Filter::CFIRfilter<float>::DesignLowPassFilter( static_cast<float>( transWidthRec ), static_cast<float>( cutoffFreqRec ), static_cast<float>( samplingFreq ), back_inserter( filterParamsRec ) );
			Core::Processing::Filter::CFIRfilter<float> filterRec( filterParamsRec.begin(), filterParamsRec.end(), downsamplingFactorRec, 1, 1e-7f );
			filterRec.Processing( time.begin(), time.end(), signal.begin(), back_inserter( refDownsampledTimeRec ), back_inserter( refDownsampledSignalRec ) );
			
			// check for correctness
			BOOST_REQUIRE( downsampledTimeProc.size() == processedLengthProc );
			BOOST_REQUIRE( downsampledTimeRec.size() == processedLengthRec );
			BOOST_REQUIRE( downsampledTimeProc == refDownsampledTimeProc );
			BOOST_REQUIRE( downsampledTimeRec == refDownsampledTimeRec );
			BOOST_REQUIRE( downsampledSignalProc == refDownsampledSignalProc );
			BOOST_REQUIRE( downsampledSignalRec == refDownsampledSignalRec );
			BOOST_REQUIRE( downsampledTimeProc.size() == downsampledSignalProc.size() );
			BOOST_REQUIRE( downsampledTimeRec.size() == downsampledSignalRec.size() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
