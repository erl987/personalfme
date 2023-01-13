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
#include <deque>
#include <string>
#include <mutex>
#include <thread>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "SeqData.h"
#include "SequencePasser.h"

using boost::unit_test::label;


/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/

/**	\defgroup	SequencePasser		Unit test for the class CSequencePasser<T>
*/

namespace Audio {
	/*@{*/
	/** \ingroup SequencePasser
	*/
	namespace CSequencePasser {
		std::deque< Utilities::CSeqData > receivedSequences;
		std::mutex receivedDataMutex;

		void FoundSequenceCallback(Utilities::CSeqData seqData)
		{
			std::unique_lock<std::mutex> lock( receivedDataMutex );
			receivedSequences.push_back( seqData );
		}



		// test section
		BOOST_AUTO_TEST_SUITE( CAudioSequencePasser_test_suite, *label("default") );

		/**	@brief		Test of set parameter function
		*/
		BOOST_AUTO_TEST_CASE( set_parameters_test_case )
		{
			Core::Audio::CSequencePasser<float> sequencePasser;
			sequencePasser.SetParameters( FoundSequenceCallback, std::function< void( const std::string& ) >() );
		}



		/**	@brief		Test of sequence putting function
		*/
		BOOST_AUTO_TEST_CASE( put_sequences_test_case )
		{
			using namespace std;
			string putInfoString;
			Utilities::CDateTime sequenceTime;
			vector<int> puttedTones;
			vector<float> puttedToneLengths, puttedTonePeriods, puttedToneFreqs, puttedAbsToneLevels;
			Utilities::CCodeData<float> sequenceData;
			deque< Utilities::CSeqDataComplete<float> > putSequences;
			Core::Audio::CSequencePasser<float> sequencePasser;
			auto delayTime = 1s;
			int numTones = 5;

			// test sequence putting
			sequencePasser.SetParameters( FoundSequenceCallback, std::function< void( const std::string& ) >() );
			sequenceTime.Set( 12, 5, 2012, Utilities::CTime( 15, 42, 2, 123 ) );
			for (int i=0; i < numTones; i++) {
				puttedTones.push_back( i );
				puttedToneLengths.push_back( 0.07f );
				puttedTonePeriods.push_back( 0.07f );
				puttedToneFreqs.push_back( 1300.0f + i );
				puttedAbsToneLevels.push_back( 0.95f + i / 10 );
			}
			putInfoString = "info string";
			sequenceData.Set( puttedTones, puttedToneLengths, puttedTonePeriods, puttedToneFreqs, puttedAbsToneLevels );
			putSequences.push_back( Utilities::CSeqDataComplete<float>( sequenceTime, sequenceData, putInfoString ) );
			sequencePasser.PutSequences( putSequences.begin(), putSequences.end() );

			// delay thread
			std::this_thread::sleep_for( delayTime );

			// check for correctness
			std::unique_lock<std::mutex> lock( receivedDataMutex );
			BOOST_REQUIRE( receivedSequences.size() == putSequences.size() );
			BOOST_REQUIRE( receivedSequences.front().GetStartTime() == putSequences.front().GetStartTime() );
			BOOST_REQUIRE( receivedSequences.front().GetCode() == putSequences.front().GetCodeData().GetTones() );
			BOOST_REQUIRE( receivedSequences.front().GetInfoString() == putSequences.front().GetInfoString() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
