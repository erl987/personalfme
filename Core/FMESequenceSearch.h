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
#include <memory>
#include <map>
#include <deque>
#include <tuple>
#include <limits>
#include <iterator>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "CodeData.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace FME {
		/**	\ingroup Core
		*	Class for finding five tone sequences according to TR-BOS in a audio signal stream.
		*/	
		template <class T> class CFMESequenceSearch
		{
		public:
			template <typename U> using ToneDataType = std::tuple< boost::posix_time::ptime, boost::posix_time::ptime, int, boost::posix_time::time_duration, boost::posix_time::time_duration, U, U >;
			template< typename U> using ToneType = std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, U, U >;

			CFMESequenceSearch(void);
			template <class In_It> CFMESequenceSearch(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio, In_It searchTonesFirst, In_It searchTonesLast, std::function<void(const std::string&)> runtimeErrorCallback);
			~CFMESequenceSearch(void);
			template <class Out_It> void GetSequences(Out_It newSequencesBegin);
			template <class In_It> void SetParameters(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio, In_It searchTonesFirst, In_It searchTonesLast, std::function<void(const std::string&)> runtimeErrorCallback);
			template <class Out_It> Out_It GetParameters(int& codeLength, double& excessTime, double& deltaTMaxTwice, double& minLength, double& maxLength, double& maxToneLevelRatio, Out_It searchTonesFirst);
			template <class In_It> void PutTonesStream(In_It newTonesBegin, In_It newTonesEnd);
		private:
			template <class In_It, class Out_It> Out_It FindFullCodeSequences(In_It tonesBegin, In_It tonesEnd, Out_It foundCodesBegin, boost::posix_time::ptime& startTimeLast, std::vector<int>& lastCode);
			std::vector <std::deque < boost::posix_time::time_duration > > CalculateToneLengths(std::vector < std::deque < boost::posix_time::ptime > > tStart, std::vector < std::deque < boost::posix_time::ptime > > tEnd);
			std::deque< typename CFMESequenceSearch<T>::template ToneDataType<T> > FindTones(std::vector<int> codeMeaning, std::vector < std::deque < boost::posix_time::time_duration > > deltaT, std::vector < std::deque < boost::posix_time::ptime > > tStartCalc, std::vector < std::deque < boost::posix_time::ptime > > tStartRef, std::vector < std::deque <T> > freq, std::vector< std::deque<T> > absToneLevel );
			template <class In_It, class Out_It> void FindCodeSequences(In_It tonesBegin, In_It tonesEnd, Out_It foundCodesBegin, boost::posix_time::ptime& calcStartTimeLast, std::vector<int>& lastCode);
			template <class Out_It> Out_It GetNextDatasets(std::deque< typename CFMESequenceSearch<T>::template ToneType<T> >& currTones, Out_It processTonesBegin);
			void SearchFullSequencesThread(void);

			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			std::unique_ptr< boost::thread > threadSequenceSearch;
			boost::mutex inputTonesMutex;
			boost::mutex resultMutex;
			boost::shared_mutex parameterMutex;
			boost::condition_variable_any newTonesCondition;
			int codeLength;
			double excessTime;
			double deltaTMaxTwice;
			double minLength;
			double maxLength;
			double maxToneLevelRatio;
			bool isInit;
			std::vector<T> searchTones;
			std::deque< typename CFMESequenceSearch<T>::template ToneType<T> > tones;
			std::deque< std::tuple< boost::posix_time::ptime, boost::posix_time::ptime, Utilities::CCodeData<T> > > foundCodes;
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::FME::CFMESequenceSearch<T>::CFMESequenceSearch()
	: isInit(false)
{
}



/**	@brief		Constructor.
*	@param		codeLength				Required length of a sequence code (5 for 5-tone-sequence)
*	@param		excessTime				This time [s] increases the length of the possible tone to ensure save detection
*	@param		deltaTMaxTwice			Two following tones with a larger distance of the start times are not considered as repetitions [s]
*	@param		minLength				Minimum length of a tone [s]
*	@param		maxLength				Maximum length of a tone [s]
*	@param		maxToneLevelRatio		Limit of the allowed ratio between the absolute signal level of the first tone compared to all other tones of a sequence (i.e.: 1 / maxToneLevelRatio <= level <= maxToneLevelRatio) [-]
*	@param		searchTonesFirst		Iterator to beginning of map with all tone frequencies and indices to be searched
*	@param		searchTonesLast			Iterator to end of map with all tone frequencies and indices to be searched
*	@param		runtimeErrorCallback	Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@exception 							None
*	@remarks 							The parameters can be reset using CFMESequenceSearch<T>::SetParameters
*/
template <class T> template <class In_It> Core::FME::CFMESequenceSearch<T>::CFMESequenceSearch(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio, In_It searchTonesFirst, In_It searchTonesLast, std::function<void(const std::string&)> runtimeErrorCallback)
	: isInit(false)
{
	SetParameters( codeLength, excessTime, deltaTMaxTwice, minLength, maxLength, maxToneLevelRatio, searchTonesFirst, searchTonesLast, runtimeErrorCallback );
}



/**
* 	@brief		Default destructor.
*/
template <class T> Core::FME::CFMESequenceSearch<T>::~CFMESequenceSearch()
{
	if ( threadSequenceSearch != nullptr ) {
		// stop running thread
		threadSequenceSearch->interrupt();
	
		// wait until thread has stopped
		threadSequenceSearch->join();
	}
}



/**	@brief		Setting of parameters.
*	@param		codeLength				Required length of a sequence code (5 for 5-tone-sequence)
*	@param		excessTime				This time [s] increases the length of the possible tone to ensure save detection
*	@param		deltaTMaxTwice			Two following tones with a larger distance of the start times are not considered as repetitions [s]
*	@param		minLength				Minimum length of a tone [s]
*	@param		maxLength				Maximum length of a tone [s]
*	@param		maxToneLevelRatio		Limit of the allowed ratio between the absolute signal level of the first tone compared to all other tones of a sequence (i.e.: 1 / maxToneLevelRatio <= level <= maxToneLevelRatio) [-]
*	@param		searchTonesFirst		Iterator to beginning of map with all tone frequencies and indices to be searched
*	@param		searchTonesLast			Iterator to end of map with all tone frequencies and indices to be searched
*	@param		runtimeErrorCallback	Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 							None
*	@exception	std::runtime_error		Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 							None
*/
template <class T> template <class In_It> void Core::FME::CFMESequenceSearch<T>::SetParameters(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio, In_It searchTonesFirst, In_It searchTonesLast, std::function<void(const std::string&)> runtimeErrorCallback)
{
	using namespace std;

	map<int,T> searchTones;

	// stop thread if it is running
	if ( threadSequenceSearch != nullptr ) {
		threadSequenceSearch->interrupt();
	
		// wait until thread has stopped
		threadSequenceSearch->join();
	}

	// lock parameter variables
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		CFMESequenceSearch<T>::codeLength = codeLength;
		CFMESequenceSearch<T>::excessTime = excessTime;
		CFMESequenceSearch<T>::deltaTMaxTwice = deltaTMaxTwice;
		CFMESequenceSearch<T>::minLength = minLength;
		CFMESequenceSearch<T>::maxLength = maxLength;
		CFMESequenceSearch<T>::maxToneLevelRatio = maxToneLevelRatio;

		searchTones.insert( searchTonesFirst, searchTonesLast );
		for ( auto it = searchTones.begin(); it != searchTones.end(); it++ ) {
			CFMESequenceSearch<T>::searchTones.push_back( it->second );
		}

		// initialize signaling of errors in the frequency search thread
		runtimeErrorSignal.disconnect_all_slots();
		runtimeErrorSignal.connect( runtimeErrorCallback );

		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}

	// start new frequency search thread
	threadSequenceSearch = std::make_unique<boost::thread>( &CFMESequenceSearch<T>::SearchFullSequencesThread, this );
}



/**	@brief		Getting parameters.
*	@param		codeLength				Required length of a sequence code (5 for 5-tone-sequence)
*	@param		excessTime				This time [s] increases the length of the possible tone to ensure save detection
*	@param		deltaTMaxTwice			Two following tones with a larger distance of the start times are not considered as repetitions [s]
*	@param		minLength				Minimum length of a tone [s]
*	@param		maxLength				Maximum length of a tone [s]
*	@param		maxToneLevelRatio		Limit of the allowed ratio between the absolute signal level of the first tone compared to all other tones of a sequence (i.e.: 1 / maxToneLevelRatio <= level <= maxToneLevelRatio) [-]
*	@param		searchTonesFirst		Iterator to beginning of map with all tone frequencies and indices to be searched	
*	@return 							Iterator to end of map with all tone frequencies and indices to be searched
*	@exception 							None
*	@remarks 							None
*/
template <class T> template <class Out_It> Out_It Core::FME::CFMESequenceSearch<T>::GetParameters(int& codeLength, double& excessTime, double& deltaTMaxTwice, double& minLength, double& maxLength, double& maxToneLevelRatio, Out_It searchTonesFirst)
{
	if ( isInit ) {
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
		codeLength = CFMESequenceSearch<T>::codeLength;
		excessTime = CFMESequenceSearch<T>::excessTime;
		deltaTMaxTwice = CFMESequenceSearch<T>::deltaTMaxTwice;
		minLength = CFMESequenceSearch<T>::minLength;
		maxLength = CFMESequenceSearch<T>::maxLength;
		maxToneLevelRatio = CFMESequenceSearch<T>::maxToneLevelRatio;
		for ( auto it = CFMESequenceSearch<T>::searchTones.begin(); it != CFMESequenceSearch<T>::searchTones.end(); it++ ) {
			*(searchTonesFirst++) = *it;
		}
	} else {
		throw std::runtime_error("Parameters are not set.");
	}

	return searchTonesFirst;
}



/**	@brief		Calculation of tone lengths for given start and stop times
*	@param		tStart					std::vector< std::deque< ptime > > container (for all possible tones in one row, in the second row several tone events can be put at once)
*	@param		tEnd					std::vector< std::deque< ptime > > container (for all possible tones in one row, in the second row several tone events can be put at once). Must be of the same size as tStart.
*	@return 							std::vector< std::deque< time_duration > > containing all tone lengths (same size as tStart, tEnd)
*	@exception 							None
*	@remarks 							This function relies on the Boost Time library
*/
template <class T> std::vector <std::deque < boost::posix_time::time_duration > > Core::FME::CFMESequenceSearch<T>::CalculateToneLengths(std::vector < std::deque < boost::posix_time::ptime > > tStart, std::vector < std::deque < boost::posix_time::ptime > > tEnd)
{
	using namespace boost::posix_time;
	using namespace std;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	vector < deque< time_duration > > deltaT( tStart.size() );

	// calculate the tone lengths
	for ( size_t f=0; f < tStart.size(); f++ ) {
		for ( size_t i=0; i < tStart[f].size(); i++ ) {
			deltaT[f].push_back( tEnd[f][i] - tStart[f][i] + microseconds( static_cast<long>( excessTime * 1.0e6 ) ) );
		}
	}

	return deltaT;
}



/**	@brief		Find tones for given tonen lengths
*	@param		codeMeaning				std::vector<int> container containing the code indices corresponding to deltaT and tStart (for all possible tones)
*	@param		deltaT					std::vector< std::deque<time_duration> > container (for all possible tones in one row, in the second row several tone events can be put at once)
*	@param		tStartCalc				std::vector< std::deque<ptime> > container (for all possible tones in one row, in the second row several tone events can be put at once). These are calculated times.
*	@param		tStartRef				std::vector< std::deque<ptime> > container (for all possible tones in one row, in the second row several tone events can be put at once). These are reference times.
*	@param		freq					std::vector< std::deque<T> > container (for all possible tones in one row, in the second row several tone events can be put at once)
*	@param		absToneLevel			std::vector< std::deque<T> > container (for all possible tones in one row, in the second row several tone events can be put at once)
*	@return 							queue containing all found tones: (reference start time, calculated start time, tone number, tone length, tone period, tone frequency [Hz] )
*	@exception 							None
*	@remarks 							This function relies on the Boost Time library. All parameters are required to have the identical size.
*/
template <class T> std::deque< typename Core::FME::CFMESequenceSearch<T>::template ToneDataType<T> > Core::FME::CFMESequenceSearch<T>::FindTones(std::vector<int> codeMeaning, std::vector < std::deque < boost::posix_time::time_duration > > deltaT, std::vector < std::deque < boost::posix_time::ptime > > tStartCalc, std::vector < std::deque < boost::posix_time::ptime > > tStartRef, std::vector < std::deque <T> > freq, std::vector< std::deque<T> > absToneLevel)
{
	using namespace boost::posix_time;
	using namespace std;

	deque< typename CFMESequenceSearch<T>::template ToneDataType<T> > tones, correctLength;
	ptime tStartCurrent, tNext;
	time_duration delta;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// find all tones with correct length
	for ( size_t f=0; f < deltaT.size(); f++ ) {		
		for ( size_t i=0; i < deltaT[f].size(); i++ ) {
			if ( ( deltaT[f][i] > microseconds( static_cast<long>( minLength * 1.0e6 ) ) ) && ( deltaT[f][i] < microseconds( static_cast<long>( maxLength * 1.0e6 ) ) ) ) {
				typename CFMESequenceSearch<T>::template ToneDataType<T> toneData( tStartRef[f][i], tStartCalc[f][i], codeMeaning[f], deltaT[f][i], time_duration( not_a_date_time ), freq[f][i], absToneLevel[f][i] );
				tones.push_back( toneData );
			}
		}
	}

	// sort the stream based on calculated times
	sort( tones.begin(), tones.end(), [=] ( auto tone1, auto tone2 ) { return ( get<1>(tone1) < get<1>( tone2 ) ); } );

	// calculate tone periods
	for (auto itMap = tones.begin(); itMap != tones.end(); itMap++) {
		// determine start time of next tone
		auto itNext = itMap;
		itNext++;
		if ( itNext != tones.end() ) {
			tNext = get<1>( *itNext );
		} else {
			// handle last tone
			tNext = ptime( not_a_date_time ); 
		}
		tStartCurrent = get<1>( *itMap );
			
		// check if the period between to the two start times or between start and stop time of the tone is larger (next tone might start before end of previous tone)
		if ( ( tNext - tStartCurrent ) < get<3>( *itMap ) ) {
			delta = get<3>( *itMap );
		} else {
			delta = tNext - tStartCurrent;
		}
			
		// store tone period
		typename CFMESequenceSearch<T>::template ToneDataType<T> correctLengthData( get<0>( *itMap ), get<1>( *itMap ), get<2>( *itMap ), get<3>( *itMap ), delta, get<5>( *itMap ), get<6>( *itMap ) );
		correctLength.push_back( correctLengthData );
	}
	tones.swap( correctLength );

	return tones;
}



/**	@brief		Find code sequences in stream of tones		
*	@param		tonesBegin				Iterator to the beginning of the container with the found tones. ( start time, tone number, tone length, tone period, tone frequency ). Datatype: std::tuple< boost::posix_time::ptime, int, boost::posix_time::time_duration, boost::posix_time::time_duration, T >. The size must be equivalent to 'codeLength'.
*	@param		tonesEnd				Iterator to one element after the end of the container with the found tones
*	@param		foundCodesBegin			Iterator to the beginning of the container which returns all found code sequences. ( reference start time, calculated start time, ( tone number, tone length [s], tone period [s], tone frequency [Hz] ) of all tones ). Use std::back_inserter. Datatype: std::tuple< boost::posix_time::ptime, boost::posix_time::ptime, Utilities::CCodeData<T> >.
*	@param		startTimeLast			Starting time of the last code sequence detected
*	@param		lastCode				Last code detected. Datatype std::vector<int> with the tone indices, must be of size codeLength.	
*	@return 							None
*	@exception 	std::runtime_error		Thrown if the input datatype is wrong
*	@remarks 							This function relies on the Boost Time library. Do not use the calculated start time returned - except for relative calculations.
*/
template <class T> template <class In_It, class Out_It> void Core::FME::CFMESequenceSearch<T>::FindCodeSequences(In_It tonesBegin, In_It tonesEnd, Out_It foundCodesBegin, boost::posix_time::ptime& calcStartTimeLast, std::vector<int>& lastCode)
{
	using namespace boost::posix_time;
	using namespace std;
	
	ptime refStartTime, calcStartTime, refStartTimeSequence, calcStartTimeSequence;
	bool isComplete;
	deque< typename CFMESequenceSearch<T>::template ToneDataType<T> > tones;
	deque< tuple< ptime, ptime, Utilities::CCodeData<T> > > foundCodes;
	Utilities::CCodeData<T> codeSeq;
	vector<T> absToneLevel;

	// check type of input data
	if ( typeid( typename iterator_traits< In_It >::value_type ) != typeid( ToneDataType<T> ) ) {
		throw std::runtime_error("Wrong type of input data.");
	}

	// obtain input data
	tones.assign( tonesBegin, tonesEnd );

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// find all code sequences - length of tones is already checked
	isComplete = false;
	for ( auto it = tones.begin(); it != tones.end(); it++ ) {
		refStartTime = get<0>( *it );
		calcStartTime = get<1>( *it );
		if ( isComplete ) {
			// check if sequence is really complete
			if ( codeSeq.GetLength() == codeLength ) {
				auto codeData = std::make_tuple( refStartTimeSequence, calcStartTimeSequence, codeSeq ); 
				foundCodes.push_back( codeData );
			}
			isComplete = false;
			codeSeq.Clear();
			absToneLevel.clear();
		}
		if ( codeSeq.GetLength() == 0 ) {
			refStartTimeSequence = refStartTime;
			calcStartTimeSequence = calcStartTime;
		}

		// ensure that the relative tone levels are similar enough
		absToneLevel.push_back( get<6>( *it ) );
		if ( codeSeq.GetLength() > 0 ) {
			if ( ( absToneLevel.front() == 0 ) || ( absToneLevel.back() / absToneLevel.front() < static_cast<T>( 1 / maxToneLevelRatio ) ) || ( absToneLevel.back() / absToneLevel.front() > static_cast<T>( maxToneLevelRatio ) ) ) {
				return; // due to the method preconditions the method can be aborted in this case
			}
		}

		// convert from Boost Time
		codeSeq.AddOneTone( get<2>( *it ), static_cast<T>( ( get<3>( *it ) ).total_microseconds() / 1.0e6 ), static_cast<T>( ( get<4>( *it ) ).total_microseconds() / 1.0e6 ), get<5>( *it ), absToneLevel.back() );

		// check tone periods - in case of too large tone period end of sequence is assumed
		if ( !( ( get<4>( *it ) > microseconds( static_cast<long>( minLength * 1.0e6 ) ) ) && ( get<4>( *it ) < microseconds( static_cast<long>( maxLength * 1.0e6 ) ) ) ) ) {
			isComplete = true;
		}
	}

	if ( isComplete ) {
		// check if sequence is really complete
		if ( codeSeq.GetLength() == codeLength ) {
			tuple< ptime, ptime, Utilities::CCodeData<T> > codeData = std::make_tuple( refStartTimeSequence, calcStartTimeSequence, codeSeq );
			foundCodes.push_back( codeData );
		}
	}

	// check if the code was detected twice
	for ( auto it0 = foundCodes.begin(); it0 != foundCodes.end(); it0++) {
		calcStartTime = get<1>( *it0 ); // calculated time stamp (only for comparison)
		vector<int> code = ( get<2>( *it0 ) ).GetTones();

		// checking with calculated time stamps
		if ( ( lastCode.size() > 0 ) && ( equal( lastCode.begin(), lastCode.end(), code.begin() ) ) && ( ( calcStartTime - calcStartTimeLast ) <= microseconds( static_cast<long>( deltaTMaxTwice * 1.0e6 ) ) ) ) {
			// delete second detection
			if ( foundCodes.size() == 1 ) {
				foundCodes.clear();
				break;
			} else {
				foundCodes.erase( it0-- );
			}
		} else {
			calcStartTimeLast = calcStartTime;
			lastCode.assign( code.begin(), code.end() );
		}
	}

	std::move( begin( foundCodes ), end( foundCodes ), foundCodesBegin );
}



/**	@brief		Find code sequences for given tones (with not yet checked tone lengths)
*	@param		tonesBegin				Iterator to the beginning of the container with all found tones. ( tone number, reference start time, calculated start time, stop time, tone frequency, absolute tone level ). Datatype: std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, T >.
*	@param		tonesEnd				Iterator to one element after the end of the found tones container.
*	@param		foundCodesBegin			Iterator to the beginning of the container which returns all found code sequences. ( reference start time, calculated start time, ( tone number, tone length [s], tone period [s], tone frequency [Hz] ) of all tones ). Use std::back_inserter. Datatype: std::tuple< boost::posix_time::ptime, boost::posix_time::ptime, Utilities::CCodeData<T> >.
*	@param		startTimeLast			Starting time of the last code sequence detected
*	@param		lastCode				Last code detected. Datatype std::vector<int> with the tone indices, must be of size codeLength
*	@return 							Iterator to one element after the end of the container returning all found code sequences
*	@exception	std::runtime_error		Thrown if the input datatype is wrong
*	@remarks 							This function relies on the Boost Time library. Do not use the calculated start time returned - except for relative calculations.
*/
template <class T> template <class In_It, class Out_It> Out_It Core::FME::CFMESequenceSearch<T>::FindFullCodeSequences(In_It tonesBegin, In_It tonesEnd, Out_It foundCodesBegin, boost::posix_time::ptime& startTimeLast, std::vector<int>& lastCode)
{
	using namespace boost::posix_time;
	using namespace std;

	vector<int> codeMeaning, currTones;
	vector< deque< time_duration > > deltaT; 
	vector< deque< ptime > > tStartCalc, tStartRef, tEndCalc;
	vector< deque<T> > freq, absToneLevel;
	deque< typename CFMESequenceSearch<T>::template ToneType<T> > tones;
	deque< typename CFMESequenceSearch<T>::template ToneDataType<T> > toneStream;
	deque< tuple< ptime, ptime, Utilities::CCodeData<T> > > foundCodes;

	if ( !isInit ) {
		throw std::runtime_error("The object was not initialized before use!");
	}

	// check type of input data
	if ( typeid( typename iterator_traits< In_It >::value_type ) != typeid(ToneType<T> ) || ( ( lastCode.size() != codeLength ) && ( lastCode.size() != 0 ) ) ) {
		throw std::runtime_error("Wrong type of input data.");
	}

	// obtain input data
	tones.assign( tonesBegin, tonesEnd );

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );
	
	// obtain start and stop times of the tones
	int toneIndexMax = std::get<0>( *max_element( tones.begin(), tones.end(), [=] ( auto tone1, auto tone2  ) { return ( get<0>( tone1 ) < get<0>( tone2 ) ); } ) );

	int toneIndex;
	codeMeaning.resize( toneIndexMax + 1 );
	tStartRef.resize( toneIndexMax + 1 );
	tStartCalc.resize( toneIndexMax + 1 );
	tEndCalc.resize( toneIndexMax + 1 );
	freq.resize( toneIndexMax + 1 );
	absToneLevel.resize( toneIndexMax + 1 );
	for (auto it = tones.begin(); it != tones.end(); it++) {
		toneIndex = get<0>( *it );
		codeMeaning[ toneIndex ] = get<0>( *it ) + 1;
		tStartRef[ toneIndex ].push_back( get<1>( *it ) );		// reference start time (absolute time stamps)
		tStartCalc[ toneIndex ].push_back( get<2>( *it ) );		// calculated start time (only useful for relative time differences)
		tEndCalc[ toneIndex ].push_back( get<3>( *it ) );		// calculated end time
		freq[ toneIndex ].push_back( get<4>( *it ) );
		absToneLevel[toneIndex].push_back( get<5>( *it ) );
	}

	// calculate the tone lengths - using calculated times
	deltaT = CalculateToneLengths( tStartCalc, tEndCalc );

	// find all tones with correct length - using reference times
	toneStream = FindTones( codeMeaning, deltaT, tStartCalc, tStartRef, freq, absToneLevel );

	// find the full code sequences
	FindCodeSequences( toneStream.begin(), toneStream.end(), back_inserter( foundCodes ), startTimeLast, lastCode );

	// handle special tones
	for (auto it = foundCodes.begin(); it != foundCodes.end(); it++) {
		currTones = ( get<2>( *it ) ).GetTones();
		for ( size_t i=0; i < currTones.size(); i++ ) {	
			// handle tone "0"
			if ( currTones[i] == 10 ) {
				currTones[i] = 0;
				( get<2>( *it ) ).SetTones( currTones );
			}

			// handle tone "R" (repetition)
			if ( i > 0 ) {
				if ( currTones[i] == 11 ) {
					currTones[i] = currTones[i-1];
					( get<2>( *it ) ).SetTones( currTones );		
				}
			}
		}
		
	}

	return std::move( begin( foundCodes ), end( foundCodes ), foundCodesBegin );
}



/**	@brief		Function containing the thread continously analyzing the tone data
*	@return 							None
*	@exception 	std::runtime_error		Thrown if the parameters of the object were not set properly before using the function
*	@exception	std::length_error		Thrown if the length of the time and the peak container is not identical
*	@remarks 							The analysis is performed whenever new data has been notified by CToneSearch<T>::PutFrequencyStream()
*/
template <class T> void Core::FME::CFMESequenceSearch<T>::SearchFullSequencesThread(void)
{
	using namespace boost::posix_time;
	using namespace std;
	
	ptime startTimeLast;
	std::vector<int> lastCode;
	deque< typename CFMESequenceSearch<T>::template ToneType<T> > currTones, processTones;
	deque< tuple< ptime, ptime, Utilities::CCodeData<T> > > newFoundCodes;

	try {	
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		startTimeLast = ptime( not_a_date_time );

		// process audio data until interruption is requested
		while ( !(boost::this_thread::interruption_requested()) ) {
			// read from signal queue	
			boost::unique_lock<boost::mutex> lock( inputTonesMutex );
			currTones.insert( currTones.end(), tones.begin(), tones.end() );
			tones.clear();

			if ( static_cast<int>( currTones.size() ) >= codeLength ) {
				lock.unlock();				
				
				// get data for next analysis step and delete the no longer required data
				processTones.clear();
				GetNextDatasets( currTones, back_inserter( processTones ) );
				
				// search for peaks in the new signal spectrogram
				FindFullCodeSequences( processTones.begin(), processTones.end(), back_inserter( newFoundCodes ), startTimeLast, lastCode );	
									
				// move result to data stream
				boost::unique_lock<boost::mutex> lockResult( resultMutex );
				foundCodes.insert( foundCodes.end(), newFoundCodes.begin(), newFoundCodes.end() );
				newFoundCodes.clear();
			} else {
				// wait for new audio data
    			newTonesCondition.wait( inputTonesMutex ); 
			}
		}
	} catch (std::exception e) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "FME sequence search thread: " + string( e.what() ) );
	}
}


/**	@brief		Put new data in the tone stream
*	@param		newTonesBegin				Iterator to the beginning of the container with new tones to be transferred to the object. The datatype must be std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, T >			
*	@param		newTonesEnd					Iterator to one element after the end of the container with the new tones
*	@return 								None
*	@exception	std::runtime_error			Thrown if the datatype of the input container is wrong
*	@remarks 								None
*/
template <class T> template <class In_It> void Core::FME::CFMESequenceSearch<T>::PutTonesStream(In_It newTonesBegin, In_It newTonesEnd)
{
	using namespace boost::posix_time;
	using namespace std;

	vector < boost::posix_time::ptime > timeTemp;
	vector < vector<T> > peaksTemp;
	deque< typename CFMESequenceSearch<T>::template ToneType<T> > newTones;

	// check type of input data
	if ( typeid( typename iterator_traits< In_It >::value_type ) != typeid( ToneType<T> ) ) {
		throw std::runtime_error("Wrong type of input data.");
	}

	// obtain input data
	newTones.assign( newTonesBegin, newTonesEnd );

	boost::unique_lock<boost::mutex> lock( inputTonesMutex );

	// add new data to the stream
	tones.insert( tones.end(), newTones.begin(), newTones.end() );
	
	// trigger excecution of sequence analysis thread
	if ( newTones.size() > 0 ) {
		newTonesCondition.notify_all();
	}
}



/**	@brief		Obtains newly detected sequences
*	@param		newSequencesBegin			Iterator to beginning of output sequence container. It will contain all new sequences found since the last call of this function. Use std::back_inserter. It must be of type std::tuple < boost::posix_time::ptime, Utilities::CCodeData<T> >.
*	@return 								None
*	@exception 								None
*	@remarks 								All returned sequences will be deleted from the object. The sequences contain: ( start time, ( tone number, tone length [s], tone period [s], tone frequency [Hz] ) of all tones ) )
*/
template <class T> template <class Out_It> void Core::FME::CFMESequenceSearch<T>::GetSequences(Out_It newSequencesBegin)
{
	using namespace boost::posix_time;
	using namespace std;

	deque< tuple < ptime, ptime, Utilities::CCodeData<T> > > newSequences;

	// copy to local data in order to reduce lock time
	boost::unique_lock<boost::mutex> lockResult( resultMutex );
	newSequences.assign( foundCodes.begin(), foundCodes.end() );

	// delete old results
	foundCodes.clear();

	lockResult.unlock();

	// remove calculated time stamp - not required and not suitable (because it is only valid for relative calculations)
	transform( begin( newSequences ), end( newSequences ), newSequencesBegin, []( auto val ) { return make_tuple( get<0>( val ), get<2>( val ) ); } );
}



/**	@brief		Finds the data required for the next analysis step and deletes no longer required data
*	@param		currTones					Queue containing all tones	
*	@param		processTonesBegin			Iterator to beginning of the container for the tones for the current analysis step. Use std::back_inserter. It must be of data type std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, T >.
*	@return 								Iterator to one element after the end of the container for the tones for the current analysis step
*	@exception 								None by the function itself
*	@remarks 								Data no longer required in the next steps is deleted from the input queues. Note that due to overlap effects 
*											the deleted data is not equivalent to the returned data for the next analysis step.
*/
template <class T> template <class Out_It> Out_It Core::FME::CFMESequenceSearch<T>::GetNextDatasets(std::deque< typename CFMESequenceSearch<T>::template ToneType<T> >& currTones, Out_It processTonesBegin)
{
	std::deque< typename CFMESequenceSearch<T>::template ToneType<T> > processTones;

	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// copy data to processed
	if ( static_cast<int>( currTones.size() ) > codeLength ) {
		processTones.assign( begin( currTones ), begin( currTones ) + codeLength );
	} else {
		processTones.assign( begin( currTones ), end( currTones ) );
	}

	// delete no longer required data
	if ( !currTones.empty() ) {
		currTones.erase( begin( currTones ), begin( currTones ) + 1 );
	}

	return std::move( begin( processTones ), end( processTones ), processTonesBegin );
}