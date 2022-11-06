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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "AudioInputParam.h"
#include "fmeDetectionTester.h"
#include <thread>
#include <chrono>

/** @brief		Loading of audio settings
*/
void FMEdetectionTests::CFMEdetectionTester::LoadAudioSettings(const std::string& audioSettingsFileName, std::string& parameterFileName, std::string& specializedParameterFileName, double& maxRequiredProcFreq, double& transWidthProc, double& transWidthRec)
{
	double sampleLength;
	float mainThreadCycleTime;
	int numChannels, maxLengthInputQueue, maxMissedAttempts, channel;
	std::vector<double> standardSamplingFreqs;
	Core::CAudioInputParam params;

	// deserialize parameter data from file
	std::ifstream ifs( audioSettingsFileName );
	boost::archive::text_iarchive ia( ifs );

	// read parameters
	if ( !ifs.eof() ) {
		ia >> params;
	} else {
		throw std::ios_base::failure( "Parameter file cannot be read." );
	}

	ifs.close();

	// obtain required parameters
	params.Get( sampleLength, numChannels, maxLengthInputQueue, maxMissedAttempts, channel, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec, mainThreadCycleTime, standardSamplingFreqs );
}


	
/** @brief		Constructor
*/
FMEdetectionTests::CFMEdetectionTester::CFMEdetectionTester(std::string audioSettingsFileName, size_t pageSize, double delayTime, double finalDelayTime, double maxDevRealTime, int downsamplingFactorProc, int downsamplingFactorRec, double samplingFreq, std::string rootDirName)
	: 	pageSize( pageSize ),
		delayTime( delayTime ),
		finalDelayTime( finalDelayTime ),
		maxDevRealTime( maxDevRealTime ),
		downsamplingFactorProc( downsamplingFactorProc ),
		downsamplingFactorRec( downsamplingFactorRec ),
		samplingFreq( samplingFreq ),
		rootDirName( rootDirName )
{
	std::string parameterFileName, specializedParameterFileName;
	double maxRequiredProcFreq, transWidthProc, transWidthRec, cutoffFreqProc, cutoffFreqRec;

	// set downsampling filters
	LoadAudioSettings( audioSettingsFileName, parameterFileName, specializedParameterFileName, maxRequiredProcFreq, transWidthProc, transWidthRec );
	cutoffFreqProc = maxRequiredProcFreq;
	cutoffFreqRec = samplingFreq / 2.0 / downsamplingFactorRec;
	CFMEdetectionTester::fullDownsampler.SetParameters( downsamplingFactorProc, cutoffFreqProc, transWidthProc, downsamplingFactorRec, cutoffFreqRec, transWidthRec, samplingFreq);
	
	// set searcher class
	CFMEdetectionTester::searchCode.reset( new Core::FME::CFME<float>( samplingFreq / downsamplingFactorProc, rootDirName + parameterFileName, rootDirName + specializedParameterFileName, std::function< void( const std::string& ) >() ) );
}



/** @brief		Performing a non-realtime test with the signal data given in the file "testFileName"
*/
std::deque< Utilities::CSeqDataComplete<float> > FMEdetectionTests::CFMEdetectionTester::PerformTest( std::vector<float> signalQueue, boost::posix_time::ptime& startTimeSeq )
{
	using namespace std;
	deque< Utilities::CSeqDataComplete<float> > foundCodes, newFoundCodes;
	deque< boost::posix_time::ptime> signalTime, signalTimePage, downsampledTimePage, downsampledTimePageRec;
	deque<float> signalQueuePage, downsampledSignalPage, downsampledSignalPageRec;

	// generate the times container with random deviations
	signalTime.resize( signalQueue.size() );
	GenerateTimes( begin( signalTime ), signalQueue.size(), startTimeSeq );
		
	// search for FME code
	if ( pageSize > static_cast<int>( signalTime.size() ) ) {
		throw std::length_error("Page size is longer than signal data length.");
	}

	// emulated real-time input of signal data	
	for (size_t i=0; i < signalTime.size(); i += pageSize ) {			
		if ( ( i + pageSize ) <= signalTime.size() ) {
			// put new data into the analysis thread
			signalTimePage.assign( signalTime.begin() + i, signalTime.begin() + i + pageSize );
			signalQueuePage.assign( signalQueue.begin() + i, signalQueue.begin() + i + pageSize );

			// two-stage downsampling of signal data
			fullDownsampler.PerformDownsampling( signalTimePage.begin(), signalTimePage.end(), signalQueuePage.begin(), back_inserter( downsampledTimePage ), back_inserter( downsampledSignalPage ), back_inserter( downsampledTimePageRec ), back_inserter( downsampledSignalPageRec ) );

			// analysis of signal data
			searchCode->PutSignalData( downsampledTimePage.begin(), downsampledTimePage.end(), downsampledSignalPage.begin(), downsampledSignalPage.end() );
			downsampledTimePageRec.clear();
			downsampledSignalPageRec.clear();
			downsampledTimePage.clear();
			downsampledSignalPage.clear();
		}
		
		// delay thread
		std::this_thread::sleep_for( std::chrono::microseconds( static_cast<long long>( delayTime * 1e6 ) ) );
		// obtain newly found sequences
		newFoundCodes = searchCode->GetSequencesDebug();							
		foundCodes.insert( foundCodes.end(), newFoundCodes.begin(), newFoundCodes.end() );
	}

	// waiting for final results
	std::this_thread::sleep_for( std::chrono::microseconds( static_cast<long long>( finalDelayTime * 1e6 ) ) );
	newFoundCodes = searchCode->GetSequencesDebug();							
	foundCodes.insert( foundCodes.end(), newFoundCodes.begin(), newFoundCodes.end() );

	return foundCodes;
}
