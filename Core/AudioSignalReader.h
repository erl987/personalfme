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
#include <thread>
#include <mutex>
#include <atomic>
#include <boost/signals2.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "AudioDevice.h"
#include "PortaudioWrapper.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Audio {
		/**	\ingroup Core
		*	Class for recording audio data from the sound device
		*/
		template <class T> class CAudioSignalReader
		{
		public:
			CAudioSignalReader(void);
			CAudioSignalReader(const Processing::CAudioDevice& device, const double& samplingFreq, const unsigned long& samplesPerBuf, const unsigned int& numChannels, const unsigned int& channel, const unsigned int& maxMissedAttempts, const unsigned int& maxLengthInputQueue, std::function<void(const std::string&)> runtimeErrorCallback);
			virtual ~CAudioSignalReader(void);
			CAudioSignalReader(const CAudioSignalReader &) = delete;
			CAudioSignalReader & operator= (const CAudioSignalReader &) = delete;
			void SetParameters(const Processing::CAudioDevice& device, const double& samplingFreq, const unsigned long& samplesPerBuf, const unsigned int& numChannels, const unsigned int& channel, const unsigned int& maxMissedAttempts, const unsigned int& maxLengthInputQueue, std::function<void(const std::string&)> runtimeErrorCallback);
			void GetParameters(Processing::CAudioDevice& device, double& samplingFreq, unsigned long& samplesPerBuf, unsigned int& numChannels, unsigned int& channel, unsigned int& maxMissedAttempts, unsigned int& maxLengthInputQueue, std::function<void(const std::string&)>& runtimeErrorCallback) const;
			template <class OutIt1, class OutIt2> void GetSignalData(OutIt1 timeFirst, OutIt2 signalFirst);
			void GetAudioDevices( std::vector<Processing::CAudioDevice>& inputDevices, Processing::CAudioDevice& stdInputDevice, const double& samplingFreq, const unsigned int& numChannels ) const;
			bool IsDeviceAvailable( const Core::Processing::CAudioDevice& device, const double& samplingFreq, const int& numChannels ) const;
			void SetAudioDevice(const Processing::CAudioDevice& device);
			void StartReading(void);
			void StopReading(void);
			bool IsReading(void) const;
		protected:
			template <class InIt1, class InIt2> void UpdateSignalQueue(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst, InIt2 signalLast);	
		private:
			void ReadAudioInputThread(void);

			Processing::CPortaudio<T> portaudio;
			std::vector< boost::posix_time::ptime > inputTime;
			std::vector<T> inputSignal;
			mutable std::timed_mutex inputSignalMutex;
			double samplingFreq;
			Processing::CAudioDevice device;
			unsigned long samplesPerBuf;
			unsigned int numChannels;
			unsigned int channel;
			unsigned int maxMissedAttempts;
			unsigned int maxLengthInputQueue;
			mutable std::mutex parameterMutex;

			std::thread threadReadAudioInput;
			mutable std::mutex threadMutex;
			std::atomic<bool> isTerminateThread = {false};
			boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
			std::function< void( const std::string& ) > runtimeErrorCallback;
			std::atomic<bool> isInit = {false};
		};
	}
}
/*@}*/



/**	@brief		Default constructor
*/
template <class T> Core::Audio::CAudioSignalReader<T>::CAudioSignalReader(void)
{
}



/**	@brief		Constructor
*	@param		device						Input device. It can be obtained from CAudioSignalReader<T>::GetAudioDevices. For default device set it to an empty name.
*	@param		samplingFreq				Sampling frequency for capturing the audio signal [Hz]
*	@param 		samplesPerBuf				Number of samples received in one reading / writing cycle
*	@param		numChannels					Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param		channel						Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'
*	@param		maxMissedAttempts			Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param		maxLengthInputQueue			Maximum senseful input queue length
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@exception	std::logic_error			Thrown if the input device is not ready or the samplingFreq is negative
*	@exception	std::length_error			Thrown if the channel number is larger than the number of channels
*	@remarks 								The set parameters can be reset by using CAudioSignalReader<T>::SetParameters
*/
template <class T> Core::Audio::CAudioSignalReader<T>::CAudioSignalReader(const Processing::CAudioDevice& device, const double& samplingFreq, const unsigned long& samplesPerBuf, const unsigned int& numChannels, const unsigned int& channel, const unsigned int& maxMissedAttempts, const unsigned int& maxLengthInputQueue, std::function<void(const std::string&)> runtimeErrorCallback)
{
	SetParameters( device, samplingFreq, samplesPerBuf, numChannels, channel, maxMissedAttempts, maxLengthInputQueue, runtimeErrorCallback );
}



/**	@brief		Setting the class parameters
*	@param		device						Input device. It can be obtained from CAudioSignalReader<T>::GetAudioDevices. For default device set it to an empty name.
*	@param		samplingFreq				Sampling frequency for capturing the audio signal [Hz]
*	@param 		samplesPerBuf				Number of samples received in one reading / writing cycle
*	@param		numChannels					Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param		channel						Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'
*	@param		maxMissedAttempts			Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param		maxLengthInputQueue			Maximum senseful input queue length
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception	std::logic_error			Thrown if the input device is not ready or the samplingFreq is negative
*	@exception	std::length_error			Thrown if the channel number is larger than the number of channels
*	@exception	std::runtime_error			Thrown if the object is in use (i.e. the thread is running) and the parameters cannot be changed
*	@remarks 								This function can safely be called repeatedly for resetting the class. If the audio capture is running it will be stopped but not restarted.
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::SetParameters(const Processing::CAudioDevice& device, const double& samplingFreq, const unsigned long& samplesPerBuf, const unsigned int& numChannels, const unsigned int& channel, const unsigned int& maxMissedAttempts, const unsigned int& maxLengthInputQueue, std::function<void(const std::string&)> runtimeErrorCallback)
{
	using namespace std;

	// check input parameters
	if ( samplingFreq < 0 ) {
		throw std::logic_error( "The sampling frequency must not be negative." );
	}
	if ( channel > numChannels ) {
		throw std::length_error( "The channel must not be larger than the total number of channels." );
	}

	// stop thread if it is running
	StopReading();

	// lock any changes in the parameter set
	if ( parameterMutex.try_lock() ) {
		lock_guard<mutex> lock( parameterMutex, std::adopt_lock );
		// set parameters
		CAudioSignalReader<T>::samplingFreq = samplingFreq;
		CAudioSignalReader<T>::device = device;
		CAudioSignalReader<T>::samplesPerBuf = samplesPerBuf;
		CAudioSignalReader<T>::numChannels = numChannels;
		CAudioSignalReader<T>::channel = channel;
		CAudioSignalReader<T>::maxMissedAttempts = maxMissedAttempts;
		CAudioSignalReader<T>::maxLengthInputQueue = maxLengthInputQueue;
		CAudioSignalReader<T>::runtimeErrorCallback = runtimeErrorCallback;

		// initialize signaling of audio errors
		if ( isInit ) {
			runtimeErrorSignal.disconnect_all_slots();
		}
		runtimeErrorSignal.connect( runtimeErrorCallback );

		// initialize audio signal capture
		portaudio.Start( device, samplingFreq, samplesPerBuf, numChannels ); // throws std::logic_error if the input device is not available
		isInit = true;
	} else {
		throw std::runtime_error( "Object is in use and the parameters cannot be changed." );
	}
}



/**	@brief		Getting the class parameters
*	@param		device						Input device. It can be obtained from CAudioSignalReader<T>::GetAudioDevices. For default device set it to an empty name.
*	@param		samplingFreq				Sampling frequency for capturing the audio signal [Hz]
*	@param 		samplesPerBuf				Number of samples received in one reading / writing cycle
*	@param		numChannels					Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param		channel						Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'.
*	@param		maxMissedAttempts			Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param		maxLengthInputQueue			Maximum senseful input queue length
*	@param		runtimeErrorCallback		Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::GetParameters(Processing::CAudioDevice& device, double& samplingFreq, unsigned long& samplesPerBuf, unsigned int& numChannels, unsigned int& channel, unsigned int& maxMissedAttempts, unsigned int& maxLengthInputQueue, std::function<void(const std::string&)>& runtimeErrorCallback) const
{
	// lock any changes in the parameter set
	std::lock_guard<std::mutex> lock( parameterMutex );

	device = CAudioSignalReader<T>::device;
	samplingFreq = CAudioSignalReader<T>::samplingFreq;
	samplesPerBuf = CAudioSignalReader<T>::samplesPerBuf;
	numChannels = CAudioSignalReader<T>::numChannels;
	channel = CAudioSignalReader<T>::channel;
	maxMissedAttempts = CAudioSignalReader<T>::maxMissedAttempts;
	maxLengthInputQueue = CAudioSignalReader<T>::maxLengthInputQueue;
	runtimeErrorCallback = CAudioSignalReader<T>::runtimeErrorCallback;
}



/**	@brief		Destructor
*/
template <class T> Core::Audio::CAudioSignalReader<T>::~CAudioSignalReader(void)
{
	StopReading();
}



/**	@brief		Starts audio capturing
*	@return 								None
*	@exception 	std::logic_error			Thrown if the audio capturing is already running
*	@remarks 								Audio capture will be stopped if CAudioSignalReader<T>::StopReading() is called or the object ist destructed
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::StartReading(void)
{
	std::lock_guard<std::mutex> lock( threadMutex );
	if ( threadReadAudioInput.joinable() ) {
		throw std::logic_error( "The audio capturing is already running." );
	}

	isTerminateThread = false;
	threadReadAudioInput = std::thread( [&]() { ReadAudioInputThread(); } );
}



/**	@brief		Stops audio capturing
*	@return 								None
*	@exception 								None
*	@remarks 								None
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::StopReading(void)
{
	std::lock_guard<std::mutex> lock( threadMutex );
	if ( threadReadAudioInput.joinable() ) {
		isTerminateThread = true;
		threadReadAudioInput.join();
	}
}



/**	@brief		Checks if the audio capturing is running
*	@return 								True if the audio capturing is running, false otherwise
*	@exception 								None
*	@remarks 								None
*/
template <class T> bool Core::Audio::CAudioSignalReader<T>::IsReading(void) const
{
	std::lock_guard<std::mutex> lock( threadMutex );
	return threadReadAudioInput.joinable();
}



/**	@brief		Passing the captured audio signal data stream
*	@param		timeFirst					Iterator to the beginning of the time data container corresponding to the signal data container after the function call. It must be of the correct size or std::back_inserter must be used.
*	@param		signalFirst					Iterator to the beginning of the signal data container after the function call. It must be of the correct size or std::back_inserter must be used.
*	@return 								Iterator to one element after the end of the time data container
*	@exception 								None
*	@remarks 								None
*/
template <class T> template <class OutIt1, class OutIt2> void Core::Audio::CAudioSignalReader<T>::GetSignalData(OutIt1 timeFirst, OutIt2 signalFirst)
{
	using namespace std;
	vector< boost::posix_time::ptime > currInputTime;
	vector<T> currInputSignal;

	// copy to local data in order to reduce lock time
	{
		lock_guard<timed_mutex> lock( inputSignalMutex );
		if ( !inputTime.empty() ) {
			currInputTime.assign( std::make_move_iterator( inputTime.begin() ), std::make_move_iterator( inputTime.end() ) );
			inputTime.clear();
			currInputSignal.assign( std::make_move_iterator( inputSignal.begin() ), std::make_move_iterator( inputSignal.end() ) );
			inputSignal.clear();
		}
	}

	// return results
	std::move( currInputTime.begin(), currInputTime.end(), timeFirst );
	std::move( currInputSignal.begin(), currInputSignal.end(), signalFirst );
}



/**	@brief		Function containing the thread continously recording the signal data from the audio device
*	@return 						None
*	@exception 	std::runtime_error	Thrown if the parameters of the object were not set properly before using the function
*	@remarks 						None
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::ReadAudioInputThread(void)
{
	using namespace boost::posix_time;
	using namespace std;

	vector<ptime> currInputTime, newInputTime;
	vector<T> newInputSignal;
	unsigned int missedAttempts = 0;
	time_duration deltaT;
	ptime time;
	size_t oldSize;

	try {
		if ( !isInit ) {
			throw std::runtime_error( "The object was not initialized before use!" );
		}

		// lock any changes in the parameter set
		lock_guard<mutex> lock( parameterMutex );

		// record audio data until interruption is requested
		deltaT = microseconds( static_cast<long>( 1.0e6 / samplingFreq ) );

		while ( true ) {
			if ( isTerminateThread ) {
				break;
			}

			// read input signal
			oldSize = newInputSignal.size();
			newInputSignal.resize( oldSize + samplesPerBuf );
			time = ptime( microsec_clock::universal_time() );
			portaudio.ReadStream( newInputSignal.begin() + oldSize, newInputSignal.end(), channel );	
						
			// set reference time stamps
			currInputTime.resize( samplesPerBuf );
			for ( size_t i=0; i < currInputTime.size(); i++ ) {
				currInputTime[i] = time;
				time = time + deltaT;				
			}
			newInputTime.insert( newInputTime.end(), std::make_move_iterator( currInputTime.begin() ), std::make_move_iterator( currInputTime.end() ) );

			// try to synchronize
			if ( inputSignalMutex.try_lock() ) {
				lock_guard<timed_mutex> lockImmediately( inputSignalMutex, adopt_lock );
				missedAttempts = 0;
				// write to signal queue
				UpdateSignalQueue( newInputTime.begin(), newInputTime.end(), newInputSignal.begin(), newInputSignal.end() );
				newInputTime.clear();
				newInputSignal.clear();
			} else {
				missedAttempts++;
				if ( missedAttempts >= maxMissedAttempts ) { // force synchronization		
					lock_guard<timed_mutex> lockForce( inputSignalMutex );
					missedAttempts = 0;
					// write to signal queue and store current time
					UpdateSignalQueue( newInputTime.begin(), newInputTime.end(), newInputSignal.begin(), newInputSignal.end() );
					newInputTime.clear();
					newInputSignal.clear();
				}
			}
		}
	} catch (std::exception& e) {
		// signal to calling thread that an error occured and the thread was finished abnormally
		runtimeErrorSignal( e.what() );
	}
}



/**	@brief		Update the input signal queue in the class
*	@param		timeFirst			Iterator to the beginning of the container with the input times for the queue corresponding to 'signal'
*	@param		timeLast			Iterator to one element after the end of of the container with the input times for the queue corresponding to 'signal'
*	@param		signalFirst			Iterator to the beginning of the container with the new input signal for the queue
*	@param		signalLast			Iterator to one element after the end of the container with the new input signal for the queue
*	@return							None
*	@exception	std::overflow_error	Thrown if the queue gets longer than constant CAudioSignalReader<T>::maxLengthInputQueue due to too slow audio processing
*	@remarks						The function itself is not thread-safe
*/
template <class T> template <class InIt1, class InIt2> void Core::Audio::CAudioSignalReader<T>::UpdateSignalQueue(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst, InIt2 signalLast)
{
	// update signal queue
	inputTime.insert( inputTime.end(), timeFirst, timeLast );
	inputSignal.insert( inputSignal.end(), signalFirst, signalLast );

	// check length of signal queue
	if ( static_cast<int>( inputSignal.size() ) > maxLengthInputQueue ) {
		throw std::overflow_error("Signal processing is not fast enough! Data was lost!");
	}
}



/**	@brief		Obtains the names of all available input devices - independently from currently running audio streams
*	@param		inputDevices			Container with all available input devices
*	@param		stdInputDevice			Standard input device	
*	@param		samplingFreq			Required sampling frequency for capturing the audio signal [Hz]
*	@param		numChannels				Required number of channels for the audio input (mono = 1, stereo = 2)
*	@return								None
*	@exception							None
*	@remarks							This method can be used before initializing the object and does not interfer with any running audio stream
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::GetAudioDevices( std::vector<Processing::CAudioDevice>& inputDevices, Processing::CAudioDevice& stdInputDevice, const double& samplingFreq, const unsigned int& numChannels ) const
{
	// only input devices are relevant for this application
	portaudio.GetAvailableInputDevices( inputDevices, stdInputDevice, samplingFreq, numChannels );
}



/**	@brief		Resets the input device
*	@param		device				Chosen input device. It can be obtained from CAudioSignalReader<T>::GetAudioDevices. For default device set it to an empty name.
*	@return							None
*	@exception	std::runtime_error	Thrown if this function is called before initializing the object, or if the object is in use (i.e. the thread is running) and the parameters cannot be changed.
*	@remarks						This method restarts an already running stream
*/
template <class T> void Core::Audio::CAudioSignalReader<T>::SetAudioDevice(const Processing::CAudioDevice& device)
{
	Processing::CAudioDevice dummy;
	unsigned long samplesPerBuf;
	double samplingFreq;
	unsigned int numChannels, channel, maxMissedAttempts, maxLengthInputQueue;
	std::function< void( const std::string& ) > runtimeErrorCallback;

	if ( !isInit ) {
		throw std::runtime_error( "The object was not initialized before use!" );
	}

	// save changed device index
	GetParameters( dummy, samplingFreq, samplesPerBuf, numChannels, channel, maxMissedAttempts, maxLengthInputQueue, runtimeErrorCallback );
	SetParameters( device, samplingFreq, samplesPerBuf, numChannels, channel, maxMissedAttempts, maxLengthInputQueue, runtimeErrorCallback );
}



/**	@brief		Determines if the given input device is available on the current system for the given parameters - independently from currently running audio streams
*	@param		device							Input device to be checked
*	@param		samplingFreq					Required sampling frequency for capturing the audio signal [Hz]
*	@param		numChannels						Required number of channels for the audio input (mono = 1, stereo = 2)
*	@return										True if the device is available, false otherwise
*	@exception									None
*	@remarks									This method can be used before initializing the object and does not interfer with any running audio stream
*/
template <class T> bool Core::Audio::CAudioSignalReader<T>::IsDeviceAvailable( const Core::Processing::CAudioDevice& device, const double& samplingFreq, const int& numChannels ) const
{
	return portaudio.IsDeviceAvailable( device, samplingFreq, numChannels );
}
