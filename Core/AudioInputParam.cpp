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

#include "AudioInputParam.h"


/** @brief	Standard constructor.
*/
Core::CAudioInputParam::CAudioInputParam(void)
{
}



/** 	@brief	Destructor.
*/
Core::CAudioInputParam::~CAudioInputParam(void)
{
}



/**	@brief		Setting the class variables.
*	@param	sampleLength					Length of timestep for sample acquisition [s]
*	@param	numChannels						Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param	maxLengthInputQueue				Maximum senseful input queue length
*	@param	maxMissedAttempts				Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param	channel							Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'
*	@param	parameterFileName				File name of general code analysis settings file. The directory has to be given relative to the current parameter file.
*	@param	specializedParameterFileName	File name of FME code analysis settings file. The directory has to be given relative to the current parameter file.
*	@param	maxRequiredProcFreq				Maximum frequency required for useful process data stream [Hz]. It is the frequency where the main leaf of the downsampling filter has the minimum attenuation of the stop band (i.e. that of first side leaf) and differs from the highest tone frequency due to the sharpness of the filter.
*	@param	transWidthProc					Transition width of the audio processing filter [Hz]
*	@param	transWidthRec					Transition width of the audio recording filter [Hz]
*	@param	mainThreadCycleTime				Time between two cycles of the main thread controlling the full audio signal evaluation [s]
*	@param	standardSamplingFreqs			Vector container storing all sampling frequencies that might be used for audio processing [Hz]
*	@return									None
*	@exception								None
*	@remarks								None
*/
void Core::CAudioInputParam::Set(double sampleLength, int numChannels, int maxLengthInputQueue, int maxMissedAttempts, int channel, std::string parameterFileName, std::string specializedParameterFileName, double maxRequiredProcFreq, double transWidthProc, double transWidthRec, float mainThreadCycleTime, std::vector<double> standardSamplingFreqs)
{
	CAudioInputParam::sampleLength = sampleLength;	
	CAudioInputParam::numChannels = numChannels;
	CAudioInputParam::maxLengthInputQueue = maxLengthInputQueue;
	CAudioInputParam::maxMissedAttempts = maxMissedAttempts;
	CAudioInputParam::channel = channel;
	CAudioInputParam::parameterFileName = parameterFileName;
	CAudioInputParam::specializedParameterFileName = specializedParameterFileName;
	CAudioInputParam::maxRequiredProcFreq = maxRequiredProcFreq;
	CAudioInputParam::transWidthProc = transWidthProc;
	CAudioInputParam::transWidthRec = transWidthRec; 
	CAudioInputParam::mainThreadCycleTime = mainThreadCycleTime;
	CAudioInputParam::standardSamplingFreqs = standardSamplingFreqs;
}



/**	@brief		Getting the class variable.
*	@param	sampleLength					Length of timestep for sample acquisition [s]
*	@param	numChannels						Number of channels required for the audio input (mono = 1, stereo = 2), usually choose 1. At least 1 is supported by all sound devices.
*	@param	maxLengthInputQueue				Maximum senseful input queue length
*	@param	maxMissedAttempts				Number of synchronization attempts in audio acquisition before synchronization is forced
*	@param	channel							Channel number for reading the audio input (first channel is 1), usually choose 1. It cannot be larger than the number of channels 'numChannels'
*	@param	parameterFileName				File name of general code analysis settings file. The directory is given relative to the current parameter file.
*	@param	specializedParameterFileName	File name of FME code analysis settings file. The directory is given relative to the current parameter file.
*	@param	maxRequiredProcFreq				Maximum frequency required for useful process data stream [Hz]. It is the frequency where the main leaf of the downsampling filter has the minimum attenuation of the stop band (i.e. that of first side leaf) and differs from the highest tone frequency due to the sharpness of the filter.
*	@param	transWidthProc					Transition width of the audio processing filter [Hz]
*	@param	transWidthRec					Transition width of the audio recording filter [Hz]
*	@param	mainThreadCycleTime				Time between two cycles of the main thread controlling the full audio signal evaluation [s]
*	@param	standardSamplingFreqs			Vector container storing all sampling frequencies that might be used for audio processing [Hz]
*	@return									None
*	@exception								None
*	@remarks								None
*/
void Core::CAudioInputParam::Get(double& sampleLength, int& numChannels, int& maxLengthInputQueue, int& maxMissedAttempts, int& channel, std::string& parameterFileName, std::string& specializedParameterFileName, double& maxRequiredProcFreq, double& transWidthProc, double& transWidthRec, float& mainThreadCycleTime, std::vector<double>& standardSamplingFreqs)
{
	sampleLength = CAudioInputParam::sampleLength;	
	numChannels = CAudioInputParam::numChannels;
	maxLengthInputQueue = CAudioInputParam::maxLengthInputQueue;
	maxMissedAttempts = CAudioInputParam::maxMissedAttempts;
	channel = CAudioInputParam::channel;
	parameterFileName = CAudioInputParam::parameterFileName;
	specializedParameterFileName = CAudioInputParam::specializedParameterFileName;
	maxRequiredProcFreq = CAudioInputParam::maxRequiredProcFreq;
	transWidthProc = CAudioInputParam::transWidthProc;
	transWidthRec = CAudioInputParam::transWidthRec; 
	mainThreadCycleTime = CAudioInputParam::mainThreadCycleTime;
	standardSamplingFreqs = CAudioInputParam::standardSamplingFreqs;
}