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
#include <map>
#include <string>
#include <functional>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BoostStdTimeConverter.h"
#include "SeqData.h"
#include "SeqDataComplete.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Audio {
		/**	\ingroup Core
		*	Class for keeping audio signal data for possible later storage
		*/
		template <class T> class CAudioSignalPreserver
		{
		public:
			CAudioSignalPreserver(void);
			CAudioSignalPreserver(const double& samplingFreq, const T& recordTimeLowerLimit, const T& recordTimeUpperLimit, const T& recordTimeBuffer, std::function< void ( const Utilities::CSeqData&, std::vector<T>, double ) > foundRecordCallback, std::function< void ( const std::string& ) > runtimeErrorCallback);
			virtual ~CAudioSignalPreserver(void);
			template <class InIt1, class InIt2> void PutSignalData(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst);
			template <class InIt> void PutSequences(InIt sequencesFirst, InIt sequencesLast);
			void SetParameters(const double& samplingFreq, const T& recordTimeLowerLimit, const T& recordTimeUpperLimit, const T& recordTimeBuffer, std::function< void ( const Utilities::CSeqData&, std::vector<T>, double ) > foundRecordCallback, std::function< void ( const std::string& ) > runtimeErrorCallback);
			void GetParameters(double& samplingFreq, T& recordTimeLowerLimit, T& recordTimeUpperLimit, T& recordTimeBuffer) const;
		protected:
			template <class InIt1, class InIt2> void TransmitRecordedSignal(const Utilities::CSeqDataComplete<T>& sequenceData, InIt1 recordedTimeFirst, InIt1 recordedTimeLast, InIt2 recordedSignalFirst);
			void DeleteSignalData(std::deque<boost::posix_time::ptime>& recordedTime, std::deque<T>& recordedSignal, const int& deleteIndex);

			std::vector< boost::posix_time::ptime > recordedTime;
			std::vector<T> recordedSignal;
			std::map< boost::posix_time::ptime, Utilities::CSeqDataComplete<T> > captureSequenceRecords;
			double samplingFreq;
			int recordTimeBufferLength;
			int recordTimeUpperLimitLength;
			int recordTimeLowerLimitLength;	
		private:
			CAudioSignalPreserver(const CAudioSignalPreserver &);				// prevent copying
    		CAudioSignalPreserver & operator= (const CAudioSignalPreserver &);	// prevent assignment
			void AudioStorageThread(void);

			std::unique_ptr< boost::thread > threadAudioStorage;
			boost::condition_variable_any sequenceRecordCondition;
			mutable boost::shared_mutex parameterMutex;
			boost::mutex recordingMutex;
			boost::signals2::signal<void ( const Utilities::CSeqData&, std::vector<T>, double )> foundRecordSignal;
			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			bool isInit;
		};
	}
}
/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Audio::CAudioSignalPreserver<T>::CAudioSignalPreserver(void)
	: isInit( false )
{
}



/**	@brief		Constructor
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		recordTimeLowerLimit		Starting time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeUpperLimit		Stopping time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeBuffer			Time buffer for ensuring that historical data recorded for detecting the sequence can be stored (in seconds, always >= 0, 0 means that no historical data is stored). It must always be of a larger absolute value than recordTimeLowerLimit.
*	@param		foundRecordCallback			Callback function called when signal recording after a sequence detection has been finished. The function parameter Utilities::CSeqData contains: (sequence time (DD, MM, YY, HH, MM, SS, millisec, sequence digits), recorded signal data, sampling frequency of recording [Hz]).
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@exception 								None
*	@remarks 								The set parameters can be reset by using CAudioSignalPreserver<T>::SetParameters. 
*											1. example: recordTimeLowerLimit = +1.0 s, recordTimeUpperLimit = +25 s => required recordTimeBuffer = 0.0 s (or set to the estimated offset time between start of sequence and finishing detection if required). The stored signal will begin 1.0 s after the detected sequence start time and will stop 25 s after that time.
*											2. example: recordTimeLowerLimit = -1.0 s, recordTimeUpperLimit = +25 s => recordTimeBuffer = 1.05 s (0.05 s is detection time offset). The stored signal will begin 1.0 s in advance of the detected sequence start time and will stop 25 s after that time.
*/
template <class T> Core::Audio::CAudioSignalPreserver<T>::CAudioSignalPreserver(const double& samplingFreq, const T& recordTimeLowerLimit, const T& recordTimeUpperLimit, const T& recordTimeBuffer, std::function< void ( const Utilities::CSeqData&, std::vector<T>, double ) > foundRecordCallback, std::function< void ( const std::string& ) > runtimeErrorCallback)
	: isInit( false )
{
	SetParameters( samplingFreq, recordTimeLowerLimit, recordTimeUpperLimit, recordTimeBuffer, foundRecordCallback, runtimeErrorCallback );
}



/**	@brief		Setting the class parameters.
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		recordTimeLowerLimit		Starting time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeUpperLimit		Stopping time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeBuffer			Time buffer for ensuring that historical data recorded for detecting the sequence can be stored (in seconds, always >= 0, 0 means that no historical data is stored). It must always be of a larger absolute value than recordTimeLowerLimit.
*	@param		foundRecordCallback			Callback function called when signal recording after a sequence detection has been finished. The function parameter Utilities::CSeqData contains: (sequence time (DD, MM, YY, HH, MM, SS, millises, sequence digits), recorded signal data, sampling frequency of recording [Hz]).
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception	std::runtime_error			Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 								This function can safely be called repeatedly for resetting the class
*											1. example: recordTimeLowerLimit = +1.0 s, recordTimeUpperLimit = +25 s => required recordTimeBuffer = 0.0 s (or set to the estimated offset time between start of sequence and finishing detection if required). The stored signal will begin 1.0 s after the detected sequence start time and will stop 25 s after that time.
*											2. example: recordTimeLowerLimit = -1.0 s, recordTimeUpperLimit = +25 s => recordTimeBuffer = 1.05 s (0.05 s is detection time offset). The stored signal will begin 1.0 s in advance of the detected sequence start time and will stop 25 s after that time.
*/
template <class T> void Core::Audio::CAudioSignalPreserver<T>::SetParameters(const double& samplingFreq, const T& recordTimeLowerLimit, const T& recordTimeUpperLimit, const T& recordTimeBuffer, std::function< void ( const Utilities::CSeqData&, std::vector<T>, double ) > foundRecordCallback, std::function< void ( const std::string& ) > runtimeErrorCallback)
{
	// stop thread if it is running
	if ( threadAudioStorage != nullptr ) {
		threadAudioStorage->interrupt();
	
		// wait until thread has stopped
		threadAudioStorage->join();
	}

	// lock any changes in the parameter set
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		CAudioSignalPreserver<T>::samplingFreq = samplingFreq;

		// initialize signaling of recorded sequences
		if ( isInit ) {
			foundRecordSignal.disconnect_all_slots();
		}
		foundRecordSignal.connect( foundRecordCallback );

		CAudioSignalPreserver<T>::recordTimeBufferLength = static_cast<int>( recordTimeBuffer * samplingFreq );
		CAudioSignalPreserver<T>::recordTimeLowerLimitLength = static_cast<int>( recordTimeLowerLimit * samplingFreq );
		CAudioSignalPreserver<T>::recordTimeUpperLimitLength = static_cast<int>( recordTimeUpperLimit * samplingFreq );
		if ( recordTimeBufferLength < -recordTimeLowerLimitLength ) {
			throw std::length_error( "Time buffer for recording must be larger than the lower time limit." );
		}
		if ( recordTimeLowerLimitLength > recordTimeUpperLimitLength ) {
			throw std::length_error( "Lower time limit must not be larger than upper time limit." );
		}
		if ( recordTimeBufferLength < 0 ) {
			throw std::length_error( "Time buffer must not be negative." );
		}

		// initialize signaling of runtime errors
		runtimeErrorSignal.disconnect_all_slots();
		runtimeErrorSignal.connect( runtimeErrorCallback );

		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}

	// start new thread
	threadAudioStorage = std::unique_ptr<boost::thread>( new boost::thread( std::bind( &CAudioSignalPreserver<T>::AudioStorageThread, this ) ) );
}



/**	@brief		Getting the class parameters.
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		recordTimeLowerLimit		Starting time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeUpperLimit		Stopping time of audio recording relative to the start of the sequence (in seconds). It can be negative (before start of the sequence) or positive (after the start of the sequence).
*	@param		recordTimeBuffer			Time buffer for ensuring that historical data recorded for detecting the sequence can be stored (in seconds, always >= 0, 0 means that no historical data is stored). It must always be of a larger absolute value than recordTimeLowerLimit.
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> void Core::Audio::CAudioSignalPreserver<T>::GetParameters(double& samplingFreq, T& recordTimeLowerLimit, T& recordTimeUpperLimit, T& recordTimeBuffer) const
{
	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	samplingFreq = CAudioSignalPreserver<T>::samplingFreq;
	recordTimeLowerLimit = CAudioSignalPreserver<T>::recordTimeLowerLimitLength / static_cast<T>( samplingFreq );
	recordTimeUpperLimit = CAudioSignalPreserver<T>::recordTimeUpperLimitLength / static_cast<T>( samplingFreq );
	recordTimeBuffer = CAudioSignalPreserver<T>::recordTimeBufferLength / static_cast<T>( samplingFreq );
}



/**	@brief		Destructor
*/
template <class T> Core::Audio::CAudioSignalPreserver<T>::~CAudioSignalPreserver(void)
{
	if ( threadAudioStorage != nullptr ) {
		// stop running thread
		threadAudioStorage->interrupt();
	
		// wait until thread has stopped
		threadAudioStorage->join();
	}
}



/**	@brief		Passing the captured audio signal data stream
*	@param		timeFirst					Iterator to the beginning of the container storing the time data corresponding to the signal data
*	@param		timeLast					Iterator to one element after the end of the container storing the time data corresponding to the signal data
*	@param		signalFirst					Iterator to the beginning of the container storing the signal data. It must be of the same size as the time data container.
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> template <class InIt1, class InIt2> void Core::Audio::CAudioSignalPreserver<T>::PutSignalData(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst)
{
	boost::unique_lock<boost::mutex> lock( recordingMutex );
	recordedTime.insert( recordedTime.end(), timeFirst, timeLast );
	recordedSignal.insert( recordedSignal.end(), signalFirst, signalFirst + std::distance( timeFirst, timeLast ) );

	// trigger excecution of controlling thread
	if ( distance( timeFirst, timeLast ) > 0 ) {
		sequenceRecordCondition.notify_all();
	}
}



/**	@brief		Passing detected sequences
*	@param		sequencesFirst				Iterator to the beginning of the container storing the sequences (format: Utilities::CSeqDataComplete<T>)
*	@param		sequencesLast				Iterator to one element after the end of the container storing the sequences
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> template <class InIt> void Core::Audio::CAudioSignalPreserver<T>::PutSequences(InIt sequencesFirst, InIt sequencesLast)
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities::Time;

	boost::unique_lock<boost::mutex> lockRecording( recordingMutex );
	for (auto itSeq =sequencesFirst; itSeq != sequencesLast; itSeq++ ) {
		captureSequenceRecords.insert( pair< ptime, Utilities::CSeqDataComplete<T> >( CBoostStdTimeConverter::ConvertToBoostTime( itSeq->GetStartTime() ), *itSeq ) );
	}

	// trigger excecution of controlling thread
	sequenceRecordCondition.notify_all();
}



/**	@brief		Function containing the thread keeping the audio data for possible later storage
*	@return 						None
*	@exception 	std::runtime_error	Thrown if the parameters of the object were not set properly before using the function
*	@remarks 						None
*/
template <class T> void Core::Audio::CAudioSignalPreserver<T>::AudioStorageThread(void)
{
	using namespace std;
	using namespace boost::posix_time;

	ptime sequenceTime, firstSequenceTime;
	int deleteIndex;
	std::map< boost::posix_time::ptime, Utilities::CSeqDataComplete<T> > localCaptureSequenceRecords;
	deque<ptime> localRecordedTime;
	deque<T> localRecordedSignal;
	bool isDelete;

	try {
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		while ( !boost::this_thread::interruption_requested() ) {		
			// copy data to local data of thread
			boost::unique_lock<boost::mutex> lockRecording( recordingMutex );
			localCaptureSequenceRecords.insert( captureSequenceRecords.begin(), captureSequenceRecords.end() );
			captureSequenceRecords.clear();
			localRecordedTime.insert( localRecordedTime.end(), recordedTime.begin(), recordedTime.end() );
			recordedTime.clear();
			localRecordedSignal.insert( localRecordedSignal.end(), recordedSignal.begin(), recordedSignal.end() );
			recordedSignal.clear();
			lockRecording.unlock();

			if ( !( localCaptureSequenceRecords.empty() ) ) {
				// sequences requiring recording have been found
				auto it = localCaptureSequenceRecords.begin();
				while ( it != localCaptureSequenceRecords.end() ) {
					isDelete = false;
					if ( localRecordedTime.size() > 0 ) {
						// check if sufficient data was recorded
						sequenceTime = get<0>( *it );
						if ( static_cast<int>( ( localRecordedTime.back() - sequenceTime ).total_microseconds() / 1.0e6 * samplingFreq ) > recordTimeUpperLimitLength ) {
							// transmit recorded signal to the connected callback functions
							TransmitRecordedSignal( get<1>( *it ), localRecordedTime.begin(), localRecordedTime.end(), localRecordedSignal.begin() );
							isDelete = true;
						}
					}

					if ( isDelete ) {
						localCaptureSequenceRecords.erase( it++ );
						if ( !( localCaptureSequenceRecords.empty() ) ) {
							if ( localRecordedTime.size() > 0 ) {
								// delete all no longer required data, only the time buffer in front of the first sequence is kept
								firstSequenceTime = get<0>( *localCaptureSequenceRecords.begin() );							
								deleteIndex = static_cast<int>( ( firstSequenceTime - localRecordedTime.front() ).total_microseconds() / 1.0e6 * samplingFreq ) - recordTimeBufferLength;	// the first entry in the map is that with the earliest sequence start time
								DeleteSignalData( localRecordedTime, localRecordedSignal, deleteIndex );
							}
						}
					} else {
						++it;
					}
				}
			} else {
				// no sequences found at the moment - delete old data older than the time buffer, which is definitively no longer required
				if ( !( localRecordedTime.empty() ) ) {
					deleteIndex = localRecordedTime.size() - recordTimeBufferLength;
					DeleteSignalData( localRecordedTime, localRecordedSignal, deleteIndex );
				}
			}

			// set thread back to waiting state
			lockRecording.lock();
			sequenceRecordCondition.wait( recordingMutex );
		}
	} catch ( const std::exception& e ) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Audio signal preserver thread: " + string( e.what() ) );
	}
}



/**	@brief		Transmitting the recorded signal after a sequence detection to all callback functions
*	@param		sequenceData		Data of detected sequence (Start time, for each tone: tone number, tone length [s], tone period [s], tone frequency [Hz], info string)
*	@param		recordedTimeFirst	Iterator to the beginning of the container storing the time corresponding to the recorded signal data
*	@param		recordedTimeLast	Iterator to one element after the end of the container storing the time corresponding to the recorded signal data
*	@param		recordedSignalFirst	Iterator to the beginning of the container storing the recorded signal data. It must have the same size as the time container.
*	@return 						None
*	@exception 						None
*	@remarks 						None
*/
template <class T> template <class InIt1, class InIt2> void Core::Audio::CAudioSignalPreserver<T>::TransmitRecordedSignal(const Utilities::CSeqDataComplete<T>& sequenceData, InIt1 recordedTimeFirst, InIt1 recordedTimeLast, InIt2 recordedSignalFirst)
{
	using namespace std;
	using Utilities::Time::CBoostStdTimeConverter;

	vector< boost::posix_time::ptime > recordedTime;
	vector<T> recordedSignal, newRecordedSignal;
	boost::posix_time::ptime sequenceTime;
	int startIndex, deleteIndex;

	// obtain input data
	recordedTime.assign( recordedTimeFirst, recordedTimeLast );
	recordedSignal.assign( recordedSignalFirst, recordedSignalFirst + distance( recordedTimeFirst, recordedTimeLast ) );
	sequenceTime = Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( sequenceData.GetStartTime() );

	// obtain the time span to be stored - inherently staying within the container limits
	startIndex = static_cast<int>( ( sequenceTime - recordedTime.front() ).total_microseconds() / 1.0e6 * samplingFreq ) + recordTimeLowerLimitLength;
	deleteIndex = static_cast<int>( ( sequenceTime - recordedTime.front() ).total_microseconds() / 1.0e6 * samplingFreq ) + recordTimeUpperLimitLength;				
	if ( startIndex < 0 ) {
		startIndex = 0;
	}
	if ( startIndex > static_cast<int>( recordedSignal.size() ) ) {
		startIndex = recordedSignal.size();
	}
	if ( deleteIndex < 0 ) {
		deleteIndex = 0;
	}
	if ( deleteIndex > static_cast<int>( recordedSignal.size() ) ) {
		deleteIndex = recordedSignal.size();
	}
	newRecordedSignal.assign( recordedSignal.begin() + startIndex, recordedSignal.begin() + deleteIndex );

	// send signal to listening functions
	foundRecordSignal( Utilities::CSeqData( sequenceData.GetStartTime(), sequenceData.GetCodeData().GetTones(), sequenceData.GetInfoString() ), newRecordedSignal, samplingFreq );
}



/**	@brief		Deleting no longer required signal data
*	@param		recordedTime		Container storing the times corresponding to the signal data container. The size will be changed by deletion.
*	@param		recordedSignal		Container storing the signal data. It must have the same size as the the time container. The size will be changed by deletion.
*	@param		deleteIndex			Index to one element after the last element to be stored
*	@return 						None
*	@exception std::length_error	Thrown if the size of the recordedTime and recordedSignal containers differs
*	@remarks 						None
*/
template <class T> void Core::Audio::CAudioSignalPreserver<T>::DeleteSignalData(std::deque<boost::posix_time::ptime>& recordedTime, std::deque<T>& recordedSignal, const int& deleteIndex)
{
	if ( recordedTime.size() != recordedSignal.size() ) {
		throw std::length_error( "The size of the recordedTime and recordedSignal containers differs. " );
	}

	if ( deleteIndex > 0 ) {
		if ( deleteIndex < static_cast<int>( recordedSignal.size() ) ) {
			recordedSignal.erase( recordedSignal.begin(), recordedSignal.begin() + deleteIndex );
			recordedTime.erase( recordedTime.begin(), recordedTime.begin() + deleteIndex );
		} else {
			recordedTime.clear();
			recordedSignal.clear();
		}
		recordedTime.shrink_to_fit();
		recordedSignal.shrink_to_fit();		
	}
}
