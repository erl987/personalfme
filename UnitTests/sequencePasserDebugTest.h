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
#include <deque>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "SeqDataComplete.h"
#include "SequencePasserDebug.h"

using boost::unit_test::label;


/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

/**	\defgroup	SequencePasserDebug		Unit test for the class CSequencePasserDebug<T>
*/

namespace Audio {
	/*@{*/
	/** \ingroup SequencePasserDebug
	*/
	namespace CSequencePasserDebug {
		std::deque< Utilities::CSeqDataComplete<float> > receivedSequences;
		std::mutex receivedDataMutex;

		void FoundSequenceCallback( const Utilities::CSeqDataComplete<float>& sequences)
		{
			receivedSequences.push_back( sequences );
		}



		// test section
		BOOST_AUTO_TEST_SUITE( CSequencePasserDebug_test_suite, *label("basic") );

		/**	@brief		Test of set parameter function
		*/
		BOOST_AUTO_TEST_CASE( set_parameters_test_case )
		{
			Core::Audio::CSequencePasserDebug<float> sequencePasser;
			sequencePasser.SetParameters( FoundSequenceCallback, std::function<void( const std::string& )>() );
		}



		/**	@brief		Test of sequence putting function
		*/
		BOOST_AUTO_TEST_CASE( put_sequences_test_case )
		{
			using namespace std;
			using namespace std::chrono_literals;

			string putInfoString;
			Utilities::CDateTime sequenceTime;
			vector<int> putTones;
			vector<float> putToneLengths, putTonePeriods, putToneFreqs, putAbsToneLevels;
			Utilities::CCodeData<float> sequenceData;
			deque< Utilities::CSeqDataComplete<float> > putSequences;
			Core::Audio::CSequencePasserDebug<float> sequencePasser;
			auto delayTime = 1s;
			int numTones = 5;

			// test sequence putting
			sequencePasser.SetParameters( FoundSequenceCallback, std::function<void( const std::string& )>() );
			sequenceTime.Set( 12, 5, 2012, Utilities::CTime( 15, 42, 2, 123 ) );
			for (int i=0; i < numTones; i++) {
				putTones.push_back( i );
				putToneLengths.push_back( 0.07f );
				putTonePeriods.push_back( 0.07f );
				putToneFreqs.push_back( 1300.0f + i );
				putAbsToneLevels.push_back( 1.9e-5f );
			}
			sequenceData.Set( putTones, putToneLengths, putTonePeriods, putToneFreqs, putAbsToneLevels );
			putInfoString = "info string";
			putSequences.push_back( Utilities::CSeqDataComplete<float>( sequenceTime, sequenceData, putInfoString ) );
			sequencePasser.PutSequences( putSequences.begin(), putSequences.end() );

			// delay thread
			std::this_thread::sleep_for( delayTime );

			// check for correctness
			std::unique_lock<std::mutex> lock( receivedDataMutex );
			BOOST_REQUIRE( receivedSequences.size() == putSequences.size() );
			BOOST_REQUIRE( receivedSequences.front() == putSequences.front() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
