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
#include <queue>
#include <tuple>
#include <memory>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace General {
		/**	\ingroup Core
		*	Class for finding tones in a frequency stream. For high time and frequency resolution, two different streams with different time resolution are required.
		*/
		template <class T> class CToneSearch
		{
		public:
			CToneSearch(void);
			CToneSearch(double maxDeltaF, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, boost::posix_time::time_duration deltaT, std::map<int,T> searchTones, std::function<void(const std::string&)> runtimeErrorCallback);
			~CToneSearch(void);
			void SetParameters(double maxDeltaF, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, boost::posix_time::time_duration deltaT, std::map<int,T> searchTones, std::function<void(const std::string&)> runtimeErrorCallback);
			void GetParameters(double& maxDeltaF, double& maxFreqDevConstrained, double& maxFreqDevUnconstrained, int& numNeighbours, double& evalToneLength, boost::posix_time::time_duration& deltaT, std::map<int,T>& searchTones);
			template <class Out_It> Out_It GetTones(Out_It tonesFirst);
			template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class In_It6> void PutFrequencyStream(In_It1 timeRefFirst, In_It1 timeRefLast, In_It2 timeCalcFirst, In_It3 streamFirst, In_It4 timeCalcCoarseFirst, In_It4 timeCalcCoarseLast, In_It5 streamCoarseFirst, In_It6 absToneLevelsCoarseFirst );
		private:
			struct PossibleTones {
				boost::posix_time::ptime timeRef;
				boost::posix_time::ptime timeCalc;
				int tone;
				double centerFreq;
				double lowerFreqLimit;
				double upperFreqLimit;
				T absToneLevel;
			};			
			CToneSearch(const CToneSearch &) = delete;					// prevent copying
   			CToneSearch & operator= (const CToneSearch &) = delete;		// prevent assignment
			template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class In_It6, class In_It7, class Out_It1> Out_It1 PerformToneSearch( In_It1 currCalcTimeCoarseFirst, In_It1 currCalcTimeCoarseLast, In_It2 currPeaksCoarseFirst, In_It3 currAbsToneLevelsCoarseFirst, In_It4 currCalcTimeFirst, In_It4 currCalcTimeLast, In_It5 currRefTimeFirst, In_It6 currPeaksFirst, In_It7 oldTonesFirst, In_It7 oldTonesLast, Out_It1 tonesFirst );
			void CreateFoundFrequencyTable(std::vector< boost::posix_time::ptime > currRefTime, std::vector< boost::posix_time::ptime > currCalcTime, std::vector < std::vector <PossibleTones> >& currPossibleTones);
			void SearchForTonesTwoStreams(std::vector< boost::posix_time::ptime > currCalcTimeCoarse, std::vector< std::vector<T> > currPeaksCoarse, std::vector< std::vector<T> > currAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > currCalcTime, std::vector< boost::posix_time::ptime > currRefTime, std::vector< std::vector<T> > currPeaks, std::vector < std::vector <PossibleTones> >& currPossibleTones);
			void DefineFrequencyLimits(std::vector < std::vector <PossibleTones> >& currPossibleTones);
			void FindStartStopTimes(std::vector < std::vector< boost::posix_time::ptime > >& tRefStart, std::vector < std::vector< boost::posix_time::ptime > >& tCalcStart, std::vector < std::vector< boost::posix_time::ptime > >& tCalcEnd, std::vector < std::vector<T> >& freq, std::vector< std::vector<T> >& absToneLevel, std::vector < std::vector<T> > peaks, std::vector < std::vector <PossibleTones> > currPossibleTones);
			void GetNextDatasets( std::vector< boost::posix_time::ptime > &currCalcTimeCoarse, std::vector< std::vector<T> > &currPeaksCoarse, std::vector< std::vector<T> > &currAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > &currCalcTime, std::vector< boost::posix_time::ptime > &currRefTime, std::vector< std::vector<T> > &currPeaks, std::vector< boost::posix_time::ptime > &processCalcTimeCoarse, std::vector<std::vector<T> > &processPeaksCoarse, std::vector< std::vector<T> > &processAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > &processCalcTime, std::vector< boost::posix_time::ptime > &processRefTime, std::vector<std::vector<T> > &processPeaks );
			void SearchTonesThread();
		
			std::unique_ptr< boost::thread > threadToneSearch;
			boost::mutex inputFrequencyMutex;
			boost::mutex resultMutex;
			boost::shared_mutex parameterMutex;
			boost::condition_variable_any newFrequencyDataCondition;
			bool isInit;
			int numNeighbours;
			boost::posix_time::time_duration evalToneLength;
			boost::posix_time::time_duration deltaT;
			double maxDeltaF;
			double maxFreqDevConstrained;
			double maxFreqDevUnconstrained;
			std::vector< boost::posix_time::ptime > timeCalcCoarse;
			std::vector< boost::posix_time::ptime > timeCalc;
			std::vector< boost::posix_time::ptime > timeRef;
			std::vector< std::vector<T> > peaksCoarse;
			std::vector< std::vector<T> > peaks;
			std::vector< std::vector<T> > absToneLevelsCoarse;
			std::map< int, T > searchTones;
			std::deque< std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, T, T > > tones;
			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
		};
	}



	namespace Exception {
		/**	This class provides an exception for the case of unsufficient length in GetNextDataset inherited from std::exception
		*/
		class streamLengthException : public virtual std::exception
		{
		public:
			/**	@brief		Provides information on the thrown exception
			*	@return		Information on the error
			*	@remarks	None
			*/
			virtual const char* what()
  			{
				return "At least one stream length is insufficient.";
  			}
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::General::CToneSearch<T>::CToneSearch(void)
	:isInit(false)
{
}



/**	@brief		Constructor for initialization of all parameters.
*	@param		maxDeltaF					Maximum allowed deviation of a tone frequency from the nominal frequency (at high frequency resolution) [%/100]		
*	@param		maxFreqDevConstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if a neighboring tone exists [%/100 of the nominal frequency distances]
*	@param		maxFreqDevUnconstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if no neighboring tone exists [%/100 of the nominal frequency]
*	@param		numNeighbours				Number of neighboring timesteps (forward and backward) considered as a tone, if a tone is detected at a certain timestep (required for high frequency, low time resolution stream)	
*	@param		evalToneLength				Length of the evalulation period for tone search (in s)
*	@param		deltaT						Time duration between two fine time resolution steps
*	@param		searchTones					Map container storing the identifier and the frequencies of all tones to be found			
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception 								None
*	@remarks 								The parameters must be set before using the class. CToneSearch<T>::SetParameters can be used as an alternative and for later changes of parameters.
*/
template <class T> Core::General::CToneSearch<T>::CToneSearch(double maxDeltaF, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, boost::posix_time::time_duration deltaT, std::map<int,T> searchTones, std::function<void(const std::string&)> runtimeErrorCallback)
	:isInit(false)
{
	SetParameters( maxDeltaF, maxFreqDevConstrained, maxFreqDevUnconstrained, numNeighbours, evalToneLength, deltaT, searchTones );
}



/**
* 	@brief		Default destructor.
*/
template <class T> Core::General::CToneSearch<T>::~CToneSearch(void)
{
	if ( threadToneSearch != nullptr ) {
		// stop running thread
		threadToneSearch->interrupt();
	
		// wait until thread has stopped
		threadToneSearch->join();
	}
}



/**	@brief		Setting of all parameters of the class.
*	@param		maxDeltaF					Maximum allowed deviation of a tone frequency from the nominal frequency (at high frequency resolution) [%/100]		
*	@param		maxFreqDevConstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if a neighboring tone exists [%/100 of the nominal frequency distances]
*	@param		maxFreqDevUnconstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if no neighboring tone exists [fraction of the nominal frequency]
*	@param		numNeighbours				Number of neighboring timesteps (forward and backward) considered as a tone, if a tone is detected at a certain timestep (required for high frequency, low time resolution stream)	
*	@param		evalToneLength				Length of the evalulation period for tone search (in s)
*	@param		deltaT						Time duration between two fine time resolution steps
*	@param		searchTones					Map container storing the identifier and the frequencies of all tones to be found	
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception	std::runtime_error			Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 								This function can be used to reset the parameters.
*/
template <class T> void Core::General::CToneSearch<T>::SetParameters(double maxDeltaF, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, boost::posix_time::time_duration deltaT, std::map<int,T> searchTones, std::function<void(const std::string&)> runtimeErrorCallback)
{
	// stop thread if it is running
	if ( threadToneSearch != nullptr ) {
		threadToneSearch->interrupt();
	
		// wait until thread has stopped
		threadToneSearch->join();
	}

	// lock parameter variables
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		// obtain new parameters
		CToneSearch<T>::maxFreqDevConstrained = maxFreqDevConstrained;
		CToneSearch<T>::maxFreqDevUnconstrained = maxFreqDevUnconstrained;
		CToneSearch<T>::maxDeltaF = maxDeltaF;
		CToneSearch<T>::searchTones = searchTones;
		CToneSearch<T>::numNeighbours = numNeighbours;
		CToneSearch<T>::evalToneLength =  boost::posix_time::microseconds( static_cast<long>( evalToneLength * 1.0e6 ) );
		CToneSearch<T>::deltaT = deltaT;
	
		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}

	// initialize signaling of errors in the tone search thread
	runtimeErrorSignal.disconnect_all_slots();
	runtimeErrorSignal.connect( runtimeErrorCallback );

	// start new tone search thread
	threadToneSearch = std::make_unique<boost::thread>( &CToneSearch<T>::SearchTonesThread, this );
}



/**	@brief		Get all parameters of the class.
*	@param		maxDeltaF					Maximum allowed deviation of a tone frequency from the nominal frequency (at high frequency resolution) [%/100]		
*	@param		maxFreqDevConstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if a neighboring tone exists [%/100 of the nominal frequency distances]
*	@param		maxFreqDevUnconstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if no neighboring tone exists [fraction of the nominal frequency]
*	@param		numNeighbours				Number of neighboring timesteps (forward and backward) considered as a tone, if a tone is detected at a certain timestep (required for high frequency, low time resolution stream)	
*	@param		evalToneLength				Length of the evalulation period for tone search (in s)
*	@param		deltaT						Time duration between two fine time resolution steps
*	@param		searchTones					Map container storing the identifier and the frequencies of all tones to be found				
*	@return 								None
*	@exception 	std::runtime_error			Thrown if the parameters have not been set before
*	@remarks 								The parameters must be set before using the class. CToneSearch<T>::SetParameters can be used as an alternative and for later changes of parameters.
*/
template <class T> void Core::General::CToneSearch<T>::GetParameters(double& maxDeltaF, double& maxFreqDevConstrained, double& maxFreqDevUnconstrained, int& numNeighbours, double& evalToneLength, boost::posix_time::time_duration& deltaT, std::map<int,T>& searchTones)
{
	if ( isInit ) {
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		maxFreqDevConstrained = CToneSearch<T>::maxFreqDevConstrained;
		maxFreqDevUnconstrained = CToneSearch<T>::maxFreqDevUnconstrained;
		maxDeltaF = CToneSearch<T>::maxDeltaF;
		searchTones = CToneSearch<T>::searchTones;
		numNeighbours = CToneSearch<T>::numNeighbours;
		evalToneLength = CToneSearch<T>::evalToneLength.total_microseconds() / 1.0e6;
		deltaT = CToneSearch<T>::deltaT;
	} else {
		throw std::runtime_error("Parameters are not set.");
	}
}



/**	@brief		Put new data in the frequency stream
*	@param		timeRefFirst				Iterator to beginning of container storing the reference timesteps at fine time resolution. Datatype: boost::posix_time::ptime.			
*	@param		timeRefLast					Iterator to end of container storing the reference timesteps at fine time resolution.
*	@param		timeCalcFirst				Iterator to beginning of container storing the calculated timesteps at fine time resolution. Datatype: boost::posix_time::ptime.			
*	@param		streamFirst					Iterator to beginning of container storing the fine time, coarse frequency resolution stream. The same number of samples will be processed as the size of the time container.Datatype: double.
*	@param		timeCalcCoarseFirst			Iterator to beginning of container storing the calculated timesteps at coarse time resolution
*	@param		timeCalcCoarseLast			Iterator to end of container storing the calculated timesteps at coarse time resolution
*	@param		streamCoarseFirst			Iterator to beginning of container storing the coarse time, fine frequency resolution stream. The same number of samples will be processed as the size of the coarse time container.
*	@param		absToneLevelsCoarseFirst	Iterator to beginning of container storing the absolute signal level of the found peaks, fine frequency resolution stream. The same number of samples will be processed as the size of the coarse time container.
*	@return 								None
*	@exception 								None
*	@remarks 								Two different stream with good frequency and good time resolution are required.
*/
template <class T> template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class In_It6> void Core::General::CToneSearch<T>::PutFrequencyStream(In_It1 timeRefFirst, In_It1 timeRefLast, In_It2 timeCalcFirst, In_It3 streamFirst, In_It4 timeCalcCoarseFirst, In_It4 timeCalcCoarseLast, In_It5 streamCoarseFirst, In_It6 absToneLevelsCoarseFirst)
{
	using namespace std;

	unsigned int numDatapoints, numDatapointsCoarse;
	
	// add new data to the streams
	boost::unique_lock<boost::mutex> lock( inputFrequencyMutex );
	numDatapoints = distance( timeRefFirst, timeRefLast );
	timeRef.insert( timeRef.end(), timeRefFirst, timeRefLast );
	timeCalc.insert( timeCalc.end(), timeCalcFirst, timeCalcFirst + numDatapoints );
	peaks.insert( peaks.end(), streamFirst, streamFirst + numDatapoints );

	numDatapointsCoarse = distance( timeCalcCoarseFirst, timeCalcCoarseLast );
	timeCalcCoarse.insert( timeCalcCoarse.end(), timeCalcCoarseFirst, timeCalcCoarseLast );
	peaksCoarse.insert( peaksCoarse.end(), streamCoarseFirst, streamCoarseFirst + numDatapointsCoarse );
	absToneLevelsCoarse.insert( absToneLevelsCoarse.end(), absToneLevelsCoarseFirst, absToneLevelsCoarseFirst + numDatapointsCoarse );

	// trigger excecution of tone analysis thread
	if ( numDatapointsCoarse > 0 ) {
		newFrequencyDataCondition.notify_all();
	}
}



/**	@brief		Obtain the newly detected tones
*	@param		tonesFirst					Iterator to beginning of a container storing the identifier, reference start, calculated start, calculated stop time [s] and frequency [Hz] of all the found tones	since last call of the method		
*	@return 								Iterator to one element after the end of the tone container
*	@exception 								None
*	@remarks 								The processed data on the streams is deleted.
*/
template <class T> template <class Out_It> Out_It Core::General::CToneSearch<T>::GetTones(Out_It tonesFirst)
{
	using namespace boost::posix_time;
	using namespace std;

	vector< tuple< int, ptime, ptime, ptime, T, T > > newTones;

	// copy to local data in order to reduce lock time
	boost::unique_lock<boost::mutex> lockResult( resultMutex );
	newTones.assign( tones.begin(), tones.end() );

	// delete old results
	tones.clear();

	lockResult.unlock();

	// copy to output
	return std::move( newTones.begin(), newTones.end(), tonesFirst );
}



/**	@brief		Prepare a table for storing the found frequencies at all times
*	@param		currRefTime					Good time, low frequency resolution reference time vector container
*	@param		currCalcTime				Good time, low frequency resolution calc time vector container
*	@param		currPossibleTones			Map container with datatype PossibleTones storing all information required for tone analysis. It will be adjusted.
*	@return 								None
*	@exception 								None
*	@remarks 								The table is used internally in the class for finding the tones.
*/
template <class T> void Core::General::CToneSearch<T>::CreateFoundFrequencyTable(std::vector< boost::posix_time::ptime > currRefTime, std::vector< boost::posix_time::ptime > currCalcTime, std::vector < std::vector <PossibleTones> >& currPossibleTones)
{
	PossibleTones possibleTonesLocal;
	int f;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
		
	// create a table storing the found frequencies and tones at all currCalcTimes including frequency limits
	currPossibleTones.resize( currCalcTime.size() );
	for ( size_t t=0; t < currCalcTime.size(); t++ ) {
		currPossibleTones[t].resize( searchTones.size() );

		f = 0;
		for ( auto it = searchTones.begin(); it != searchTones.end(); it++ ) {
			// set standard values
			possibleTonesLocal.timeRef = currRefTime[t];
			possibleTonesLocal.timeCalc = currCalcTime[t];
			possibleTonesLocal.tone = -1;
			possibleTonesLocal.centerFreq = 0;
			possibleTonesLocal.absToneLevel = 0;
			currPossibleTones[t][f] = possibleTonesLocal;
			f++;
		}
	}
}



/**	@brief		Search for tones using a fine frequency and a coarse frequency resolution stream
*	@param		currCalcTimeCoarse			Good frequency, low time resolution calculated time vector container 
*	@param		currPeaksCoarse				Good frequency, low time resolution frequency peaks vector container
*	@param		currAbsToneLevelsCoarse		Good frequency, low time resolution absolute signal levels (of the peaks) container
*	@param		currCalcTime				Good time, low frequency resolution calculated time vector container
*	@param		currRefTime					Good time, low frequency resolution reference time vector container
*	@param		currPeaks					Good time, low frequency resolution frequency peaks vector container
*	@param		currPossibleTones			Map container with datatype PossibleTones storing all information required for tone analysis. It will be adjusted.
*	@return 								None
*	@exception 								None
*	@remarks 								The function searches for tones a coarse time (i.e. fine frequency resolution) stream. The frequency resolution of the found tones is therefore good, while the time resolution is bad. To ensure safe time localization, the found tones is also indicated in the neighbouring timesteps.
*/
template <class T> void Core::General::CToneSearch<T>::SearchForTonesTwoStreams(std::vector< boost::posix_time::ptime > currCalcTimeCoarse, std::vector< std::vector<T> > currPeaksCoarse, std::vector< std::vector<T> > currAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > currCalcTime, std::vector< boost::posix_time::ptime > currRefTime, std::vector< std::vector<T> > currPeaks, std::vector < std::vector <PossibleTones> >& currPossibleTones)
{
	using namespace std;

	int f;
	T foundFreq, currAbsToneLevel;
	vector< boost::posix_time::ptime > calcTimeCoarseData;
	vector< boost::posix_time::ptime > calcTimeData;
	PossibleTones possibleTonesLocal;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// obtain data
	calcTimeCoarseData.assign( currCalcTimeCoarse.begin(), currCalcTimeCoarse.end() );
	calcTimeData.assign( currCalcTime.begin(), currCalcTime.end() );

	// search for all frequencies at high frequency resolution
	f = 0;
	for ( auto it2 = searchTones.begin(); it2 != searchTones.end(); it2++ ) {
		foundFreq = it2->second;
		
		// map to fine time resolution steps
		for ( size_t tCoarse=0; tCoarse < calcTimeCoarseData.size(); tCoarse++ ) {
			// search for tones for this time in the high frequency resolution stream
			auto itCoarse = find_if( currPeaksCoarse[tCoarse].begin(), currPeaksCoarse[tCoarse].end(), [=](T val){ return ( std::abs( val - foundFreq ) / foundFreq <= maxDeltaF ); } );
			if ( itCoarse != currPeaksCoarse[tCoarse].end() ) {	
				// find the corresponding t for the fine resolution spectra
				auto it = find_if( calcTimeData.begin(), calcTimeData.end(), [=]( auto val ) { return ( val >= calcTimeCoarseData[tCoarse] ); } );
				int t;
				if ( it != calcTimeData.end() ) {
					t = static_cast<int>(  distance( calcTimeData.begin(), it ) );
				} else {
					t = static_cast<int>( calcTimeData.size() - 1 );
				}
				possibleTonesLocal.timeRef = currRefTime[t];
				possibleTonesLocal.timeCalc = currCalcTime[t];
				possibleTonesLocal.tone = it2->first;
				possibleTonesLocal.centerFreq = *itCoarse;
				possibleTonesLocal.absToneLevel = currAbsToneLevelsCoarse[tCoarse][ std::distance( currPeaksCoarse[tCoarse].begin(), itCoarse ) ];

				// due to bad time resolution neighboring times are also considered
				auto possibleTimePeriod = boost::posix_time::time_period( calcTimeCoarseData[tCoarse], calcTimeCoarseData[tCoarse] );
				possibleTimePeriod.expand( deltaT * numNeighbours );
				for ( int tLocal = t - numNeighbours; tLocal <= t + numNeighbours; tLocal++ ) {
					if ( ( tLocal >= 0 ) && ( tLocal < static_cast<int>( currPossibleTones.size() ) ) ) {
						if ( possibleTimePeriod.contains( currCalcTime[tLocal] ) ) {
							possibleTonesLocal.timeRef = currRefTime[tLocal];
							possibleTonesLocal.timeCalc = currCalcTime[tLocal];
							currAbsToneLevel = currPossibleTones[tLocal][f].absToneLevel;
							currPossibleTones[tLocal][f] = possibleTonesLocal;

							// ensure that always the maximum signal level of the tone is chosen
							if ( currAbsToneLevel > currPossibleTones[tLocal][f].absToneLevel ) {
								currPossibleTones[tLocal][f].absToneLevel = currAbsToneLevel;
							}
						}
					}
				}
			}
		}
		f++;
	}
}


 
/**	@brief		Calculate the limits for frequency detection in the low frequency - high time resolution stream
*	@param		currPossibleTones			Map container with datatype PossibleTones storing all information required for tone analysis. It will be adjusted.
*	@return 								None
*	@exception 								None
*	@remarks 								This function prepares the table for tone detection in the low frequency resolution (i.e. high time resolution) stream. This is possible, because the indicated tones in the table were found before in a high frequency, low time resolution stream.
*/
template <class T> void Core::General::CToneSearch<T>::DefineFrequencyLimits(std::vector < std::vector <PossibleTones> >& currPossibleTones)
{
	using namespace std;

	vector <double> frequencies;
	vector <double>::iterator iterator;
	vector <double>::reverse_iterator rIterator;
	int lowerDummy = 0;
	int upperDummy = 100000;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	for ( size_t t=0; t < currPossibleTones.size(); t++ ) {
		frequencies.resize( currPossibleTones[t].size() );
		for ( size_t f=0; f < currPossibleTones[t].size(); f++) {
			frequencies[f] = currPossibleTones[t][f].centerFreq;
		}

		for ( size_t f=0; f < currPossibleTones[t].size(); f++ ) {
			// search for tones with higher or lower frequencies
			if ( currPossibleTones[t][f].centerFreq > 0 ) {
				// define upper frequency limit
				replace_if( frequencies.begin(), frequencies.end(), [=]( auto val ) { return( val == upperDummy );  }, lowerDummy );
				iterator = find_if( frequencies.begin(), frequencies.end(), [=](auto val) { return ( val > currPossibleTones[t][f].centerFreq ); } );
				if ( iterator != frequencies.end() ) {
					// tone with higher frequency exists
					currPossibleTones[t][f].upperFreqLimit = currPossibleTones[t][f].centerFreq + maxFreqDevConstrained * ( *iterator - currPossibleTones[t][f].centerFreq );
				} else {
					// no tone with higher frequency exists
					currPossibleTones[t][f].upperFreqLimit = ( 1 + maxFreqDevUnconstrained ) * currPossibleTones[t][f].centerFreq;
				}

				// define lower frequency limit
				replace_if( frequencies.begin(), frequencies.end(), [=]( auto val ) { return ( val == lowerDummy ); }, upperDummy );
				rIterator = find_if( frequencies.rbegin(), frequencies.rend(), [=]( auto val ) { return ( val < currPossibleTones[t][f].centerFreq ); } );
				if ( rIterator != frequencies.rend() ) {
					// tone with lower frequency exists
					currPossibleTones[t][f].lowerFreqLimit = currPossibleTones[t][f].centerFreq - maxFreqDevConstrained * ( currPossibleTones[t][f].centerFreq - *rIterator );
				} else {
					// no tone with lower frequency exists
					currPossibleTones[t][f].lowerFreqLimit = ( 1 - maxFreqDevUnconstrained ) * currPossibleTones[t][f].centerFreq;
				}
			} else {
				currPossibleTones[t][f].lowerFreqLimit = 0;
				currPossibleTones[t][f].upperFreqLimit = 0;
			}
		}
	}
}



/**	@brief		Find the start and stop time of the tones in the high time resolution table
*	@param		tRefStart					Storing the reference start times of all found tones after the call. First dimension: index of tone according to foundTones, second dimension: start times for all found tones
*	@param		tCalcStart					Storing the calculated start times of all found tones after the call. First dimension: index of tone according to foundTones, second dimension: start times for all found tones
*	@param		tCalcEnd					Storing the calculated end times of all found tones after the call. First dimension: index of tone according to foundTones, second dimension: end times for all found tones
*	@param		freq						Storing the frequencies of all found tones after the call. First dimension: index of tone according to foundTones, second dimension: frequencies for all found tones
*	@param		absToneLevel				Storing the absolute signal levels for all found tones after the call. First dimension: index of tone according to foundTones, second dimension: absolute signal levels for all found tones
*	@param		peaks						Good time, low frequency resolution frequency peaks vector container, must have the same size as currTime
*	@param		currPossibleTones			Map container with datatype PossibleTones storing all information required for tone analysis. It will be adjusted.
*	@return 								None
*	@exception 								None
*	@remarks 								The calculated start times are required for relative calculations (differences), while the reference start time is required for the absolute time stamp.
*/
template <class T> void Core::General::CToneSearch<T>::FindStartStopTimes(std::vector < std::vector< boost::posix_time::ptime > >& tRefStart, std::vector < std::vector< boost::posix_time::ptime > >& tCalcStart, std::vector < std::vector< boost::posix_time::ptime > >& tCalcEnd, std::vector < std::vector<T> >& freq, std::vector< std::vector<T> >& absToneLevel, std::vector < std::vector<T> > currPeaks, std::vector < std::vector <PossibleTones> > currPossibleTones)
{
	using boost::numeric_cast;
	using namespace std;

	int f;
	vector<bool> newTone;
	vector< vector<T> > peaksData;
	typename vector<T>::iterator it;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// obtain data
	peaksData.assign( currPeaks.begin(), currPeaks.end() );

	// find start and stop time of possible tones using the high time and low frequency resolution stream
	tRefStart.resize( searchTones.size() );
	tCalcStart.resize( searchTones.size() );
	tCalcEnd.resize( searchTones.size() );
	freq.resize( searchTones.size() );
	absToneLevel.resize( searchTones.size() );
	newTone.resize( searchTones.size(), true );
	f = 0;
	for ( auto it2 = searchTones.begin(); it2 != searchTones.end(); it2++ ) {
		// search for all high resolution times
		for ( size_t t=0; t < peaksData.size(); t++ ) {
			// search with low frequency resolution (possible because the tones were found before with high frequency resolution)
			it = find_if( peaksData[t].begin(), peaksData[t].end(), [=]( auto val ) { return ( val >= currPossibleTones[t][f].lowerFreqLimit ) && ( val <= currPossibleTones[t][f].upperFreqLimit ); } );
			if ( ( it != peaksData[t].end() ) && ( currPossibleTones[t][f].tone >= 0 ) ) {
				if ( newTone[f] ) {
					// start time is only set if it is a newly detected tone not detected immediately before
					newTone[f] = false;
					tRefStart[f].push_back( currPossibleTones[t][f].timeRef );
					tCalcStart[f].push_back( currPossibleTones[t][f].timeCalc );
					tCalcEnd[f].push_back(currPossibleTones[t][f].timeCalc );
					freq[f].push_back( numeric_cast<T>( currPossibleTones[t][f].centerFreq ) );
					absToneLevel[f].push_back( currPossibleTones[t][f].absToneLevel );
				} else {
					// otherwise the stop time is set
					tCalcEnd[f].back() = currPossibleTones[t][f].timeCalc;
					if ( currPossibleTones[t][f].absToneLevel > absToneLevel[f].back() ) {
						absToneLevel[f].back() = currPossibleTones[t][f].absToneLevel; // the signal level of the tone should finally be the maximum signal level found
					}
				}
			} else {
				newTone[f] = true;
			}
		}
		f++;
	}
}
		


/**	@brief		Executes the search of tones in the combined low / high frequency resolution streams
*	@param		currCalcTimeCoarseFirst			Iterator to beginning of good frequency, low time resolution calculated time vector container 
*	@param		currCalcTimeCoarseLast			Iterator to end of good frequency, low time resolution calculated time vector container
*	@param		currPeaksCoarseFirst			Iterator to beginning of good frequency, low time resolution frequency peaks vector container
*	@param		currAbsToneLevelsCoarseFirst	Iterator to beginning of the good frequency, low time resolution absolute signal levels (of the peaks) vector container
*	@param		currCalcTimeFirst				Iterator to beginning of good time, low frequency resolution calculated time vector container
*	@param		currCalcTimeLast				Iterator to end of good time, low frequency resolution calculated time vector container
*	@param		currRefTimeFirst				Iterator to beginning of good time, low frequency resolution reference time vector container
*	@param		currPeaksFirst					Iterator to beginning of good time, low frequency resolution frequency peaks vector container
*	@param		oldTonesFirst					Iterator to beginning of container storing the tones which started during the last analysis step: (tone index, start time)
*	@param		oldTonesLast					Iterator to end of container storing the tones which started during the last analysis step: (tone index, start time)
*	@param		tonesFirst						Iterator to beginning of a container storing the identifier, reference start, calculated start and stop times [s] and frequency [Hz] of all found tones
*	@return 									Iterator to end of tone container
*	@exception 	std::runtime_error				Thrown if the parameters have not been set before
*	@remarks 									The approach is to used jointly a high frequency and high time resolution stream of the same signal data. 
*												The high frequency resolution stream allows a precise determination of the tone frequencies, but gives only a rough estimate of the times of the tones. 
*												Therefore in a second calculation, using the estimated times of the tones, the times are determined precisely with the high time resolution stream.
*/
template <class T> template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class In_It6, class In_It7, class Out_It1> Out_It1 Core::General::CToneSearch<T>::PerformToneSearch(In_It1 currCalcTimeCoarseFirst, In_It1 currCalcTimeCoarseLast, In_It2 currPeaksCoarseFirst, In_It3 currAbsToneLevelsCoarseFirst, In_It4 currCalcTimeFirst, In_It4 currCalcTimeLast, In_It5 currRefTimeFirst, In_It6 currPeaksFirst, In_It7 oldTonesFirst, In_It7 oldTonesLast, Out_It1 tonesFirst)
{
	using namespace boost::posix_time;
	using namespace std;

	vector< ptime > currCalcTimeCoarse, currCalcTime, currRefTime;
	vector< vector<T> > currPeaksCoarse, currPeaks, currFreq, currAbsToneLevelsCoarse, currToneAbsToneLevel;
	vector< vector< ptime > > currTRef, currTStart, currTEnd;
	vector < vector <PossibleTones> > currPossibleTones;
	deque< tuple< int, ptime, ptime, ptime, T, T > > currTones, oldTones;

	// check if parameters were set before
	if ( !isInit ) {
		throw std::runtime_error("Parameters are not set.");		
	}

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
	
	// obtain input data
	currCalcTimeCoarse.assign( currCalcTimeCoarseFirst, currCalcTimeCoarseLast );
	currPeaksCoarse.assign( currPeaksCoarseFirst, currPeaksCoarseFirst + currCalcTimeCoarse.size() );
	currAbsToneLevelsCoarse.assign( currAbsToneLevelsCoarseFirst, currAbsToneLevelsCoarseFirst + currCalcTimeCoarse.size() );

	currCalcTime.assign( currCalcTimeFirst, currCalcTimeLast );
	currRefTime.assign( currRefTimeFirst, currRefTimeFirst + currCalcTime.size() );
	currPeaks.assign( currPeaksFirst, currPeaksFirst + currCalcTime.size() );

	oldTones.assign( oldTonesFirst, oldTonesLast );

	// create a table storing all found frequencies and tones at all times
	CreateFoundFrequencyTable( currRefTime, currCalcTime, currPossibleTones );

	// search for tones in the coarse frequency stream (the result contains only broad estimates of the times)
	SearchForTonesTwoStreams( currCalcTimeCoarse, currPeaksCoarse, currAbsToneLevelsCoarse, currCalcTime, currRefTime, currPeaks, currPossibleTones );

	// define the frequency limits for tone detection at low frequency resolution
	DefineFrequencyLimits( currPossibleTones );

	// find the precise start and stop times of the tones in the fine frequency stream
	FindStartStopTimes( currTRef, currTStart, currTEnd, currFreq, currToneAbsToneLevel, currPeaks, currPossibleTones );

	// set output data
	for (size_t f=0; f < currFreq.size(); f++) {
		for (size_t i=0; i < currFreq[f].size(); i++) {
			// include tones started during the last analysis step
			for (size_t k=0; k < oldTones.size(); k++) {
				if ( get<0>( oldTones[k] ) == f ) {
					if ( currTStart[f][i] == currCalcTime.front() ) { // check if the tone has started at the beginning of the step, in this case include the old tone into this tone
						currTRef[f][i] = get<1>( oldTones[k] );
						currTStart[f][i] = get<2>( oldTones[k] );
						if ( get<5>( oldTones[k] ) > currToneAbsToneLevel[f][i] ) {
							currToneAbsToneLevel[f][i] = get<5>( oldTones[k] ); // the old tone has a larger signal level and is therefore considered as the maximum signal level of the tone
						}
						oldTones.erase( oldTones.begin() + k );
						break;
					} else { // include old tone as a separate tone
						currTones.push_back( oldTones[k] );
						oldTones.erase( oldTones.begin() + k );
						break;
					}
				}
			}
			
			// generate the new tone
			currTones.push_back( std::make_tuple( f, currTRef[f][i], currTStart[f][i], currTEnd[f][i], currFreq[f][i], currToneAbsToneLevel[f][i] ) );
		}
	}

	// add all old tones ending exactly at the end of the last analysis step
	for (size_t i=0; i < oldTones.size(); i++) {
		currTones.push_back( oldTones[i] );
	}

	// sort the stream based on calculated times
	sort( currTones.begin(), currTones.end(), [=]( auto tone1, auto tone2 ) { return ( get<2>( tone1 ) < get<2>( tone2 ) ); } );

	return std::move( currTones.begin(), currTones.end(), tonesFirst );
}



/**	@brief		Function containing the thread continously analyzing the frequency data
*	@return 						None
*	@exception 	std::runtime_error	Thrown if the parameters of the object were not set properly before using the function
*	@exception	std::length_error	Thrown if the length of the time and the peak container is not identical
*	@remarks 						The analysis is performed whenever new data has been notified by CToneSearch<T>::PutFrequencyStream()
*/
template <class T> void Core::General::CToneSearch<T>::SearchTonesThread()
{
	using namespace boost::posix_time;
	using namespace std;
	
	bool isEnoughData = true;
	vector< ptime > currCalcTimeCoarse, processCalcTimeCoarse, currCalcTime, processCalcTime, processRefTime, currRefTime;
	vector< vector<T> > currPeaksCoarse, processPeaksCoarse, currAbsToneLevelsCoarse, processAbsToneLevelsCoarse, currPeaks, processPeaks;
	vector<T> newData;
	deque< tuple< int, ptime, ptime, ptime, T, T > > newTones, oldTones;
	
	try {
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		// process audio data until interruption is requested
		while ( !(boost::this_thread::interruption_requested()) ) {
			// read from signal queue	
			boost::unique_lock<boost::mutex> lock( inputFrequencyMutex );
			currCalcTimeCoarse.insert( currCalcTimeCoarse.end(), timeCalcCoarse.begin(), timeCalcCoarse.end() );
			currPeaksCoarse.insert( currPeaksCoarse.end(), peaksCoarse.begin(), peaksCoarse.end() );
			currAbsToneLevelsCoarse.insert( currAbsToneLevelsCoarse.end(), absToneLevelsCoarse.begin(), absToneLevelsCoarse.end() );
			currCalcTime.insert( currCalcTime.end(), timeCalc.begin(), timeCalc.end() );
			currRefTime.insert( currRefTime.end(), timeRef.begin(), timeRef.end() );
			currPeaks.insert( currPeaks.end(), peaks.begin(), peaks.end() );
			
			timeCalcCoarse.clear();
			peaksCoarse.clear();
			absToneLevelsCoarse.clear();
			timeCalc.clear();
			timeRef.clear();
			peaks.clear();

			// check for identical lengths of time and peak containers
			if ( ( currCalcTimeCoarse.size() != currPeaksCoarse.size() ) || ( currCalcTime.size() != currPeaks.size() ) ) {
				throw std::length_error( "The length of the time and the peak container is not identical." );
			}

			if ( ( isEnoughData ) && ( !currCalcTimeCoarse.empty() ) && ( !currCalcTime.empty() ) && ( ( currCalcTimeCoarse.back() - currCalcTimeCoarse.front() ) > evalToneLength ) && ( ( currCalcTime.back() - currCalcTime.front() ) > evalToneLength ) ) {		
				isEnoughData = true;
				lock.unlock();
				
				// get data for next analysis step and delete the used data
				try {
					GetNextDatasets( currCalcTimeCoarse, currPeaksCoarse, currAbsToneLevelsCoarse, currCalcTime, currRefTime, currPeaks, processCalcTimeCoarse, processPeaksCoarse, processAbsToneLevelsCoarse, processCalcTime, processRefTime, processPeaks );
				} catch ( Exception::streamLengthException e ) {
					isEnoughData = false; // aborting because at least one stream is not long enough for including neighboring tones
				}
				if ( isEnoughData ) {
					// search for peaks in the new signal spectrogram
					newTones.clear();
					PerformToneSearch( processCalcTimeCoarse.begin(), processCalcTimeCoarse.end(), processPeaksCoarse.begin(), processAbsToneLevelsCoarse.begin(), processCalcTime.begin(), processCalcTime.end(), processRefTime.begin(), processPeaks.begin(), oldTones.begin(), oldTones.end(), back_inserter( newTones ) );
				
					// identify all tones not yet finished in this analysis step
					oldTones.clear();
					for (size_t i=0; i < newTones.size(); i++) {
						if ( processCalcTime.size() > 0 ) {
							if ( ( processCalcTime.back() <= get<3>( newTones[i] ) ) ) {
								oldTones.push_back( newTones[i] );
								newTones.erase( newTones.begin() + i );
								i--;
							}
						}
					}				
									
					// move result to data stream
					boost::unique_lock<boost::mutex> lockResult( resultMutex );
					tones.insert( tones.end(), newTones.begin(), newTones.end() );
				}
			} else {		
				isEnoughData = true;

				// wait for new audio data
    			newFrequencyDataCondition.wait( inputFrequencyMutex ); 
			}
		}
	} catch ( const std::exception& e ) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Tone search thread: " + string( e.what() ) );
	}
}



/**	@brief		Finds the data for the next analysis step from the combined coarse / fine resolution queues and deletes no longer required data from the queues
*	@param		currCalcTimeCoarse			Queue containing the calculated times with coarse resolution corresponding to currPeaksCoarse	
*	@param		currPeaksCoarse				Queue containing all peaks corresponding to coarse time resolution
*	@param		currAbsToneLevelsCoarse		Queue containing all absolute signal levels (of the peaks) corresponding to coarse time resolution
*	@param		currCalcTime				Queue containing the calculated times with fine resolution corresponding to currPeaks
*	@param		currRefTime					Queue containing the reference times with fine resolution corresponding to currPeaks
*	@param		currPeaks					Queue containing all peaks corresponding to fine time resolution
*	@param		processCalcTimeCoarse		Output of the coarse calculated time data for the next analysis step
*	@param		processPeaksCoarse			Output of the peak data for the analysis step (coarse time resolution)
*	@param		processAbsToneLevelsCoarse	Output of the absolute signal levels data (of the peaks) for the analysis step (coarse time resolution)
*	@param		processCalcTime				Output of the fine calculated time data for the next analysis step
*	@param		processRefTime				Output of the fine reference time data for the next analysis step
*	@param		processPeaks				Output of the peak data for the analysis step (fine time resolution)
*	@return 								None
*	@exception 	streamLengthException		Thrown if the length of either the fine or the coarse resolution stream is not long enough. In this case no input data is changed and not output data is set.
*	@remarks 								Data no longer required in the next steps is deleted from the input queues. Note that due to overlap effects 
*											the deleted data is not equivalent to the returned data for the next analysis step.
*/
template <class T> void Core::General::CToneSearch<T>::GetNextDatasets(std::vector< boost::posix_time::ptime > &currCalcTimeCoarse, std::vector< std::vector<T> > &currPeaksCoarse, std::vector< std::vector<T> > &currAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > &currCalcTime, std::vector< boost::posix_time::ptime > &currRefTime, std::vector< std::vector<T> > &currPeaks, std::vector< boost::posix_time::ptime > &processCalcTimeCoarse, std::vector<std::vector<T> > &processPeaksCoarse, std::vector< std::vector<T> > &processAbsToneLevelsCoarse, std::vector< boost::posix_time::ptime > &processCalcTime, std::vector< boost::posix_time::ptime > &processRefTime, std::vector<std::vector<T> > &processPeaks)
{
	using namespace boost::posix_time;
	using namespace std;

	ptime lowerTimeLimit;
	double deltaT;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
	
	// determine required length of step
	auto it = find_if( currCalcTime.begin(), currCalcTime.end(), [=] (ptime t) { return ( t >= currCalcTime.front() + evalToneLength ); } );
	if ( it != currCalcTime.end() ) {
		it++; // the last timestep must be included in the data
	}	
	if ( it == currCalcTime.end() ) {
		it--; // pointing to last timestep
	}
	auto itDelete = it;
	
	// map to the same time in the coarse time resolution stream
	auto itCoarse = find_if( currCalcTimeCoarse.begin(), currCalcTimeCoarse.end(), [=] (ptime t) { return ( t >= *it ); } );
	if ( itCoarse == currCalcTimeCoarse.end() ) {
		itCoarse--; // pointing to last valid timestep
	}
	auto itCoarseDeleteUpperLimit = itCoarse;
				
	// include neighboring larger time steps to ensure detection of all tones near the limits of the step (smaller neighbours are automatically still in the queue)
	if ( ( distance( currCalcTime.begin(), it ) + numNeighbours ) < static_cast<int>( currCalcTime.size() ) ) {
		advance( it, numNeighbours );
	} else {
		it = currCalcTime.end();
	}
	if ( ( it == currCalcTime.end() ) || ( itCoarse == currCalcTimeCoarse.end() ) ) {
		throw Exception::streamLengthException(); // the required inclusion of larger neighboring steps is not possible because the streams are too short
	} else {
		// the coarse resolution time must always be larger than the fine resolution time
		while ( *itCoarse < *it ) {
			itCoarse++;
			if ( itCoarse == currCalcTimeCoarse.end() ) {
				throw Exception::streamLengthException(); // the required inclusion of larger neighboring steps is not possible because the streams are too short
			}
		}
		itCoarse++; // the last timestep must be included in the data
	}
	it = itDelete;

	// determine the last data points to be deleted excluding the neighboring time steps to ensure overlap detection (only required for the coarse stream)
	auto itDeleteCoarse = itCoarseDeleteUpperLimit;
	if ( currCalcTime.size() > 1 ) {
		deltaT = static_cast<double>( ( currCalcTime.back() - currCalcTime.front() ).total_microseconds() ) / ( currCalcTime.size() - 1 );
		lowerTimeLimit = *it - microseconds( static_cast<long>( numNeighbours * deltaT ) );
		if ( ( itDeleteCoarse == currCalcTimeCoarse.begin() ) || ( lowerTimeLimit <= currCalcTimeCoarse.front() ) ) {
			itDeleteCoarse = currCalcTimeCoarse.begin();
		} else {
			// the coarse resolution time must always be smaller than the fine resolution time
			while ( *itDeleteCoarse > lowerTimeLimit ) {
				itDeleteCoarse--;
				if ( itDeleteCoarse == currCalcTimeCoarse.begin() ) {
					break;
				}
			}
		}
	} else {
		itDeleteCoarse = currCalcTime.begin();
	}

	// copy data to be processed
	processCalcTimeCoarse.assign( currCalcTimeCoarse.begin(), itCoarse );
	processPeaksCoarse.assign( currPeaksCoarse.begin(), currPeaksCoarse.begin() + processCalcTimeCoarse.size() );
	processAbsToneLevelsCoarse.assign( currAbsToneLevelsCoarse.begin(), currAbsToneLevelsCoarse.begin() + processCalcTimeCoarse.size() );
	processCalcTime.assign( currCalcTime.begin(), it );
	processRefTime.assign( currRefTime.begin(), currRefTime.begin() + processCalcTime.size() );
	processPeaks.assign( currPeaks.begin(), currPeaks.begin() + processCalcTime.size() );

	// delete no longer required data (without additional data at the end for ensuring tone detection near the limits)
	currPeaksCoarse.erase( currPeaksCoarse.begin(), currPeaksCoarse.begin() + distance( currCalcTimeCoarse.begin(), itDeleteCoarse ) );
	currAbsToneLevelsCoarse.erase( currAbsToneLevelsCoarse.begin(), currAbsToneLevelsCoarse.begin() + distance( currCalcTimeCoarse.begin(), itDeleteCoarse ) );
	currCalcTimeCoarse.erase( currCalcTimeCoarse.begin(), itDeleteCoarse );
	currPeaks.erase( currPeaks.begin(), currPeaks.begin() + distance( currCalcTime.begin(), itDelete ) );
	currRefTime.erase( currRefTime.begin(), currRefTime.begin() + distance( currCalcTime.begin(), itDelete ) );
	currCalcTime.erase( currCalcTime.begin(), itDelete );
}