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
/*@{*/
/** \ingroup UnitTests
*/

#include <numeric>
#include <math.h>
#include <boost/test/unit_test.hpp>
#include <boost/math/constants/constants.hpp>
#include "DataProcessing.h"


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	DataProcessing		Unit test for the class CDataProcessing
*/

namespace ProcessingTests {
	/*@{*/
	/** \ingroup DataProcessing
	*/
	namespace DataProcessing {
		/**	@brief		Generate a sinus function as test data
		*/
		void GenerateSinus(std::vector<float>& x, std::vector<float>& y, const float& spacing, const float& endValue)
		{
			using namespace std;

			vector<int> numbers;

			// generate a sinus function
			numbers.resize( static_cast<int>( endValue / spacing ) + 1 );
			iota( numbers.begin(), numbers.end(), 0 );
			x.resize( numbers.size() );
			transform( numbers.begin(), numbers.end(), x.begin(), [=](float val){ return ( val * spacing ); } );
			y.resize( x.size() );
			transform( x.begin(), x.end(), y.begin(), [](float val){ return ( sin( val ) ); } );
		}



		// Test section
		BOOST_AUTO_TEST_SUITE( DataProcessing_test_suite );



		/**	@brief		Testing of peaking finding
		*/
		BOOST_AUTO_TEST_CASE( FindPeaks_test_case )
		{
			using namespace std;
			using namespace boost::math::constants;

			vector<float> x, minPeaks, maxPeaks;
			vector<float> y;
			float delta = 0.01f;
			float spacing = pi<float>() / 300;
			float yEnd = 3.0f * pi<float>();

			// generate test data
			GenerateSinus( x, y, spacing, yEnd );

			Core::Processing::CDataProcessing<float>::FindPeaks( x.begin(), x.end(), y.begin(), std::back_inserter( minPeaks ), std::back_inserter( maxPeaks ), delta );

			// check if the extrema of the sinus-function in the range have been found
			BOOST_REQUIRE( minPeaks.size() == 1 );
			BOOST_REQUIRE( std::abs( minPeaks[0] - 3 / 2.0 * pi<float>() ) < 1e-5 );
			BOOST_REQUIRE( maxPeaks.size() == 2 );
			BOOST_REQUIRE( std::abs( maxPeaks[0] - 1 / 2.0 * pi<float>() ) < 1e-5 );
			BOOST_REQUIRE( std::abs( maxPeaks[1] - 5 / 2.0 * pi<float>() ) < 1e-5 );

			// test unusual function calls
			x.pop_back(); // reduce x-size
			Core::Processing::CDataProcessing<float>::FindPeaks( x.begin(), x.end(), y.begin(), std::back_inserter( minPeaks ), std::back_inserter( maxPeaks ), delta ); // this should not throw an exception

			vector<float> xEmpty, yEmpty;
			Core::Processing::CDataProcessing<float>::FindPeaks( xEmpty.begin(), xEmpty.end(), yEmpty.begin(), std::back_inserter( minPeaks ), std::back_inserter( maxPeaks ), delta ); // this should not throw an exception

			BOOST_CHECK_THROW( Core::Processing::CDataProcessing<float>::FindPeaks( x.begin(), x.end(), y.begin(), minPeaks.begin(), maxPeaks.begin(), -delta ), std::out_of_range );
		}



		/**	@brief		Testing of limiting data ranges
		*/
		BOOST_AUTO_TEST_CASE( LimitDataRange_test_case )
		{
			using namespace std;
			using namespace boost::math::constants;

			vector<float> x, limitedX;
			vector<float> y, limitedY;
			float lowerLimit, upperLimit;
			float spacing = pi<float>() / 300;
			float yEnd = 3.0f * pi<float>();

			// generate test data
			GenerateSinus( x, y, spacing, yEnd );

			// simple test
			lowerLimit = x.front();
			upperLimit = x.back();
			Core::Processing::CDataProcessing<float>::LimitDataRange( x.begin(), x.end(), y.begin(), back_inserter( limitedX ), back_inserter( limitedY ), lowerLimit, upperLimit );
			BOOST_REQUIRE( limitedX.size() == x.size() / 2 ); // the data is limited to the Nyquist frequency
			BOOST_REQUIRE( limitedY.size() == y.size() / 2 ); // the data is limited to the Nyquist frequency

			// test of further cutting
			lowerLimit = x.front();
			upperLimit = pi<float>();
			limitedX.clear();
			limitedY.clear();
			Core::Processing::CDataProcessing<float>::LimitDataRange( x.begin(), x.end(), y.begin(), back_inserter( limitedX ), back_inserter( limitedY ), lowerLimit, upperLimit );
			BOOST_REQUIRE( ( limitedX.size() >= 300 ) && ( limitedX.size() <= 301 ) ); // the data is limited to the Nyquist frequency - with workaround for precision problems with ARM-NEON
			BOOST_REQUIRE( ( limitedY.size() >= 300 ) && ( limitedY.size() <= 301 ) ); // the data is limited to the Nyquist frequency - with workaround for precision problems with ARM-NEON

			// test unusual function calls
			x.pop_back(); // reduce x-size
			lowerLimit = x.front();
			upperLimit = x.back();
			limitedX.clear();
			limitedY.clear();
			Core::Processing::CDataProcessing<float>::LimitDataRange( x.begin(), x.end(), y.begin(), back_inserter( limitedX ), back_inserter( limitedY ), lowerLimit, upperLimit ); // this should not throw an exception

			vector<float> xEmpty, yEmpty;
			Core::Processing::CDataProcessing<float>::LimitDataRange( xEmpty.begin(), xEmpty.end(), yEmpty.begin(), back_inserter( limitedX ), back_inserter( limitedY ), lowerLimit, upperLimit ); // this should not throw an exception

			limitedX.clear();
			limitedY.clear();
			Core::Processing::CDataProcessing<float>::LimitDataRange( x.begin(), x.end(), y.begin(), back_inserter( limitedX ), back_inserter( limitedY ), lowerLimit, -upperLimit );
			BOOST_REQUIRE( limitedX.empty() );
			BOOST_REQUIRE( limitedY.empty() );
		}



		/**	@brief		Testing of time conversion
		*/
		BOOST_AUTO_TEST_CASE( PrimeNumbers_test_case )
		{
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 1 ) == false );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 2 ) == true );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 3 ) == true );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 4 ) == false );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 5 ) == true );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 6 ) == false );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 7 ) == true );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::IsPrimeNumber( 127 ) == true );
		}



		/**	@brief		Testing of even number conversion
		*/
		BOOST_AUTO_TEST_CASE( MakeEven_test_case )
		{
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( 5.3434f ) == 6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( 4.34343f ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( 4 ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( 5 ) == 6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( -7.83434f ) == -8 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( 0 ) == 0 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::MakeEven( -2.9999f ) == -2 );
		}



		/**	@brief		Testing of next smaller even number conversion
		*/
		BOOST_AUTO_TEST_CASE( NextSmallerEven_test_case )
		{
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( 5.3434f ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( 4.34343f ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( 4 ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( 5 ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( -7.83434f ) == -8 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( 0 ) == 0 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextSmallerEven( -2.9999f ) == -4 );
		}



		/**	@brief		Testing of next larger even number conversion
		*/
		BOOST_AUTO_TEST_CASE( NextLargerEven_test_case )
		{
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( 5.3434f ) == 6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( 4.34343f ) == 6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( 4 ) == 4 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( 5 ) == 6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( -7.83434f ) == -6 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( 0 ) == 0 );
			BOOST_REQUIRE( Core::Processing::CDataProcessing<float>::NextLargerEven( -2.9999f ) == -2 );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/