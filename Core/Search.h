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
#include <deque>
#include <map>
#include <tuple>
#include <string>
#include <fstream>
#include <memory>
#include <thread>
#include <chrono>
#if defined _WIN32
	#include <boost/config/compiler/visualc.hpp> // Supression of C4503: truncation warning, required for Boost Signals2 library
#endif
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "ToneSearch.h"
#include "AnalysisParam.h"
#include "FrequencySearch.h"
#include "SeqData.h"
#include "SeqDataComplete.h"


/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace General {
		/**	\ingroup Core
		*	Abstract base class for finding sequences in an audio signal stream. This class requires calls to a virtual function in the analysis thread. 
		*	In order to prevent 'pure virtual function calls', the thread must be started manually at the end of the derived class constructor and manually stopped
		*	in the derived class destructor by calling StartThread() and StopThread(). Otherwise indeterministic crashes with 'pure virtual function call' will occur.
		*/
		template <class T> class CSearch
		{
		public:
			CSearch(void);
			CSearch(double samplingFreq, std::string parameterFileName, std::function<void(const std::string&)> runtimeErrorCallback);
			virtual ~CSearch(void);
			void SetParameters(double samplingFreq, std::string parameterFileName, std::function<void(const std::string&)> runtimeErrorCallback);
			void GetParameters(double& samplingFreq);
			template <class In_It1, class In_It2> void PutSignalData(In_It1 refTimeFirst, In_It1 refTimeLast, In_It2 signalFirst, In_It2 signalLast);
			virtual std::deque< Utilities::CSeqDataComplete<T> > GetSequencesDebug(void) = 0;
			virtual std::deque< Utilities::CSeqData > GetSequences(void) = 0;
			void SaveParameters(std::string filterFileName, CAnalysisParam params);
		protected:
			void LoadParameters(std::string filterFileName, CAnalysisParam &params);
			template <class Out_It> Out_It CalculateTones(Out_It tonesFirst);
			template <class In_It, class In_It2, class In_It3> void SetNewSignalData(In_It calcSignalTimeFirst, In_It calcSignalTimeLast, In_It2 refSignalTimeFirst, In_It2 refSignalTimeLast, In_It3 signalFirst, In_It3 signalLast);
			virtual void PerformSpecializedCalculation(std::vector< std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, T, T > > tones) = 0;
			void AnalysisThread(void);
			void StartThread();
			void StopThread();
		private:
			std::unique_ptr< boost::thread > analysisThread;
			boost::mutex signalMutex;
			boost::shared_mutex parameterMutex;
			General::CToneSearch<T> toneSearch;			
			General::CFrequencySearch<T> freqSearch;
			General::CFrequencySearch<T> freqSearchCoarse;
			CAnalysisParam params;
			double samplingFreq;
			double sampleLengthCoarse;
			int maxPeaksCoarse;
			double maxDeltaF;
			double sampleLength;
			int maxPeaks;
			double searchTimestep;
			boost::posix_time::ptime lastCalcTime;
			std::vector< boost::posix_time::ptime > calcSignalTime;
			std::vector< boost::posix_time::ptime > refSignalTime;
			std::vector<T> signal;
			std::vector<T> searchFreqs;
			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			bool isInit;
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::General::CSearch<T>::CSearch()
	: isInit(false), 
	  lastCalcTime( boost::posix_time::ptime( boost::posix_time::not_a_date_time ) )
{
}



/**	@brief		Constructor for initialization of all parameters.
*	@param		samplingFreq						Sampling frequency [Hz]
*	@param		parameterFileName					File name of parameter file for general sequence search (*.dat), it can be given relative to the current path. It is assumed that all underlying parameter files are located in the same path.
*	@param		runtimeErrorCallback				Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T> Core::General::CSearch<T>::CSearch(double samplingFreq, std::string parameterFileName, std::function<void(const std::string&)> runtimeErrorCallback)
	: lastCalcTime( boost::posix_time::ptime( boost::posix_time::not_a_date_time ) ),
	  isInit(false)
{
	SetParameters( samplingFreq, parameterFileName, runtimeErrorCallback );
}



/**
* 	@brief		Default destructor.
*/
template <class T> Core::General::CSearch<T>::~CSearch()
{
	StopThread();
}



/**	@brief		Set parameters of the class.
*	@param		samplingFreq						Sampling frequency [Hz]
*	@param		parameterFileName					File name of parameter file for general sequence search (*.dat) with the full absolute path
*	@param		runtimeErrorCallback				Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T> void Core::General::CSearch<T>::SetParameters(double samplingFreq, std::string parameterFileName, std::function<void(const std::string&)> runtimeErrorCallback)
{
	using namespace std;
	using namespace boost::filesystem;

	int maxPeaks, maxPeaksCoarse, freqResolution, freqResolutionCoarse, numNeighbours;
	double sampleLength, sampleLengthCoarse, maxDeltaF, overlap, overlapCoarse, delta, deltaCoarse, maxFreqDevConstrained, maxFreqDevUnconstrained, evalToneLength;
	double searchTimestep;
	vector<double> searchFreqs;
	map< int, T > searchTones;

	// lock parameter variables
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex );

	// assign parameters
	CSearch<T>::samplingFreq = samplingFreq;

	// initialize calculation objects
	LoadParameters( ( absolute( path( parameterFileName ) ) ).string(), params );
	params.Get( sampleLength, sampleLengthCoarse, maxPeaks, maxPeaksCoarse, freqResolution, freqResolutionCoarse, maxDeltaF, overlap, overlapCoarse, delta, deltaCoarse, maxFreqDevConstrained, maxFreqDevUnconstrained, numNeighbours, evalToneLength, searchTimestep, back_inserter( searchFreqs ) );	
	
	CSearch<T>::searchFreqs.resize( searchFreqs.size() );
	for (size_t i=0; i < searchFreqs.size(); i++) {
		CSearch<T>::searchFreqs[i] = static_cast<T>( searchFreqs[i] );
	}	

	freqSearch.SetParameters( sampleLength, freqResolution, samplingFreq, maxPeaks, overlap, delta, CSearch<T>::searchFreqs.begin(), CSearch<T>::searchFreqs.end(), runtimeErrorCallback );
	freqSearchCoarse.SetParameters( sampleLengthCoarse, freqResolutionCoarse, samplingFreq, maxPeaksCoarse, overlapCoarse, deltaCoarse, CSearch<T>::searchFreqs.begin(), CSearch<T>::searchFreqs.end(), runtimeErrorCallback );

	CSearch<T>::sampleLengthCoarse = sampleLengthCoarse;
	CSearch<T>::maxPeaksCoarse = maxPeaksCoarse;
	CSearch<T>::maxDeltaF = maxDeltaF;
	CSearch<T>::sampleLength = sampleLength;
	CSearch<T>::maxPeaks = maxPeaks;
	CSearch<T>::searchTimestep = searchTimestep;

	// initialize signaling of errors in the tone search thread
	runtimeErrorSignal.disconnect_all_slots();
	runtimeErrorSignal.connect( runtimeErrorCallback );

	// obtain tones from the frequency stream
	for (size_t i=0; i < searchFreqs.size(); i++) {
		searchTones.insert( pair<int,T>( i + 1, static_cast<T>( searchFreqs[i] ) ) );
	}
	auto deltaT = boost::posix_time::microseconds( static_cast<long>( static_cast<int>( sampleLength / 1000 * samplingFreq ) / samplingFreq * 1e6 ) ); // time stepping with fine time resolution
	toneSearch.SetParameters( maxDeltaF, maxFreqDevConstrained, maxFreqDevUnconstrained, numNeighbours, evalToneLength, deltaT, searchTones, runtimeErrorCallback );

	isInit = true;
}



/**	@brief		Starting the analysis thread required for processing
*	@return 										None
*	@exception 										None
*	@remarks 										This function is required because automatic starting of the thread is not possible due to calls to the pure virtual function 'PerformSpecializedCalculation' of the derived class.
*													Otherwise program crashes with 'pure virtual function call' might be possible before and after construction and destruction of the derived class. Call this function from the
*													derived class in the constructor.
*/
template <class T> void Core::General::CSearch<T>::StartThread(void)
{
	StopThread();

	// start analysis thread
	analysisThread = std::make_unique<boost::thread>( &CSearch<T>::AnalysisThread, this );
}



/**	@brief		Stopping the analysis thread required for processing
*	@return 										None
*	@exception 										None
*	@remarks 										This function is required because automatic stopping of the thread is not possible due to calls to the pure virtual function 'PerformSpecializedCalculation' of the derived class.
*													Otherwise program crashes with 'pure virtual function call' might be possible before and after construction and destruction of the derived class. Call this function from the
*													derived class in the denstructor.
*/
template <class T> void Core::General::CSearch<T>::StopThread(void)
{
	if ( analysisThread != nullptr ) {
		// stop running thread
		analysisThread->interrupt();
	
		// wait until thread has stopped
		analysisThread->join();

		analysisThread.reset();
	}
}



/**	@brief		Get parameters of the class.
*	@param		samplingFreq						Sampling frequency [Hz]
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T> void Core::General::CSearch<T>::GetParameters(double& samplingFreq)
{
	// lock parameter variables
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	if ( isInit ) {
		samplingFreq = CSearch<T>::samplingFreq;
	} else {
		throw std::runtime_error("Parameters are not set.");
	}
}



/**	@brief		Add new signal data to stream.
*	@param		refTimeFirst						Iterator to beginning of container with new reference times belonging to the signal data - the time must be in the format boost::posix_time:ptime. The values are required to be the real timestamps, medium precision around 15 ms is fully acceptable. It is not used directly for evaluation purposes.
*	@param		refTimeLast							Iterator to one element after the end of the container with the new reference times belonging to the signal data
*	@param		signalFirst							Iterator to beginning of signal data stream corresponding to the sampling frequency
*	@param		signalLast							Iterator to one element after the end of the signal data stream corresponding to the sampling frequency
*	@return 	std::runtime_error					Thrown if the analysis thread was not started before calling the function or was already stopped. Starting is performed with StartThread() from the derived class.
*	@exception 	std::length_error					Thrown if time and signal container have different lengths
*	@remarks 										The data size should be at least 100 - 1000. The smaller the datasize, the larger the deviation in length of the calculated time stamps will be (< 2 µs per call of this function)
*/
template <class T> template <class In_It1, class In_It2> void Core::General::CSearch<T>::PutSignalData(In_It1 refTimeFirst, In_It1 refTimeLast, In_It2 signalFirst, In_It2 signalLast)
{
	using namespace std;
	using namespace boost::posix_time;

	vector<ptime> newCalcSignalTime;

	// check if analysis thread is running
	if ( analysisThread == nullptr ) {
		throw std::runtime_error( "The analysis thread was not started before calling the function." );
	}

	// check for identical size of the data containers
	if ( distance( refTimeFirst, refTimeLast ) != distance( signalFirst, signalLast ) ) {
		throw std::length_error( "Signal time and data container do not have the identical length." );
	}

	// calculated theoretical times (required due to too low time resolution of typical timestamps in refSignalTime)
	newCalcSignalTime.resize( distance( refTimeFirst, refTimeLast ) );
	if ( newCalcSignalTime.size() > 0 ) {
		if ( !( lastCalcTime.is_not_a_date_time() ) ) {
			newCalcSignalTime[0] = lastCalcTime + microseconds( static_cast<long>( 1.0e6 / samplingFreq ) ); // maximum error is 1 µs
		} else {
			newCalcSignalTime[0] = *refTimeFirst; // the first calculated time stamps is defined here - its absolute value is without any importance
		}
		if ( newCalcSignalTime.size() > 1 ) {
			// calculation of time stamps using the sampling frequency, this is identical to the theoretical time stamp
			for (size_t i=1; i < newCalcSignalTime.size(); i++) {
				newCalcSignalTime[i] = newCalcSignalTime[0] + microseconds( static_cast<long>( i * 1.0e6 / samplingFreq ) ); // maximum error is 1 µs
			}
		}
		lastCalcTime = newCalcSignalTime.back();
	}

	// lock signal data mutex
	boost::unique_lock<boost::mutex> lock( signalMutex );

	// push into data stream
	refSignalTime.insert( refSignalTime.end(), refTimeFirst, refTimeLast );
	calcSignalTime.insert( calcSignalTime.end(), newCalcSignalTime.begin(), newCalcSignalTime.end() );
	signal.insert( signal.end(), signalFirst, signalLast );
}



/**	@brief		Obtaining a tone stream from a signal stream
*	@param		tonesFirst							Iterator to the container with the found tones. Use std::back_inserter. Datatype: std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, T > (tone index, reference start time, calculated start time, calculated stop time, tone frequency).
*	@return 										Iterator to one element after the end of the container with the found tones
*	@exception	std::runtime_error					Thrown if the object was not initialized using the constructor or alternatively SetParameters before calling this function
*	@remarks 										The input data is internally stored in a continuous queue.
*/
template <class T> template <class Out_It> Out_It Core::General::CSearch<T>::CalculateTones(Out_It tonesFirst)
{
	using boost::numeric_cast;
	using namespace boost::posix_time;
	using namespace std;

	vector< tuple< int, ptime, ptime, ptime, T, T > > newTones;
	vector<ptime> timeCalc, timeCalcCoarse, timeRef, timeRefCoarse;
	vector< vector<T> > peaks, peaksCoarse, absToneLevels, absToneLevelsCoarse;

	// check if parameters were set before
	if ( !isInit ) {
		throw std::runtime_error("Parameters are not set.");		
	}

	// obtain latest results from coarse frequency peak search - these are usually older results!
	freqSearchCoarse.GetPeaks( back_inserter( timeCalcCoarse ), back_inserter( timeRefCoarse ), back_inserter( peaksCoarse ), back_inserter( absToneLevelsCoarse ) );
	
	// obtain latest results from fine frequency peak search - these are usually older results!
	freqSearch.GetPeaks( back_inserter( timeCalc ), back_inserter( timeRef ), back_inserter( peaks ), back_inserter( absToneLevels ) );

	// start calculation of tones in an own thread with the newly available frequency peaks - reference times are required for adding the real timestamp to the tones
	if ( !( peaks.empty() && peaksCoarse.empty() ) ) {
		toneSearch.PutFrequencyStream( timeRef.begin(), timeRef.end(), timeCalc.begin(), peaks.begin(), timeCalcCoarse.begin(), timeCalcCoarse.end(), peaksCoarse.begin(), absToneLevelsCoarse.begin() );
	}

	// obtain results from tone search - these are usually older results!
	toneSearch.GetTones( back_inserter( newTones ) );

	// copy to output
	for (size_t i=0; i < newTones.size(); i++) {
		*(tonesFirst++) = newTones[i];
	}

	return tonesFirst;
}



/**	@brief		Feeding the analysis threads for obtaining a tone stream from a signal stream
*	@param		calcSignalTimeFirst					Iterator to the beginning of the container with the calculated timesteps corresponding to the data (datatype: boost::posix_time::ptime)
*	@param		calcSignalTimeLast					Iterator to one element after the end of the container with the calculated timesteps
*	@param		refSignalTimeFirst					Iterator to the beginning of the container with the reference timesteps corresponding to the data (datatype: boost::posix_time::ptime)
*	@param		refSignalTimeLast					Iterator to one element after the end of the container with the reference timesteps
*	@param		signalFirst							Iterator to the beginning of the signal container corresponding to the timesteps (datatype: T)
*	@param		signalLast							Iterator to one element after the end of the signal container
*	@return 										None
*	@exception	std::runtime_error					Thrown if the object was not initialized using the constructor or alternatively SetParameters before calling this function
*	@exception 	std::length_error					Thrown if time and signal container have different lengths
*	@remarks 										The time and signal containers must be of the same size. The input data is internally stored in a continuous queue.
*/
template <class T> template <class In_It, class In_It2, class In_It3> void Core::General::CSearch<T>::SetNewSignalData(In_It calcSignalTimeFirst, In_It calcSignalTimeLast, In_It2 refSignalTimeFirst, In_It2 refSignalTimeLast, In_It3 signalFirst, In_It3 signalLast)
{
	using boost::numeric_cast;
	using namespace boost::posix_time;
	using namespace std;

	vector<ptime> newCalcSignalTime, newRefSignalTime;
	vector<T> newSignal;

	// check if parameters were set before
	if ( !isInit ) {
		throw std::runtime_error("Parameters are not set.");		
	}

	// check for identical size of the data containers
	if ( distance( calcSignalTimeFirst, calcSignalTimeLast ) != distance( signalFirst, signalLast ) ) {
		throw std::length_error( "Signal time and data container do not have the identical length." );
	}

	// lock parameter variables
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// obtain input data
	newCalcSignalTime.assign( calcSignalTimeFirst, calcSignalTimeLast );
	newRefSignalTime.assign( refSignalTimeFirst, refSignalTimeLast );
	newSignal.assign( signalFirst, signalLast );

	// start calculation of spectrogram with low time and good frequency resolution in an own thread - high accuracy calculated times are required
	freqSearchCoarse.PutSignal( newCalcSignalTime.begin(), newCalcSignalTime.end(), newRefSignalTime.begin(), newRefSignalTime.end(), newSignal.begin(), newSignal.end() );

	// start calculation of spectrogram with good time and low frequency resolution in an own thread - high accuracy calculated times are required
	freqSearch.PutSignal( newCalcSignalTime.begin(), newCalcSignalTime.end(), newRefSignalTime.begin(), newRefSignalTime.end(), newSignal.begin(), newSignal.end() );
}



/**	@brief		Load sequence search parameters.
*	@param		filterFileName							Name of data file (*.dat) with the full absolute path
*	@param		params									Parameters for sequence search
*	@return 											None
*	@exception 					std::ios_base::failure	Thrown if parameter file cannot be read
*	@remarks 											None
*/
template <class T> void Core::General::CSearch<T>::LoadParameters(std::string filterFileName, CAnalysisParam &params)
{
	// deserialize parameter data from file
	std::ifstream ifs( filterFileName );
    boost::archive::text_iarchive ia( ifs );
	
	// read parameters
	if ( !ifs.eof() ) {
		ia >> params;
	} else {
		throw std::ios_base::failure("Parameter file cannot be read.");
	}

	ifs.close();
}



/**	@brief		Save sequence search parameters.
*	@param		filterFileName							Name of data file (*.dat) with the full absolute path
*	@param		params									Parameters for sequence search
*	@return 											None
*	@exception 											None
*	@remarks 											None
*/
template <class T> void Core::General::CSearch<T>::SaveParameters(std::string filterFileName, CAnalysisParam params)
{
	// initialize serialization
	std::ofstream ofs( filterFileName );
	boost::archive::text_oarchive oa( ofs );

	// serialize parameter object
	const CAnalysisParam constParams = params; // workaround
	oa << constParams;

	ofs.close();
}



/**	@brief		Analysis thread
*	@return 											None
*	@exception 	std::length_error						Thrown if time and signal container of the class have different sizes
*	@remarks 											None
*/
template <class T> void Core::General::CSearch<T>::AnalysisThread(void)
{
	using namespace boost::posix_time;
	using namespace std;

	vector<T> procSignal;
	vector<ptime> procCalcSignalTime, procRefSignalTime;
	vector< tuple< int, ptime, ptime, ptime, T, T > > newTones;
	
	try {
		// lock parameter variables
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		// process audio data until interruption is requested
		while ( !(boost::this_thread::interruption_requested()) ) {
			// lock signal data mutex
			boost::unique_lock<boost::mutex> lock( signalMutex );
			if ( !( signal.empty() ) ) {
				procSignal.assign( signal.begin(), signal.end() );
				signal.clear();
				procCalcSignalTime.assign( calcSignalTime.begin(), calcSignalTime.end() );
				calcSignalTime.clear();
				procRefSignalTime.assign( refSignalTime.begin(), refSignalTime.end() );
				refSignalTime.clear();
			}
			lock.unlock();

			// analysis of filtered signal data for finding the tone stream
			if ( !( procSignal.empty() ) ) {
				SetNewSignalData( procCalcSignalTime.begin(), procCalcSignalTime.end(), procRefSignalTime.begin(), procRefSignalTime.end(), procSignal.begin(), procSignal.end() );		
			}
			procSignal.clear();
			newTones.clear();
			CalculateTones( back_inserter( newTones ) );

			// analysis of tone data for finding the sequence codes in the derived class
			if ( !( newTones.empty() ) ) {
				PerformSpecializedCalculation( newTones );
			}

			// delay thread
			std::this_thread::sleep_for( std::chrono::microseconds( static_cast<long long>( searchTimestep * 1e6 ) ) );
		}
	} catch (const std::exception& e) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Analysis thread: " + string( e.what() ) );
	}
}
