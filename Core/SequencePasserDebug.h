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

#include "SequencePasser.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Audio {
		/**	\ingroup Core
		*	Class for passing detected sequences with full data (debug data) to all connected caller functions
		*/
		template <class T> class CSequencePasserDebug : public CSequencePasser<T>
		{
		public:
			CSequencePasserDebug(void);
			CSequencePasserDebug(std::function<void ( const Utilities::CSeqDataComplete<T>& )> foundSequenceCallbackDebug, std::function< void( const std::string& ) > runtimeErrorCallback);
			virtual ~CSequencePasserDebug(void) {};
			void SetParameters(std::function<void ( const Utilities::CSeqDataComplete<T>& )> foundSequenceCallbackDebug, std::function< void( const std::string& ) > runtimeErrorCallback);
		protected:
			virtual void SignalNewSequence(const Utilities::CSeqDataComplete<T>& sequenceData);
		private:
			CSequencePasserDebug(const CSequencePasserDebug &);					// prevent copying
    		CSequencePasserDebug & operator= (const CSequencePasserDebug &);	// prevent assignment

			boost::shared_mutex parameterMutexDebug;
			boost::signals2::signal<void ( const Utilities::CSeqDataComplete<T>& )> foundSequenceSignalDebug;
		};
	}
}
/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Audio::CSequencePasserDebug<T>::CSequencePasserDebug(void)
	: CSequencePasser<T>()
{
}



/**	@brief		Constructor
*	@param		foundSequenceCallbackDebug	Function, which is called in case of a detected sequence. The (full debug) sequence data is provided to the function as a parameter.
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@exception 								None
*	@remarks 								The set parameters can be reset by using CSequencePasser<T>::SetParameters
*/
template <class T> Core::Audio::CSequencePasserDebug<T>::CSequencePasserDebug(std::function<void ( const Utilities::CSeqDataComplete<T>& )> foundSequenceCallbackDebug, std::function< void( const std::string& ) > runtimeErrorCallback)
	: CSequencePasser<T>()
{
	SetParameters( foundSequenceCallbackDebug, runtimeErrorCallback );
}



/**	@brief		Setting the class parameters
*	@param		foundSequenceCallbackDebug	Function, which is called in case of a detected sequence. The (full debug) sequence data is provided to the function as a parameter.
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return									None
*	@exception 								None
*	@remarks 								None
*/
template <class T> void Core::Audio::CSequencePasserDebug<T>::SetParameters(std::function<void ( const Utilities::CSeqDataComplete<T>& )> foundSequenceCallbackDebug, std::function< void( const std::string& ) > runtimeErrorCallback)
{
	std::function<void ( const Utilities::CSeqData& )> dummy;

	CSequencePasser<T>::SetParameters( dummy, runtimeErrorCallback );

	// lock any changes in the parameter set
	boost::unique_lock<boost::shared_mutex> lock( parameterMutexDebug );

	// initialize signaling of found sequences
	foundSequenceSignalDebug.disconnect_all_slots();
	foundSequenceSignalDebug.connect( foundSequenceCallbackDebug );
}



/** @brief		Sending a signal to all connected functions including the full (debug) information
*	@param		sequenceData			Containing the data of the found sequence: ( start time of sequence (DD, MM, YYYY, HH, MM, SS, millisec), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing additional information on the sequence (not used for FME sequences) ).
*	@return 							None			
*	@exception							None
*	@remarks							None
*/
template <class T> void Core::Audio::CSequencePasserDebug<T>::SignalNewSequence( const Utilities::CSeqDataComplete<T>& sequenceData)
{
	// lock any changes in the parameter set
	boost::shared_lock<boost::shared_mutex> lock( parameterMutexDebug );

	// fire signal to connected functions
	foundSequenceSignalDebug( sequenceData );
}
