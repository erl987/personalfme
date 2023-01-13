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
	namespace General {
		/** \ingroup Core
		*	Class representing parameters for 5-tone-sequence evaluation
		*/
		class CAnalysisParam
		{
		public:
			AUDIOSP_API CAnalysisParam(void);
			AUDIOSP_API ~CAnalysisParam(void);
			template <class Archive> void serialize(Archive & ar, const unsigned int version);
			template <class In_It> void Set(double sampleLength, double sampleLengthCoarse, int maxNumPeaks, int maxNumPeaksCoarse, int freqResolution, int freqResolutionCoarse, double maxDeltaF, double overlap, double overlapCoarse, double delta, double deltaCoarse, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, double searchTimestep, In_It searchFreqsFirst, In_It searchFreqsLast);
			template <class Out_It> void Get(double& sampleLength, double& sampleLengthCoarse, int& maxNumPeaks, int& maxNumPeaksCoarse, int& freqResolution, int& freqResolutionCoarse, double& maxDeltaF, double& overlap, double& overlapCoarse, double& delta, double& deltaCoarse, double& maxFreqDevConstrained, double& maxFreqDevUnconstrained, int& numNeighbours, double& evalToneLength, double& searchTimestep, Out_It searchFreqsFirst);
		private:
			double sampleLength;
			double sampleLengthCoarse;
			int maxNumPeaks;
			int maxNumPeaksCoarse;
			int freqResolution;
			int freqResolutionCoarse;
			double maxDeltaF;
			double overlap;
			double overlapCoarse;
			double delta;
			double deltaCoarse;
			double maxFreqDevConstrained;
			double maxFreqDevUnconstrained;
			int numNeighbours;
			double evalToneLength;
			double searchTimestep;
			std::vector<double> searchFreqs;
		};
	}
}
/*@}*/


/**	@brief		Serialization using boost::serialize
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <class Archive> void Core::General::CAnalysisParam::serialize(Archive & ar, const unsigned int version)
{
	// serialize all parameters
	ar & sampleLength;
	ar & sampleLengthCoarse;
	ar & maxNumPeaks;
	ar & maxNumPeaksCoarse;
	ar & freqResolution;
	ar & freqResolutionCoarse;
	ar & maxDeltaF;
	ar & overlap;
	ar & overlapCoarse;
	ar & delta;
	ar & deltaCoarse;
	ar & maxFreqDevConstrained;
	ar & maxFreqDevUnconstrained;
	ar & numNeighbours;
	ar & evalToneLength;
	ar & searchTimestep;
	ar & searchFreqs;
}



/**	@brief		Setting the class variables.
*	@param		sampleLength				Length of a single section used for spectrogram calculation with good time and low frequency resolution [ms].
*	@param		sampleLengthCoarse			Length of a single section used for spectrogram calculation with low time and good frequency resolution [ms].
*	@param		maxNumPeaks					Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good time resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		maxNumPeaksCoarse			Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good frequency resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		freqResolution				Frequency resolution, given in number of samples for good time resolution analysis
*	@param		freqResolutionCoarse		Frequency resolution given in umber of samples for low time resolution analysis
*	@param		maxDeltaF					Relative maximum allowed deviation of a certain frequency for detection as a certain tone [%/100]
*	@param		overlap						Relative overlap in the good time resolution spectrogram [%/100]
*	@param		overlapCoarse				Relative overlap in the good frequency resolution spectrogram [%/100]
*	@param		delta						Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good time resolution stream)
*	@param		deltaCoarse					Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good frequency resolution stream)
*	@param		maxFreqDevConstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if a neighboring tone exists [%/100 of the nominal frequency distances]
*	@param		maxFreqDevUnconstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if no neighboring tone exists [fraction of the nominal frequency]
*	@param		numNeighbours				Number of neighboring timesteps (forward and backward) considered as a tone, if a tone is detected at a certain timestep (required for high frequency, low time resolution stream)	
*	@param		evalToneLength				Length of the evalulation period for tone search (in s)
*	@param		searchTimestep				Step time for main analysis thread loop in class CSearch [s]
*	@param		searchFreqsFirst			Iterator to beginning of container with all tone frequencies [Hz].	
*	@param		searchFreqsLast				Iterator to one element after the end of the container with all tone frequencies [Hz].
*	@return									None
*	@exception								None
*	@remarks								None
*/
template <class In_It> void Core::General::CAnalysisParam::Set(double sampleLength, double sampleLengthCoarse, int maxNumPeaks, int maxNumPeaksCoarse, int freqResolution, int freqResolutionCoarse, double maxDeltaF, double overlap, double overlapCoarse, double delta, double deltaCoarse, double maxFreqDevConstrained, double maxFreqDevUnconstrained, int numNeighbours, double evalToneLength, double searchTimestep, In_It searchFreqsFirst, In_It searchFreqsLast)
{
	CAnalysisParam::sampleLength = sampleLength;
	CAnalysisParam::sampleLengthCoarse = sampleLengthCoarse;
	CAnalysisParam::maxNumPeaks = maxNumPeaks;
	CAnalysisParam::maxNumPeaksCoarse = maxNumPeaksCoarse;
	CAnalysisParam::freqResolution = freqResolution;
	CAnalysisParam::freqResolutionCoarse = freqResolutionCoarse;
	CAnalysisParam::maxDeltaF = maxDeltaF;
	CAnalysisParam::overlap = overlap;
	CAnalysisParam::overlapCoarse = overlapCoarse;
	CAnalysisParam::delta = delta;
	CAnalysisParam::deltaCoarse = deltaCoarse;
	CAnalysisParam::maxFreqDevConstrained = maxFreqDevConstrained;
	CAnalysisParam::maxFreqDevUnconstrained = maxFreqDevUnconstrained;
	CAnalysisParam::numNeighbours = numNeighbours;
	CAnalysisParam::evalToneLength = evalToneLength;
	CAnalysisParam::searchTimestep = searchTimestep;
	CAnalysisParam::searchFreqs.assign( searchFreqsFirst, searchFreqsLast );
}



/**	@brief		Getting the class variable.
*	@param		sampleLength				Length of a single section used for spectrogram calculation with good time and low frequency resolution [ms].
*	@param		sampleLengthCoarse			Length of a single section used for spectrogram calculation with low time and good frequency resolution [ms].
*	@param		maxNumPeaks					Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good time resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		maxNumPeaksCoarse			Maximum allowed number of frequency peaks at a certain timestep in order to consider a potential tone at this timestep (good frequency resolution stream). A high number of peaks at one single timestep suggests noise, therefore set the value low.
*	@param		freqResolution				Frequency resolution, given in number of samples for good time resolution analysis
*	@param		freqResolutionCoarse		Frequency resolution given in umber of samples for low time resolution analysis
*	@param		maxDeltaF					Relative maximum allowed deviation of a certain frequency for detection as a certain tone [%/100]
*	@param		overlap						Relative overlap in the good time resolution spectrogram [%/100]
*	@param		overlapCoarse				Relative overlap in the good frequency resolution spectrogram [%/100]
*	@param		delta						Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good time resolution stream)
*	@param		deltaCoarse					Threshold value defining minimum distance between the peak and the left neighboring opposed peak (good frequency resolution stream)
*	@param		maxFreqDevConstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if a neighboring tone exists [%/100 of the nominal frequency distances]
*	@param		maxFreqDevUnconstrained		Maximum allowed deviation of a tone frequency from the nominal frequeny (at low frequency resolution), if no neighboring tone exists [fraction of the nominal frequency]
*	@param		numNeighbours				Number of neighboring timesteps (forward and backward) considered as a tone, if a tone is detected at a certain timestep (required for high frequency, low time resolution stream)	
*	@param		evalToneLength				Length of the evalulation period for tone search (in s)
*	@param		searchTimestep				Step time for main analysis thread loop in class CSearch [s]
*	@param		searchFreqsFirst			Iterator to beginning of container with all tone frequencies [Hz]. Use std::back_inserter.
*	@return									None
*	@exception								None
*	@remarks								None
*/
template <class Out_It> void Core::General::CAnalysisParam::Get(double& sampleLength, double& sampleLengthCoarse, int& maxNumPeaks, int& maxNumPeaksCoarse, int& freqResolution, int& freqResolutionCoarse, double& maxDeltaF, double& overlap, double& overlapCoarse, double& delta, double& deltaCoarse, double& maxFreqDevConstrained, double& maxFreqDevUnconstrained, int& numNeighbours, double& evalToneLength, double &searchTimestep, Out_It searchFreqsFirst)
{
	sampleLength = CAnalysisParam::sampleLength;
	sampleLengthCoarse = CAnalysisParam::sampleLengthCoarse;
	maxNumPeaks = CAnalysisParam::maxNumPeaks;
	maxNumPeaksCoarse = CAnalysisParam::maxNumPeaksCoarse;
	freqResolution = CAnalysisParam::freqResolution;
	freqResolutionCoarse = CAnalysisParam::freqResolutionCoarse;
	maxDeltaF = CAnalysisParam::maxDeltaF;
	overlap = CAnalysisParam::overlap;
	overlapCoarse = CAnalysisParam::overlapCoarse;
	delta = CAnalysisParam::delta;
	deltaCoarse = CAnalysisParam::deltaCoarse;
	maxFreqDevConstrained = CAnalysisParam::maxFreqDevConstrained;
	maxFreqDevUnconstrained = CAnalysisParam::maxFreqDevUnconstrained;
	numNeighbours = CAnalysisParam::numNeighbours;
	evalToneLength = CAnalysisParam::evalToneLength;
	searchTimestep = CAnalysisParam::searchTimestep;
	for (std::size_t i=0; i < CAnalysisParam::searchFreqs.size(); i++) {
		*(searchFreqsFirst++) = CAnalysisParam::searchFreqs[i];
	}
}
