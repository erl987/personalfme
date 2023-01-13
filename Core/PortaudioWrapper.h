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
#include <string>
#include <typeinfo>
#include <limits>
#include <map>
#include <mutex>
#include <type_traits>
#include <boost/algorithm/string.hpp>
#include "AudioDevice.h"
#include "portaudio.h"
#ifdef __linux
	#include <pa_linux_alsa.h>
#endif
#include "ErrorStreamManipulator.h"
#include "publicAudioSPDefinitions.h"


/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace Processing {
		/** \ingroup Core
		*	Class for input and output of audio signals with high performance. The class is based on the Portaudio-library (www.portaudio.com). The class is thread-safe.
		*/
		template <class T>
		class CPortaudio
		{
			// ensures the correct template type
			static_assert( std::is_same<T, int>::value || std::is_same<T, float>::value || std::is_same<T, short>::value || std::is_same<T, char>::value || std::is_same<T, unsigned char>::value, "no supported type" );
		public:
			CPortaudio();
			~CPortaudio();
			void Start( const Processing::CAudioDevice& device, double samplingFreq, unsigned long samplesPerBuf, int numChannels );
			bool GetStatus( Processing::CAudioDevice& currActiveDevice, double& currSamplingFreq, unsigned long& currSamplesPerBuf, int& currNumChannels ) const;
			template <class ForwardIterator> unsigned long WriteStream( ForwardIterator first, ForwardIterator last );
			template <class ForwardIterator> unsigned long ReadStream( ForwardIterator first, ForwardIterator last, const unsigned int& channel );			
			
			void GetDeviceDefaults( double& defaultSamplingFreq, int& maxNumChannels, const Processing::CAudioDevice& device ) const;
			bool IsDeviceAvailable( const Processing::CAudioDevice& device, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			void GetAvailableInputDevices( std::vector<Processing::CAudioDevice>& inputDevices, Processing::CAudioDevice& stdInputDevice, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			void GetAvailableOutputDevices( std::vector<Processing::CAudioDevice>& outputDevices, Processing::CAudioDevice& stdOutputDevice, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			static void GetMinMaxAmplitude( T& minAmpl, T& maxAmpl );
		private:
			void GetAvailableInputDevices( std::map<PaDeviceIndex, Processing::CAudioDevice>& inputDevices, std::pair<PaDeviceIndex, Processing::CAudioDevice>& stdInputDevice, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			void GetAvailableOutputDevices( std::map<PaDeviceIndex, Processing::CAudioDevice>& outputDevices, std::pair<PaDeviceIndex, Processing::CAudioDevice>& stdOutputDevice, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			PaDeviceIndex GetDeviceIndex( const Processing::CAudioDevice& device, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			PaStreamParameters DefineIOParams( const PaDeviceIndex& deviceIndex, const AudioDeviceType& deviceType, const int& numChannels ) const;
			bool IsDeviceAvailable( const PaDeviceIndex& deviceIndex, const AudioDeviceType& deviceType, const double& samplingFreq = 0, const int& numChannels = 0 ) const;
			void CheckForError( PaError error ) const;			
			static PaSampleFormat GetSampleFormat();

			CPortaudio( const CPortaudio & ) = delete;
    		CPortaudio & operator= (const CPortaudio &) = delete;
		
			PaStream* activeStream;
			Processing::CAudioDevice activeDevice;
			unsigned long activeSamplesPerBuf;
			unsigned int activeNumChannels;
			std::vector<T> buffer;
			mutable std::mutex portaudioMutex;
		};
	}

	namespace Exception {
		/**	This class provides exceptions for Portaudio inherited from std::exception
		*/
		class portaudioException : public std::runtime_error
		{
		public:
			/**	@brief		Provides information on the thrown exception
			*	@return		Information on the Portaudio error
			*	@remarks	See http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html for details on portaudio errors
			*/
			portaudioException( PaError error )
			: std::runtime_error( "Portaudio error: " + std::string( Pa_GetErrorText( error ) ) )
			{}
		};

		
		
		/**	This class provides a device-not-ready exception for Portaudio inherited from std::exception
		*/
		class audioDeviceNotReadyException : public std::runtime_error
		{
		public:
			/**	@brief		Provides information on the thrown exception
			*	@return		Information on the device not available
			*	@remarks	See http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html for details on portaudio errors
			*/
			virtual const char* what()
  			{
				char* errorInput = const_cast<char*>( "No input device is available." );
				char* errorOutput = const_cast<char*>( "No output device is available." );
				
				if ( inputDevice ) {
					return errorInput;
				} else {
					return errorOutput;
				}
  			}
			/**	@brief		Returns the type of the invalid device
			*	@return		Set to true if the input device is not available and to false if the output device is not available
			*	@remarks	None
			*/
			bool deviceType() {
				return inputDevice;
			}
			audioDeviceNotReadyException( bool inputDevice )
			: runtime_error(""), inputDevice( inputDevice ) {}
		private:
			bool inputDevice; // true if the input device is not ready and false if the output device is not ready
		};
	}
}

/*@}*/


/**	@brief 	Standard constructor.
*/
template <class T>
Core::Processing::CPortaudio<T>::CPortaudio(void)
	: activeStream(nullptr),
	  activeSamplesPerBuf(0),
	  activeNumChannels(0)
{
	// under Linux the frequent misleading warnings due to the ALSA-configuration are suppressed
	// under Windows debug output of the Portaudio library is suppressed
	Helper::CErrorStreamManipulator closeStream;

	// initalize Portaudio
	CheckForError( Pa_Initialize() );	// in case of an initialization error an exception will be thrown (never calling the destructor)
}



/** @brief	Destructor.
*			Automatic release of all resources.
*/
template <class T>
Core::Processing::CPortaudio<T>::~CPortaudio(void)
{
	// under Windows debug output of the Portaudio library is suppressed
	Helper::CErrorStreamManipulator closeStream;

	// savely terminate Portaudio, also stopping all streams (never called when Pa_Initalize has failed)
	Pa_Terminate();
}



/** @brief		Setting the sample format of the class			
*	@return										Portaudio sample format corresponding to the template type
*	@exception									None
*	@remarks									A static assert check is performed in the class to ensure that the class template parameter (datatype for audio recording) is one of the following types: (int, float, short, char, unsigned char).
*												None							
*/
template <class T>
PaSampleFormat Core::Processing::CPortaudio<T>::GetSampleFormat(void)
{
	using namespace std;
	PaSampleFormat sampleFormat = 0;

	if ( typeid( T ) == typeid( int ) ) {
		sampleFormat = paInt32;
	} else if ( typeid( T ) == typeid( float ) ) {
		sampleFormat = paFloat32;
	} else if ( typeid( T ) == typeid( short ) ) {
		sampleFormat = paInt16;
	} else if ( typeid( T ) == typeid( char ) ) {
		sampleFormat = paInt8;
	} else if ( typeid( T ) == typeid( unsigned char ) ) {
		sampleFormat = paUInt8;
	}

	return sampleFormat;
}



/** @brief		Setting the Portaudio-library structure PaStreamParameters
*	@param		deviceIndex						Index of the audio device. Can be obtained from CPortaudio::GetDeviceIndex.
*	@param		deviceType						Specifying if it is an input or output device
*	@param		numChannels						Number of audio channels (1 for mono, 2 for stereo)					
*	@return										Final PaStreamParameters-structure
*	@exception	std::runtime_error				Thrown if the device index is not representing a valid device
*	@remarks									This method can be called without influencing any active streams									
*/
template <class T>
PaStreamParameters Core::Processing::CPortaudio<T>::DefineIOParams( const PaDeviceIndex& deviceIndex, const AudioDeviceType& deviceType, const int& numChannels ) const
{
	PaStreamParameters ioParameters;

	if ( ( deviceIndex < 0 ) || ( deviceIndex >= Pa_GetDeviceCount() ) ) {
		throw std::runtime_error( "The device index is not valid" );
	}

	ioParameters.device = deviceIndex;
	ioParameters.channelCount = numChannels;
	ioParameters.sampleFormat = GetSampleFormat();
	if ( deviceType == IN_DEVICE ) {
		ioParameters.suggestedLatency = Pa_GetDeviceInfo( deviceIndex )->defaultLowInputLatency;
	} else {
		ioParameters.suggestedLatency = Pa_GetDeviceInfo( deviceIndex )->defaultLowOutputLatency;
	}
	ioParameters.hostApiSpecificStreamInfo = nullptr;

	return ioParameters;
}



/** @brief		Starts or restarts an active stream for audio input or output
*	@param		device							Chosen input or output device. The available devices can be obtained from CPortaudio::GetAvailableDevices. The the default device is an object with emtpy device name and driver.
*	@param		samplingFreq					Required sampling frequency [Hz]
*	@param		samplesPerBuf					Number of samples received in one reading / writing cycle
*	@param		numChannels						Required number of audio channels (1 for mono, 2 for stereo)
*	@return										None	
*	@exception	std::logic_error				Thrown if the chosen device is not available or if the specification is ambiguous
*	@exception	Exception::portaudioException	Thrown if starting of the chosen device failed
*	@remarks									Full-duplex input and output streams are not supported
*												It is guaranteed that in case of failure no stream is active after the method call and that the class member 'activeDevice' has been invalidated.
*/
template <class T>
void Core::Processing::CPortaudio<T>::Start( const Processing::CAudioDevice& device, double samplingFreq, unsigned long samplesPerBuf, int numChannels )
{
	PaDeviceIndex deviceIndex;
	PaError error;
	PaStreamParameters stream;
	PaStreamParameters *currInputStream = nullptr;
	PaStreamParameters *currOutputStream = nullptr;

	// ensure that there is no concurrent access to the stream
	std::lock_guard<std::mutex> lock( portaudioMutex );

	// under Windows debug output of the Portaudio library is suppressed
	Helper::CErrorStreamManipulator closeStream;
	
	// close an existing audio stream
	CPortaudio::activeDevice = CAudioDevice();
	if ( activeStream != nullptr ) {
		error = Pa_CloseStream( activeStream );
		CheckForError( error );
		activeStream = nullptr;
	}

	// check if the required device is available and set the stream parameters
	deviceIndex = GetDeviceIndex( device, samplingFreq, numChannels );
	if ( deviceIndex == paNoDevice ) {
		throw std::logic_error( "The device is not available or the device names are ambiguous." );
	}

	// open new audio stream
	stream = DefineIOParams( deviceIndex, device.GetType(), numChannels );
	if ( device.GetType() == IN_DEVICE ) {
		currInputStream = &stream;
	} else {
		currOutputStream = &stream;
	}
	error = Pa_OpenStream( &activeStream, currInputStream, currOutputStream, samplingFreq, samplesPerBuf, paClipOff, NULL, NULL );
	CheckForError( error );	
	if( !activeStream ) {
		throw Exception::portaudioException( error );
	}

	// set the audio threads to high priority (only for the ALSA-driver on Linux)
	#ifdef __linux
		PaAlsa_EnableRealtimeScheduling( activeStream, true );
	#endif

	// start new audio stream
	error = Pa_StartStream( activeStream );
	CheckForError( error );

	// set input / output buffer
	buffer.resize( samplesPerBuf * numChannels );

	// set the class members
	CPortaudio::activeSamplesPerBuf = samplesPerBuf;
	CPortaudio::activeNumChannels = numChannels;
	CPortaudio::activeDevice = device;
}



/**	@brief		Obtains the current status of the audio recording / playing
*	@param		currActiveDevice		Contains the currently active device. If no device is active, it is an invalidated audio device object.
*	@param		currSamplingFreq		Contains the sampling frequency of the currently active device [Hz]. It represents the real frequency and may deviate from the originally set one. If there is no stream active, it is set to zero.
*	@param		currSamplesPerBuf		Contains the number of samples received in one reading / writing cycle. It is equal to the value set during starting the stream.
*	@param		currNumChannels			Contains the number of available audio channels (1 for mono, 2 for stereo). It is equal to the value set during starting the stream.
*	@return								True if there is an active stream that allows recording or playing, false otherwise
*	@exception							None
*	@remarks							The type of the stream (recording / playing) can be obtained from the 'currActiveDevice' parameter using its CAudioDevice::GetType() method.
*										This method can be called safely without interfering with any active stream.
*/
template <class T>
bool Core::Processing::CPortaudio<T>::GetStatus( Processing::CAudioDevice& currActiveDevice, double& currSamplingFreq, unsigned long& currSamplesPerBuf, int& currNumChannels ) const
{
	bool isActive;

	std::lock_guard<std::mutex> lock( portaudioMutex );

	if ( Pa_IsStreamActive( activeStream ) <= 0 ) {
		currActiveDevice = CAudioDevice();
		currSamplingFreq = 0;
		currSamplesPerBuf = 0;
		currNumChannels = 0;
		isActive = false;
	} else {
		currActiveDevice = CPortaudio<T>::activeDevice;		
		currSamplingFreq = Pa_GetStreamInfo( activeStream )->sampleRate;
		currSamplesPerBuf = CPortaudio<T>::activeSamplesPerBuf;
		currNumChannels = CPortaudio<T>::activeNumChannels;
		isActive = true;
	}

	return isActive;
}



/** @brief		Error checker for Portaudio library functions.
*	@param		error							Portaudio error code
*	@return										None
*	@exception	Exception::portaudioException	Thrown for any Portaudio error
*	@remarks									The following Portaudio error codes are defined: 
*												see http://portaudio.com/docs/v19-doxydocs/portaudio_8h.html#2e45bf8b5145f131a91c128af2bdaec7
*/
template <class T>
void Core::Processing::CPortaudio<T>::CheckForError(PaError error) const
{
	if ( error != paNoError ) {
		throw Exception::portaudioException(error);
	}
}



/**	@brief		Sends audio signal to output on all available channels.
*	@param		first									Iterator to first element of output signal.
*	@param		last									Iterator to one element after the end of the output signal. Validity of this iterator cannot be checked automatically!
*	@exception	Exception::portaudioException			Thrown for any Portaudio error (see Exception::portaudioException for details)								
*	@exception	Exception::audioDeviceNotReadyException	Thrown if no audio output device is available
*	@return												Number of processed data samples
*	@remarks											A sample of length CPortaudio::SamplesPerBuf will be played. If output is
*														shorter, the rest will be zero-padded. If it is longer, only CPortaudio::SamplesPerBuf will be played. The data is not changed.
*/
template <class T>
template <class ForwardIterator>
unsigned long Core::Processing::CPortaudio<T>::WriteStream(ForwardIterator first, ForwardIterator last)
{
	PaError err;
	std::vector<T> data;
	unsigned long numProcessed = 0;

	std::unique_lock<std::mutex> lock( portaudioMutex );

	// check if output device is open
	if ( ( !activeDevice.IsSet() ) || ( activeDevice.GetType() != OUT_DEVICE ) ) {
		throw Exception::audioDeviceNotReadyException( false );
	}

	// convert data from queue
	data.assign( first, last );
	data.resize( activeSamplesPerBuf );
	for (unsigned long j=0; j < activeSamplesPerBuf; j++) {
		for (unsigned int k=0; k < activeNumChannels; k++) {
			buffer[activeNumChannels*j+k] = data[j]; // array for all channels, equivalent to "buffer[j][k]"
		}
	}
	
	if ( static_cast<unsigned long>( distance( first, last ) ) < activeSamplesPerBuf ) {
		numProcessed = distance( first, last );
	} else {
		numProcessed = activeSamplesPerBuf;
	}

	// write stream
    err = Pa_WriteStream( activeStream, buffer.data(), activeSamplesPerBuf );
	CheckForError( err );

	return numProcessed;
}



/**	@brief		Receives audio signal from input stream.
*	@param		first									Iterator of the first element of the input signal to be stored
*	@param		last									Iterator of the element after the last element of the input signal
*	@param		channel									Index of channel for reading the signal starting with 1, ...
*	@exception	Exception::portaudioException			Thrown for any Portaudio error (see Exception::portaudioException for details)								
*	@exception	std::runtime_error						Thrown if the channel index is invalid
*	@exception	Exception::audioDeviceNotReadyException	Thrown if no input device is available
*	@return												Number of stored elements
*	@remarks											A sample of length CPortaudio::activeSamplesPerBuf will be acquired. If the number of available elements 
*														is smaller, the rest will be skipped. If more elements are available, the rest will be zero-padded.
*/
template <class T>
template <class ForwardIterator>
unsigned long Core::Processing::CPortaudio<T>::ReadStream(ForwardIterator first, ForwardIterator last, const unsigned int& channel)
{
	PaError err;
	std::vector<T> newSignal;

	std::unique_lock<std::mutex> lock( portaudioMutex );

	if ( ( channel < 1 ) || ( channel > activeNumChannels ) ) {
		throw std::runtime_error( "The channel index is invalid." );
	}

	// check if input device is open
	if ( ( !activeDevice.IsSet() ) || ( activeDevice.GetType() != IN_DEVICE ) ) {
		throw Exception::audioDeviceNotReadyException( true );
	}
	 
	// read stream
    err = Pa_ReadStream( activeStream, buffer.data(), activeSamplesPerBuf );
    CheckForError( err );

	// convert data to queue from only one channel
	newSignal.resize( activeSamplesPerBuf );
	for (unsigned long j=0; j < activeSamplesPerBuf; j++) {
		newSignal[j] = buffer[activeNumChannels*j + channel - 1]; // array for all channels, equivalent to "buffer[j][channel]"
	}
	std::move( newSignal.begin(), newSignal.end(), first );
	
	return activeSamplesPerBuf;
}


/**	@brief		Obtains all available input devices.
*	@param		inputDevices					Containing all input devices
*	@param		stdInputDevice					Containing the default input device
*	@param		samplingFreq					Required supported sampling frequency of the devices [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the devices. Omit it if the maximum number of channels is required.
*	@exception									None
*	@return										None
*	@remarks									All devices including the default device are checked to be available for the required sampling frequency and number of channels.
*												The method can be called without interfering with any active stream.
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetAvailableInputDevices( std::vector<Processing::CAudioDevice>& inputDevices, Processing::CAudioDevice& stdInputDevice, const double& samplingFreq, const int& numChannels ) const
{
	std::map<PaDeviceIndex, Processing::CAudioDevice> inputDeviceMap;
	std::pair<PaDeviceIndex, Processing::CAudioDevice> stdInputDevicePair;

	GetAvailableInputDevices( inputDeviceMap, stdInputDevicePair, samplingFreq, numChannels );

	// remove the internal information on the Portaudio index
	inputDevices.clear();
	for ( auto inDevice : inputDeviceMap ) {
		inputDevices.push_back( inDevice.second );
	}

	stdInputDevice = stdInputDevicePair.second;
}



/**	@brief		Obtains all available output devices.
*	@param		outputDevices					Containing all output devices
*	@param		stdOutputDevice					Containing the default output device
*	@param		samplingFreq					Required supported sampling frequency of the devices [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the devices. Omit it if the maximum number of channels is required.
*	@exception									None
*	@return										None
*	@remarks									All devices including the default device are checked to be available for the required sampling frequency and number of channels
*												The method can be called without interfering with any active stream.
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetAvailableOutputDevices( std::vector<Processing::CAudioDevice>& outputDevices, Processing::CAudioDevice& stdOutputDevice, const double& samplingFreq, const int& numChannels ) const
{
	std::map<PaDeviceIndex, Processing::CAudioDevice> outputDeviceMap;
	std::pair<PaDeviceIndex, Processing::CAudioDevice> stdOutputDevicePair;

	GetAvailableOutputDevices( outputDeviceMap, stdOutputDevicePair, samplingFreq, numChannels );

	// remove the internal information on the Portaudio index
	outputDevices.clear();
	for ( auto outDevice : outputDeviceMap ) {
		outputDevices.push_back( outDevice.second );
	}

	stdOutputDevice = stdOutputDevicePair.second;
}



/**	@brief		Determines if the device is available with the specified parameters.
*	@param		deviceIndex						Portaudio device index
*	@param		samplingFreq					Required supported sampling frequency of the devices [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the devices. Omit it if the maximum number of channels is required.
*	@exception									None
*	@return										None
*	@remarks									It is checked if the device is available for the required sampling frequency and number of channels.
*												The method can be called without interfering with any active stream.
*/
template <class T>
bool Core::Processing::CPortaudio<T>::IsDeviceAvailable( const PaDeviceIndex& deviceIndex, const AudioDeviceType& deviceType, const double& samplingFreq, const int& numChannels ) const
{
	double requiredSamplingFreq;
	PaStreamParameters parameters;
	int maxNumChannels;
	bool isAvailable = false;

	if ( deviceType == IN_DEVICE ) {
		maxNumChannels = Pa_GetDeviceInfo( deviceIndex )->maxInputChannels;
	} else {
		maxNumChannels = Pa_GetDeviceInfo( deviceIndex )->maxOutputChannels;
	}

	if ( maxNumChannels > 0 ) {
		// check for validity of the device
		if ( numChannels == 0 ) {
			parameters = DefineIOParams( deviceIndex, deviceType, maxNumChannels );
		} else {
			parameters = DefineIOParams( deviceIndex, deviceType, numChannels );
		}
		if ( samplingFreq == 0 ) {
			requiredSamplingFreq = Pa_GetDeviceInfo( deviceIndex )->defaultSampleRate;
		} else {
			requiredSamplingFreq = samplingFreq;
		}
		parameters.device = deviceIndex;
		if ( deviceType == IN_DEVICE ) {
			if ( Pa_IsFormatSupported( &parameters, nullptr, requiredSamplingFreq ) == 0 ) {
				isAvailable = true;
			}
		} else {
			if ( Pa_IsFormatSupported( nullptr, &parameters, requiredSamplingFreq ) == 0 ) {
				isAvailable = true;
			}
		}
	}

	return isAvailable;
}


/**	@brief		Obtains all available input devices.
*	@param		inputDevices					Containing all input devices and their corresponding internal Portaudio device indices
*	@param		stdInputDevice					Containing the default input device and its correpesponding internal Portaudio device index
*	@param		samplingFreq					Required supported sampling frequency of the devices [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the devices. Omit it if the maximum number of channels is required.
*	@exception									None					
*	@return										None
*	@remarks									All devices including the default device are checked to be available for the required sampling frequency and number of channels.
*												The method can be called without interfering with any active stream.
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetAvailableInputDevices( std::map<PaDeviceIndex, Processing::CAudioDevice>& inputDevices, std::pair<PaDeviceIndex, Processing::CAudioDevice>& stdInputDevice, const double& samplingFreq, const int& numChannels ) const
{
	using namespace std;

	bool isStandardDeviceAvailable;
	string currDeviceName, currDriverName, stdDeviceName, stdDriverName;

	// under Linux the frequent misleading warnings due to the ALSA-configuration are suppressed
	// under Windows debug output of the Portaudio library is suppressed
	Helper::CErrorStreamManipulator closeStream;

	// obtain all devices
	inputDevices.clear();
	for ( PaDeviceIndex currDeviceIndex=0; currDeviceIndex < Pa_GetDeviceCount(); currDeviceIndex++ ) {
		if ( IsDeviceAvailable( currDeviceIndex, IN_DEVICE, samplingFreq, numChannels ) ) {
			currDeviceName = Pa_GetDeviceInfo( currDeviceIndex )->name;
			currDriverName = Pa_GetHostApiInfo( Pa_GetDeviceInfo( currDeviceIndex )->hostApi )->name;
			inputDevices.insert( make_pair( currDeviceIndex, Processing::CAudioDevice( IN_DEVICE, currDeviceName, currDriverName ) ) );
		}
	}
		
	// obtain standard device
	if ( ( Pa_GetDefaultInputDevice() != paNoDevice ) && ( Pa_GetDefaultHostApi() >= 0 ) ) {
		isStandardDeviceAvailable = IsDeviceAvailable( Pa_GetDefaultInputDevice(), IN_DEVICE, samplingFreq, numChannels );
	} else {
		isStandardDeviceAvailable = false;
	}

	if (isStandardDeviceAvailable ) {
		stdDeviceName = Pa_GetDeviceInfo( Pa_GetDefaultInputDevice() )->name;
		stdDriverName = Pa_GetHostApiInfo( Pa_GetDefaultHostApi() )->name;
		stdInputDevice = make_pair( Pa_GetDefaultInputDevice(), Processing::CAudioDevice( IN_DEVICE, stdDeviceName, stdDriverName ) );
	} else {
		stdInputDevice = make_pair( paNoDevice, Processing::CAudioDevice() );
	}
}



/**	@brief		Obtains all available output devices.
*	@param		outputDevices					Containing all output devices and their corresponding internal Portaudio device indices
*	@param		stdOutputDevice					Containing the default output device and its correpesponding internal Portaudio device index
*	@param		samplingFreq					Required supported sampling frequency of the devices [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the devices. Omit it if the maximum number of channels is required.
*	@exception									None
*	@return										None
*	@remarks									All devices including the default device are checked to be available for the required sampling frequency and number of channels.
*												The method can be called without interfering with any active stream.
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetAvailableOutputDevices( std::map<PaDeviceIndex, Processing::CAudioDevice>& outputDevices, std::pair<PaDeviceIndex, Processing::CAudioDevice>& stdOutputDevice, const double& samplingFreq, const int& numChannels ) const
{
	using namespace std;

	bool isStandardDeviceAvailable;
	string currDeviceName, currDriverName, stdDeviceName, stdDriverName;

	// under Linux the frequent misleading warnings due to the ALSA-configuration are suppressed
	// under Windows debug output of the Portaudio library is suppressed
	Helper::CErrorStreamManipulator closeStream;

	// obtain all devices
	outputDevices.clear();
	for ( PaDeviceIndex currDeviceIndex = 0; currDeviceIndex < Pa_GetDeviceCount(); currDeviceIndex++ ) {
		if ( IsDeviceAvailable( currDeviceIndex, OUT_DEVICE, samplingFreq, numChannels ) ) {
			currDeviceName = Pa_GetDeviceInfo( currDeviceIndex )->name;
			currDriverName = Pa_GetHostApiInfo( Pa_GetDeviceInfo( currDeviceIndex )->hostApi )->name;
			outputDevices.insert( make_pair( currDeviceIndex, Processing::CAudioDevice( OUT_DEVICE, currDeviceName, currDriverName ) ) );
		}
	}

	// obtain standard device
	if ( ( Pa_GetDefaultOutputDevice() != paNoDevice ) && ( Pa_GetDefaultHostApi() >= 0 ) ) {
		isStandardDeviceAvailable = IsDeviceAvailable( Pa_GetDefaultOutputDevice(), OUT_DEVICE, samplingFreq, numChannels );
	} else {
		isStandardDeviceAvailable = false;
	}

	if ( isStandardDeviceAvailable ) {
		stdDeviceName = Pa_GetDeviceInfo( Pa_GetDefaultOutputDevice() )->name;
		stdDriverName = Pa_GetHostApiInfo( Pa_GetDefaultHostApi() )->name;
		stdOutputDevice = make_pair( Pa_GetDefaultOutputDevice(), Processing::CAudioDevice( OUT_DEVICE, stdDeviceName, stdDriverName ) );
	} else {
		stdOutputDevice = make_pair( paNoDevice, Processing::CAudioDevice() );
	}
}



/**	@brief		Obtains the default sampling rate and maximum number of channels for a device
*	@param		defaultSamplingFreq				Default sampling frequency of the device [Hz]
*	@param		maxNumChannels					Maximum number of channels of the device (1: mono, 2: stereo)
*	@param		device							The device for which the information is required
*	@exception	std::runtime_error				Thrown if the device is not available						
*	@return										None
*	@remarks									This method can be called without interfering with any active stream
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetDeviceDefaults( double& defaultSamplingFreq, int& maxNumChannels, const Processing::CAudioDevice& device ) const
{
	PaDeviceIndex deviceIndex;

	deviceIndex = GetDeviceIndex( device );

	if ( deviceIndex == paNoDevice ) {
		throw std::runtime_error( "The device " + device.GetDeviceName() + " is not available." );
	}

	defaultSamplingFreq = Pa_GetDeviceInfo( deviceIndex )->defaultSampleRate;
	if ( device.GetType() == Core::IN_DEVICE ) {
		maxNumChannels = Pa_GetDeviceInfo( deviceIndex )->maxInputChannels;
	} else {
		maxNumChannels = Pa_GetDeviceInfo( deviceIndex )->maxOutputChannels;
	}
}



/**	@brief		Returns the minimum and maximum amplitude for the current data type
*	@param		minAmpl					Minimum amplitude
*	@param		maxAmpl					Maximum amplitude
*	@exception							None								
*	@return								None
*	@remarks							The amplitudes are not symmetric for all data types
*/
template <class T>
void Core::Processing::CPortaudio<T>::GetMinMaxAmplitude(T& minAmpl, T& maxAmpl)
{
	using namespace std;

	// set minimum and maximum amplitude
	if  ( typeid( T ) == typeid( float ) ) { // paFloat32
		minAmpl = -1.0f;
		maxAmpl = 1.0f;
	} else {
		minAmpl = numeric_limits<T>::min();
		maxAmpl = numeric_limits<T>::max();
	}
}



/** @brief		Determines the internal Portaudio device index of an audio device
*	@param		device							Audio device (can be input or output) for which the internal Portaudio device index is required
*	@param		samplingFreq					Required supported sampling frequency of the device [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the device. Omit it if the maximum number of channels is required.
*	@return										Internal Portaudio device index. paNoDevice (-1) is returned if the device was not found
*	@exception	std::logic_error				Thrown if the specification of the device name and driver is ambiguous within the system
*	@remarks									It is sufficient to specify the first unambiguous part of the device and driver name. The comparison is lower-case.
*												The availability of the device may depend on the sampling rate and number of channels, therefore they have to be specified.
*												The method can be called without interfering with any active streams.
*/
template <class T>
PaDeviceIndex Core::Processing::CPortaudio<T>::GetDeviceIndex( const Processing::CAudioDevice& device, const double& samplingFreq, const int& numChannels ) const
{
	using namespace std;

	PaDeviceIndex deviceIndex;
	vector<PaDeviceIndex> possibleDeviceIndices;
	string currDeviceName, requiredDeviceName, currDriverName, requiredDriverName;
	map<PaDeviceIndex, Processing::CAudioDevice> availableDevices;
	pair<PaDeviceIndex, Processing::CAudioDevice> stdDevice;

	deviceIndex = paNoDevice;

	if ( device.IsSet() ) {
		// obtain all available devices
		if ( device.GetType() == Core::IN_DEVICE ) {
			GetAvailableInputDevices( availableDevices, stdDevice, samplingFreq, numChannels );
		} else {
			GetAvailableOutputDevices( availableDevices, stdDevice, samplingFreq, numChannels );
		}

		requiredDeviceName = device.GetDeviceName();
		boost::algorithm::to_lower( requiredDeviceName );
		requiredDriverName = device.GetDriverName();

		// determine the device index from the name
		if ( !requiredDeviceName.empty() ) {
			if ( requiredDriverName.empty() ) {
				if ( Pa_GetDefaultHostApi() >= 0 ) {
					requiredDriverName = Pa_GetHostApiInfo( Pa_GetDefaultHostApi() )->name;
				} else {
					return paNoDevice;
				}
			}
			boost::algorithm::to_lower( requiredDriverName );

			// non-default device
			for ( auto currDevice : availableDevices ) {
				// check if the current device and driver name are starting with the required names (allowing for specifying only a unique first part of the names)
				currDeviceName = currDevice.second.GetDeviceName();
				boost::algorithm::to_lower( currDeviceName );
				
				currDriverName = currDevice.second.GetDriverName();
				boost::algorithm::to_lower( currDriverName );

				if ( ( currDeviceName.find( requiredDeviceName ) == 0 ) && ( currDriverName.find( requiredDriverName ) == 0 ) ) {
					possibleDeviceIndices.push_back( currDevice.first );
				}
			}
			// check for uniqueness of the given device name
			if ( possibleDeviceIndices.size() == 1 ) {
				deviceIndex = possibleDeviceIndices.front();
			} else if ( possibleDeviceIndices.size() > 1 ) {
				throw std::logic_error( "The specification of device name and driver is not unique within the system." );
			}
		} else {
			// default device
			deviceIndex = stdDevice.first;
		}
	}

	return deviceIndex;
}



/** @brief		Determines the if a certain device is available for the required sample rate and number of channels
*	@param		device							Audio device (can be input or output) for which the availability is checked
*	@param		samplingFreq					Required supported sampling frequency of the device [Hz]. Omit it if the default sampling frequency is required.
*	@param		numChannels						Required supported number of channels of the device. Omit it if the maximum number of channels is required.
*	@return										True if the device is available, false otherwise
*	@exception	std::logic_error				Thrown if the specification of the device name and driver is ambiguous within the system
*	@remarks									It is sufficient to specify the first unambiguous part of the device and driver name. The comparison is lower-case.
*												The availability of the device may depend on the sampling rate and number of channels, therefore they have to be specified.
*												The method can be called without interfering with any active streams.
*/
template <class T>
bool Core::Processing::CPortaudio<T>::IsDeviceAvailable( const Processing::CAudioDevice& device, const double& samplingFreq, const int& numChannels ) const
{
	if ( GetDeviceIndex( device, samplingFreq, numChannels ) != paNoDevice ) {
		return true;
	} else {
		return false;
	}
}