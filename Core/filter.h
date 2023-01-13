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
#include <boost/cast.hpp>
#include "DataProcessing.h"

/*@{*/
/** \ingroup Core
*/
namespace Core{
	namespace Processing {
		namespace Filter {
			/** \ingroup Core
			*	Class for filtering data with digital filters in the time domain. This is an abstract base class for implementing filters. The class is especially well-suited for processing arbitrary length input signals.
			*/
			template <class T> class CFilter
			{
			public:
				CFilter(void);
				~CFilter(void) {};
				template <class InIt> CFilter(InIt bFirst, InIt bLast, int downsamplingFactor, int upsamplingFactor);
				template <class InIt, class OutIt> OutIt Processing(InIt signalFirst, InIt signalLast, OutIt filteredSignalFirst);
				template <class InIt1, class InIt2, class OutIt1, class OutIt2> OutIt2 Processing(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst, OutIt1 filteredTimeFirst, OutIt2 filteredSignalFirst);
				void GetParams(int& downsamplingFactor, int& upsamplingFactor);
				int ProcessedLength(int dataLength);
			protected:
				virtual void DownsamplingFilter(const std::vector<T>& inputSignal, std::vector<T>& outputFilteredSignal) = 0;
				template <class InIt> void SetParams(InIt bFirst, InIt bLast, int downsamplingFactor, int upsamplingFactor);
				template <class InIt, class OutIt> OutIt Upsampling(InIt inFirst, InIt inLast, OutIt outFirst);
				template <class InIt, class OutIt> int Downsampling(InIt inFirst, InIt inLast, OutIt outFirst, int firstDatapoint);			
				int UpsamplingLength(int dataLength, int upsamplingFactor);
				int DownsamplingLength(int dataLength, int downsamplingFactor, int firstDatapoint);
				CFilter(const CFilter &);				// prevent copying
    			CFilter & operator= (const CFilter &);	// prevent assignment

				std::vector<T> b;
				std::vector<T> previousSignal;
				std::vector<T> previousDatapointUpsampling;
				int downsamplingFactor;
				int upsamplingFactor;
				int firstDatapoint;
				bool isInit;
			};
		}
	}
}

/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::Processing::Filter::CFilter<T>::CFilter(void)
	: isInit(false)
{
}



/**	@brief 		Constructor
*	@param		bFirst					Iterator to the beginning of the container storing the b-filter parameters of the FIR-filter
*	@param		bLast					Iterator to one element after the end of the container storing the b-filter parameters
*	@param		downsamplingFactor		Downsampling factor: every downsamplingFactor-th datapoint is used
*	@param		upsamplingFactor		Upsampling factor: diving the section between two datapoints in upsamplingFactor sections
*	@exception	std::runtime_error		Thrown if the downsampling or the upsampling factor is smaller than 1
*	@remarks							Upsampling and downsampling factors are automatically minimized by the greatest common divisor.
*/
template <class T> template <class InIt> Core::Processing::Filter::CFilter<T>::CFilter(InIt bFirst, InIt bLast, int downsamplingFactor, int upsamplingFactor)
	: isInit(false)
{
	SetParams( bFirst, bLast, downsamplingFactor, upsamplingFactor );
}



/**	@brief 		Safely resetting filter parameters (also possible for an already used object)
*	@param		bFirst					Iterator to the beginning of the container storing the b-filter parameters of the FIR-filter
*	@param		bLast					Iterator to one element after the end of the container storing the b-filter parameters
*	@param		downsamplingFactor		Downsampling factor: every downsamplingFactor-th datapoint is used
*	@param		upsamplingFactor		Upsampling factor: diving the section between two datapoints in upsamplingFactor sections
*	@return								None
*	@exception	std::length_error		Thrown if the filter parameters are empty
*	@exception	std::runtime_error		Thrown if the downsampling or the upsampling factor is smaller than 1
*	@remarks							Upsampling and downsampling factors are automatically minimized by the greatest common divisor.
*/
template <class T> template <class InIt> void Core::Processing::Filter::CFilter<T>::SetParams(InIt bFirst, InIt bLast, int downsamplingFactor, int upsamplingFactor)
{
	using namespace std;
	int gcd;

	//minimization of factors
	gcd = Processing::CDataProcessing<T>::GreatestCommonDivisor( downsamplingFactor, upsamplingFactor );
	downsamplingFactor = downsamplingFactor / gcd;
	upsamplingFactor = upsamplingFactor / gcd;

	// check factors
	if ( ( downsamplingFactor < 1 ) || ( upsamplingFactor < 1 ) ) {
		throw std::runtime_error( "Downsampling and upsampling factors must be at least 1." );
	}

	// assign input values
	firstDatapoint = 0;
	b.assign( bFirst, bLast );
	if ( b.empty() ) {
		throw std::length_error( "Filter parameters must not be empty." );
	}

	CFilter<T>::downsamplingFactor = downsamplingFactor;
	CFilter<T>::upsamplingFactor = upsamplingFactor;

	// set storage container for recently processed data (required for restarting the filtering without losses)
	previousSignal.assign( b.size() - 1, 0 );
	previousDatapointUpsampling.clear();
}



/** @brief		Upsampling of a dataset
*	@param		inFirst					Iterator to beginning of the data to be processed
*	@param		inLast					Iterator to end of the data to be processed
*	@param		outFirst				Iterator to the beginning of the processed data
*	@return								Iterator to the end of the processed data
*	@exception	std::rutime_error		Thrown if the class has not been initialized before use with the function SetParams() or the constructor
*	@remarks							The size of the output container must either be determined by CFilter<T>::UpsamplingLength in advance or std::back_inserter must be used.
*/
template <class T> template <class InIt, class OutIt> OutIt Core::Processing::Filter::CFilter<T>::Upsampling(InIt inFirst, InIt inLast, OutIt outFirst)
{
	using boost::numeric_cast;
	std::vector<T> data, upsampledData;
	T slope, invUpsamplingFactor;

	// check if filter parameters have been initialized
	if ( !isInit ) {
		throw std::runtime_error( "Object has not been initialized before use." );
	}

	// copy data
	data.assign( inFirst, inLast );
	upsampledData.resize( UpsamplingLength( data.size(), upsamplingFactor ) );

	// add last previous datapoint for interpolation in front of the current data
	if ( ( !previousDatapointUpsampling.empty() ) && ( !data.empty() ) ) { // it will not be used for the first execution of the function
		upsampledData.resize( upsampledData.size() + 1 );
		data.insert( data.begin(), previousDatapointUpsampling.begin(), previousDatapointUpsampling.end() );
	}

	// prepare upsampled data
	for (size_t i=0; i < data.size(); i++) {
		upsampledData[ upsamplingFactor * i ] = data[i];
	}

	// calculate upsampled data points by linear interpolation
	if ( data.size() > 1 ) {
		invUpsamplingFactor = static_cast<T>( 1.0 / upsamplingFactor );
		for (size_t i=0; i < ( data.size() - 1 ); i++) {
			slope = ( data[i + 1] - data[i] ) * invUpsamplingFactor;
			for (int j=1; j < upsamplingFactor; j++) {
				upsampledData[ upsamplingFactor * i + j ] = data[i] + slope * j;
			}
		}
	}

	// delete last previous datapoint
	if ( ( !previousDatapointUpsampling.empty() ) && ( !upsampledData.empty() ) ) {
		upsampledData.erase( upsampledData.begin() );
	}

	// save last datapoint for further processing steps
	if ( data.size() > 0 ) {
		previousDatapointUpsampling.assign( 1, data.back() );
	}

	// set output data
	return std::move( upsampledData.begin(), upsampledData.end(), outFirst );
}



/** @brief		Obtaining the length of an upsampled dataset
*	@param		dataLength				Length of dataset to be upsampled
*	@param		upsamplingFactor		Upsampling factor
*	@return								Length of upsampled dataset
*	@exception							None
*	@remarks							None
*/
template <class T> int Core::Processing::Filter::CFilter<T>::UpsamplingLength(int dataLength, int upsamplingFactor)
{
	int upsamplingLength;

	if ( dataLength < 1 ) {
		upsamplingLength = 0;
	} else {
		upsamplingLength = ( dataLength - 1 ) * upsamplingFactor + 1;
		if ( !previousDatapointUpsampling.empty() ) { // if this is not the first processing of the dataset, interpolation in front of the current data is required
			upsamplingLength += upsamplingFactor - 1;
		}
	}

	return upsamplingLength;
}



/** @brief		Obtaining the length of a downsampled dataset
*	@param		dataLength				Length of dataset to be downsampled
*	@param		downsamplingFactor		Downsampling factor
*	@param		firstDatapoint			Index of the first datapoint to be used
*	@return								Length of downsampled dataset
*	@exception							None
*	@remarks							None
*/
template <class T> int Core::Processing::Filter::CFilter<T>::DownsamplingLength(int dataLength, int downsamplingFactor, int firstDatapoint)
{
	int downsamplingLength;

	if ( dataLength < firstDatapoint ) {
		downsamplingLength = 0;
	} else {
		downsamplingLength = static_cast<int>( ceil( ( dataLength - firstDatapoint ) / static_cast<double>( downsamplingFactor ) ) );
	}

	return downsamplingLength;
}



/**	@brief 		Filtering / resampling function depending on the set upsampling and downsampling factors
*	@param		signalFirst				Iterator to the beginning of the container storing the data to be resampled
*	@param		signalLast				Iterator to the end of the container storing the data to be resampled
*	@param		filteredSignalFirst		Iterator to the beginning of the container storing the resampled data after the call. It is required to have the correct size. Do not use std::back_inserter if efficiency is important.
*	@return								Iterator to the end of the container storing the downsampled data
*	@exception	std::rutime_error		Thrown if the class has not been initialized before use with the function SetParams() or the constructor
*	@remarks							The algorithm follows the IEEE Programs for Digital Signal Processing. Wiley & Sons 1979, program 8.2.
*/
template <class T> template <class InIt, class OutIt> OutIt Core::Processing::Filter::CFilter<T>::Processing(InIt signalFirst, InIt signalLast, OutIt filteredSignalFirst)
{
	using namespace std;
	vector<T> signal, filteredSignal, upsampledSignal;
	
	// check if filter parameters have been initialized
	if ( !isInit ) {
		throw std::runtime_error( "Object has not been initialized before use." );
	}

	// check input and output datatypes
	static_assert( boost::is_same< typename iterator_traits<InIt>::value_type, T >::value, "Input iterator type must be identical to the class template parameter." );

	// upsampling
	signal.assign( signalFirst, signalLast );
	if ( upsamplingFactor > 1 ) {
		upsampledSignal.resize( UpsamplingLength( signal.size(), upsamplingFactor ) );
		Upsampling( signal.begin(), signal.end(), upsampledSignal.begin() );
		signal.assign( upsampledSignal.begin(), upsampledSignal.end() );
	}
	
	// downsampling
	if ( ( downsamplingFactor > 1 ) || ( ( downsamplingFactor == 1 ) && ( upsamplingFactor == 1 ) ) ) {
		filteredSignal.resize( DownsamplingLength( signal.size(), downsamplingFactor, firstDatapoint ) );
		DownsamplingFilter( signal, filteredSignal );
	} else {
		filteredSignal.assign( signal.begin(), signal.end() );
	}

	// set output data
	return std::move( filteredSignal.begin(), filteredSignal.end(), filteredSignalFirst );
}



/**	@brief 		Resampling function including the corresponding time data container (without filtering the time data)
*	@param		timeFirst				Iterator to the beginning of the container storing the time corresponding to the data to be resampled
*	@param		timeLast				Iterator to the end of the time container
*	@param		signalFirst				Iterator to the beginning of the container storing the data to be resampled. The length has to be the same as for the time container.
*	@param		filteredTimeFirst		Iterator to the beginning of the container storing the resampled time after the call. The times are not filtered. It is required to have the correct size. Do not use std::back_inserter if efficiency is important.
*	@param		filteredSignalFirst		Iterator to the beginning of the container storing the resampled data after the call. It is required to have the correct size. Do not use std::back_inserter if efficiency is important.
*	@return								Iterator to the end of the container storing the downsampled data
*	@exception	std::rutime_error		Thrown if the class has not been initialized before use with the function SetParams() or the constructor
*	@remarks							The algorithm follows the IEEE Programs for Digital Signal Processing. Wiley & Sons 1979, program 8.2.
*/
template <class T> template <class InIt1, class InIt2, class OutIt1, class OutIt2> OutIt2 Core::Processing::Filter::CFilter<T>::Processing(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst, OutIt1 filteredTimeFirst, OutIt2 filteredSignalFirst)
{
	int currentFirstDatapoint;
	std::vector<T> signal;

	// obtain input data
	signal.assign( signalFirst, signalFirst + distance( timeFirst, timeLast ) );
	currentFirstDatapoint = firstDatapoint;

	// resampling of the signal (including required filtering)
	Processing( signal.begin(), signal.end(), filteredSignalFirst ); // firstDatapoint is reset here to the new state due to the processed data

	// resampling of the time (without filtering)
	Downsampling( timeFirst, timeLast, filteredTimeFirst, currentFirstDatapoint );

	return filteredSignalFirst;
}



/**	@brief 		Get resampling parameters
*	@param		downsamplingFactor		Downsampling factor
*	@param		upsamplingFactor		Upsampling factor
*	@return								None
*	@remarks							None
*/
template <class T> void Core::Processing::Filter::CFilter<T>::GetParams(int& downsamplingFactor, int& upsamplingFactor)
{
	downsamplingFactor = CFilter<T>::downsamplingFactor;
	upsamplingFactor = CFilter<T>::upsamplingFactor;
}



/** @brief		Downsampling of a dataset.
*	@param		inFirst					Iterator to the beginning of the data to be processed, [inFirst inOut)
*	@param		inLast					Iterator to the end of data to be processed, [inFirst inOut)
*	@param		outFirst				Iterator to the beginning of processed data
*	@param		firstDatapoint			First datapoint of the container to used
*	@return								Starting index to be put into firstDatapoint for the next function call in order to ensure smooth downsampling with constant distances between the datapoints.
*	@exception							None
*	@remarks							The output iterator must be set to the appropriate length with CFilter<T>::DownsamplingLength() before calling the function or std::back_inserter must be used. This is less efficient.
*/
template <class T> template <class InIt, class OutIt> int Core::Processing::Filter::CFilter<T>::Downsampling(InIt inFirst, InIt inLast, OutIt outFirst, int firstDatapoint)
{
	int newFirstDatapoint;
	int lastIndex;
	std::vector< typename std::iterator_traits<InIt>::value_type > data, downsampledData;

	// copy data
	data.assign( inFirst, inLast );

	// downsampling
	downsampledData.resize( DownsamplingLength( data.size(), downsamplingFactor, firstDatapoint ) );
	for ( size_t i = 0; i < downsampledData.size(); i++ ) {
		downsampledData[i] = data[ firstDatapoint + i * downsamplingFactor ];
	}
	lastIndex = firstDatapoint + ( downsampledData.size() - 1 ) * downsamplingFactor;
	std::move( downsampledData.begin(), downsampledData.end(), outFirst );

	// calculate index of next datapoint
	newFirstDatapoint = lastIndex + downsamplingFactor - data.size();

	return newFirstDatapoint;
}



/** @brief		Predicting the resampled length of a dataset
*	@param		dataLength				Length of the original dataset
*	@return								Length of the resampled dataset
*	@exception							None
*	@remarks							The length is only valid for the current state of the object and may differ in other cases.
*/
template <class T> int Core::Processing::Filter::CFilter<T>::ProcessedLength(int dataLength)
{
	int resampledLength;

	resampledLength = UpsamplingLength( dataLength, upsamplingFactor );
	resampledLength = DownsamplingLength( resampledLength, downsamplingFactor, firstDatapoint );

	return resampledLength;
}