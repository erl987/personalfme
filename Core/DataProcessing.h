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

This file contains code under the following copyright:
Copyright(C) 2012 Eli Billauer, released to the public domain. Any use is allowed (see http://www.billauer.co.il/peakdet.html).
*/
#pragma once

#include <vector>
#include <limits>
#include <algorithm>
#include <iterator>
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/conversion/cast.hpp>

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Processing {
		/**	\ingroup Core
		*	Class for digital data processing.
		*/
		template <class T> class CDataProcessing
		{
		public:
			CDataProcessing(void);
			~CDataProcessing(void);
			template <class InputIterator1, class InputIterator2, class OutputIterator> static void FindPeaks(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator minPeaksFirst, OutputIterator maxPeaksFirst, double delta);
			template <class InputIterator1, class InputIterator2, class OutputIterator> static void SubstractBaseline(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator outputFirst, T lowerBound, T upperBound);
			template <class InputIterator1, class InputIterator2, class OutputIterator1, class OutputIterator2> static void LimitDataRange(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator1 xOutputFirst, OutputIterator2 dataOutputFirst, T lowerBound, T upperBound);
			template <class InputIterator, class OutputIterator> static void NormalizeData(InputIterator dataFirst, InputIterator dataLast, OutputIterator outputFirst);
			template <class InputIterator, class OutputIterator> static void HammingWindow( InputIterator dataFirst, InputIterator dataLast, OutputIterator outputFirst);
			static int GreatestCommonDivisor(int number1, int number2);
			static bool IsPrimeNumber(int number);
			static int MakeEven(const T& number);
			static int NextSmallerEven(const T& number);
			static int NextLargerEven(const T& number);
		private:
			CDataProcessing(const CDataProcessing &);				// prevent copying
    		CDataProcessing & operator= (const CDataProcessing &);	// prevent assignment
		};
	}
}

/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::Processing::CDataProcessing<T>::CDataProcessing(void)
{
}



/**	@brief		Destructor.
*/
template <class T> Core::Processing::CDataProcessing<T>::~CDataProcessing(void)
{
}



/**	@brief 		Finds peaks in a given dataset.
*			
*	@param		xFirst				Iterator to beginning of abscissa data (e.g. time / frequency, ...)
*	@param		xLast				Iterator to end of abscissa data
*	@param		dataFirst			Iterator to beginning of ordinate data (e.g. power density, ...), same length as x (or larger) needed
*	@param		minPeaksFirst		Iterator to beginning of peak minimum container
*	@param		maxPeaksFirst		Iterator to beginning of peak maximum container
*	@param		delta				Threshold value defining minimum distance between the peak and the left neighboring opposed peak
*	@return							None
*	@exception	std::out_of_range	Thrown if the variable delta is not positive
*	@exception						Exception of std::iterator will be thrown if the container for data is smaller than for x
*	@remarks						Based on an algorithm from Eli Billauer, which is released to the public domain. Any use is allowed (see http://www.billauer.co.il/peakdet.html).
*									Call this function with std::back_inserter(minPeaks), std::back_inserter(maxPeaks). If the container data is larger than x, only the number of elements in x will be processed, beginning with element 0.
*/
template <class T> template <class InputIterator1, class InputIterator2, class OutputIterator> void Core::Processing::CDataProcessing<T>::FindPeaks(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator minPeaksFirst, OutputIterator maxPeaksFirst, double delta) {
	using namespace std;

	T current;
	T min = numeric_limits<T>::max();
	T max = -numeric_limits<T>::max();
	T minPos, maxPos;
	std::vector <T> x, data, minPeaks, maxPeaks;
	bool lookForMax = true;

	// copy dataset
	x.assign( xFirst, xLast );
	data.assign( dataFirst, dataFirst + distance( xFirst, xLast ) );

	// check input parameters
	if (delta < 0) {
		throw std::out_of_range("Delta must be positive!");
	}

	for (size_t i=0; i < data.size(); i++) {
		current = data[i];

		// check current value for possible maximum
		if ( current > max ) {
			max = current;
			maxPos = x[i];
		}

		// check current value for possible minimum
		if ( current < min ) {
			min = current;
			minPos = x[i];
		}

		if (lookForMax) {
			// check if next minimum to the left was more than delta lower
			if ( current < (max - delta) ) {
				maxPeaks.push_back( maxPos );
				min = current;
				minPos = x[i];
				lookForMax = false;
			}
		} else {
			// check if next maximum to the left was more than delta higher
			if ( current > (min + delta) ) {
				minPeaks.push_back( minPos );
				max = current;
				maxPos = x[i];
				lookForMax = true;
			}
		}
	}

	// copy results
	std::move( minPeaks.begin(), minPeaks.end(), minPeaksFirst );
	std::move( maxPeaks.begin(), maxPeaks.end(), maxPeaksFirst );
}



/**	@brief 		Cuts all data outside the boundaries in both "x" and "data".
*	@param		xFirst					Iterator to beginning of the abscissa value container
*	@param		xLast					Iterator to end of the abscissa value container
*	@param		dataFirst				Iterator to beginning of the data value container
*	@param		xOutputFirst			Iterator to beginning of bounded abscissa value container, use std::back_inserter because the size cannot be predicted
*	@param		dataOutputFirst			Iterator to beginning of bounded data value container, use std::back_inserter because the size cannot be predicted
*	@param		lowerBound				Lower boundary value of "x" to be used
*	@param		upperBound 				Upper boundary value of "x" to be used
*	@return								None
*	@exception							Exception of std::iterator will be thrown if the container for data is smaller than for x
*	@remarks							std::back_inserter should be used for the output containers. If size of data container is larger than that of the x container, only the number of elements in x will be processed, beginning with element 0.
*/
template <class T> template <class InputIterator1, class InputIterator2, class OutputIterator1, class OutputIterator2> void Core::Processing::CDataProcessing<T>::LimitDataRange(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator1 xOutputFirst, OutputIterator2 dataOutputFirst, T lowerBound, T upperBound)
{
	using namespace std;
	
	int minPos, maxPos;
	std::vector<T> x, data;
	typename std::vector<T>::iterator min, max;

	// assign data
	x.assign( xFirst, xLast );
	data.assign( dataFirst, dataFirst + std::distance( xFirst, xLast ) );
	
	// limit to Nyquist frequency
	x.resize( data.size() / 2 );
	data.resize( data.size() / 2 );

	// limit data range to lower boundary
	min = find_if( x.begin(), x.end(), [=]( auto val ) { return ( val >= lowerBound );  } );
	minPos = distance( x.begin(), min );
	x.erase( x.begin(), min );
	data.erase( data.begin(), data.begin() + minPos );

	// limit data range to upper boundary
	max = find_if( x.begin(), x.end(), [=](auto val) { return ( val > upperBound ); } );
	maxPos = distance( x.begin(), max );
	x.erase( max, x.end() );
	data.erase( data.begin() + maxPos, data.end() );

	// set return data
	std::move( x.begin(), x.end(), xOutputFirst );
	std::move( data.begin(), data.end(), dataOutputFirst );
}



/**	@brief			Subtracts the baseline, which is assumed to be the average.
*	@param			xFirst			Iterator to beginning of container with the abscissa values (i.e. frequency)
*	@param			xLast			Iterator to end of container with the abscissa values (i.e. frequency)
*	@param			dataFirst		Iterator to beginning of container with the ordinate values, i.e. data to be processed
*	@param			outputFirst		Iterator to beginning of container with the substracted data (will be of same size as x)
*	@param			lowerBound		Lowest value of the abscissa to be considered for the calculation of the average
*	@param			upperBound 		Highest value of the abscissa to be considered for the calculation of the average
*	@return							None
*	@exception						Exception of std::iterator will be thrown if the container for data is smaller than for x
*	@remarks						If the container for data is larger than for x, the considered number of elements will be the same as for x, beginning with element 0.
*/
template <class T> template <class InputIterator1, class InputIterator2, class OutputIterator> void Core::Processing::CDataProcessing<T>::SubstractBaseline(InputIterator1 xFirst, InputIterator1 xLast, InputIterator2 dataFirst, OutputIterator outputFirst, T lowerBound, T upperBound)
{
	using boost::numeric_cast;
	T average;
	std::vector<T> x, data, dataCopy;

	// assign data
	x.assign( xFirst, xLast );
	data.assign( dataFirst, dataFirst + std::distance( xFirst, xLast ) );

	// delete data outside of the requested range
	LimitDataRange( x.begin(), x.end(), data.begin(), std::back_inserter( x ), std::back_inserter( dataCopy ), lowerBound, upperBound );
	
	// calculate average of data (only in the requested range)
	average = accumulate( dataCopy.begin(), dataCopy.end(), numeric_cast<T>( 0 ) );
	average /= numeric_cast<T>( dataCopy.size() );	
		
	// substract average
	for (size_t i=0; i < data.size(); i++) {
		data[i] -= average;
	}

	// set return data
	std::move( data.begin(), data.end(), outputFirst );
}



/**	@brief			Normalizes a dataset.
*	@param			dataFirst		Iterator to beginning of container for the data to be processed
*	@param			dataLast		Iterator to end of the container for the data to be processed
*	@param			outputFirst		Iterator to beginning of container for the normalized data, must have same size as the data container
*	@return							None
*	@exception						None
*	@remarks						Negative values are replaced by zeros.
*/
template <class T> template <class InputIterator, class OutputIterator> void Core::Processing::CDataProcessing<T>::NormalizeData(InputIterator dataFirst, InputIterator dataLast, OutputIterator outputFirst)
{
	using namespace std;

	T maxValue;
	vector<T> data;

	// assign data
	data.assign( dataFirst, dataLast );

	// find maximum data value
	maxValue = *max_element( data.begin(), data.end() );

	// normalize with maximum data value
	if ( maxValue != 0 ) {
		transform( data.begin(), data.end(), data.begin(), [=](T val){ return ( val / maxValue ); } );
	
		// cut negative values
		replace_if( data.begin(), data.end(), [](T val){ return ( val < 0 ); }, static_cast<T>( 0 ) );
	} else {
		data.resize( data.size() ); // in order to prevent division by zero all elements are here set to zero
	}

	// set return data
	std::move( data.begin(), data.end(), outputFirst );
}



/**	@brief	Applies the Hamming-window on the data.
*	@param		dataFirst			Iterator to beginning of data container.
*	@param		dataLast			Iterator to one element after the end of the data container.
*	@param		outputFirst			Iterator to the beginning of the result data container, the input data is multiplied with the Hamming-window.
*	@return							None
*	@exception						None
*	@remarks						The Hamming-window is useful for Fourier-transformation applications.
*/
template <class T> template <class InputIterator, class OutputIterator> void Core::Processing::CDataProcessing<T>::HammingWindow(InputIterator dataFirst, InputIterator dataLast, OutputIterator outputFirst)
{
	std::vector<T> input, output;
	int windowLength;

	// obtain input data
	input.assign( dataFirst, dataLast );

	// apply Hamming-window
	windowLength = static_cast<int>( input.size() );

	output.resize( input.size() );
	for (int i=0; i < static_cast<int>( input.size() ); i++) {
		output[i] = static_cast<T>( input[i] * ( 0.54 - 0.46 * cos( 2 * boost::math::constants::pi<T>() * i / ( windowLength - 1 ) ) ) );
	}

	// set output data
	std::move( output.begin(), output.end(), outputFirst );
}



/**	@brief 		Greatest common divisor
*	@param		number1					First integer number
*	@param		number2					Second integer number
*	@return								Greatest common divisor of number1 and number2
*	@remarks							None
*/
template <class T> int Core::Processing::CDataProcessing<T>::GreatestCommonDivisor(int number1, int number2)
{
	int residual;

	// Euclid alogrithm
	while ( number2 != 0 ) {
		residual = number1 % number2;
		number1 = number2;
		number2 = residual;
	}

	return number1;
}



/**	@brief 		Determines if a number is a prime number
*	@param		number					Number to be tested
*	@return								True if the number is a prime number, false otherwise
*	@remarks							The algorithm is only efficient for small numbers
*/
template <class T> bool Core::Processing::CDataProcessing<T>::IsPrimeNumber(int number)
{
	bool isPrimeNumber = true;

	// 1 is no prime number by definition
	if ( number == 1 ) {
		return false;
	}

	if ( number == 2 ) {
		return true;
	}

	// check if divisor can be found
	for (int i=2; i <= static_cast<int>( ceil( sqrt( static_cast<double>( number ) ) ) ); i++) {
		if ( ( number % i ) == 0 ) {
			isPrimeNumber = false;
		}
	}

	return isPrimeNumber;
}



/**	@brief		Adjusts odd number to the closest even number
*	@param		number					Number to be adjusted if it is odd
*	@return								Even integer number
*	@exception							None
*	@remarks							If the number is exactly inbetween two even numbers, the larger one is chosen
*/
template <class T> int Core::Processing::CDataProcessing<T>::MakeEven(const T& number)
{
	int nextSmallerEven, nextLargerEven, evenNumber;

	// find next even numbers
	nextSmallerEven = NextSmallerEven( number );
	nextLargerEven = NextLargerEven( number );

	// adjust the odd number to the closest even one
	if ( ( nextLargerEven - number ) > ( number - nextSmallerEven ) ) {
		evenNumber = nextSmallerEven;
	} else {
		evenNumber = nextLargerEven;
	}

	return evenNumber;
}



/**	@brief		Adjusts odd number to the next smaller even number
*	@param		number					Number to be adjusted if it is odd
*	@return								Next smaller even integer number
*	@exception							None
*	@remarks							None
*/
template <class T> int Core::Processing::CDataProcessing<T>::NextSmallerEven(const T& number)
{
	int nextSmallerEven;

	// find next smaller even number
	nextSmallerEven = static_cast<int>( floor( number ) );
	if ( ( nextSmallerEven % 2 ) != 0 ) {
		nextSmallerEven -= 1;
	}

	return nextSmallerEven;
}



/**	@brief		Adjusts odd number to the next larger even number
*	@param		number					Number to be adjusted if it is odd
*	@return								Next larger even integer number
*	@exception							None
*	@remarks							None
*/
template <class T> int Core::Processing::CDataProcessing<T>::NextLargerEven(const T& number)
{
	int nextLargerEven;

	// find next larger even number
	nextLargerEven = NextSmallerEven( number );
	if ( nextLargerEven != number ) {
		nextLargerEven += 2;
	}

	return nextLargerEven;
}