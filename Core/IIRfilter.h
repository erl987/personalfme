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
#include <algorithm>
#include "filter.h"

/*@{*/
/** \ingroup Core
*/
namespace Core{
	namespace Processing {
		namespace Filter {
			/** \ingroup Core
			*	Class for filtering data with digital IIR-filters in the time domain. The class is especially well-suited for processing arbitrary length input signals.
			*/
			template <class T> class CIIRfilter : public CFilter<T>
			{
			public:
				CIIRfilter(void);
				~CIIRfilter(void) {};
				template <class InIt1, class InIt2> CIIRfilter(InIt1 aFirst, InIt1 aLast, InIt2 bFirst, InIt2 bLast, int downsamplingFactor = 1, int upsamplingFactor = 1);
				template <class InIt1, class InIt2> void SetParams(InIt1 aFirst, InIt1 aLast, InIt2 bFirst, InIt2 bLast, int downsamplingFactor = 1, int upsamplingFactor = 1);
			private:				
				virtual void DownsamplingFilter(const std::vector<T>& inputSignal, std::vector<T>& filteredSignal);
				CIIRfilter(const CIIRfilter &);					// prevent copying
    			CIIRfilter & operator= (const CIIRfilter &);	// prevent assignment

				std::vector<T> a;
				std::vector<T> previousFilteredSignal;
			};
		}
	}
}

/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Processing::Filter::CIIRfilter<T>::CIIRfilter(void)
	: CFilter<T>()
{
}



/**	@brief 		Constructor
*	@param		aFirst					Iterator to the beginning of the container storing the a-filter parameters of the IIR-filter
*	@param		aLast					Iterator to the end of the container storing the a-filter parameters
*	@param		bFirst					Iterator to the beginning of the container storing the b-filter parameters of the IIR-filter
*	@param		bLast					Iterator to the end of the container storing the b-filter parameters
*	@param		downsamplingFactor		Downsampling factor: every downsamplingFactor-th datapoint is used. Can be leaved out together with upsamplingFactor if only filtering is used.
*	@param		upsamplingFactor		Upsampling factor: diving the section between two datapoints in upsamplingFactor sections. Can be leaved out if only filtering is used.
*	@exception	std::runtime_error		Thrown if the downsampling or upsampling factor is smaller than 1.
*	@remarks							Upsampling and downsampling factors are automatically minimized by the greatest common divisor. If it is used for downsampling, the filter must be correctly designed regarding the cut-off frequency at fc = 0.8 * ( upsamplingFactor / downsamplingFactor * sampling rate / 2 ).
*										The algorithm follows the IEEE Programs for Digital Signal Processing. Wiley & Sons 1979, program 8.2. A 8-order IIR lowpass Chebyshev type I filter with passband ripples smaller than 0.05 dB is recommended for downsampling.
*/
template <class T> template <class InIt1, class InIt2> Core::Processing::Filter::CIIRfilter<T>::CIIRfilter(InIt1 aFirst, InIt1 aLast, InIt2 bFirst, InIt2 bLast, int downsamplingFactor, int upsamplingFactor)
	: CFilter<T>()
{
	SetParams( aFirst, aLast, bFirst, bLast, downsamplingFactor, upsamplingFactor );
}



/**	@brief 		Safely resetting filter parameters (also possible for an already used object)
*	@param		aFirst					Iterator to the beginning of the container storing the a-filter parameters of the IIR-filter
*	@param		aLast					Iterator to the end of the container storing the a-filter parameters
*	@param		bFirst					Iterator to the beginning of the container storing the b-filter parameters of the IIR-filter
*	@param		bLast					Iterator to the end of the container storing the b-filter parameters
*	@param		downsamplingFactor		Downsampling factor: every downsamplingFactor-th datapoint is used. Can be leaved out together with upsamplingFactor if only filtering is used.
*	@param		upsamplingFactor		Upsampling factor: diving the section between two datapoints in upsamplingFactor sections. Can be leaved out if only filtering is used.
*	@return								None
*	@exception	std::length_error		Thrown if the filter parameters are empty
*	@exception	std::runtime_error		Thrown if the downsampling or upsampling factor is smaller than 1.
*	@remarks							Upsampling and downsampling factors are automatically minimized by the greatest common divisor. If it is used for downsampling, the filter must be correctly designed regarding the cut-off frequency at fc = 0.8 * ( upsamplingFactor / downsamplingFactor * sampling rate / 2 ).
*										The algorithm follows the IEEE Programs for Digital Signal Processing. Wiley & Sons 1979, program 8.2. A 8-order IIR lowpass Chebyshev type I filter with passband ripples smaller than 0.05 dB is recommended for downsampling.
*/
template <class T> template <class InIt1, class InIt2> void Core::Processing::Filter::CIIRfilter<T>::SetParams(InIt1 aFirst, InIt1 aLast, InIt2 bFirst, InIt2 bLast, int downsamplingFactor, int upsamplingFactor)
{
	using namespace std;

	CFilter<T>::SetParams( bFirst, bLast, downsamplingFactor, upsamplingFactor );

	// assign input values
	a.assign( aFirst, aLast );
	if ( a.empty() ) {
		throw std::length_error( "Filter parameters must not be empty." );
	}

	// set storage container for recently processed data (required for restarting the filtering without losses)
	previousFilteredSignal.assign( a.size() - 1, 0 );

	this->isInit = true;
}



/** @brief		Downsampling and filtering of a dataset. This function is specifically implemented for each filter type.
*	@param		signal					Data to be processed
*	@param		filteredSignal			Downsampled output data
*	@return								None
*	@exception							None
*	@remarks							This function performs efficiently filtering and downsampling at the same time
*/
template <class T> void Core::Processing::Filter::CIIRfilter<T>::DownsamplingFilter(const std::vector<T>& inputSignal, std::vector<T>& filteredSignal)
{
	std::vector<T> downsampledSignal, a, b;
	int filterLengthA, filterLengthB, signalIndex;
	T filtered;
	std::vector<T> signal;
	signal.assign( inputSignal.begin(), inputSignal.end() );

	a.assign( CIIRfilter::a.begin(), CIIRfilter::a.end() );
	reverse( a.begin(), a.end() ); // improving efficiency in the loop
	b.assign( CIIRfilter::b.begin(), CIIRfilter::b.end() );
	reverse( b.begin(), b.end() );

	// perform filtering
	filteredSignal.resize( signal.size() );
	filterLengthA = static_cast<int>( a.size() );
	filterLengthB = static_cast<int>( b.size() );
	signalIndex = filterLengthB - 1;

	// add previous data in front of the current data
	signal.insert( signal.begin(), this->previousSignal.begin(), this->previousSignal.end() );
	filteredSignal.insert( filteredSignal.begin(), previousFilteredSignal.begin(), previousFilteredSignal.end() );

	if ( signalIndex < static_cast<int>( signal.size() ) ) {
		for (size_t counter = previousFilteredSignal.size(); counter < filteredSignal.size(); counter++) {
			filtered = 0;
			for (int k = 0; k < filterLengthB; k++) {
				filtered += b[k] * signal[ signalIndex - ( filterLengthB - 1 ) + k ];
			}
			for (int l = 0; l < ( filterLengthA - 1 ); l++) {
				filtered -= a[l] * filteredSignal[ signalIndex - ( filterLengthA - 1 ) + l ];
			}
			filtered /= a.back();
			filteredSignal[counter] = filtered;
			signalIndex++;
		}
	}

	// delete temporary previous data
	signal.erase( signal.begin(), signal.begin() + this->previousSignal.size() );
	filteredSignal.erase( filteredSignal.begin(), filteredSignal.begin() + previousFilteredSignal.size() );

	// preserve the latest data corresponding to the length of the filter (for restarting the filter)
	if ( signal.size() >= b.size() ) {
		this->previousSignal.insert( this->previousSignal.end(), signal.begin() + signal.size() - ( filterLengthB - 1 ), signal.end() );
	} else {
		this->previousSignal.insert( this->previousSignal.end(), signal.begin(), signal.end() );
	}

	if ( filteredSignal.size() >= a.size() ) {
		previousFilteredSignal.insert( previousFilteredSignal.end(), filteredSignal.begin() + filteredSignal.size() - ( filterLengthA - 1 ), filteredSignal.end() );
	} else {
		previousFilteredSignal.insert( previousFilteredSignal.end(), filteredSignal.begin(), filteredSignal.end() );
	}
	this->previousSignal.erase( this->previousSignal.begin(), this->previousSignal.begin() + this->previousSignal.size() - ( filterLengthB - 1 ) );
	previousFilteredSignal.erase( previousFilteredSignal.begin(), previousFilteredSignal.begin() + previousFilteredSignal.size() - ( filterLengthA - 1 ) );

	// downsampling
	if ( this->downsamplingFactor > 1 ) {
		downsampledSignal.resize( this->DownsamplingLength( filteredSignal.size(), this->downsamplingFactor, this->firstDatapoint ) );
		this->firstDatapoint = this->Downsampling( filteredSignal.begin(), filteredSignal.end(), downsampledSignal.begin(), this->firstDatapoint );
	} else {
		downsampledSignal.assign( filteredSignal.begin(), filteredSignal.end() );
	}

	// set output data
	filteredSignal.assign( downsampledSignal.begin(), downsampledSignal.end() );
}
