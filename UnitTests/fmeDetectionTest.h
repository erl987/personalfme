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
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "basicFunctions.h"
#include "BoostStdTimeConverter.h"
#include "FME.h"
#include "AudioInputParam.h"
#include "ProduceFMECode.h"
#include "RandomFMEParams.h"
#include "StatisticalAnalysis.h"
#include "fmeDetectionTester.h"
#include "FMEServerDebug.h"
#include "PortaudioWrapper.h"
#include "SeqDataComplete.h"

using boost::unit_test::label;

/**	\defgroup	UnitTest	Unit test module.
*/

/*@{*/
/** \ingroup UnitTests
*/


/**	\defgroup	FEMdetectionTests	Unit tests for FME-detection algorithms (realtime and non-realtime)
*/

/*@{*/
/** \ingroup FMEdetectionTests
*/
namespace FMEdetectionTests {
	using namespace std::chrono_literals;

	// general parameters for test settings
	const size_t pageSize = 1024;
	const double delayTime = 1e-7;			// in s
	const double finalDelayTime = 0.05;		// in s, needs to be increased on slow computers (for example: Raspberry Pi)
	const auto realTimeDelayTime = 500ms;
	const double maxDevRealTime = 15;		// in ms
	const int downsamplingFactorProc = 15;
	const int downsamplingFactorRec = 5;
	const double samplingFreq = 96'000;		// in Hz
	const float signalLoudness = 10.0f;		// in %
	const float minSignalAmpl = -1.0f; const float maxSignalAmpl = 1.0f;
	const int lengthCode = 5;

	// parameters for real-time tests
	const std::string serverAddress = "localhost";
	const int numChannels = 1;
	const int samplesPerBuf = 20'000;

	// parameters for random tone generation
	const unsigned int numTestCasesNonRealtime = 100;
	const unsigned int numTestCasesRealtime = 20;
	const bool isAllTonesIdentical = false;
	const bool isCodesBiased = false;
	const int minCodeDigit = 0; const int maxCodeDigit = 9;
	const float minToneAmp = 0.0f; const float maxToneAmp = 0.0f;
	const float minDeltaF = -2.0f; const float maxDeltaF = 2.0f;
	const float minDeltaLength = -5.0f; const float maxDeltaLength = 5.0f;
	const float minDeltaCycle = -5.0f; const float maxDeltaCycle = 5.0f;
	const float minSNR = -15.0f; const float maxSNR = 10.0f;

	// file and path names
	const std::string rootDirName = "../";
	const std::string audioSettingsFileName = "../audioSettings.dat";
	const std::string testFileName = "./fmeDetectionTests/signal.txt"; 
	const std::string multipleTestDirName = "./fmeDetectionTests";
	const std::string multipleTestFileNameBase = "test";	// only stored in case of errors
	const bool isSaveSignalsOnFile = true;					// flag stating the test signals of the non-realtime test are stored on file in case of errors (storing in the text files takes significant time)
	const std::string multipleTestFileEnding = ".txt";
	const std::string requirementsFileName = "testRequirements.txt";
	const std::string resultsFileName = "testResults.txt";
	const std::string fmeProductionSettingsFileName = "./fmeProductionParams.dat";
	const std::string parameterFileName = "../params.dat";
	const std::string specializedParameterFileName = "../fmeParams.dat";

	const std::string histogramFileName = "histogram.txt";
	unsigned int numLevelHistogramBins = 15;
	FMEdetectionTests::CStatisticalAnalysis<float> statisticalAnalyser;
	std::mutex realTimeCodeMutex;
	std::deque< Utilities::CSeqDataComplete<float> > foundRealTimeCodes;


	/** @brief		Initialize / teardown class for all tests
	*/
	class Fixture {
	public:
		Fixture() {
			#ifdef __linux
				boost::filesystem::current_path( "./UnitTests" );
			#endif
			statisticalAnalyser = FMEdetectionTests::CStatisticalAnalysis<float>( fmeProductionSettingsFileName, minSNR, maxSNR, numLevelHistogramBins );
		}
		~Fixture() {
			#ifdef __linux
				boost::filesystem::current_path( ".." );
			#endif
		}
	};


	/** @brief		Storage of parameter files for FME-detection
	*/
	void WriteParameterFile(std::string parameterFileName, std::string specializedParameterFileName)
	{
		using namespace std;

		double sampleLength = 9.0;
		double sampleLengthCoarse = 3.0 * sampleLength;
		double maxDeltaF = 0.0325;
		double overlap = 0.0;
		double overlapCoarse = 0.0;
		double delta = 0.5;
		double deltaCoarse = 0.3;
		int maxNumPeaks = 1;
		int maxNumPeaksCoarse = 1;
		int numNeighbours = 4;
		double evalToneLength = 0.09;
		double maxFreqDevConstrained = 0.48;
		double maxFreqDevUnconstrained = 0.06;
		const int freqResolution = 256;
		const int freqResolutionCoarse = 512;
		double searchTimestep = 0.01;
		const int numFreqs = 11;
		double freq[numFreqs] = { 1060, 1160, 1270, 1400, 1530, 1670, 1830, 2000, 2200, 2400, 2600 };
		vector<double> searchFreqs( numFreqs );

		for (size_t i=0; i < 11; i++) {
			searchFreqs[i] = freq[i];
		}

		Core::General::CAnalysisParam params;
		params.Set( sampleLength, sampleLengthCoarse, maxNumPeaks, maxNumPeaksCoarse, freqResolution, freqResolutionCoarse, maxDeltaF, overlap, overlapCoarse, delta, deltaCoarse, maxFreqDevConstrained, maxFreqDevUnconstrained, numNeighbours, evalToneLength, searchTimestep, searchFreqs.begin(), searchFreqs.end() );
		Core::FME::CFME<float> fmeParamSetting;
		fmeParamSetting.SaveParameters( parameterFileName, params );

		int codeLength = 5;
		double excessTime = 0.009;
		double deltaTMaxTwice = 1.02;
		double minLength = 0.055;
		double maxLength = 0.09;
		double maxToneLevelRatio = 16; // radio scanners have typically an power ratio of 9 (amplitude ratio of 3) between the tones 1 and "R"

		Core::FME::CFMEAnalysisParam fmeParams;
		fmeParams.Set( codeLength, excessTime, deltaTMaxTwice, minLength, maxLength, maxToneLevelRatio );
		fmeParamSetting.SaveFMEParameters( specializedParameterFileName, fmeParams );
	}



	/**	@brief		Generating a fully random FME-sequence for tests
	*/
	template <class InIt1, class InIt2, class InIt3, class InIt4, class InIt5> std::vector<float> GenerateFMECode(InIt1 codeFirst, InIt1 codeLast, InIt2 toneAmpFirst, InIt3 deltaFfirst, InIt4 deltaLengthFirst, InIt5 deltaCycleFirst, float SNR, double& seqStartOffset)
	{
		using namespace std;
		vector<float> fmeCode, toneAmp, deltaF, deltaLength, deltaCycle;
		vector<int> code;

		// obtaining input data
		code.assign( codeFirst, codeLast );
		toneAmp.assign( toneAmpFirst, toneAmpFirst + distance( codeFirst, codeLast ) );
		deltaF.assign( deltaFfirst, deltaFfirst + distance( codeFirst, codeLast ) );
		deltaLength.assign( deltaLengthFirst, deltaLengthFirst + distance( codeFirst, codeLast ) );
		deltaCycle.assign( deltaCycleFirst, deltaCycleFirst + distance( codeFirst, codeLast ) );
			
		// generate code
		Core::FME::CProduceFMECode<float> codeProducer( fmeProductionSettingsFileName, minSignalAmpl, maxSignalAmpl );
		codeProducer.GenerateFMECode( code.begin(), code.end(), signalLoudness, toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), true, SNR, isCodesBiased, back_inserter( fmeCode ), seqStartOffset );

		return fmeCode;
	}



	/**	@brief		Storing the complete signal sequence on file - this file format can directly be used for the simple single non-realtime tests (single_non_realtime_test_case)
	*/
	template <class InIt> void SaveSignalOnFile(std::string fileName, InIt signalFirst, InIt signalLast)
	{
		using namespace std;
		vector<float> signal;

		// obtain input data
		signal.assign( signalFirst, signalLast );

		ofstream out( fileName );
		for (size_t i=0; i < signal.size(); i++) {
			out << signal[i] << "\n";
		}
		out.close();
	}



	/**	@brief		Checking if a code with the given parameters must be or must not be detected according to the standard TR-BOS FME
	*/
	void CheckFailing(std::vector<float> deltaF, std::vector<float> deltaLength, std::vector<float> deltaCycle, bool& mustSucceed, bool& mustFail)
	{
		using namespace std;

		// limits of the TR-BOS FME
		float maxDeltaFMust = 2.0f;			// maximum deviation of frequency that must be detected [%]
		float maxDeltaFCan = 4.5f;			// maximum deviation of frequency that is allowed to be detected [%]
		float maxDeltaLengthMust = 5.0f;	// maximum deviation of tone length that must be detected [ms]
		float maxDeltaLengthCan = 20.0f;	// maximum deviation of tone length that is allowed to be detected [ms]
		float maxDeltaCycleMust = 5.0f;		// maximum deviation of tone period that must be detected [ms]
		float maxDeltaCycleCan = 20.0f;		// maximum deviation of tone period that is allowed to be detected [ms]
	
		// check if the sequence is within the required limits of the TR-BOS FME
		mustSucceed = true;
		if ( find_if( deltaF.begin(), deltaF.end(), [=](float val){ return( std::abs( val ) > maxDeltaFMust ); } ) != deltaF.end() ) {
			mustSucceed = false;
		}
		if ( find_if( deltaLength.begin(), deltaLength.end(), [=](float val){ return( std::abs( val ) > maxDeltaLengthMust ); } ) != deltaLength.end() ) {
			mustSucceed = false;
		}
		if ( find_if( deltaCycle.begin(), deltaCycle.end(), [=](float val){ return( std::abs( val ) > maxDeltaCycleMust ); } ) != deltaCycle.end() ) {
			mustSucceed = false;
		}
		
		// check if the sequence is outside of the allowed limits of the TR-BOS FME
		mustFail = false;
		if ( find_if( deltaF.begin(), deltaF.end(), [=](float val){ return( std::abs( val ) >= maxDeltaFCan ); } ) != deltaF.end() ) {
			mustSucceed = false;
			mustFail = true;
		}
		if ( find_if( deltaLength.begin(), deltaLength.end(), [=](float val){ return( std::abs( val ) >= maxDeltaLengthCan ); } ) != deltaLength.end() ) {
			mustSucceed = false;
			mustFail = true;
		}
		if ( find_if( deltaCycle.begin(), deltaCycle.end(), [=](float val){ return( std::abs( val ) >= maxDeltaCycleCan ); } ) != deltaCycle.end() ) {
			mustSucceed = false;
			mustFail = true;
		}
	}



	/**	@brief		Determining if the found code and the reference code are identical
	*/
	template <class InIt1, class InIt2> bool CheckForCorrectness(InIt1 foundCodesFirst, InIt1 foundCodesLast, InIt2 refCodeFirst, InIt2 refCodeLast)
	{
		bool isCorrectCode;
		std::vector< Utilities::CSeqDataComplete<float> > foundCodes;
		std::vector<int> refCode;

		// obtain input data
		foundCodes.assign( foundCodesFirst, foundCodesLast );
		refCode.assign( refCodeFirst, refCodeLast );

		if ( ( foundCodes.size() == 1 ) && ( foundCodes.front().GetCodeData().GetTones() == refCode ) ) {
			isCorrectCode = true;
		} else {
			isCorrectCode = false;
		}

		return isCorrectCode;
	}



	/**	@brief		Resetting the parameter and the results file
	*/
	void PrepareResultsFiles(void)
	{
		std::ofstream requirementsFile( multipleTestDirName + "/" + requirementsFileName, std::ios_base::trunc );
		requirementsFile << "time\tcode1\tdeltaLength1\tdeltaCycle1\tdeltaF1\ttoneAmp1\tcode2\tdeltaLength2\tdeltaCycle2\tdeltaF2\ttoneAmp2\tcode3\tdeltaLength3\tdeltaCycle3\tdeltaF3\ttoneAmp3\tcode4\tdeltaLength4\tdeltaCycle4\tdeltaF4\ttoneAmp4\tcode5\tdeltaLength5\tdeltaCycle5\tdeltaF5\ttoneAmp5\tSNR\tmustSucceed\tmustFail\ttestID\n";

		std::ofstream resultsFile( multipleTestDirName + "/" + resultsFileName, std::ios_base::trunc );
		resultsFile << "time\tcode1\tlength1\tcycle1\tf1\tcode2\tlength2\tcycle2\tf2\tcode3\tlength3\tcycle3\tf3\tcode4\tlength4\tcycle4\tf4\tcode5\tlength5\tcycle5\tf5\tisSuccessfull\ttestID\n";
	}



	/**	@brief		Storing the given parameters and the results on file
	*/
	void SaveResultsOnFile(int testID, bool isCorrectCode, std::deque< Utilities::CSeqDataComplete<float> > foundCodes, boost::posix_time::ptime time, std::vector<int> testCode, std::vector<float> toneAmp, std::vector<float> deltaF, std::vector<float> deltaLength, std::vector<float> deltaCycle, float SNR, bool mustSucceed, bool mustFail)
	{
		using namespace std;
		vector<int> foundCode;
		vector<float> foundFreq, foundLength, foundCycle, absToneLevel;

		// save requirements on file
		ofstream requirementsFile( multipleTestDirName + "/" + requirementsFileName, ios_base::app );
		requirementsFile << boost::lexical_cast<string>( time.time_of_day().total_microseconds() / 1.0e6f ) << "\t";
		for (size_t i=0; i < testCode.size(); i++) {
			requirementsFile << testCode[i] << "\t" << deltaLength[i] << "\t" << deltaCycle[i] << "\t" << deltaF[i] << "\t" << toneAmp[i] << "\t";
		}
		requirementsFile << SNR << "\t" << mustSucceed << "\t" << mustFail << "\t" << testID << "\n";

		// save results on file
		ofstream resultsFile( multipleTestDirName + "/" + resultsFileName, ios_base::app );
		if ( isCorrectCode ) {
			for ( const auto& currCode : foundCodes ) {
				foundCode = currCode.GetCodeData().GetTones();
				foundLength = currCode.GetCodeData().GetToneLengths();
				foundCycle = currCode.GetCodeData().GetTonePeriods();
				foundFreq = currCode.GetCodeData().GetToneFrequencies();
				absToneLevel = currCode.GetCodeData().GetAbsToneLevels();

				if ( currCode == foundCodes.front() ) {
					// store start time of detection
					resultsFile << boost::lexical_cast<string>( Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime( currCode.GetStartTime() ).time_of_day().total_microseconds() / 1.0e6f ) << "\t";

					// store code detection details
					for ( size_t i = 0; i < foundCode.size(); i++ ) {
						resultsFile << foundCode[i] << "\t" << foundLength[i] << "\t" << foundCycle[i] << "\t" << foundFreq[i] << "\t";
					}
					resultsFile << "1\t" << boost::lexical_cast<string>( testID ) << "\n";
				}
				
				// prepare statistical analysis of results
				statisticalAnalyser.PushValues( foundFreq, deltaF, foundLength, deltaLength, foundCycle, deltaCycle, absToneLevel, testCode, SNR );
			}
		} else {
			// no successful detection
			resultsFile << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t0\t" << boost::lexical_cast<string>( testID ) << "\n";
		}
	}



	/**	@brief		Performing the analysis of the tests and plotting the result diagram
	*/
	void FinalAnalysis(void)
	{
		using namespace std;
		float minFreqError, maxFreqError, meanFreqError, minLengthError, maxLengthError, meanLengthError, minCycleError, maxCycleError, meanCycleError;
		std::vector< std::tuple<float, float, float, float, float> > levelHistogram;

		// get statistical results
		statisticalAnalyser.GetFreqStatistics( minFreqError, maxFreqError, meanFreqError );
		statisticalAnalyser.GetLengthStatistics( minLengthError, maxLengthError, meanLengthError );
		statisticalAnalyser.GetCycleStatistics( minCycleError, maxCycleError, meanCycleError );
		statisticalAnalyser.GetToneLevelStatistics( levelHistogram );

		// print statistical results
		cout << "Frequency detection errors:\tMinimum:\t" << minFreqError << " Hz\tMean:\t" << meanFreqError << " Hz\tMaximum:\t" << maxFreqError << " Hz" << endl;
		cout << "Tone length detection errors:\tMinimum:\t" << minLengthError << " ms\tMean:\t" << meanLengthError << " ms\tMaximum:\t" << maxLengthError << " ms" << endl;
		cout << "Tone cycle detection errors:\tMinimum:\t" << minCycleError << " ms\tMean:\t" << meanCycleError << " ms\tMaximum:\t" << maxCycleError << " ms" << endl;
		
		// store the histogram on file
		{
			ofstream out( histogramFileName );
			out << "SNR" << "\t" << "min" << "\t" << "mean" << "\t" << "max" << "\t" << "standard deviation" << endl;
			for ( size_t i = 0; i < levelHistogram.size(); i++ ) {
				out << get<0>( levelHistogram[i] ) << "\t" << get<1>( levelHistogram[i] ) << "\t" << get<2>( levelHistogram[i] ) << "\t" << get<3>( levelHistogram[i] ) << "\t" << get<4>( levelHistogram[i] ) << endl;
			}
		}

        cout << endl << "The test results can be visualized with `python visualize_multiple_detections_result.py` (requires Python)" << endl;
	}



	/**	@brief		Function called if the FME detection server receives an incoming message
	*/
	void OnFoundSequence(const Utilities::CSeqDataComplete<float>& sequenceData)
	{
		std::unique_lock<std::mutex> lock( realTimeCodeMutex );
		foundRealTimeCodes.push_back( sequenceData );
	}



	/**	@brief		Playing the passed signal on the sound device
	*/
	template <class InIt> boost::posix_time::ptime PlaySequence(InIt signalFirst, InIt signalLast)
	{
		std::deque<float> signal;
		boost::posix_time::ptime startTimePlay;
		Core::Processing::CPortaudio<float> portaudio;
		
		portaudio.Start( Core::Processing::CAudioDevice( Core::OUT_DEVICE ), samplingFreq, samplesPerBuf, numChannels );

		// obtain input data
		signal.assign( signalFirst, signalLast );
		
		// capture starting time of sequence play
		startTimePlay = boost::posix_time::ptime( boost::posix_time::microsec_clock::universal_time() );

		// output of signal
		while ( !signal.empty() ) {
			if ( signal.size() > samplesPerBuf ) {
				portaudio.WriteStream( begin( signal ), begin( signal ) + samplesPerBuf );
				signal.erase( begin( signal ), begin( signal ) + samplesPerBuf );
			} else {
				portaudio.WriteStream( begin( signal ), end( signal ) );
				signal.clear();
			}
		}

		return startTimePlay;
	}
	

	
	// Test section
	BOOST_FIXTURE_TEST_SUITE( FMEDetection_test_suite, Fixture )

	/**	@brief		Single non-realtime testing of the FME analysis algorithm
	*/
	BOOST_AUTO_TEST_CASE( single_non_realtime_case, *label("basic") )
	{
		using namespace std;
		boost::posix_time::ptime startTimeSeq;
		vector<int> code;
		vector<float> signal;
		Utilities::CDateTime time;
		deque< Utilities::CSeqDataComplete<float> > foundCodes;

		//WriteParameterFile( parameterFileName, specializedParameterFileName );

		// initialize operations	
		cout << "Testing of FME-sequence signal processing algorithm ...\n";
		FMEdetectionTests::CFMEdetectionTester tester( audioSettingsFileName, pageSize, delayTime, finalDelayTime, maxDevRealTime, downsamplingFactorProc, downsamplingFactorRec, samplingFreq, rootDirName );

		// perform test
		LoadSignalData( testFileName, back_inserter( signal ) );
		foundCodes = tester.PerformTest( signal, startTimeSeq );
		
		// output of result
		for ( auto it = foundCodes.begin(); it != foundCodes.end(); it++ ) {
			time = it->GetStartTime();
			code = it->GetCodeData().GetTones();
			cout << setfill('0') << setw(2) << time.Day() << "." << setw(2) << time.Month() << "." << setw(4) << time.Year() << " ";
			cout << setfill('0') << setw(2) << time.TimeOfDay().Hour() << ":" << setw(2) << time.TimeOfDay().Minute() << ":" << setw(2) << time.TimeOfDay().Second() << "\t";
			for_each( code.begin(), code.end(), [](int a) { std::cout << a; } );
			cout << "\n";
		}
		cout << "\n";
	}



	/**	@brief		Realtime high-throughput testing of the FME analysis algorithm
	*/
	BOOST_AUTO_TEST_CASE( multiple_realtime_case, *label("realtime") )
	{
		using namespace std;
		float SNR;
		double seqOffsetTime;
		vector<float> fmeCodeSignal;
		vector<int> testCode( lengthCode );
		vector<float> toneAmp( lengthCode );
		vector<float> deltaF( lengthCode );
		vector<float> deltaLength( lengthCode );
		vector<float> deltaCycle( lengthCode );
		bool mustSucceed, mustFail, isCorrectCode;
		boost::posix_time::ptime outStartTime;
		Networking::CFMEServerDebug server;
		unique_ptr< std::thread > serverThread;

		try {
			// intialize operations
			cout << "Multiple real-time testing of FME-sequence signal processing algorithm ...\n";
			FMEdetectionTests::CRandomFMEParams randomProducer( isAllTonesIdentical, lengthCode, minCodeDigit, maxCodeDigit, minToneAmp, maxToneAmp, minDeltaF, maxDeltaF, minDeltaLength, maxDeltaLength, minDeltaCycle, maxDeltaCycle, minSNR, maxSNR );
			PrepareResultsFiles();

			// start the server for listening for incoming FME detections - the thread will automatically be stopped when the server is closed
			server.Init( serverAddress, Networking::CFMEServerDebug::GetStandardPort(), OnFoundSequence );
			serverThread.reset( new std::thread( [&]() { server.Run(); } ) );

			for ( unsigned int testID = 0; testID < numTestCasesRealtime; testID++ ) {
				// generate test sequence
				randomProducer.DesignParams( testCode.begin(), toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), SNR );
				fmeCodeSignal = GenerateFMECode( testCode.begin(), testCode.end(), toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), SNR, seqOffsetTime );

				// play test sequence
				outStartTime = PlaySequence( fmeCodeSignal.begin(), fmeCodeSignal.end() );
				outStartTime += boost::posix_time::microseconds( static_cast<long>( seqOffsetTime * 1.0e6 ) );

				// wait for some time, until it can be assumed that the sequenc detection was finished and data was transmitted
				std::this_thread::sleep_for( realTimeDelayTime );

				// determine if the sequence must be successfully detected or must not be detected (outside of the range defined by the standard TR-BOS FME)
				CheckFailing( deltaF, deltaLength, deltaCycle, mustSucceed, mustFail );

				// check for correctness
				std::unique_lock<std::mutex> lock( realTimeCodeMutex );
				isCorrectCode = CheckForCorrectness( foundRealTimeCodes.begin(), foundRealTimeCodes.end(), testCode.begin(), testCode.end() );
				if ( mustSucceed ) {
					if ( !isCorrectCode ) {
						BOOST_ERROR( "Code detection failed." );
					}
				}
				if ( mustFail ) {
					if ( isCorrectCode ) {
						BOOST_ERROR( "Code detection should have failed." );
					}
				}

				// store results on file
				SaveResultsOnFile( testID, isCorrectCode, foundRealTimeCodes, outStartTime, testCode, toneAmp, deltaF, deltaLength, deltaCycle, SNR, mustSucceed, mustFail );
				foundRealTimeCodes.clear();
			}

			// final analysis of results
			FinalAnalysis();
			server.Stop();
			serverThread->join();
		} catch ( std::exception e ) {
			server.Stop();
			serverThread->join();
			throw;
		}
	}



	/**	@brief		Non-realtime high-throughput testing of the FME analysis algorithm
	*/
	BOOST_AUTO_TEST_CASE( multiple_non_realtime_case, *label("advanced") )
	{
		using namespace std;
		float SNR;
		double seqOffsetTime;
		bool mustSucceed, mustFail, isCorrectCode, isError;
		string multipleTestFileName;
		boost::posix_time::ptime startTimeSeq;
		deque< Utilities::CSeqDataComplete<float> > foundCodes;
		vector<float> fmeCodeSignal;
		vector<int> testCode( lengthCode );
		vector<float> toneAmp( lengthCode );
		vector<float> deltaF( lengthCode );
		vector<float> deltaLength( lengthCode );
		vector<float> deltaCycle( lengthCode );

		// initialize operations
		cout << "Multiple testing of FME-sequence signal processing algorithm ...\n";
		PrepareResultsFiles();
		FMEdetectionTests::CFMEdetectionTester tester( audioSettingsFileName, pageSize, delayTime, finalDelayTime, maxDevRealTime, downsamplingFactorProc, downsamplingFactorRec, samplingFreq, rootDirName );
		FMEdetectionTests::CRandomFMEParams randomProducer( isAllTonesIdentical, lengthCode, minCodeDigit, maxCodeDigit, minToneAmp, maxToneAmp, minDeltaF, maxDeltaF, minDeltaLength, maxDeltaLength, minDeltaCycle, maxDeltaCycle, minSNR, maxSNR );

		for (unsigned int testID=0; testID < numTestCasesNonRealtime; testID++) {
			randomProducer.DesignParams( testCode.begin(), toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), SNR );
			fmeCodeSignal = GenerateFMECode( testCode.begin(), testCode.end(), toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), SNR, seqOffsetTime );
			multipleTestFileName = multipleTestDirName + "/" + multipleTestFileNameBase + "_" + boost::lexical_cast<string>( testID ) + multipleTestFileEnding;

			// perform test
			foundCodes = tester.PerformTest( fmeCodeSignal, startTimeSeq );
			startTimeSeq += boost::posix_time::microseconds( static_cast<long>( seqOffsetTime * 1.0e6 ) );

			// determine if the sequence must be successfully detected or must not be detected (outside of the range defined by the standard TR-BOS FME)
			CheckFailing( deltaF, deltaLength, deltaCycle, mustSucceed, mustFail );

			// check for correctness
			isCorrectCode = CheckForCorrectness( foundCodes.begin(), foundCodes.end(), testCode.begin(), testCode.end() );	
			isError = false;
			if ( mustSucceed ) {
				if ( !isCorrectCode ) {
					isError = true;
					BOOST_ERROR( "Code detection failed." );
				}
			}
			if ( mustFail ) {
				if ( isCorrectCode ) {
					isError = true;
					BOOST_ERROR( "Code detection should have failed." );
				}
			}

			// store requirements and results on file
			if ( ( isSaveSignalsOnFile ) && ( isError ) ) {
				SaveSignalOnFile( multipleTestFileName, fmeCodeSignal.begin(), fmeCodeSignal.end() );
			}
			SaveResultsOnFile( testID, isCorrectCode, foundCodes, startTimeSeq, testCode, toneAmp, deltaF, deltaLength, deltaCycle, SNR, mustSucceed, mustFail );
		}

		// final analysis of results
		FinalAnalysis();
	}

	BOOST_AUTO_TEST_SUITE_END()
}

/*@}*/
/*@}*/
