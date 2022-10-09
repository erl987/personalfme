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
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "AudioSignalReader.h"

using boost::unit_test::label;


/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

/**	\defgroup	AudioSignalReader	Unit test for the class CAudioSignalReader<T>
*/

namespace Audio {
	/*@{*/
	/** \ingroup AudioSignalReader
	*/
	namespace CAudioSignalReader {
		struct Params {
			Core::Processing::CAudioDevice device;
			double samplingFreq;
			unsigned long samplesPerBuf;
			unsigned int numChannels;
			unsigned int channel;
			unsigned int maxMissedAttempts;
			unsigned int maxLengthInputQueue;
		};

		const Params inParams = { Core::Processing::CAudioDevice( Core::IN_DEVICE ), 44100, 2205, 1, 1, 10, 100000 };

		void RuntimeErrorCallback(const std::string&)
		{
		}

		// test section
		BOOST_AUTO_TEST_SUITE( CAudioSignalReader_test_suite, *label("audio") );

		/**	@brief		Test of set and get functions
		*/
		BOOST_AUTO_TEST_CASE( set_get_test_case )
		{
			Params outParams;
			std::function< void( const std::string& ) > runtimeErrorCallback;
			Core::Audio::CAudioSignalReader<float> signalReader;

			// test without audio error callback function
			signalReader.SetParameters( inParams.device, inParams.samplingFreq, inParams.samplesPerBuf, inParams.numChannels, inParams.channel, inParams.maxMissedAttempts, inParams.maxLengthInputQueue, runtimeErrorCallback );
		
			// test with audio error callback function		
			signalReader.SetParameters( inParams.device, inParams.samplingFreq, inParams.samplesPerBuf, inParams.numChannels, inParams.channel, inParams.maxMissedAttempts, inParams.maxLengthInputQueue, RuntimeErrorCallback );
		
			// resetting test
			signalReader.GetParameters( outParams.device, outParams.samplingFreq, outParams.samplesPerBuf, outParams.numChannels, outParams.channel, outParams.maxMissedAttempts, outParams.maxLengthInputQueue, runtimeErrorCallback );
			signalReader.SetParameters( outParams.device, outParams.samplingFreq, outParams.samplesPerBuf, outParams.numChannels, outParams.channel, outParams.maxMissedAttempts, outParams.maxLengthInputQueue, runtimeErrorCallback );

			BOOST_REQUIRE( inParams.device == outParams.device );
			BOOST_REQUIRE( inParams.samplingFreq == outParams.samplingFreq );
			BOOST_REQUIRE( inParams.samplesPerBuf == outParams.samplesPerBuf );
			BOOST_REQUIRE( inParams.numChannels == outParams.numChannels );
			BOOST_REQUIRE( inParams.channel == outParams.channel );
			BOOST_REQUIRE( inParams.maxMissedAttempts == outParams.maxMissedAttempts );
			BOOST_REQUIRE( inParams.maxLengthInputQueue == outParams.maxLengthInputQueue );
		}

	
	
		/** @brief		Test of capturing the signal for the audio stream
		*/
		BOOST_AUTO_TEST_CASE( get_signal_data_test_case )
		{
			using namespace boost::posix_time;
			using namespace std;
			vector<ptime> time;
			vector<float> signal;
			auto delayTime = 1s;

			Core::Audio::CAudioSignalReader<float> signalReader;
			signalReader.SetParameters( inParams.device, inParams.samplingFreq, inParams.samplesPerBuf, inParams.numChannels, inParams.channel, inParams.maxMissedAttempts, inParams.maxLengthInputQueue, std::function< void(const std::string&) >() );
			signalReader.StartReading();

			// delay thread
			std::this_thread::sleep_for( delayTime );

			signalReader.GetSignalData( back_inserter( time ), back_inserter( signal ) );

			BOOST_REQUIRE( time.size() > 0 );
			BOOST_REQUIRE( time.size() == signal.size() );
		}



		/**	@brief		Test of getting and setting audio devices
		*/
		BOOST_AUTO_TEST_CASE( get_set_audio_devices_test_case )
		{
			double samplingFreq = 96000.0; // in Hz
			unsigned int numChannels = 1;
			std::vector<Core::Processing::CAudioDevice> devices;
			Core::Processing::CAudioDevice stdDevice;
			std::function< void(const std::string&) > runtimeErrorCallback;

			Core::Audio::CAudioSignalReader<float> signalReader( inParams.device, inParams.samplingFreq, inParams.samplesPerBuf, inParams.numChannels, inParams.channel, inParams.maxMissedAttempts, inParams.maxLengthInputQueue, runtimeErrorCallback );
			signalReader.GetAudioDevices( devices, stdDevice, samplingFreq, numChannels );
		
			// resetting test
			signalReader.SetAudioDevice( devices.front() );
			signalReader.GetAudioDevices( devices, stdDevice, samplingFreq, numChannels );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
