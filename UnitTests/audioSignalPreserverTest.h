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
#include <string>
#include <deque>
#include <random>
#include <chrono>
#include <mutex>
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "BoostStdTimeConverter.h"
#include "SeqData.h"
#include "SeqDataComplete.h"
#include "AudioSignalPreserver.h"
#include "basicFunctions.h"

using boost::unit_test::label;


/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

/**	\defgroup	AudioSignalPreserver	Unit test for the class CAudioSignalPreserver<T>
*/

namespace Audio {
	/*@{*/
	/** \ingroup AudioSignalPreserver
	*/
	namespace CAudioSignalPreserver {
		using namespace std::chrono_literals;

		const double samplingFreq = 15000.0f;
		const float recordTimeLowerLimit = 0.2f;
		const float recordTimeUpperLimit = 1.0f;
		const float recordTimeBuffer = 0.1f;
		const float maxSignal = 0.5f;
		const int testSignalLength = static_cast<int>( 2.0f * samplingFreq );	// in number of samples
		const auto delayTime = 1s;
		const unsigned int maxPutDelayTime = 100;								// ms
		Utilities::CSeqData receivedCode;
		std::vector<float> receivedData;
		double receivedSamplingFreq;
		std::mutex receivingMutex;
		
		/**	@brief		Test callback function for the CAudioSignalPreserver<T>-class
		*/
		void foundRecordCallback(const Utilities::CSeqData& code, std::vector<float> data, double samplingFreq)
		{
			std::unique_lock<std::mutex>( receivingMutex );
			receivedCode = code;
			receivedData = data;
			receivedSamplingFreq = samplingFreq;
		}



		/**	@brief		Finding the signal data expected to be returned to the callback by the CAudioSignalPreserver<T>-class
		*/
		template <class InIt1, class InIt2, class OutIt> void PredictReturnedData(InIt1 timeFirst, InIt1 timeLast, InIt2 signalFirst, boost::posix_time::ptime seqTime, OutIt requiredSignalFirst)
		{
			unsigned int startIndex, deleteIndex;
			int recordTimeLowerLimitLength, recordTimeUpperLimitLength;
			std::vector< boost::posix_time::ptime > time;
			std::vector<float> signal, requiredSignal;

			// obtain input data
			time.assign( timeFirst, timeLast );
			signal.assign( signalFirst, signalFirst + std::distance( timeFirst, timeLast ) );

			recordTimeLowerLimitLength = static_cast<int>( recordTimeLowerLimit * samplingFreq );
			recordTimeUpperLimitLength = static_cast<int>( recordTimeUpperLimit * samplingFreq );
			startIndex = static_cast<unsigned int>( ( ( seqTime - time.front() ).total_microseconds() / 1.0e6 ) * samplingFreq ) + recordTimeLowerLimitLength;
			deleteIndex = static_cast<unsigned int>( ( ( seqTime - time.front() ).total_microseconds() / 1.0e6 ) * samplingFreq ) + recordTimeUpperLimitLength;
			requiredSignal.assign( signal.begin() + startIndex, signal.begin() + deleteIndex );

			// set output data
			std::move( requiredSignal.begin(), requiredSignal.end(), requiredSignalFirst );
		}



		// test section
		BOOST_AUTO_TEST_SUITE( CAudioSignalPreserver_test_suite, *label("basic") );

		/**	@brief		Test of set and get functions
		*/
		BOOST_AUTO_TEST_CASE( set_get_test_case )
		{
			double receivedSamplingFreq;
			float receivedRecordTimeLowerLimit, receivedRecordTimeUpperLimit, receivedRecordTimeBuffer;

			Core::Audio::CAudioSignalPreserver<float> signalPreserver;
			signalPreserver.SetParameters( samplingFreq, recordTimeLowerLimit, recordTimeUpperLimit, recordTimeBuffer, foundRecordCallback, std::function<void( const std::string& )>() );
			signalPreserver.GetParameters( receivedSamplingFreq, receivedRecordTimeLowerLimit, receivedRecordTimeUpperLimit, receivedRecordTimeBuffer );
			BOOST_REQUIRE( receivedSamplingFreq == samplingFreq );
			BOOST_REQUIRE( receivedRecordTimeLowerLimit == recordTimeLowerLimit );
			BOOST_REQUIRE( receivedRecordTimeUpperLimit == recordTimeUpperLimit );
			BOOST_REQUIRE( receivedRecordTimeBuffer == recordTimeBuffer );
		}



		/**	@brief		Test of callback mechanism
		*/
		BOOST_AUTO_TEST_CASE( callback_test_case )
		{
			using namespace std;
			using namespace boost::posix_time;
			using namespace Utilities::Time;
			typedef mt19937::result_type seed_type;

			unsigned int signalIndex, signalIndexOld;
			vector<ptime> time;
			vector<int> putTones;
			vector<float> signal, requiredSignal, putToneLengths, putTonePeriods, putToneFreqs, putAbsToneLevels;
			vector< Utilities::CSeqDataComplete<float> > sequences;
			string infoString;			
			Utilities::CDateTime seqTime;
			
			// generate test data
			GenerateTestData( testSignalLength, maxSignal, samplingFreq, back_inserter( time ), back_inserter( signal ) );

			// set test sequence
			seqTime = CBoostStdTimeConverter::ConvertToStdTime( ptime( microsec_clock::universal_time() ) );
			for (int i=0; i < 5; i++) {
				putTones.push_back( i );
				putToneLengths.push_back( 0.07f );
				putTonePeriods.push_back( 0.07f );
				putToneFreqs.push_back( 1300.0f );
				putAbsToneLevels.push_back( 0.98f );
			}
			infoString = "info string";
			sequences.push_back( Utilities::CSeqDataComplete<float>( seqTime, Utilities::CCodeData<float>( putTones, putToneLengths, putTonePeriods, putToneFreqs, putAbsToneLevels ), infoString ) );

			// feed data to the class with random page sizes and random delay times
			Core::Audio::CAudioSignalPreserver<float> signalPreserver( samplingFreq, recordTimeLowerLimit, recordTimeUpperLimit, recordTimeBuffer, foundRecordCallback, std::function<void( const std::string& )>() );
			signalPreserver.PutSequences( sequences.begin(), sequences.end() );

			mt19937 generator( static_cast<seed_type>( std::chrono::high_resolution_clock::now().time_since_epoch().count() ) );
			auto randomIndex = std::bind( std::uniform_int_distribution<unsigned int>( 0, signal.size() / 10 ), generator );
			auto randomDelay = std::bind( std::uniform_int_distribution<unsigned int>( 0, maxPutDelayTime ), generator );
			signalIndexOld = 0;
			signalIndex = randomIndex();
			while ( signalIndex < signal.size() ) {
				signalPreserver.PutSignalData( time.begin() + signalIndexOld, time.begin() + signalIndex, signal.begin() + signalIndexOld );
				signalIndexOld = signalIndex;
				signalIndex += randomIndex();
				std::this_thread::sleep_for( std::chrono::milliseconds{ randomDelay() } );
			}
			time.erase( time.begin() + signalIndexOld, time.end() );
			signal.erase( signal.begin() + signalIndexOld, signal.end() );

			// delay thread
			std::this_thread::sleep_for( delayTime );

			// test for correctness
			std::unique_lock<std::mutex>( receivingMutex );
			PredictReturnedData( time.begin(), time.end(), signal.begin(), CBoostStdTimeConverter::ConvertToBoostTime( seqTime ), back_inserter( requiredSignal ) );
			BOOST_REQUIRE( receivedSamplingFreq == samplingFreq );
			BOOST_REQUIRE( receivedCode.GetCode() == putTones );
			BOOST_REQUIRE( receivedCode.GetStartTime() == seqTime );
			BOOST_REQUIRE( receivedCode.GetInfoString() == infoString );
			BOOST_REQUIRE( receivedData == requiredSignal );
		}
	
		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
