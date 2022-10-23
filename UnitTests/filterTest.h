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

#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include "basicFunctions.h"
#include "IIRfilter.h"
#include "FIRfilter.h"

using boost::unit_test::label;


/**	\defgroup	FilterTests		Unit tests for the filtering functions in the Core-library.
*/

/*@{*/
/** \ingroup FilterTests
*/
namespace FilterTests {
	const std::string testDataFileName = "signal.txt";
	const std::string matlabProcessedFileName = "matlabProcessed.txt";
	const std::string aFilterParamFileName = "a.txt";
	const std::string bFilterParamFileName = "b.txt";
	const std::string downsamplingFactorFileName = "downsamplingFactor.txt";
	const std::string upsamplingFactorFileName = "upsamplingFactor.txt";
	const std::string isFIRfileName = "isFIR.txt";
	const std::string outputFileName = "cppProcessed.txt";
	const double maxAllowedError = 1e-7;	// maximum absolute deviation between CFilter and MATLAB results to be accepted as correct
	const int minStep = 0;					// minimum random step size for performing the filtering
	const int maxStep = 2000;				// maximum random step size for performing the filtering


	/** @brief		Initialize / teardown class for all tests
	*/
	class Fixture {
	public:
		Fixture() { 
			#ifdef __linux
				boost::filesystem::current_path( "./UnitTests" );
			#endif
		}
		~Fixture() {
			#ifdef __linux
				boost::filesystem::current_path( ".." );
			#endif
		}
	};


	/** @brief		Preparing the tests by performing MATLAB calculations and loading the results.
	*	@param		matlabTestFileName		Name of the MATLAB script performing the reference calculations in MATLAB
	*	@param		a						a-Filter parameter container
	*	@param		b						b-Filter paramater container
	*	@param		signal					Signal data container
	*	@param		matlabProcessed			MATLAB filtered / resampled data
	*	@param		downsamplingFactor		Downsampling factor
	*	@param		upsamplingFactor		Upsampling factor
	*	@param		isFIR					Flag stating if FIR- (true) or IIR-filtering (false) will be performed
	*	@return								None
	*	@exception							None
	*	@remarks							None
	*/
	void PrepareTests(std::string matlabTestFileName, std::vector<double>& a, std::vector<double>& b, std::vector<double>& signal, std::vector<double>& matlabProcessed, int& downsamplingFactor, int& upsamplingFactor, bool& isFIR)
	{
		using namespace std;

		auto callResult = system( ( octaveExe + " " + matlabTestFileName ).c_str() );

		// load filter parameters
		LoadVector( aFilterParamFileName, a );
		LoadVector( bFilterParamFileName, b );
		
		// load test data
		LoadVector( testDataFileName, signal );

		// load MATLAB processed data
		LoadVector( matlabProcessedFileName, matlabProcessed );

		// load resampling factors
		LoadSingleNumber( downsamplingFactorFileName, downsamplingFactor );
		LoadSingleNumber( upsamplingFactorFileName, upsamplingFactor );

		// load filter type
		LoadSingleNumber( isFIRfileName, isFIR );
	}



	/** @brief		Calculating the maximum difference between CFilter and MATLAB-filtered results.
	*	@param		cppResultFirst			Iterator to the beginning of the container with the CFilter-results
	*	@param		cppResultLast			Iterator to one element after the end of the container with the CFilter-results
	*	@param		matlabResultFirst		Iterator to the beginning of the container storing the MATLAB-results. It must have at least the size of the cppResult-container.
	*	@return								Maximum absolute difference.
	*	@exception							None
	*	@remarks							None
	*/
	template <class InIt1, class InIt2> double CalcMaxError(InIt1 cppResultFirst, InIt1 cppResultLast, InIt2 matlabResultFirst)
	{
		using namespace std;
		double error;
		vector<double> difference, cppResult, matlabResult;

		// obtain input data
		cppResult.reserve( distance( cppResultFirst, cppResultLast ) );
		matlabResult.reserve( distance( cppResultFirst, cppResultLast ) );
		while ( cppResultFirst != cppResultLast ) {
			cppResult.push_back( *(cppResultFirst++) );
			matlabResult.push_back( *(matlabResultFirst++ ) );
		}

		// search maximum error
		transform( cppResult.begin(), cppResult.end(), matlabResult.begin(), back_inserter( difference ), []( double val1, double val2 ){ return ( std::abs( val1 - val2 ) ); } );
		error = *max_element( difference.begin(), difference.end() );

		return error;
	}



	/** @brief		Performing the CFilter-testing
	*	@param		matlabTestFileName		Name of the MATLAB-script for performing the MATLAB reference calculations
	*	@return								None
	*	@exception							None
	*	@remarks							This function is the basis for all unit tests of CFilter and derivatives.
	*/
	double PerformTesting(std::string matlabTestFileName)
	{
		using namespace std;

		vector<double> a, b, signal, downsampledSignal, matlabProcessed;
		bool isFIR;
		int oldIndex, newIndex, downsamplingFactor, upsamplingFactor;
		unique_ptr< Core::Processing::Filter::CFilter<double> > filter;
		random_device rd;

		// prepare random number generation
		default_random_engine generator( rd() );
		function<int()> random = std::bind( std::uniform_int_distribution<>( minStep, maxStep ), generator );

		// prepare testing
		PrepareTests( matlabTestFileName, a, b, signal, matlabProcessed, downsamplingFactor, upsamplingFactor, isFIR );

		// perform testing with random data snippets
		if ( isFIR ) {
			filter.reset( new Core::Processing::Filter::CFIRfilter<double>( b.begin(), b.end(), downsamplingFactor, upsamplingFactor ) );
		} else {
			filter.reset( new Core::Processing::Filter::CIIRfilter<double>( a.begin(), a.end(), b.begin(), b.end(), downsamplingFactor, upsamplingFactor ) );
		}
		downsampledSignal.reserve( signal.size() );
		oldIndex = 0;
		newIndex = random();
		while ( newIndex <= static_cast<int>( signal.size() ) ) {
			filter->Processing( signal.begin() + oldIndex, signal.begin() + newIndex, back_inserter( downsampledSignal ) );
			oldIndex = newIndex;
			newIndex += random();
		}

		// store results
		ofstream out( outputFileName );
		for (size_t i=0; i < downsampledSignal.size(); i++) {
			out << setprecision(14) << downsampledSignal[i] << endl;
		}
		out.close();

		// calculate maximum difference to MATLAB result
		return CalcMaxError( downsampledSignal.begin(), downsampledSignal.end(), matlabProcessed.begin() );
	}



	// Test section - the specific filter settings are defined by the MATLAB script
	BOOST_FIXTURE_TEST_SUITE( Filter_test_suite, Fixture, *label("basic") )

	BOOST_AUTO_TEST_CASE( transition_width_test_case )
	{
		using namespace Core::Processing::Filter;
		using namespace std;
		double samplingFreq = 96000;
		int filterOrder = 80;
		
		// check typical input
		BOOST_REQUIRE( std::abs( ( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.33, samplingFreq ) * samplingFreq - 3528.5900775343180 ) ) < 1e-5 );
		BOOST_REQUIRE( std::abs( ( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.05, samplingFreq ) * samplingFreq - 2180.6421607732773 ) ) < 1e-5 );
		BOOST_REQUIRE( std::abs( ( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.33, 1280 ) * 1280 - 45.461992835998586 ) ) < 1e-5 ); // smallest possible sampling frequency allowed for the chosen filter
		BOOST_REQUIRE( std::abs( ( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.99, samplingFreq ) * samplingFreq - 3526.4762770570810 ) ) < 1e-5 );
		BOOST_REQUIRE( std::abs( ( CFIRfilter<double>::CalcTransitionWidth( 4, 0.8, samplingFreq ) * samplingFreq - 34390.101018548012 ) ) < 1e-5 );

		// check error conditions
		BOOST_CHECK_THROW( CFIRfilter<double>::CalcTransitionWidth( filterOrder + 1, 0.33, samplingFreq ), std::length_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::CalcTransitionWidth( filterOrder, -0.33, samplingFreq ), std::range_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.33, -samplingFreq ), std::range_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::CalcTransitionWidth( filterOrder, 0.33, 1279 ), std::runtime_error ); // largest sampling frequency not allowed for the chosen filter
	}



	BOOST_AUTO_TEST_CASE( digital_filter_gain_test_case )
	{
		using namespace Core::Processing::Filter;
		using namespace std;
		vector<double> b, gainAbs, dummy;
		vector< complex<double> > gain;

		// check typical filter
		CFIRfilter<double>::DesignLowPassFilter( 80, 0.33, back_inserter( b ) );
		CFIRfilter<double>::DigitalFilterGain( 2001, 96000, b.begin(), b.end(), back_inserter( gain ) );
		transform( gain.begin(), gain.end(), back_inserter( gainAbs ), []( complex<double> val ){ return ( std::abs( val ) ); } );
		BOOST_REQUIRE( std::abs( gainAbs[20] - 1.0006354603495715 ) < 1e-5 ); // 480 Hz
		BOOST_REQUIRE( std::abs( gainAbs[600] - 0.28122926175388685 ) < 1e-5 ); // 14400 Hz
		BOOST_REQUIRE( std::abs( gainAbs[800] - 0.00032833168682757394 ) < 1e-5 ); // 19200 Hz

		// check close to limit conditions
		gain.clear();
		gainAbs.clear();
		CFIRfilter<double>::DigitalFilterGain( 33, 96000, b.begin(), b.end(), back_inserter( gain ) ); // the number of data points is as close as possible to the lower limit
		transform( gain.begin(), gain.end(), back_inserter( gainAbs ), []( complex<double> val ){ return ( std::abs( val ) ); } );
		BOOST_REQUIRE( std::abs( gainAbs[0] - 1.0000000000000000 ) < 1e-5 ); // 0 Hz
		BOOST_REQUIRE( std::abs( gainAbs[10] - 0.15479407159828981 ) < 1e-5 ); // 15000 Hz

		gain.clear();
		gainAbs.clear();
		CFIRfilter<double>::DigitalFilterGain( 2001, 5, b.begin(), b.end(), back_inserter( gain ) ); // the sampling frequency is very small
		transform( gain.begin(), gain.end(), back_inserter( gainAbs ), []( complex<double> val ){ return ( std::abs( val ) ); } );
		BOOST_REQUIRE( std::abs( gainAbs[600] - 0.28122926175388685 ) < 1e-5 ); // 0.75 Hz

		// check error conditions
		BOOST_CHECK_THROW( CFIRfilter<double>::DigitalFilterGain( 32, 96000, b.begin(), b.end(), back_inserter( gain ) ), std::range_error ); // the number of data points is too small
		BOOST_CHECK_THROW( CFIRfilter<double>::DigitalFilterGain( 2001, -96000, b.begin(), b.end(), back_inserter( gain ) ), std::range_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::DigitalFilterGain( 2001, 96000, dummy.begin(), dummy.end(), back_inserter( gain ) ), std::runtime_error );
	}



	BOOST_AUTO_TEST_CASE( filter_transition_width_design_test_case )
	{
		using namespace std;
		vector<double> b;
		double samplingFreq = 96000;
	
		// check typical inputs
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 350, 3200, samplingFreq, back_inserter( b ) );
		BOOST_REQUIRE( ( b.size() - 1 ) == 812 );

		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 500, 3000, samplingFreq, back_inserter( b ) );
		BOOST_REQUIRE( ( b.size() - 1 ) == 566 );

		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 200, 500, samplingFreq, back_inserter( b ), 500, 4000 );
		BOOST_REQUIRE( ( b.size() - 1 ) == 1418 );

		// check very small transition width
		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 1, 3000, samplingFreq, back_inserter( b ), 500, 1500 );
		BOOST_REQUIRE( ( b.size() - 1 ) == 1500 );

		// check very large transition width
		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 2704, 3000, samplingFreq, back_inserter( b ) );
		BOOST_REQUIRE( ( b.size() - 1 ) == 58 );

		// check very small cutoff frequency
		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 1, 400, samplingFreq, back_inserter( b ), 800 );
		BOOST_REQUIRE( ( b.size() - 1 ) == 1000 );

		// check very large cutoff frequency
		b.clear();
		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 3019, 0.98 * ( 2 * samplingFreq ) / 2, 2 * samplingFreq, back_inserter( b ) );
		BOOST_REQUIRE( ( b.size() - 1 ) == 188 );

		// invalid input test cases
		// check out-of-bound cutoff frequency inputs
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 500, -3000, samplingFreq, back_inserter( b ) ), std::range_error );
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 1, 0.6 * samplingFreq, samplingFreq, back_inserter( b ) ), std::range_error );
		
		// check out-of-bound sampling frequency input
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 500, 3000, -samplingFreq, back_inserter( b ) ), std::range_error );
		
		// check out-of-bound transition width input
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( -500, 3000, samplingFreq, back_inserter( b ) ), std::range_error );
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 1.5 * 3000, 3000, samplingFreq, back_inserter( b ) ), std::range_error );

		// check not allowed cutoff frequencies (leading to negative ideal cutoff frequencies)
		BOOST_CHECK_THROW( Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( 1, 5, samplingFreq, back_inserter( b ) ), std::domain_error );
	}



	BOOST_AUTO_TEST_CASE( freq_response_model_test_case )
	{
		using namespace Core::Processing::Filter;
		std::vector<double> b, dummy;
		int filterOrder = 200;
		double normCutoffFreq = 0.33;
		double deltaF = 20;
		double samplingFreq = 96000;
		alglib::spline1dinterpolant freqResponseModel;

		Core::Processing::Filter::CFIRfilter<double>::DesignLowPassFilter( filterOrder, normCutoffFreq, back_inserter( b ) );

		// check typical input
		CFIRfilter<double>::GenerateFreqResponseModel( b.begin(), b.end(), deltaF, samplingFreq, freqResponseModel );
		BOOST_REQUIRE( std::abs( CFIRfilter<double>::GetFreqResponse( 1793.1, freqResponseModel ) - 0.99996531759347573 ) < 1e-5 );

		// checking error conditions
		BOOST_CHECK_THROW( CFIRfilter<double>::GenerateFreqResponseModel( b.begin(), b.end(), -deltaF, samplingFreq, freqResponseModel ), std::range_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::GenerateFreqResponseModel( b.begin(), b.end(), deltaF, -samplingFreq, freqResponseModel ), std::range_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::GenerateFreqResponseModel( b.begin(), b.end(), 2 * samplingFreq, samplingFreq, freqResponseModel ), std::logic_error );
		BOOST_CHECK_THROW( CFIRfilter<double>::GenerateFreqResponseModel( dummy.begin(), dummy.end(), deltaF, samplingFreq, freqResponseModel ), std::runtime_error );

		BOOST_CHECK_THROW( CFIRfilter<double>::GetFreqResponse( -500.0, freqResponseModel ), std::range_error );
		BOOST_REQUIRE( CFIRfilter<double>::GetFreqResponse( 1.05 * samplingFreq / 2.0, freqResponseModel ) == CFIRfilter<double>::GetFreqResponse( 1.0 * samplingFreq / 2.0, freqResponseModel ) ); 
		BOOST_REQUIRE( std::abs( CFIRfilter<double>::GetFreqResponse( 0, freqResponseModel ) - 1.0 ) < 1e-5 );
	}



	BOOST_AUTO_TEST_CASE( iir_filter_high_fc_test_case )
	{
		double error;
		std::string matlabTestFileName = "iirFilterHighFCtestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( fir_filter_upsampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "firFilterUpsamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( iir_filter_upsampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "iirFilterUpsamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( fir_filter_high_fc_test_case )
	{
		double error;
		std::string matlabTestFileName = "firFilterHighFCtestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( fir_filter_low_fc_test_case )
	{
		double error;
		std::string matlabTestFileName = "firFilterLowFCtestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( iir_filter_low_fc_test_case )
	{
		double error;
		std::string matlabTestFileName = "iirFilterLowFCtestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( fir_filter_downsampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "firFilterDownsamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( iir_filter_downsampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "iirFilterDownsamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( fir_filter_resampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "firFilterResamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( iir_filter_resampling_test_case )
	{
		double error;
		std::string matlabTestFileName = "iirFilterResamplingTestCase.m";

		error = PerformTesting( matlabTestFileName );
		BOOST_REQUIRE( error < maxAllowedError );
	}



	BOOST_AUTO_TEST_CASE( empty_data_test_case )
	{
		std::vector<double> filteredSignal;
		std::vector<double> signal( 0 );	// the dataset is empty
		std::vector<double> b( 21, 1 );		// values are of no importance here
		Core::Processing::Filter::CFIRfilter<double> filter ( b.begin(), b.end(), 3, 2 );

		filter.Processing( signal.begin(), signal.end(), std::back_inserter( filteredSignal ) );

		BOOST_REQUIRE( filteredSignal.empty() );
	}



	BOOST_AUTO_TEST_CASE( get_params_case )
	{
		int downsamplingFactorOut, upsamplingFactorOut;
		int downsamplingFactorIn = 3;
		int upsamplingFactorIn = 2;
		std::vector<double> b( 21, 1 ); // values are of no importance here
		Core::Processing::Filter::CFIRfilter<double> filter ( b.begin(), b.end(), downsamplingFactorIn, upsamplingFactorIn );

		filter.GetParams( downsamplingFactorOut, upsamplingFactorOut );

		BOOST_REQUIRE( downsamplingFactorIn == downsamplingFactorOut );
		BOOST_REQUIRE( upsamplingFactorIn == upsamplingFactorOut );
	}



	BOOST_AUTO_TEST_CASE( filter_design_test_case )
	{
		using namespace std;
		using namespace boost::posix_time;
		using namespace boost::gregorian;

		vector<double> a, b, signal, downsampledSignal, matlabProcessed;
		vector< boost::posix_time::ptime > time, downsampledTime;
		int oldIndex, newIndex, downsamplingFactor, upsamplingFactor;
		bool dummy;
		random_device rd;
		string matlabTestFileName = "filterDesignTestCase.m";

		// prepare random number generation
		default_random_engine generator( rd() );
		function<int()> random = std::bind( std::uniform_int_distribution<>( minStep, maxStep ), generator );

		// prepare testing
		PrepareTests( matlabTestFileName, a, b, signal, matlabProcessed, downsamplingFactor, upsamplingFactor, dummy );

		time.resize( signal.size() );
		for (size_t i=0; i < signal.size(); i++) {
			time[i] = ptime( date( 2002, Jan, 10 ), seconds( i ) );
		}

		// perform downsampling
		Core::Processing::Filter::CFIRfilter<double> filter( downsamplingFactor, 1, 1e-7f );
		downsampledTime.reserve( signal.size() );
		downsampledSignal.reserve( signal.size() );
		oldIndex = 0;
		newIndex = random();
		while ( newIndex <= static_cast<int>( signal.size() ) ) {
			filter.Processing( time.begin() + oldIndex, time.begin() + newIndex, signal.begin() + oldIndex, back_inserter( downsampledTime ), back_inserter( downsampledSignal ) );
			oldIndex = newIndex;
			newIndex += random();
		}

		// store results
		std::ofstream out( outputFileName );
		for (size_t i=0; i < downsampledSignal.size(); i++) {
			out << setprecision(14) << downsampledSignal[i] << endl;
		}
		out.close();

		// calculate maximum difference to MATLAB result
		BOOST_REQUIRE( CalcMaxError( downsampledSignal.begin(), downsampledSignal.end(), matlabProcessed.begin() ) < maxAllowedError );

		// check time downsampling
		for (int i=0; i < oldIndex; i += downsamplingFactor) {
			BOOST_REQUIRE( downsampledTime[ i / downsamplingFactor ] == time[i] );
		}
	}

	BOOST_AUTO_TEST_SUITE_END()
}

/*@}*/
/*@}*/