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

#include <deque>
#include <string>
#include <functional>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include "SeqData.h"
#include "SeqDataComplete.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Audio {
		/**	\ingroup Core
		*	Class for passing detected sequences to all connected caller functions
		*/
		template <class T> class CSequencePasser
		{
		public:
			CSequencePasser(void);
			CSequencePasser(std::function<void ( const Utilities::CSeqData& )> foundSequenceCallback, std::function<void(const std::string&)> runtimeErrorCallback);
			virtual ~CSequencePasser(void);
			template <class InIt> void PutSequences(InIt sequencesFirst, InIt sequencesLast);
			void SetParameters(std::function<void ( const Utilities::CSeqData& )> foundSequenceCallback, std::function<void(const std::string&)> runtimeErrorCallback);
		protected:
			virtual void SignalNewSequence(const Utilities::CSeqDataComplete<T>& sequenceData);

			std::deque< Utilities::CSeqDataComplete<T> > foundSequences;
		private:
			CSequencePasser(const CSequencePasser &);					// prevent copying
    		CSequencePasser & operator= (const CSequencePasser &);		// prevent assignment
			void SequencePassingThread(void);

			std::unique_ptr< boost::thread > threadSequencePassing;
			boost::condition_variable_any sequenceIsFoundCondition;
			boost::shared_mutex parameterMutex;
			boost::mutex sequenceMutex;
			boost::signals2::signal< void ( const Utilities::CSeqData& ) > foundSequenceSignal;
			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			bool isInit;
		};
	}
}
/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Audio::CSequencePasser<T>::CSequencePasser(void)
	: isInit( false)
{
}



/**	@brief		Constructor
*	@param		foundSequenceCallback		Function, which is called in case of a detected sequence. The sequence data is provided to the function as a parameter: start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), tone numbers of the sequence, string containing additional information (empty for FME-sequences)
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@exception 								None
*	@remarks 								The set parameters can be reset by using CSequencePasser<T>::SetParameters
*/
template <class T> Core::Audio::CSequencePasser<T>::CSequencePasser(std::function<void ( const Utilities::CSeqData& )> foundSequenceCallback, std::function<void(const std::string&)> runtimeErrorCallback)
	: isInit( false)
{
	SetParameters( foundSequenceCallback, runtimeErrorCallback );
}



/**	@brief		Setting the class parameters
*	@param		foundSequenceCallback		Function, which is called in case of a detected sequence. The sequence data is provided to the function as a parameter: start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), tone numbers of the sequence, string containing additional information (empty for FME-sequences)
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return									None
*	@exception	std::runtime_error			Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 								None
*/
template <class T> void Core::Audio::CSequencePasser<T>::SetParameters(std::function<void ( const Utilities::CSeqData& )> foundSequenceCallback, std::function<void(const std::string&)> runtimeErrorCallback)
{
	// stop thread if it is running
	if ( threadSequencePassing != nullptr ) {
		threadSequencePassing->interrupt();
	
		// wait until thread has stopped
		threadSequencePassing->join();
	}

	// lock any changes in the parameter set
	boost::unique_lock<boost::shared_mutex> lock( parameterMutex, boost::try_to_lock );
	if ( lock.owns_lock() ) {
		// initialize signaling of found sequences
		if ( isInit ) {
			foundSequenceSignal.disconnect_all_slots();
		}
		foundSequenceSignal.connect( foundSequenceCallback );

		// initialize signaling of runtime errors
		runtimeErrorSignal.disconnect_all_slots();
		runtimeErrorSignal.connect( runtimeErrorCallback );

		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}

	// start new thread
	threadSequencePassing = std::unique_ptr<boost::thread>( new boost::thread( boost::bind( &CSequencePasser<T>::SequencePassingThread, this ) ) );
}



/**	@brief		Destructor
*/
template <class T> Core::Audio::CSequencePasser<T>::~CSequencePasser(void)
{
	if ( threadSequencePassing != nullptr ) {
		// stop running thread
		threadSequencePassing->interrupt();
	
		// wait until thread has stopped
		threadSequencePassing->join();
	}
}



/**	@brief		Passing the detected sequences
*	@param		sequencesFirst				Iterator to the beginning of the container storing the sequences (format: Utilities::CSeqDataComplete<T>)
*	@param		sequencesLast				Iterator to one element after the end of the container storing the sequences
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> template <class InIt> void Core::Audio::CSequencePasser<T>::PutSequences(InIt sequencesFirst, InIt sequencesLast)
{
	boost::unique_lock<boost::mutex> lock( sequenceMutex );
	foundSequences.insert( foundSequences.end(), sequencesFirst, sequencesLast );

	// wake up the waiting thread for passing the sequence
	sequenceIsFoundCondition.notify_all();
}



/**	@brief		Function containing the thread passing the detected sequences to all connected caller functions
*	@return 						None
*	@exception 	std::runtime_error	Thrown if the parameters of the object were not set properly before using the function
*	@remarks 						None
*/
template <class T> void Core::Audio::CSequencePasser<T>::SequencePassingThread(void)
{
	using namespace std;
	using namespace boost::posix_time;
	
	deque< Utilities::CSeqDataComplete<T> > sequenceCopy;

	try {
		if ( !isInit ) {
			throw std::runtime_error("The object was not initialized before use!");
		}

		// lock any changes in the parameter set
		boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

		// record audio data until interruption is requested
		while ( !( boost::this_thread::interruption_requested() ) ) {
			// obtain time and sequence
			boost::unique_lock<boost::mutex> lock( sequenceMutex );
			sequenceCopy.assign( foundSequences.begin(), foundSequences.end() );
			foundSequences.clear();

			if ( sequenceCopy.size() > 0 ) {
				lock.unlock();

				while ( !sequenceCopy.empty() ) {			
					// fire signal to the callback function
					SignalNewSequence( sequenceCopy.front() );				
					sequenceCopy.pop_front();
				}
			} else {
				// set thread back to waiting state
				sequenceIsFoundCondition.wait( sequenceMutex );
			}
		}
	} catch ( const std::exception& e ) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( "Sequence passer thread: " + string( e.what() ) );
	}
}



/** @brief		Sending a signal to all connected functions
*	@param		sequenceData		Tuple containing the data of the found sequence: ( start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing additional information on the sequence (not used for FME sequences) )
*	@return 						None			
*	@exception						None
*	@remarks						None
*/
template <class T> void Core::Audio::CSequencePasser<T>::SignalNewSequence(const Utilities::CSeqDataComplete<T>& sequenceData)
{
	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutex );

	// fire signal to connected functions
	foundSequenceSignal( Utilities::CSeqData( sequenceData.GetStartTime(), sequenceData.GetCodeData().GetTones(), sequenceData.GetInfoString() ) );
}
