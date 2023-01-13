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

#include <boost/lexical_cast.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include "FMEGenerateParam.h"
#include "ProduceCode.h"


/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace FME {
		/** \ingroup Core
		*	Class for generation of FME-codes according to TR-BOS FME. For testing purposes deviations from TR-BOS can be obtained.
		*	This class is a specialization of a general class for the generation of tone sequences.
		*/
		template <class T> class CProduceFMECode:
			public Core::General::CProduceCode<T>
		{
		public:
			CProduceFMECode(void);
			CProduceFMECode(std::string parameterFileName, T minAmpl, T maxAmpl);
			~CProduceFMECode(void);
			template <class In_It, class Out_It> Out_It GenerateFMECode(In_It tonesBegin, In_It tonesEnd, T pctLoudness, Out_It fmeCodeBegin);
			template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class Out_It> Out_It GenerateFMECode(In_It1 tonesBegin, In_It1 tonesEnd, T pctLoudness, In_It2 toneAmpBegin, In_It3 deltaFBegin, In_It4 deltaLengthBegin, In_It5 deltaCycleBegin, bool isWhiteNoise, T SNR, bool isBiased, Out_It fmeCodeBegin, double& seqStartOffset);
			void SaveParameters(std::string fmeFileName, CFMEGenerateParam params);
			void LoadParameters(std::string fmeFileName, CFMEGenerateParam &params);
		private:
			CProduceFMECode(const CProduceFMECode &);				// prevent copying
    		CProduceFMECode & operator= (const CProduceFMECode &);	// prevent assignment
			template <class In_It, class Out_It> Out_It ConvertTonesToFreqs(In_It tonesBegin, In_It tonesEnd, Out_It codeBegin);

			double tLength;
			double toneLength;
			double pauseTime;
			int lengthFMECode;
			T refAmpl;
			T groundOffset;
			std::vector<T> toneFreq;
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::FME::CProduceFMECode<T>::CProduceFMECode()
{
}



/**	@brief		Constructor.
*	@param		parameterFileName	Name of file containing the parameters required for generating the FME-code sequence
*	@param		minAmpl				Minimum (usually negative) amplitude of the audio output device (using Portaudio with datatype "float", it is -1.0)
*	@param		maxAmpl				Maximum (usually positive) amplitude of the audio output device (using Portaudio with datatype "float", it is +1.0)
*	@exception 						None
*	@remarks 						None
*/
template <class T> Core::FME::CProduceFMECode<T>::CProduceFMECode(std::string parameterFileName, T minAmpl, T maxAmpl)
	: Core::General::CProduceCode<T>()
{
	using namespace std;

	double samplingFreq, tLength, toneLength, pauseTime;
	int lengthFMECode;
	vector<double> toneFreq, biasFilterParams;
	CFMEGenerateParam params;
	
	// load parameters from file
	LoadParameters( parameterFileName, params );
	params.Get( samplingFreq, tLength, toneLength, pauseTime, lengthFMECode, std::back_inserter( toneFreq ), back_inserter( biasFilterParams ) );
			
	// set class variables
	CProduceFMECode::tLength = tLength;
	CProduceFMECode::toneLength = toneLength;
	CProduceFMECode::pauseTime = pauseTime;
	CProduceFMECode::lengthFMECode = lengthFMECode;
	CProduceFMECode::refAmpl = ( maxAmpl - minAmpl ) / 2 ;
	CProduceFMECode::groundOffset = ( minAmpl + maxAmpl ) / 2;
	CProduceFMECode::toneFreq.resize( toneFreq.size() );
	for (size_t i=0;  i < toneFreq.size(); i++) {
		CProduceFMECode::toneFreq[i] = static_cast<T>( toneFreq[i] );
	}

	// set class variable of base class
	Core::General::CProduceCode<T>::samplingFreq = samplingFreq;
	Core::General::CProduceCode<T>::biasFilterParams.resize(biasFilterParams.size());
	for (size_t i=0; i < biasFilterParams.size(); i++) {
		Core::General::CProduceCode<T>::biasFilterParams[i] = biasFilterParams[i];
	}
}



/**
* 	@brief		Default constructor.
*/
template <class T> Core::FME::CProduceFMECode<T>::~CProduceFMECode()
{
}



/**	@brief		Generator for FME-code sequences according to TR-BOS FME. Deviations for testing purposes are possible.
*	@param		tonesBegin			Iterator to beginning of the container with the tones (i.e. the digits) - for example: {2, 5, 6, 3, 4} for the code 25634. It must have the size of 5.
*	@param		tonesEnd			Iterator to one element after the end of the container with the tones.
*	@param		pctLoudness			Loudness of the FME-sequence relative to the maximum possible amplitude of the audio device in percent.
*	@param		toneAmpBegin		Relative deviation (in precent) of the tone amplitudes. Must be a container of length 5.
*	@param		deltaFBegin			Relative deviation (in percent) of the tone frequencies. Must be a container of length 5.
*	@param		deltaLengthBegin	Deviation (in ms) of the tone lengths (i.e. the time from the start to the end of a tone). Must be a container of length 5.
*	@param		deltaCycleBegin		Deviation (in ms) of the tone cycles (i.e. the time from the start one tone to the start of the next). Must be a container of length 5.
*	@param		isWhiteNoise		Flag stating if white noise should be present
*	@param		SNR					Signal to noise ratio. Ignored if "isWhiteNoise" is false
*	@param		isBiased			Flag stating if a frequency bias should be present. This is useful simulating an audio input device with limited frequency bandwidth.
*	@param		fmeCodeBegin		Iterator to the beginning of the container with the FME-code sequence. The container must have the size ( samplingFreq * tLength + 1 ) or use std::back_inserter.
*	@param		seqStartOffset		Offset time between start of signal and start of sequence [s]
*	@return							Iterator to one element after the end of the container with the FME-code sequence
*	@exception	std::length_error	Thrown if the number of tones (digits) is not conformant to the required length (5) of TR-BOS FME, or if a cycle time is shorter than the corresponding tone length.
*	@remarks 						Use the method GenerateFMECode(In_It, In_It, T, Out_It) in order to generate code sequences conforming to TR-BOS FME
*/
template <class T> template <class In_It1, class In_It2, class In_It3, class In_It4, class In_It5, class Out_It> Out_It Core::FME::CProduceFMECode<T>::GenerateFMECode(In_It1 tonesBegin, In_It1 tonesEnd, T pctLoudness, In_It2 toneAmpBegin, In_It3 deltaFBegin, In_It4 deltaLengthBegin, In_It5 deltaCycleBegin, bool isWhiteNoise, T SNR, bool isBiased, Out_It fmeCodeBegin, double& seqStartOffset)
{
	using namespace std;

	vector<int> tones;	
	vector<T> time, fmeCode, singleCode, singleTone, noNoiseSequence, deltaF, toneAmp, freq, deltaLength, deltaCycle;
	double amplSignal;
	int codeLength;

	// read input data
	tones.assign( tonesBegin, tonesEnd );
	codeLength = static_cast<int>( tones.size() );
	if ( codeLength != lengthFMECode ) {
		std::length_error("The FME code must have a length of " + boost::lexical_cast<string>( lengthFMECode ) + "." );
	}

	deltaF.resize( codeLength );
	toneAmp.resize( codeLength );
	deltaLength.resize( codeLength );
	deltaCycle.resize( codeLength );
	for (size_t i=0; i < deltaF.size(); i++) {
		deltaF[i] = *(deltaFBegin++);
		toneAmp[i] = *(toneAmpBegin++);
		deltaLength[i] = *(deltaLengthBegin++);
		deltaCycle[i] = *(deltaCycleBegin++);

		// check the length of cycle and tone
		if ( ( deltaCycle[i] - deltaLength[i] ) < 0 ) {
			throw std::length_error("The cycle time must be longer than the length of the tone.");
		}
	}

	// convert to frequencies
	freq.resize( tones.size() );
	ConvertTonesToFreqs( tones.begin(), tones.end(), freq.begin() );

	// generate white noise
	this->GenerateWhiteNoise( SNR, isWhiteNoise, pctLoudness / 100.0f * refAmpl, tLength, *max_element( toneAmp.begin(), toneAmp.end() ), amplSignal, back_inserter( fmeCode ) );
	
	// generate a single FME tone sequence (without repetition)
	for ( int f=0; f < codeLength; f++ ) {
		singleTone.assign( static_cast<int>( this->samplingFreq * ( toneLength + deltaCycle[f] / 1000.0f ) ), 0 ); // defines cycle time
		this->GenerateSinusTone( amplSignal * ( 1 + toneAmp[f] / 100.0f ), freq[f] * ( 1 + deltaF[f] / 100.0f ), toneLength + deltaLength[f] / 1000.0f, singleTone.begin() ); // defines tone length

		// construct the full single FME code sequence
		singleCode.insert( singleCode.end(), singleTone.begin(), singleTone.end() );
	}

	// construct full code sequence with one repetition from the single code sequence and the noise
	noNoiseSequence.resize( fmeCode.size() );
	copy( singleCode.begin(), singleCode.end(), noNoiseSequence.begin() + 1 + static_cast<int>( this->samplingFreq * pauseTime ) );
	copy( singleCode.begin(), singleCode.end(), noNoiseSequence.begin() + 1 + static_cast<int>( this->samplingFreq * ( 2 * pauseTime + codeLength * toneLength ) ) );
	transform( noNoiseSequence.begin(), noNoiseSequence.end(), fmeCode.begin(), fmeCode.begin(), [](T x, T y) { return x + y; } );

	// add amplitude offset if required
	transform( fmeCode.begin(), fmeCode.end(), fmeCode.begin(), [=](T x) { return ( x + groundOffset ); } );

	// simulate frequency bias
	if ( isBiased ) {
		this->FrequencyBias( fmeCode.begin(), fmeCode.end(), fmeCode.begin() );
	}

	// write output data
	seqStartOffset = pauseTime;
	return std::move( fmeCode.begin(), fmeCode.end(), fmeCodeBegin );
}



/**	@brief		Generator for FME-code sequences fully conformant to TR-BOS FME.
*	@param		tonesBegin			Iterator to beginning of the container with the tones (i.e. the digits) - for example: {2, 5, 6, 3, 4} for the code 25634. It must have the size of 5.
*	@param		tonesEnd			Iterator to one element after the end of the container with the tones
*	@param		pctLoudness			Loudness of the FME-sequence relative to the maximum possible amplitude of the audio device in percent
*	@param		fmeCodeBegin		Iterator to the beginning of the container with the FME-code sequence. The container must have the size ( samplingFreq * tLength + 1 ) or use std::back_inserter.
*	@return							Iterator to one element after the end of the container with the FME-code sequence
*	@exception	std::length_error	Thrown if the number of tones (digits) is not conformant to the required length (5) of TR-BOS FME
*	@remarks 						Use the method GenerateFMECode(In_It1, In_It1, T, In_It2, In_It3, In_It4, In_It5, bool, TR, bool, std::string, T, Out_It) in order to manipulate the generator parameters
*/
template <class T> template <class In_It, class Out_It> Out_It Core::FME::CProduceFMECode<T>::GenerateFMECode(In_It tonesBegin, In_It tonesEnd, T pctLoudness, Out_It fmeCodeBegin)
{
	using namespace std;
	T SNR;
	bool isWhiteNoise, isBiased;
	vector <T> toneAmp, deltaF, deltaLength, deltaCycle;
	double seqStartOffset;

	// set standard parameters
	isWhiteNoise = false;
	SNR = 0.0f;
	toneAmp.assign( lengthFMECode, 0.0f );
	deltaF.assign( lengthFMECode, 0.0f );
	deltaLength.assign( lengthFMECode, 0.0f );
	deltaCycle.assign( lengthFMECode, 0.0f );
	isBiased = false;

	// generate FME-code with standard parameters
	auto fmeCodeEnd = GenerateFMECode( tonesBegin, tonesEnd, pctLoudness, toneAmp.begin(), deltaF.begin(), deltaLength.begin(), deltaCycle.begin(), isWhiteNoise, SNR, isBiased, fmeCodeBegin, seqStartOffset );

	return fmeCodeEnd;
}



/**	@brief		Function converting the tones (digits) in the FME-sequence into the frequencies.
*	@param		tonesBegin			Iterator to beginning of the container with the tones (i.e. the digits) - for example: {2, 5, 6, 3, 4} for the code 25634. It must have the size of 5.
*	@param		tonesEnd			Iterator to one element after the end of the container with the tones.
*	@param		codeBegin			Iterator to the beginning of the container with the frequency sequence. The container must have the size 5.
*	@return							Iterator to one element after the end of the container with the frequency sequence
*	@exception	std::length_error	Thrown if the number of tones (digits) is not conformant to the required length (5) of TR-BOS FME
*	@exception	std::out_of_range	Thrown if the the tone digits are not corresponding to the stored tone frequencies
*	@remarks 						This function handles tone repetitions "R" and the tone "0"
*/
template <class T> template <class In_It, class Out_It> Out_It Core::FME::CProduceFMECode<T>::ConvertTonesToFreqs( In_It tonesBegin, In_It tonesEnd, Out_It codeBegin)
{
	using namespace std;

	vector<int> tones;
	vector<T> code;
	const int toneZero = 10;
	const int toneRepetition = 11;

	// read input data
	tones.assign( tonesBegin, tonesEnd );
	if ( tones.size() != lengthFMECode ) {
		std::length_error("The FME code must have a length of " + boost::lexical_cast<string>( lengthFMECode ) + "." );
	}

	// handle tone "0"
	replace( tones.begin(), tones.end(), 0, toneZero );

	// handle tone repetitions (tone "R")
	for ( size_t i=1; i < tones.size(); i++ ) {
		if ( tones[i] == tones[i-1] ) {
			tones[i] = toneRepetition;
		}
	}
	
	// convert to frequencies
	code.resize( tones.size() );
	for (size_t i=0; i < code.size(); i++) {
		code[i] = toneFreq.at( tones[i] - 1 );	// an out-of-range exception might be thrown
	}


	// set output data
	return std::move( code.begin(), code.end(), codeBegin );
}



/**	@brief		Load FME parameters according to TR-BOS FME.
*	@param		fmeFileName								Name of data file with the full absolute path
*	@param		params									Parameters for FME-code generation
*	@return 											None
*	@exception 	std::ios_base::failure					Thrown if parameter file cannot be read
*	@remarks 											None
*/
template <class T> void Core::FME::CProduceFMECode<T>::LoadParameters(std::string fmeFileName, CFMEGenerateParam &params)
{
	// deserialize parameter data from file
	std::ifstream ifs( fmeFileName );
	boost::archive::text_iarchive ia( ifs );

	// read parameters
	if ( !ifs.eof() ) {
		ia >> params;
	} else {
		throw std::ios_base::failure("Parameter file cannot be read.");
	}

	ifs.close();
}



/**	@brief		Save FME parameters according to TR-BOS FME.
*	@param		fmeFileName								Name of data file (*.dat)
*	@param		params									Parameters for FME-code generation
*	@return 											None
*	@exception 											None
*	@remarks 											None
*/
template <class T> void Core::FME::CProduceFMECode<T>::SaveParameters(std::string fmeFileName, CFMEGenerateParam params)
{
	// initialize serialization
	std::ofstream ofs( fmeFileName );
	boost::archive::text_oarchive oa( ofs );

	// serialize parameter object
	const CFMEGenerateParam constParams = params; // workaround
	oa << constParams;

	ofs.close();
}