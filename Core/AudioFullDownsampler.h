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

#include <vector>
#include <boost/thread.hpp>
#include "FIRfilter.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Audio {
		/**	\ingroup Core
		*	Class for the downsampling of the audio signal implementing multi-step downsampling for efficient combined processing of recording and processing data
		*/
		template <class T> class CAudioFullDownsampler
		{
		public:
			CAudioFullDownsampler(void);
			CAudioFullDownsampler(const unsigned int& downsamplingFactorProc, const double& cutoffFreqProc, const double& transWidthProc, const unsigned int& downsamplingFactorRec, const double& cutoffFreqRec, const double& transWidthRec, const double& samplingFreq);
			virtual ~CAudioFullDownsampler(void);
			void SetParameters(const unsigned int& downsamplingFactorProc, const double& cutoffFreqProc, const double& transWidthProc, const unsigned int& downsamplingFactorRec, const double& cutoffFreqRec, const double& transWidthRec, const double& samplingFreq);
			void GetParameters(unsigned int& downsamplingFactorProc, double& cutoffFreqProc, double& transWidthProc, unsigned int& downsamplingFactorRec, double& cutoffFreqRec, double& transWidthRec, double& samplingFreq);
			template <class InIt1, class InIt2, class OutIt1, class OutIt2, class OutIt3, class OutIt4> void PerformDownsampling(InIt1 inputTimeFirst, InIt1 inputTimeLast, InIt2 inputSignalFirst, OutIt1 processTimeFirst, OutIt2 processSignalFirst, OutIt3 recordTimeFirst, OutIt4 recordSignalFirst);
			void GetProcessedLengths(const size_t& newDataLength, size_t& newContainerSizeProc, size_t& newContainerSizeRec);		
		protected:
			static void SetDownsampler(Processing::Filter::CFIRfilter<T>& downsampler, const unsigned int& downsamplingFactor, const double& cutoffFreq, const double& transWidth, const double& samplingFreq);

			Processing::Filter::CFIRfilter<T> downsamplerProc;
			Processing::Filter::CFIRfilter<T> downsamplerRec;
			Processing::Filter::CFIRfilter<T> downsamplerSec;
			bool isProcDownsampling;
			bool isRecDownsampling;
			bool isReducedProcDownsampling;
			bool isReducedRecDownsampling;
			double cutoffFreqProc;
			double cutoffFreqRec;
			double transWidthProc;
			double transWidthRec;
			double samplingFreq;
		private:
			CAudioFullDownsampler(const CAudioFullDownsampler &);					// prevent copying
    		CAudioFullDownsampler & operator= (const CAudioFullDownsampler &);		// prevent assignment

			boost::shared_mutex parameterMutex;
			bool isInit;
		};
	}
}
/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Audio::CAudioFullDownsampler<T>::CAudioFullDownsampler(void)
	: isInit( false)
{
}



/**	@brief		Constructor
*	@param		downsamplingFactorProc					Downsampling factor required for the processing output data
*	@param		cutoffFreqProc							Cutoff frequency of the downsampling filter for the processing output data [Hz]
*	@param		transWidthProc							Transition width of the audio processing filter [Hz]
*	@param		downsamplingFactorRec					Downsampling factor required for the recording output data
*	@param		cutoffFreqRec							Cutoff frequency of the downsampling filter for the recording output data [Hz]
*	@param		transWidthRec							Transition width of the audio recording filter [Hz]
*	@param		samplingFreq							Sampling frequency of the input data [Hz]
*	@exception 											None
*	@remarks 											None
*/
template <class T> Core::Audio::CAudioFullDownsampler<T>::CAudioFullDownsampler(const unsigned int& downsamplingFactorProc, const double& cutoffFreqProc, const double& transWidthProc, const unsigned int& downsamplingFactorRec, const double& cutoffFreqRec, const double& transWidthRec, const double& samplingFreq)
{
	SetParameters( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
}



/**	@brief		Destructor
*/
template <class T> Core::Audio::CAudioFullDownsampler<T>::~CAudioFullDownsampler(void)
{
}



/**	@brief		Performing the downsampling during the signal processing
*	@param		inputTimeFirst							Iterator to the beginning of the container with the time data
*	@param		inputTimeLast							Iterator to one element after the end of the container with the time data
*	@param		inputSignalFirst						Iterator to the beginning of the container with the signal data
*	@param		processTimeFirst						Iterator to the beginning of the downsampled time corresponding to the processing data. It must be of correct size or std::back_inserter must be used, this might be less efficient.
*	@param		processSignalFirst						Iterator to the beginning of the downsampled signal data for processing. It must be of correct size or std::back_inserter must be used, this might be less efficient. The size is identical to that of processTimeFirst.
*	@param		recordTimeFirst							Iterator to the beginning of the downsampled time corresponding to the recorded data. It must be of correct size or std::back_inserter must be used.
*	@param		recordSignalFirst						Iterator to the beginning of the downsampled signal data for recording and possible later reuse. It must be of correct size or std::back_inserter must be used. The size is identical to that of recordTimeFirst.
*	@return 											None
*	@exception 											None
*	@remarks 											None
*/
template <class T> template <class InIt1, class InIt2, class OutIt1, class OutIt2, class OutIt3, class OutIt4> void Core::Audio::CAudioFullDownsampler<T>::PerformDownsampling(InIt1 inputTimeFirst, InIt1 inputTimeLast, InIt2 inputSignalFirst, OutIt1 processTimeFirst, OutIt2 processSignalFirst, OutIt3 recordTimeFirst, OutIt4 recordSignalFirst)
{
	using namespace std;

	size_t newContainerSizeProc, newContainerSizeRec;
	vector< typename iterator_traits<InIt1>::value_type > downsampledRecTime, downsampledProcTime;
	vector< typename iterator_traits<InIt2>::value_type > downsampledRecSignal, downsampledProcSignal;

	// preallocate containers for efficiency reasons
	GetProcessedLengths( distance( inputTimeFirst, inputTimeLast ), newContainerSizeProc, newContainerSizeRec );
	downsampledRecTime.resize( newContainerSizeRec );
	downsampledRecSignal.resize( newContainerSizeRec );
	downsampledProcTime.resize( newContainerSizeProc );
	downsampledProcSignal.resize( newContainerSizeProc );
	
	// perform downsampling
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex );
	if ( isReducedProcDownsampling ) { // reduced effort for downsampling of processing data
		downsamplerRec.Processing( inputTimeFirst, inputTimeLast, inputSignalFirst, downsampledRecTime.begin(), downsampledRecSignal.begin() );
		downsamplerSec.Processing( downsampledRecTime.begin(), downsampledRecTime.end(), downsampledRecSignal.begin(), downsampledProcTime.begin(), downsampledProcSignal.begin() );
	} else if ( isReducedRecDownsampling ) { // reduced effort for downsampling of recording data
		downsamplerProc.Processing( inputTimeFirst, inputTimeLast, inputSignalFirst, downsampledProcTime.begin(), downsampledProcSignal.begin() );
		downsamplerSec.Processing( downsampledProcTime.begin(), downsampledProcTime.end(), downsampledProcSignal.begin(), downsampledRecTime.begin(), downsampledRecSignal.begin() );
	} else { // separate downsampling for processing and recording data
		if ( isProcDownsampling ) {
			downsamplerProc.Processing( inputTimeFirst, inputTimeLast, inputSignalFirst, downsampledProcTime.begin(), downsampledProcSignal.begin() );
		} else { // filtering is not required
			downsampledProcTime.assign( inputTimeFirst, inputTimeLast );
			downsampledProcSignal.assign( inputSignalFirst, inputSignalFirst + distance( inputTimeFirst, inputTimeLast ) );
		}
		if ( isRecDownsampling ) {
			downsamplerRec.Processing( inputTimeFirst, inputTimeLast, inputSignalFirst, downsampledRecTime.begin(), downsampledRecSignal.begin() );
		} else { // filtering is not required
			downsampledRecTime.assign( inputTimeFirst, inputTimeLast );
			downsampledRecSignal.assign( inputSignalFirst, inputSignalFirst + distance( inputTimeFirst, inputTimeLast ) );		
		}
	}

	// set output values
	std::move( downsampledProcTime.begin(), downsampledProcTime.end(), processTimeFirst );
	std::move( downsampledProcSignal.begin(), downsampledProcSignal.end(), processSignalFirst );
	std::move( downsampledRecTime.begin(), downsampledRecTime.end(), recordTimeFirst );
	std::move( downsampledRecSignal.begin(), downsampledRecSignal.end(), recordSignalFirst );
}



/**	@brief		Obtaining the lengths of the processed and recorded data containers in the next downsampling operation
*	@param		newDataLength							Length of the next dataset to be downsampled
*	@param		newContainerSizeProc					Length of the processing dataset obtained by the next downsampling
*	@param		newContainerSizeRec						Length of the recording dataset obtained by the next downsampling
*	@return 											None
*	@exception 											None
*	@remarks 											The output may not be identical for following function calls even for identical newDataLength inputs because of the coupled downsampling for both data streams
*/
template <class T> void Core::Audio::CAudioFullDownsampler<T>::GetProcessedLengths(const size_t& newDataLength, size_t& newContainerSizeProc, size_t& newContainerSizeRec)
{
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	if ( isReducedProcDownsampling ) {
		newContainerSizeRec = downsamplerRec.ProcessedLength( newDataLength );
		newContainerSizeProc = downsamplerSec.ProcessedLength( newContainerSizeRec );
	} else if ( isReducedRecDownsampling ) {
		newContainerSizeProc = downsamplerProc.ProcessedLength( newDataLength );	
		newContainerSizeRec = downsamplerSec.ProcessedLength( newContainerSizeProc );
	} else {
		if ( isProcDownsampling ) {
			newContainerSizeProc = downsamplerProc.ProcessedLength( newDataLength );
		} else {
			newContainerSizeProc = newDataLength;
		}
		if ( isRecDownsampling ) {
			newContainerSizeRec = downsamplerRec.ProcessedLength( newDataLength );
		} else {
			newContainerSizeRec = newDataLength;
		}
	}
}



/**	@brief		Setting the class parameters
*	@param		downsamplingFactorProc					Downsampling factor required for the processing output data
*	@param		cutoffFreqProc							Cutoff frequency of the downsampling filter for the processing output data [Hz]
*	@param		transWidthProc							Transition width of the audio processing filter [Hz]
*	@param		downsamplingFactorRec					Downsampling factor required for the recording output data
*	@param		cutoffFreqRec							Cutoff frequency of the downsampling filter for the recording output data [Hz]
*	@param		transWidthRec							Transition width of the audio recording filter [Hz]
*	@param		samplingFreq							Sampling frequency of the input data [Hz]
*	@return												None
*	@exception 											None
*	@remarks 											None
*/
template <class T> void Core::Audio::CAudioFullDownsampler<T>::SetParameters(const unsigned int& downsamplingFactorProc, const double& cutoffFreqProc, const double& transWidthProc, const unsigned int& downsamplingFactorRec, const double& cutoffFreqRec, const double& transWidthRec, const double& samplingFreq)
{
	using namespace std;
	vector<T> recFilterParams, secFilterParams;

	boost::unique_lock<boost::shared_mutex> lock( parameterMutex );

	// set class parameters
	CAudioFullDownsampler<T>::cutoffFreqProc = cutoffFreqProc;
	CAudioFullDownsampler<T>::cutoffFreqRec = cutoffFreqRec;
	CAudioFullDownsampler<T>::transWidthProc = transWidthProc;
	CAudioFullDownsampler<T>::transWidthRec = transWidthRec;
	CAudioFullDownsampler<T>::samplingFreq = samplingFreq;
	
	// define multi-step downsampling filters, if this has a performance benefit
	isProcDownsampling = false;
	isRecDownsampling = false;
	isReducedProcDownsampling = false;
	isReducedRecDownsampling = false;
	if ( ( downsamplingFactorProc != downsamplingFactorRec ) && ( ( downsamplingFactorProc % downsamplingFactorRec ) == 0 ) && ( downsamplingFactorRec > 1 ) ) {			// first step: downsampling for recording, second step: downsampling for processing	
		CAudioFullDownsampler<T>::SetDownsampler( downsamplerRec, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
		CAudioFullDownsampler<T>::SetDownsampler( downsamplerSec, downsamplingFactorProc / downsamplingFactorRec, cutoffFreqProc, transWidthProc, samplingFreq / downsamplingFactorRec );
		isReducedProcDownsampling = true;
	} else if ( ( downsamplingFactorProc != downsamplingFactorRec ) && ( ( downsamplingFactorRec % downsamplingFactorProc ) == 0 ) && ( downsamplingFactorProc > 1 ) ) {	// first step: downsampling for processing, second step: downsampling for recording
		CAudioFullDownsampler<T>::SetDownsampler( downsamplerProc, downsamplingFactorProc, cutoffFreqProc, transWidthProc, samplingFreq );
		CAudioFullDownsampler<T>::SetDownsampler( downsamplerSec, downsamplingFactorRec / downsamplingFactorProc, cutoffFreqRec, transWidthRec, samplingFreq / downsamplingFactorProc );
		isReducedRecDownsampling = true;	
	} else {
		// define filter for processing data
		if ( downsamplingFactorProc > 1 ) {
			CAudioFullDownsampler<T>::SetDownsampler( downsamplerProc, downsamplingFactorProc, cutoffFreqProc, transWidthProc, samplingFreq );
			isProcDownsampling = true;
		}

		// define filter for recording data
		if ( downsamplingFactorRec > 1 ) {
			CAudioFullDownsampler<T>::SetDownsampler( downsamplerRec, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq );
			isRecDownsampling = true;
		}	
	}

	isInit = true;
}



/**	@brief		Initializing the passed downsampling FIR-filter
*	@param		downsampler								Downsampling FIR-filter to be initialized
*	@param		downsamplingFactor						Downsampling factor of the filter
*	@param		cutoffFreq								Cutoff frequency of the downsampling filter [Hz]
*	@param		transWidth								Transition width of the audio processing filter [Hz]
*	@param		samplingFreq							Sampling frequency of the input data [Hz]
*	@return												None
*	@exception 											None
*	@remarks 											None
*/
template <class T> void Core::Audio::CAudioFullDownsampler<T>::SetDownsampler(Processing::Filter::CFIRfilter<T>& downsampler, const unsigned int& downsamplingFactor, const double& cutoffFreq, const double& transWidth, const double& samplingFreq)
{
	using namespace std;
	vector<T> filterParams;

	Processing::Filter::CFIRfilter<T>::DesignLowPassFilter( static_cast<float>( transWidth ), static_cast<float>( cutoffFreq ), static_cast<float>( samplingFreq ), back_inserter( filterParams ) );
	if ( std::is_same<T,float>::value ) {
		downsampler.SetParams( filterParams.begin(), filterParams.end(), downsamplingFactor, 1, 1e-7f );  //accuracy of 1e-7f for the symmetry limit is required due to datatype float
	} else {
		downsampler.SetParams( filterParams.begin(), filterParams.end(), downsamplingFactor, 1 );	
	}
}



/**	@brief		Getting the class parameters
*	@param		downsamplingFactorProc					Downsampling factor required for the processing output data
*	@param		cutoffFreqProc							Cutoff frequency of the downsampling filter for the processing output data [Hz]
*	@param		transWidthProc							Transition width of the audio processing filter [Hz]
*	@param		downsamplingFactorRec					Downsampling factor required for the recording output data
*	@param		cutoffFreqRec							Cutoff frequency of the downsampling filter for the recording output data [Hz]
*	@param		transWidthRec							Transition width of the audio recording filter [Hz]
*	@param		samplingFreq							Sampling frequency of the input data [Hz]
*	@return												None
*	@exception 											None
*	@remarks 											None
*/
template <class T> void Core::Audio::CAudioFullDownsampler<T>::GetParameters(unsigned int& downsamplingFactorProc, double& cutoffFreqProc, double& transWidthProc, unsigned int& downsamplingFactorRec, double& cutoffFreqRec, double& transWidthRec, double& samplingFreq)
{
	int downsamplingFactor, upsamplingFactor;

	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	downsamplerProc.GetParams( downsamplingFactor, upsamplingFactor );
	downsamplingFactorProc = downsamplingFactor;
	cutoffFreqProc = CAudioFullDownsampler<T>::cutoffFreqProc;
	transWidthProc = CAudioFullDownsampler<T>::transWidthProc;

	downsamplerRec.GetParams( downsamplingFactor, upsamplingFactor );
	downsamplingFactorRec = downsamplingFactor;
	cutoffFreqRec = CAudioFullDownsampler<T>::cutoffFreqRec;
	transWidthRec = CAudioFullDownsampler<T>::transWidthRec;

	samplingFreq = CAudioFullDownsampler<T>::samplingFreq;
}
