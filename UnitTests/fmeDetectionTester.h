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
#include <string>
#include <deque>
#include <fstream>
#include <memory>
#include <random>
#include "FME.h"
#include "AudioFullDownsampler.h"
#include "SeqDataComplete.h"

/*@{*/
/** \ingroup UnitTests
*/

/*@{*/
/** \ingroup FMEdetectionTests
*/

namespace FMEdetectionTests {
	template <class OutIt1> void LoadSignalData(std::string testFileName, OutIt1 signalDataFirst);

	/**	\ingroup FMEdetectionTests
	*	Class performing non-real time FME detection tests
	*/
	class CFMEdetectionTester
	{
	public:
		CFMEdetectionTester(std::string audioSettingsFileName, size_t pageSize, double delayTime, double finalDelayTime, double maxDevRealTime, int downsamplingFactorProc, int downsamplingFactorRec, double samplingFreq, std::string rootDirName);
		virtual ~CFMEdetectionTester() {};
		std::deque< Utilities::CSeqDataComplete<float> > PerformTest( std::vector<float> signalQueue, boost::posix_time::ptime& startTimeSeq );
	protected:
		void LoadAudioSettings(const std::string& audioSettingsFileName, std::string& parameterFileName, std::string& specializedParameterFileName, double& maxRequiredProcFreq, double& transWidthProc, double& transWidthRec);
		template <class OutIt1> void GenerateTimes( OutIt1 signalTimeFirst, const unsigned int& numDatapoints, boost::posix_time::ptime& startTime );

		size_t pageSize;
		double delayTime;
		double finalDelayTime;
		double maxDevRealTime;
		int downsamplingFactorProc;
		int downsamplingFactorRec;
		double samplingFreq;
		std::string rootDirName;
		float maxSignalAmpl;
		Core::Audio::CAudioFullDownsampler<float> fullDownsampler;
		std::unique_ptr< Core::General::CSearch<float> > searchCode;
	};
}
/*@}*/
/*@}*/



/** @brief		Loading signal data from file and generating a time container simulating time capture uncertainities
*/
template <class OutIt1>
void FMEdetectionTests::LoadSignalData(std::string testFileName, OutIt1 signalDataFirst)
{
	using namespace std;

	float value;
	vector<float> signal;
			
	// load audio signal
	ifstream in( testFileName );
	if ( !in.is_open() ) {
		throw std::runtime_error( "The signal test data file could not be opened." );
	}
	signal.reserve( 300'000 );	// this is a hint that is sufficient for this testing purpose
	while( !in.eof() ) {
		in >> value;
		signal.push_back( static_cast<float>( value ) );
	}
	in.close();

	// set output values
	std::move( signal.begin(), signal.end(), signalDataFirst );
}


/** @brief		Loading signal data from file and generating a time container simulating time capture uncertainities
*/
template <class OutIt1>
void FMEdetectionTests::CFMEdetectionTester::GenerateTimes( OutIt1 signalTimeFirst, const unsigned int& numDatapoints, boost::posix_time::ptime& startTime )
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	ptime inputTime;
	vector<ptime> time;

	// prepare random number generation
	mt19937 eng( static_cast<unsigned long>( std::time( nullptr ) ) );
	uniform_real_distribution<double> dist( -maxDevRealTime, maxDevRealTime );
	auto random = bind( dist, eng );

	startTime = ptime( microsec_clock::universal_time() );
	time.resize( numDatapoints );
	for ( size_t i = 0; i < time.size(); i++ ) {
		inputTime = startTime + microseconds( static_cast<long>( i / samplingFreq * 1.0e6 + random() * 1.0e3 ) ); // including a random error simulating inaccuracies of time capture due to the OS
		time[i] = inputTime;
	}

	// set output values
	std::move( time.begin(), time.end(), signalTimeFirst );
}