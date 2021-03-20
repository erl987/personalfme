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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include "FMEAnalysisParam.h"


/** @brief	Standard constructor.
*/
Core::FME::CFMEAnalysisParam::CFMEAnalysisParam(void)
{
}



/** @brief	Destructor.
*/
Core::FME::CFMEAnalysisParam::~CFMEAnalysisParam(void)
{
}



/**	@brief		Setting the class variables.
*	@param		codeLength				Required length of a sequence code (5 for 5-tone-sequence)
*	@param		excessTime				This time [s] increases the length of the possible tone to ensure save detection
*	@param		deltaTMaxTwice			Two following tones with a larger distance of the start times are not considered as repetitions [s]
*	@param		minLength				Minimum length of a tone [s]
*	@param		maxLength				Maximum length of a tone [s]
*	@param		maxToneLevelRatio		Limit of the allowed ratio between the absolute signal level of the first tone compared to all other tones of a sequence (i.e.: 1 / maxToneLevelRatio <= level <= maxToneLevelRatio) [-]
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Core::FME::CFMEAnalysisParam::Set(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio )
{
	CFMEAnalysisParam::codeLength = codeLength;
	CFMEAnalysisParam::excessTime = excessTime;
	CFMEAnalysisParam::deltaTMaxTwice = deltaTMaxTwice;
	CFMEAnalysisParam::minLength = minLength;
	CFMEAnalysisParam::maxLength = maxLength;
	CFMEAnalysisParam::maxToneLevelRatio = maxToneLevelRatio;
}



/**	@brief		Getting the class variables.
*	@param		codeLength				Required length of a sequence code (5 for 5-tone-sequence)
*	@param		excessTime				This time [s] increases the length of the possible tone to ensure save detection
*	@param		deltaTMaxTwice			Two following tones with a larger distance of the start times are not considered as repetitions [s]
*	@param		minLength				Minimum length of a tone [s]
*	@param		maxLength				Maximum length of a tone [s]
*	@param		maxToneLevelRatio		Limit of the allowed ratio between the absolute signal level of the first tone compared to all other tones of a sequence (i.e.: 1 / maxToneLevelRatio <= level <= maxToneLevelRatio) [-]
*	@return								None
*	@exception							None
*	@remarks							None
*/
void Core::FME::CFMEAnalysisParam::Get(int& codeLength, double& excessTime, double& deltaTMaxTwice, double& minLength, double& maxLength, double& maxToneLevelRatio )
{
	codeLength = CFMEAnalysisParam::codeLength;
	excessTime = CFMEAnalysisParam::excessTime;
	deltaTMaxTwice = CFMEAnalysisParam::deltaTMaxTwice;
	minLength = CFMEAnalysisParam::minLength;
	maxLength = CFMEAnalysisParam::maxLength;
	maxToneLevelRatio = CFMEAnalysisParam::maxToneLevelRatio;
}
