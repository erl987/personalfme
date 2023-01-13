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
#include <queue>
#include <array>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <memory>
#include <boost/signals2.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include "FFT.h"
#include "DataProcessing.h"



/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace General {
		/**	\ingroup Core
		*	Class for calculation of frequency streams from a signal stream. In order to work properly, reliable parametes have to be used.
		*/
		template <class T>
		class CFrequencySearch
		{
		public:
			CFrequencySearch(void);
			template <class InputIterator> CFrequencySearch(double sampleLength, int freqResolution, double samplingFreq,int maxNumPeaks, double overlap, double delta, InputIterator searchFreqFirst, InputIterator searchFreqLast, std::function<void(const std::string&)> runtimeErrorCallback);
			~CFrequencySearch(void);
			template <class InputIterator> void SetParameters(double sampleLength, int freqResolution, double samplingFreq, int maxNumPeaks, double overlap, double delta, InputIterator searchFreqFirst, InputIterator searchFreqLast, std::function<void(const std::string&)> runtimeErrorCallback);
			template <class OutputIterator> void GetParameters(double& sampleLength, int& freqResolution, double& samplingFreq, int& maxNumPeaks, double& overlap, double& delta, OutputIterator searchFreqFirst);
			template <class In_It1, class In_It2, class In_It3> void PutSignal(In_It1 timeCalcFirst, In_It1 timeCalcLast, In_It2 timeRefFirst, In_It2 timeRefLast, In_It3 signalFirst, In_It3 signalLast);
			template <class Out_It1, class Out_It2, class Out_It3, class Out_It4> Out_It1 GetPeaks(Out_It1 timeCalcFirst, Out_It2 timeRefFirst, Out_It3 peaksFirst, Out_It4 absToneLevelsFirst );
		private:
			CFrequencySearch(const CFrequencySearch &) = delete;					// prevent copying
    		CFrequencySearch & operator= (const CFrequencySearch &) = delete;		// prevent assignment
			template <class In_It1, class Out_It1, class Out_It2, class Out_It3, class Out_It4> void SearchFrequencyPeaks(boost::posix_time::ptime startTimeCalc, boost::posix_time::ptime startTimeRef, In_It1 currentSignalFirst, In_It1 currentSignalLast, Out_It1 timeCalcFirst, Out_It2 timeRefFirst, Out_It3 peaksFirst, Out_It4 absToneLevelsFirst );
			void FrequencySearchThread(void);

			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			std::unique_ptr<boost::thread > threadFrequencySearch;
			boost::mutex inputSignalMutex;
			boost::mutex resultMutex;
			boost::shared_mutex parameterMutex;
			boost::condition_variable_any newSignalDataCondition;
			Core::Processing::CFFT<double> fft;
			std::vector< boost::posix_time::ptime > signalCalcTime;
			std::vector< boost::posix_time::ptime > signalRefTime;
			std::vector<T> signal;
			std::vector< boost::posix_time::ptime > peaksCalcTime;
			std::vector< boost::posix_time::ptime > peaksRefTime;
			std::vector< std::vector<T> > peaks;
			std::vector< std::vector<T> > absToneLevels;
			std::vector<T> searchFreqs;
			int numSamples;
			int freqResolution;
			int maxNumPeaks;
			double samplingFreq;
			double overlap;
			double delta;
			bool isInit;
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T>
Core::General::CFrequencySearch<T>::CFrequencySearch(void)
	:isInit(false)
{
}



/**	@brief		Constructor for initialization of all parameters.
*	@param		sampleLength				Length of a single section used for spectrogram calculation [ms]
*	@param		freqResolution				Frequency resolution, given in number of samples
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		maxNumPeaks					Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good time resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		overlap						Relative overlap in the good time resolution spectrogram [%/100]
*	@param		delta						Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good time resolution stream)
*	@param		searchFreqFirst				Iterator to beginning of container storing the frequencies of all tones.
*	@param		searchFreqLast				Iterator to end of container storing the frequencies of all tones.
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception 								None
*	@remarks 								The parameters must be set before using the class. CFrequenySearch<T>::SetParameters can be used as an alternative and for later changes of parameters.
*/
template <class T>
template <class InputIterator> Core::General::CFrequencySearch<T>::CFrequencySearch(double sampleLength, int freqResolution, double samplingFreq, int maxNumPeaks, double overlap, double delta, InputIterator searchFreqFirst, InputIterator searchFreqLast, std::function<void(const std::string&)> runtimeErrorCallback)
	:isInit(false)
{
	// set parameters
	SetParameters(sampleLength, freqResolution, samplingFreq, maxNumPeaks, overlap, delta, searchFreqFirst, searchFreqLast, runtimeErrorCallback);
}



/**	@brief		Destructor.
*/
template <class T>
Core::General::CFrequencySearch<T>::~CFrequencySearch()
{
	if ( threadFrequencySearch != nullptr ) {
		// stop running thread
		threadFrequencySearch->interrupt();
	
		// wait until thread has stopped
		threadFrequencySearch->join();
	}
}



/**	@brief		Setting of the class parameters.
*	@param		sampleLength				Length of a single section used for spectrogram calculation [ms]
*	@param		freqResolution				Frequency resolution, given in number of samples
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		maxNumPeaks					Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good time resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		overlap						Relative overlap in the good time resolution spectrogram [%/100]
*	@param		delta						Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good time resolution stream)
*	@param		searchFreqFirst				Iterator to beginning of container storing the frequencies of all tones.
*	@param		searchFreqLast				Iterator to end of container storing the frequencies of all tones.
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception	std::runtime_error			Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 								This function can be used to reset the parameters.
*/
template <class T>
template <class InputIterator> void Core::General::CFrequencySearch<T>::SetParameters(double sampleLength, int freqResolution, double samplingFreq, int maxNumPeaks, double overlap, double delta, InputIterator searchFreqFirst, InputIterator searchFreqLast, std::function<void(const std::string&)> runtimeErrorCallback)
{
	std::map< T, std::tuple< T, std::vector<T>, std::vector<T> > > filterParam;
	std::vector < std::complex<T> > gain;
	
	// stop thread if it is running
	if ( threadFrequencySearch != nullptr ) {
		threadFrequencySearch->interrupt();
	
		// wait until thread has stopped
		threadFrequencySearch->join();
	}

	// lock parameter variables
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		// obtain new parameters
		CFrequencySearch<T>::numSamples = static_cast<int>( sampleLength / 1000 * samplingFreq );
		CFrequencySearch<T>::freqResolution = freqResolution;
		CFrequencySearch<T>::samplingFreq = samplingFreq;
		CFrequencySearch<T>::maxNumPeaks = maxNumPeaks;
		CFrequencySearch<T>::overlap = overlap;
		CFrequencySearch<T>::delta = delta;
		CFrequencySearch<T>::searchFreqs.assign( searchFreqFirst, searchFreqLast );

		// adjust parameters
		CFrequencySearch<T>::fft.Init( CFrequencySearch<T>::freqResolution );

		// initialize signaling of errors in the frequency search thread
		runtimeErrorSignal.disconnect_all_slots();
		runtimeErrorSignal.connect( runtimeErrorCallback );

		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}

	// start new frequency search thread
	threadFrequencySearch = std::make_unique<boost::thread>( &CFrequencySearch<T>::FrequencySearchThread, this );
}



/**	@brief		Obtain class parameters.
*	@param		sampleLength				Length of a single section used for spectrogram calculation [ms]
*	@param		freqResolution				Frequency resolution, given in number of samples
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		maxNumPeaks					Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good time resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		overlap						Relative overlap in the good time resolution spectrogram [%/100]
*	@param		delta						Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good time resolution stream)
*	@param		searchFreqFirst				Set to iterator pointing to a container storing the frequencies of all tones. Use std::back_inserter(searchFreq) if you do not know the length of the container in advance.
*	@return 								None
*	@exception 	std::runtime_error			Thrown if the parameters have not been set before
*	@remarks 								The parameters must be set before using the class. CFrequenySearch<T>::SetParameters can be used as an alternative and for later changes of parameters.
*/
template <class T>
template <class OutputIterator> void Core::General::CFrequencySearch<T>::GetParameters(double& sampleLength, int& freqResolution, double& samplingFreq, int& maxNumPeaks, double& overlap, double& delta, OutputIterator searchFreqFirst)
{
	if ( isInit ) {
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		sampleLength = 1000.0 * CFrequencySearch<T>::numSamples / CFrequencySearch<T>::samplingFreq;
		freqResolution = CFrequencySearch<T>::freqResolution;
		samplingFreq = CFrequencySearch<T>::samplingFreq;
		maxNumPeaks = CFrequencySearch<T>::maxNumPeaks;
		overlap = CFrequencySearch<T>::overlap;
		delta = CFrequencySearch<T>::delta;
		for (size_t i=0; i < this->CFrequencySearch<T>::searchFreqs.size(); i++) {
			*(this->searchFreqsFirst++) = this->CFrequencySearch<T>::searchFreqs[i];
		}
	} else {
		throw std::runtime_error("Parameters are not set.");
	}
}



/**	@brief		Put new signal data into the class.
*	@param		timeCalcFirst		Iterator to beginning of container with new calculated times belonging to the signal data - the data type is boost::posix_time:ptime
*	@param		timeCalcLast		Iterator to end of container with new calculated times belonging to the signal data
*	@param		timeRefFirst		Iterator to beginning of container with new reference times belonging to the signal data - the data type is boost::posix_time:ptime
*	@param		timeRefLast			Iterator to end of container with new reference times belonging to the signal data
*	@param		signalFirst			Iterator to beginning of container with new signal data
*	@param		signalLast			Iterator to end of container with new signal data
*	@return 						None
*	@exception 						None
*	@remarks 						For oversampling filtering the data needs to be low-passed filtered before passing the data into this class. Downsampling however is performed in the class.
*/
template <class T>
template <class In_It1, class In_It2, class In_It3> void Core::General::CFrequencySearch<T>::PutSignal(In_It1 timeCalcFirst, In_It1 timeCalcLast, In_It2 timeRefFirst, In_It2 timeRefLast, In_It3 signalFirst, In_It3 signalLast)
{
	using namespace std;

	// add new data to signal data stream
	boost::unique_lock<boost::mutex> lock( inputSignalMutex );
	signalCalcTime.insert( signalCalcTime.end(), timeCalcFirst, timeCalcLast );
	signalRefTime.insert( signalRefTime.end(), timeRefFirst, timeRefLast );
	signal.insert( signal.end(), signalFirst, signalLast );

	// trigger excecution of frequency analysis thread
	if ( distance( timeCalcFirst, timeCalcLast ) > 0 ) {
		newSignalDataCondition.notify_all();
	}
}



/**	@brief		Find frequency peaks in a signal data stream.
*	@param		startTimeCalc		Calculated start time of the signal data to be processed - high relative precision, but not useful for absolute time (data type: boost::posix_time::ptime)
*	@param		startTimeRef		Reference start time of the signal data to be processed - absolute precision depending on the operating system (around 15 ms) (data type: boost::posix_time::ptime)
*	@param		currentSignalFirst	Iterator to beginning of the container with the signal data to be processed (data type: T)
*	@param		currentSignalLast	Iterator to end of the container with the signal data to processed
*	@param		timeCalcFirst		Iterator to beginning of the container with the corresponding calculated times. It will be set after calling the function. It is given as absolute time with data type boost::posix_time::ptime.
*	@param		timeCalcFirst		Iterator to beginning of the container with the corresponding reference times. It will be set after calling the function. It is given as absolute time with data type boost::posix_time::ptime.
*	@param		peaksFirst			Iterator to beginning of the container with the found peaks. It will be set after calling the function. It is a 2D-container, the first dimension is corresponding to the timesteps and the second one to maxNumPeaks. The first dimension size can be obtained by CFFT<T>::GetNumSpectrogramTimesteps.		
*	@param		absToneLevelsFirst	Iterator to beginning of the container with the absolute signal level of the found peaks (in 'peaksFirst'). It will be set after calling the function. It is a 2D-container, the first dimension is corresponding to the timesteps and the second one to maxNumPeaks.
*	@return 						None
*	@exception 						None
*	@remarks 						None
*/
template <class T>
template <class In_It1, class Out_It1, class Out_It2, class Out_It3, class Out_It4> void Core::General::CFrequencySearch<T>::SearchFrequencyPeaks(boost::posix_time::ptime startTimeCalc, boost::posix_time::ptime startTimeRef, In_It1 currentSignalFirst, In_It1 currentSignalLast, Out_It1 timeCalcFirst, Out_It2 timeRefFirst, Out_It3 peaksFirst, Out_It4 absToneLevelsFirst)
{
	using boost::numeric_cast;
	using namespace boost::posix_time;
	using namespace Core::Processing;
	using namespace std;

	vector< vector<double> > spectrum;
	vector< vector<T> > peaks, absToneLevels;
	vector<double> time, freq, currentSignal, currentSpectrum, normalizedSpectrum, minPeaksNew, maxPeaksNew;
	vector<T> maxPeaks, absToneLevelsNew;
	
	// obtain input data
	currentSignal.resize( distance( currentSignalFirst, currentSignalLast ) );
	transform( currentSignalFirst, currentSignalLast, begin( currentSignal ), []( auto val ) { return ( numeric_cast<double>( val ) ); } ); // converts to double

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
	
	// calculate spectrogram with relative times - datatype double is required for FFT in order to ensure sufficient accuracy for the time
	spectrum.resize( Core::Processing::CFFT<T>::GetNumSpectrogramTimesteps( numeric_cast<int>( currentSignal.size() ), overlap, numSamples ) );
	for (size_t i=0; i < spectrum.size(); i++) {
		spectrum[i].resize( freqResolution );
	}

	freq.resize( freqResolution );
	time.resize( Processing::CFFT<T>::GetNumSpectrogramTimesteps( currentSignal.size(), overlap, numSamples ) );
	fft.Spectrogram( spectrum.begin(), freq.begin(), time.begin(), currentSignal.begin(), currentSignal.end(), numSamples, overlap, samplingFreq );

	// analyze spectrogram
	for ( size_t i=0; i < spectrum.size(); i++ ) {
		currentSpectrum.assign( spectrum[i].begin(), spectrum[i].end() );

		// normalize data
		normalizedSpectrum.resize( currentSpectrum.size() );
		CDataProcessing<double>::NormalizeData( currentSpectrum.begin(), currentSpectrum.end(), normalizedSpectrum.begin() );

		// find peaks
		minPeaksNew.clear();
		maxPeaksNew.clear();
		CDataProcessing<double>::FindPeaks( freq.begin(), freq.end(), normalizedSpectrum.begin(), back_inserter( minPeaksNew ), back_inserter( maxPeaksNew ), delta );
		
		// ignore a too large number of peaks, which indicates noise
		if ( numeric_cast<int>( maxPeaksNew.size() ) > maxNumPeaks ) {
			maxPeaksNew.clear();
		}

		maxPeaks.resize( maxPeaksNew.size() );
		transform( begin( maxPeaksNew ), end( maxPeaksNew ), begin( maxPeaks ), []( auto val ) { return static_cast<T>( val ); } ); // convert back to datatype T
		peaks.push_back( maxPeaks );

		// determine the absolute peak levels
		absToneLevelsNew.clear();
		for ( auto currPeak : maxPeaksNew ) {
			auto currPeakIndex = distance( begin( freq ), find_if( begin( freq ), end( freq ), [=]( auto val ) { return ( val >= currPeak ); } ) );
			absToneLevelsNew.push_back( static_cast<T>( *( begin( currentSpectrum ) + currPeakIndex ) ) ); // convert back to datatype T
		}
		absToneLevels.push_back( absToneLevelsNew );
	}

	// set output containers
	transform( begin( time ), end( time ), timeCalcFirst, [=]( auto currTime ) { return ( startTimeCalc + microseconds( static_cast<long>( currTime * 1.0e6 ) ) ); } );	// output time is absolute - conversion errors < 1 µs are not relevant here (ms-range)
	transform( begin( time ), end( time ), timeRefFirst, [=]( auto currTime ) { return ( startTimeRef + microseconds( static_cast<long>( currTime * 1.0e6 ) ) ); } );	// the reference timestamps are interpolated by calculated timesteps
	move( begin( peaks ), end( peaks ), peaksFirst );
	move( begin( absToneLevels ), end( absToneLevels ), absToneLevelsFirst );
}



/**	@brief		Function containing the thread continously analyzing the signal data
*	@return 						None
*	@exception 	std::runtime_error	Thrown if the parameters of the object were not set properly before using the function
*	@remarks 						The analysis is performed whenever new data has been notified by CFrequencySearch<T>::PutSignal()
*/
template <class T>
void Core::General::CFrequencySearch<T>::FrequencySearchThread()
{
	using namespace std;

	vector< boost::posix_time::ptime > newCalcTimes, newRefTimes;
	deque< boost::posix_time::ptime> currentCalcTime, currentRefTime;
	deque<T> currentSignal;
	vector< vector<T> > newPeaks, newAbsToneLevels;
	
	try {
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		newPeaks.resize( Processing::CFFT<T>::GetNumSpectrogramTimesteps( numSamples, overlap, numSamples ) );
		newAbsToneLevels.resize( newPeaks.size() );
		for (size_t i=0; i < newPeaks.size(); i++) {
			newPeaks[i].resize( maxNumPeaks );
			newAbsToneLevels[i].resize( maxNumPeaks );
		}
		newCalcTimes.resize( newPeaks.size() );
		newRefTimes.resize( newPeaks.size() );

		// process audio data until interruption is requested
		while ( !(boost::this_thread::interruption_requested()) ) {
			// read from signal queue	
			boost::unique_lock<boost::mutex> lock( inputSignalMutex );
			currentCalcTime.insert( currentCalcTime.end(), signalCalcTime.begin(), signalCalcTime.end() );
			currentRefTime.insert( currentRefTime.end(), signalRefTime.begin(), signalRefTime.end() );
			currentSignal.insert( currentSignal.end(), signal.begin(), signal.end() );
			signalCalcTime.clear();
			signalRefTime.clear();
			signal.clear();
		
			if ( (int)(currentSignal.size()) >= numSamples ) {
				// search for peaks in the new signal spectrogram
				lock.unlock();
				
				SearchFrequencyPeaks( currentCalcTime.front(), currentRefTime.front(), currentSignal.begin(), currentSignal.begin() + numSamples, newCalcTimes.begin(), newRefTimes.begin(), newPeaks.begin(), newAbsToneLevels.begin() );
				currentCalcTime.erase( currentCalcTime.begin(), currentCalcTime.begin() + numSamples );
				currentRefTime.erase( currentRefTime.begin(), currentRefTime.begin() + numSamples );
				currentSignal.erase( currentSignal.begin(), currentSignal.begin() + numSamples );

				// move result to data stream
				boost::unique_lock<boost::mutex> lockResult( resultMutex );
				peaksCalcTime.insert( peaksCalcTime.end(), newCalcTimes.begin(), newCalcTimes.end() );
				peaksRefTime.insert( peaksRefTime.end(), newRefTimes.begin(), newRefTimes.end() );
				peaks.insert( peaks.end(), newPeaks.begin(), newPeaks.end() );
				absToneLevels.insert( absToneLevels.end(), newAbsToneLevels.begin(), newAbsToneLevels.end() );
			} else {
				// wait for new audio data
    			newSignalDataCondition.wait( inputSignalMutex ); 
			}
		}
	} catch (std::exception e) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Frequency search thread: " + string( e.what() ) );
	}
}



/**	@brief		Gets all found peaks beginning from the last call of this function
*	@param		timeCalcFirst		Iterator to beginning of container that will store the calculated times (datatype boost::posix_time::ptime) corresponding to all new found peaks
*	@param		timeRefFirst		Iterator to beginning of container that will store the reference times (datatype boost::posix_time::ptime) corresponding to all new found peaks
*	@param		peaksFirst			Iterator to beginning of container that will store all new peaks (data format: std::vector<T>)	
*	@param		absToneLevelsFirst	Iterator to beginning of container that will store the absolute signal levels of all found peaks (in 'peaksFirst', data format: std::vector<T>)
*	@return 						Iterator to the end of the time container
*	@exception 						None
*	@remarks 						All data that is transfered with this function is deleted and no longer accessible. Reference time: Absolute time stamp with around 15 ms precision (depending on the operating system), required for reference.
*									Calculated time: High precision relative time stamps. The absolute value is not useful.
*/
template <class T>
template <class Out_It1, class Out_It2, class Out_It3, class Out_It4> Out_It1 Core::General::CFrequencySearch<T>::GetPeaks(Out_It1 timeCalcFirst, Out_It2 timeRefFirst, Out_It3 peaksFirst, Out_It4 absToneLevelsFirst)
{
	std::vector< boost::posix_time::ptime > newPeaksCalcTime, newPeaksRefTime;
	std::vector < std::vector<T> > newPeaks, newAbsToneLevels;

	// copy to local data in order to reduce lock time
	boost::unique_lock<boost::mutex> lockResult( resultMutex );
	newPeaksCalcTime.assign( peaksCalcTime.begin(), peaksCalcTime.end() );
	newPeaksRefTime.assign( peaksRefTime.begin(), peaksRefTime.end() );
	newPeaks.assign( peaks.begin(), peaks.end() );
	newAbsToneLevels.assign( absToneLevels.begin(), absToneLevels.end() );

	// delete old results
	peaksCalcTime.clear();
	peaksRefTime.clear();
	peaks.clear();
	absToneLevels.clear();

	lockResult.unlock();

	// copy to output
	std::move( newPeaksRefTime.begin(), newPeaksRefTime.end(), timeRefFirst );
	std::move( newPeaks.begin(), newPeaks.end(), peaksFirst );
	std::move( newAbsToneLevels.begin(), newAbsToneLevels.end(), absToneLevelsFirst );
	return std::move( newPeaksCalcTime.begin(), newPeaksCalcTime.end(), timeCalcFirst );
}