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
/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

#include <iostream>
#include <deque>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "PortaudioWrapper.h"
#include "ProduceFMECode.h"
#include "PluginLoader.h"
#include "AudioPlugin.h"
#include "basicFunctions.h"

using boost::unit_test::label;


/**	\defgroup	PortaudioTests	Unit tests for the Portaudio-library.
*/

/*@{*/
/** \ingroup PortaudioTests
*/
namespace PortaudioTests {
	/** @brief		Saving a parameter file for generating FME-tone sequences
	*	@param		samplingFreq			Sampling frequency [Hz]
	*	@param		toneFreqs				Vector storing the tone frequencies corresponding to the different tones
	*	@param		fmeParamFileName		Name of the parameter file to be stored
	*	@param		biasParamsFileName		Name of the file storing the bias b-filter parameters
	*	@return								None
	*	@exception							None
	*	@remarks							None
	*/
	void WriteFMEParamFile(double samplingFreq, std::vector<float> toneFreqs, std::string fmeParamFileName, std::string biasParamsFileName)
	{
		Core::FME::CFMEGenerateParam fmeParams;
		std::vector<float> biasFilterParams;

		LoadVector( biasParamsFileName, biasFilterParams );
		fmeParams.Set( samplingFreq, 2.50, 0.07, 0.60, 5, toneFreqs.begin(), toneFreqs.end(), biasFilterParams.begin(), biasFilterParams.end() );
		Core::FME::CProduceFMECode<float> codeGenerator;
		codeGenerator.SaveParameters( fmeParamFileName, fmeParams );
	}
	


	// Test section
	BOOST_AUTO_TEST_SUITE( Portaudio_test_suite, *label("audio") );

	BOOST_AUTO_TEST_CASE( portaudio_available_devices_test_case )
	{
		using namespace Core::Processing;

		int numChannels = 2;
		double sampleLength = 0.05;
		double samplingFreq = 48'000.0;

		int maxNumChannels;
		float minAmpl, maxAmpl;
		double defaultSamplingFreq;
		std::vector<Core::Processing::CAudioDevice> availableInputDevices, availableOutputDevices;
		Core::Processing::CAudioDevice defaultInputDevice, defaultOutputDevice;
		Core::Processing::CPortaudio<float> portaudio;

		// test input
		portaudio.GetAvailableInputDevices( availableInputDevices, defaultInputDevice, samplingFreq, numChannels );
		BOOST_REQUIRE( !availableInputDevices.empty() );
		portaudio.GetAvailableInputDevices( availableInputDevices, defaultInputDevice, samplingFreq );
		BOOST_REQUIRE( !availableInputDevices.empty() );
		portaudio.GetAvailableInputDevices( availableInputDevices, defaultInputDevice );
		BOOST_REQUIRE( !availableInputDevices.empty() );

		portaudio.GetDeviceDefaults( defaultSamplingFreq, maxNumChannels, defaultInputDevice );

		// test output
		portaudio.GetAvailableOutputDevices( availableOutputDevices, defaultOutputDevice, samplingFreq, numChannels );
		BOOST_REQUIRE( !availableOutputDevices.empty() );
		portaudio.GetAvailableOutputDevices( availableOutputDevices, defaultOutputDevice, samplingFreq );
		BOOST_REQUIRE( !availableOutputDevices.empty() );
		portaudio.GetAvailableOutputDevices( availableOutputDevices, defaultOutputDevice );
		BOOST_REQUIRE( !availableOutputDevices.empty() );

		portaudio.GetDeviceDefaults( defaultSamplingFreq, maxNumChannels, defaultOutputDevice );

		portaudio.GetMinMaxAmplitude( minAmpl, maxAmpl );
		BOOST_REQUIRE( minAmpl == -1.0f );
		BOOST_REQUIRE( maxAmpl == 1.0f );
	}



	BOOST_AUTO_TEST_CASE( portaudio_play_test_case )
	{
		using namespace std;

		int numChannels = 2;
		double samplingFreq = 96'000.0;
		int samplesPerBuf = 10'000;
		double numSeconds = 2;
		unsigned long numProcessed;
		deque<float> inputSignal;
		vector<float> toneFreqs = { 1060, 1160, 1270, 1400, 1530, 1670, 1830, 2000, 2200, 2400, 2600 };
		vector<float> deltaF, deltaLength, deltaCycle, ampl;
		Core::Processing::CPortaudio<float> portaudio;
		vector<int> code;
		#if defined(_WIN32)
			string biasParamsFileName = "biasFilterParams.txt"; // the filter is designed for fs = 96'000 Hz
			string fmeParamFileName = "fmeProductionParams.dat";
		#elif defined(__linux)
			string biasParamsFileName = "./UnitTests/biasFilterParams.txt"; // the filter is designed for fs = 96'000 Hz
			string fmeParamFileName = "./UnitTests/fmeProductionParams.dat";
		#endif
		float minAmpl, maxAmpl;
		bool writeFile = false;
		double tStartSeqOffset;
		Core::Processing::CAudioDevice currActiveDevice;
		double currSamplingFreq;
		unsigned long currSamplesPerBuf;
		int currNumChannels;

		// generate FME code
		if ( writeFile ) {
			WriteFMEParamFile( samplingFreq, toneFreqs, fmeParamFileName, biasParamsFileName );
		}
	
		code =			{ 2, 5, 8, 3, 0 };
		ampl =			{ 0, 0, 0, 0, 0 };
		deltaLength =	{ 0, 0, 0, 0, 0 };
		deltaCycle =	{ 0, 0, 0, 0, 0 };
		deltaF =		{ 1.0, 1.0, 1.0, -2.0f, 0.0f };

		portaudio.GetMinMaxAmplitude( minAmpl, maxAmpl );
		Core::FME::CProduceFMECode<float> codeGenerator( fmeParamFileName, minAmpl, maxAmpl );
		codeGenerator.GenerateFMECode( code.begin(), code.end(), 10.0f, ampl.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), true, 10.0f, true, back_inserter( inputSignal ), tStartSeqOffset );

		// start a Portaudio stream
		portaudio.Start( Core::Processing::CAudioDevice( Core::OUT_DEVICE ), samplingFreq, samplesPerBuf, numChannels );

		// test restarting with an invalid stream
		BOOST_CHECK_THROW( portaudio.Start( Core::Processing::CAudioDevice(), samplingFreq, samplesPerBuf, numChannels ), std::logic_error );
		BOOST_REQUIRE( portaudio.GetStatus( currActiveDevice, currSamplingFreq, currSamplesPerBuf, currNumChannels ) == false );
		BOOST_CHECK_THROW( portaudio.WriteStream( inputSignal.begin(), inputSignal.end() ), Core::Exception::audioDeviceNotReadyException );

		// test restarting of the stream
		Core::Processing::CAudioDevice deviceSet = Core::Processing::CAudioDevice( Core::OUT_DEVICE );
		portaudio.Start(deviceSet, samplingFreq, samplesPerBuf, numChannels );
		BOOST_REQUIRE( portaudio.GetStatus( currActiveDevice, currSamplingFreq, currSamplesPerBuf, currNumChannels ) == true );
		BOOST_REQUIRE( currActiveDevice == deviceSet );
		BOOST_REQUIRE( samplingFreq == currSamplingFreq );
		BOOST_REQUIRE( samplesPerBuf == currSamplesPerBuf );
		BOOST_REQUIRE( numChannels == currNumChannels );
		BOOST_CHECK_THROW( portaudio.ReadStream( inputSignal.begin(), inputSignal.end(), 1 ), Core::Exception::audioDeviceNotReadyException );

		cout << "Output ...\n";
		// output of signal
		for (size_t i=0; i < inputSignal.size(); i++) {	
			if ( (inputSignal.begin() + samplesPerBuf) <= inputSignal.end() ) {
				numProcessed = portaudio.WriteStream( inputSignal.begin(), inputSignal.begin() + samplesPerBuf );
				for ( unsigned long j=0; j < numProcessed; j++ ) {
					inputSignal.pop_front();
				}
			} else {
				numProcessed = portaudio.WriteStream( inputSignal.begin(), inputSignal.end() );
				inputSignal.clear();
			}
		}
	}


	BOOST_AUTO_TEST_CASE( portaudio_record_test_case )
	{
		using namespace std;
		using namespace Utilities::Plugins;

		vector<float> data;
		int oldSize;
		Core::Processing::CPortaudio<float> portaudio;
		map< string, unique_ptr<Utilities::Plugins::AudioPlugin> > plugins;
		unique_ptr<Utilities::Plugins::AudioPlugin> audioPlugin;

		int dataLength = 1024;
		vector<float> newData( dataLength );
		int numChannels = 2;
		double samplingFreq = 44100.0;
		int samplesPerBuf = 64;
		int numSeconds = 20;
		string fileName = "audio.wav";
		#if defined (_WIN32)
			#ifdef NDEBUG
				boost::filesystem::path pluginPath = "../Release";
			#else
				boost::filesystem::path pluginPath = "../Debug";
			#endif
		#elif defined (__linux)
			// Linux
			boost::filesystem::path pluginPath = ".";
		#endif

		// load the audio format plugins
		plugins = LoadPlugins<Utilities::Plugins::AudioPlugin>( pluginPath );
		audioPlugin = move( plugins[ AudioPlugin::CompleteID( "WAV" ) ] );

		// init Portaudio
		portaudio.Start( Core::Processing::CAudioDevice( Core::IN_DEVICE ), samplingFreq, samplesPerBuf, numChannels );
		BOOST_CHECK_THROW( portaudio.WriteStream( data.begin(), data.end() ), Core::Exception::audioDeviceNotReadyException );

		// record audio signal
		cout << "Starting test recording ...";
		data.reserve( static_cast<int>( numSeconds * samplingFreq ) );
		while ( data.size() < ( numSeconds * samplingFreq ) ) {
			oldSize = data.size();
			data.resize( oldSize + samplesPerBuf );
			portaudio.ReadStream( data.begin() + oldSize, data.end(), 1 );
		}
		cout << " finished" << endl;

		// save audio signal on file
		audioPlugin->SetSamplingFreq( static_cast<int>( samplingFreq ) );
		audioPlugin->Save( fileName, data.begin(), data.end(), false );
	}

	BOOST_AUTO_TEST_SUITE_END();
}

/*@}*/
/*@}*/
