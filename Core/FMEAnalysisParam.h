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
#pragma once

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
		*	Class representing parameters for 5-tone-sequence evaluation
		*/
		class CFMEAnalysisParam
		{
		public:
			AUDIOSP_API CFMEAnalysisParam(void);
			AUDIOSP_API ~CFMEAnalysisParam(void);
			template <class Archive> void serialize(Archive & ar, const unsigned int version);
			AUDIOSP_API void Set(int codeLength, double excessTime, double deltaTMaxTwice, double minLength, double maxLength, double maxToneLevelRatio);
			AUDIOSP_API void Get(int& codeLength, double& excessTime, double& deltaTMaxTwice, double& minLength, double& maxLength, double& maxToneLevelRatio);
		private:
			int codeLength;
			double excessTime;
			double deltaTMaxTwice;
			double minLength;
			double maxLength;
			double maxToneLevelRatio;
		};
	}
}
/*@}*/


/**	@brief		Serialization using boost::serialize
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <class Archive> void Core::FME::CFMEAnalysisParam::serialize(Archive & ar, const unsigned int version)
{
	// serialize all parameters
	ar & codeLength;
	ar & excessTime;
	ar & deltaTMaxTwice;
	ar & minLength;
	ar & maxLength;
	ar & maxToneLevelRatio;
}