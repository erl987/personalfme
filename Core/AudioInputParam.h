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
#include <string>
#include <boost/serialization/nvp.hpp>

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
	/** \ingroup Core
	*	Class representing parameters for setting the audio device.
	*/
	class CAudioInputParam
	{
	public:
		AUDIOSP_API CAudioInputParam(void);
		AUDIOSP_API ~CAudioInputParam(void);
		template <class Archive> void serialize(Archive & ar, const unsigned int version);
		AUDIOSP_API void Set(double sampleLength, int numChannels, int maxLengthInputQueue, int maxMissedAttempts, int channel, std::string parameterFileName, std::string specializedParameterFileName, double maxRequiredProcFreq, double transWidthProc, double transWidthRec, float mainThreadCycleTime, std::vector<double> standardSamplingFreqs);
		AUDIOSP_API void Get(double& sampleLength, int& numChannels, int& maxLengthInputQueue, int& maxMissedAttempts, int& channel, std::string& parameterFileName, std::string& specializedParameterFileName, double& maxRequiredProcFreq, double& transWidthProc, double& transWidthRec, float& mainThreadCycleTime, std::vector<double>& standardSamplingFreqs);
	private:
		double sampleLength;	
		int numChannels;
		int maxLengthInputQueue;
		int maxMissedAttempts;
		int channel;
		std::string parameterFileName;
		std::string specializedParameterFileName;
		double maxRequiredProcFreq;
		double transWidthProc;
		double transWidthRec;
		float mainThreadCycleTime;
		std::vector<double> standardSamplingFreqs;
	};
}
/*@}*/


/**	@brief		Serialization using boost::serialize
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <class Archive> void Core::CAudioInputParam::serialize(Archive & ar, const unsigned int version)
{
	// serialize all parameters
	ar & BOOST_SERIALIZATION_NVP( sampleLength );	
	ar & BOOST_SERIALIZATION_NVP( numChannels );
	ar & BOOST_SERIALIZATION_NVP( maxLengthInputQueue );
	ar & BOOST_SERIALIZATION_NVP( maxMissedAttempts );
	ar & BOOST_SERIALIZATION_NVP( channel );
	ar & BOOST_SERIALIZATION_NVP( parameterFileName );
	ar & BOOST_SERIALIZATION_NVP( specializedParameterFileName );
	ar & BOOST_SERIALIZATION_NVP( maxRequiredProcFreq );
	ar & BOOST_SERIALIZATION_NVP( transWidthProc );
	ar & BOOST_SERIALIZATION_NVP( transWidthRec );
	ar & BOOST_SERIALIZATION_NVP( mainThreadCycleTime );
	ar & BOOST_SERIALIZATION_NVP( standardSamplingFreqs );
}
