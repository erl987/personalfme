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

#include <boost/serialization/vector.hpp>
#include <vector>

#if defined _WIN32 || defined __CYGWIN__
	#ifdef AUDIOSP_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define AUDIOSP_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define AUDIOSP_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define AUDIOSP_API __attribute__ ((visibility ("default")))
	#else
		#define AUDIOSP_API
	#endif		
#endif

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace FME {
		/** \ingroup Core
		*	Class representing parameters for FME code generation.
		*/
		class CFMEGenerateParam
		{
		public:
			AUDIOSP_API CFMEGenerateParam(void);
			AUDIOSP_API ~CFMEGenerateParam(void);
			template <class Archive> void serialize(Archive & ar, const unsigned int version);
			template <class InIt1, class InIt2> void Set(double samplingFreq, double tLength, double toneLength, double pauseTime, int lengthFMECode, InIt1 toneFreqBegin, InIt1 toneFreqEnd, InIt2 biasFilterParamsBegin, InIt2 biasFilterParamsEnd);
			template <class OutIt1, class OutIt2> OutIt1 Get(double &samplingFreq, double &tLength, double &toneLength, double &pauseTime, int &lengthFMECode, OutIt1 toneFreqBegin, OutIt2 biasFilterParamsBegin);
		private:
			double samplingFreq;
			double tLength;
			double toneLength;
			double pauseTime;
			int lengthFMECode;
			std::vector<double> toneFreq;
			std::vector<double> biasFilterParams;
		};
	}
}



/**	@brief		Setting the class variables.
*	@param		samplingFreq			Sampling frequency [Hz]
*	@param		tLength					Length of the whole code sequence (must be at least of the length of the code itself) [s]
*	@param		toneLength				Length of each single tone of the code [s]
*	@param		pauseTime				Length of the pause before code repetition [s]
*	@param		lengthFMECode			Number of code digits (must be 5 for FME-code according to TR-BOS)
*	@param		toneFreqBegin			Iterator to beginning of a container with all frequencies of the digits - first elements corresponds to "1", one element before the end is "0" and the last is "R"
*	@param		toneFreqEnd				Iterator to one element after the end of the container with all frequencies of the digits [Hz]
*	@param		biasFilterParamsBegin	Iterator to beginning of the container with the b-filter parameters of the FIR-filter used for simulating limited capabilities of input devices (frequency bias)
*	@param		biasFilterParamsEnd		Iterator to one element after the end of the container with the b-filter parameters of the FIR-filter used for simulating limited capabilities of input devices (frequency bias)
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class InIt1, class InIt2> void Core::FME::CFMEGenerateParam::Set(double samplingFreq, double tLength, double toneLength, double pauseTime, int lengthFMECode, InIt1 toneFreqBegin, InIt1 toneFreqEnd, InIt2 biasFilterParamsBegin, InIt2 biasFilterParamsEnd)
{
	CFMEGenerateParam::samplingFreq = samplingFreq;
	CFMEGenerateParam::tLength = tLength;
	CFMEGenerateParam::toneLength = toneLength;
	CFMEGenerateParam::pauseTime = pauseTime;
	CFMEGenerateParam::lengthFMECode = lengthFMECode;
	CFMEGenerateParam::toneFreq.assign( toneFreqBegin, toneFreqEnd );
	CFMEGenerateParam::biasFilterParams.assign( biasFilterParamsBegin, biasFilterParamsEnd );
}



/**	@brief		Getting the class variables.
*	@param		samplingFreq			Sampling frequency [Hz]
*	@param		tLength					Length of the whole code sequence (must be at least of the length of the code itself) [s]
*	@param		toneLength				Length of each single tone of the code [s]
*	@param		pauseTime				Length of the pause before code repetition [s]
*	@param		lengthFMECode			Number of code digits (must be 5 for FME-code according to TR-BOS)
*	@param		toneFreqBegin			Iterator to beginning of a container with all frequencies of the digits - first elements corresponds to "1", one element before the end is "0" and the last is "R"
*	@param		biasFilterParamsBegin	Iterator to beginning of a container with the b-filter parameters of the FIR-filter used for simulating limited capabilities of input devices (frequency bias)
*	@return								Iterator to one element after the end of the container with all frequencies of the digits [Hz]
*	@exception							None
*	@remarks							None
*/
template <class OutIt1, class OutIt2> OutIt1 Core::FME::CFMEGenerateParam::Get(double &samplingFreq, double &tLength, double &toneLength, double &pauseTime, int &lengthFMECode, OutIt1 toneFreqBegin, OutIt2 biasFilterParamsBegin)
{
	samplingFreq = CFMEGenerateParam::samplingFreq;
	tLength = CFMEGenerateParam::tLength;
	toneLength = CFMEGenerateParam::toneLength;
	pauseTime = CFMEGenerateParam::pauseTime;
	lengthFMECode = CFMEGenerateParam::lengthFMECode;
	
	for (size_t i=0; i < toneFreq.size(); i++) {
		*(toneFreqBegin++) = toneFreq[i];
	}
	for (size_t i=0; i < biasFilterParams.size(); i++) {
		*(biasFilterParamsBegin++) = biasFilterParams[i];
	}

	return toneFreqBegin;
}



/**	@brief		Serialization using boost::serialize
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <class Archive> void Core::FME::CFMEGenerateParam::serialize(Archive & ar, const unsigned int version)
{
	// serialize all parameters
	ar & samplingFreq;
	ar & tLength;
	ar & toneLength;
	ar & pauseTime;
	ar & lengthFMECode;
	ar & toneFreq;
	ar & biasFilterParams;
}