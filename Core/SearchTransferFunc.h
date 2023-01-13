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
#include "libalglib/optimization.h"
#include "DataProcessing.h"

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
	namespace Processing {
		namespace Filter {
			namespace Solver {
				AUDIOSP_API void GetFuncVal(const alglib::real_1d_array &currFilterOrder, alglib::real_1d_array &currTransWidthResidual, void *ptr);
				template <class T> int FindTransitionWidth(const int& startFilterOrder, const T& goalTransWidth, const T& cutoffFreq, const T& samplingFreq, const int& maxFilterOrder, const double& diffStep = 1e-4, const int& itMax = 500, const double& minResValue = 1e-10, const double& minResFunction = 1e-10, const double& minResGradient = 1e-10);
				AUDIOSP_API extern double samplingFreq;
				AUDIOSP_API extern double normCutoffFreq;
				AUDIOSP_API extern double goalTransWidth;
				AUDIOSP_API extern double minFilterOrder;
			}
		}
	}
}

/*@}*/



/**	@brief		Finding a FIR-filter fulfilling the given transition width
*	@param		startFilterOrder			Filter order used for starting the algorithm. If the start value is inappropriate, the resulting filter might be in a local minimum and thus wrong.
*	@param		goalTransWidth				Required width of the transition zone of the filter [Hz]
*	@param		cutoffFreq					Cutoff frequency of the (real) low-pass filter [Hz]. It is the frequency where the main leaf of the filter has the minimum attenuation of the stop band (i.e. that of first side leaf).
*	@param		samplingFreq				Sampling frequency [Hz]
*	@param		maxFilterOrder				Maximum filter order allowed
*	@param		diffStep					Frequency stepping used for solving the nonlinear filter response function equation [Hz]. The default value is 1e-4.
*	@param		itMax						Maximum number of iterations for solving the nonlinear equation. The default value is 500.
*	@param		minResValue					Minimum required step residuum (stopping criterion in solving the nonlinear equation, see Alglib documentation for details). The default value is 1e-10.
*	@param		minResFunction				Minimum required function residuum (stopping criterion in solving the nonlinear equation, see Alglib documentation for details). The default value is 1e-10.
*	@param		minResGradient				Minimum required gradient residuum (stopping criterion in solving the nonlinear equation, see Alglib documentation for details). The default value is 1e-10.
*	@return									Filter order for which the chosen filter type gives a transition width equal to 'goalTransWidth'. This order is always even. A small deviation might occur because due to the discrete and even filter order an exact matching of the transition width might not be possible.
*	@exception	std::domain_error			Thrown if the maximum filter order is smaller than the minimum filter order required for realizing the given cutoff frequency
*	@exception	std::range_error			Thrown if the relative cutoff frequency is larger than 0.5 of sampling frequency
*	@remarks								None
*/
template <typename T>
int Core::Processing::Filter::Solver::FindTransitionWidth(const int& startFilterOrder, const T& goalTransWidth, const T& cutoffFreq, const T& samplingFreq, const int& maxFilterOrder, const double& diffStep, const int& itMax, const double& minResValue, const double& minResFunction, const double& minResGradient)
{
	using namespace std;
	using namespace alglib;
	int finalFilterOrder;
	double realNormCutoffFreq;
	minlmstate state;
    minlmreport optimizationReport;
	real_1d_array filterOrder, lowerBoundFilterOrder, upperBoundFilterOrder;

	// set up the optimization function
	realNormCutoffFreq = cutoffFreq / samplingFreq * 2.0;

        if (realNormCutoffFreq > 1.0) {
            throw std::range_error("Relative cutoff frequency larger than 0.5 of sampling frequency");
        }

	filterOrder.setlength( 1 );
	filterOrder(0) = static_cast<double>( startFilterOrder );
	lowerBoundFilterOrder.setlength( 1 );
	lowerBoundFilterOrder(0) = CDataProcessing<double>::NextLargerEven( 3.3 / realNormCutoffFreq - 1 ) + 2; // minimum order leading definitively to an ideal filter cutoff frequency larger than zero
	upperBoundFilterOrder.setlength( 1 );
	upperBoundFilterOrder(0) = static_cast<double>( maxFilterOrder );
	if ( lowerBoundFilterOrder(0) > upperBoundFilterOrder(0) ) {
		throw std::domain_error( "The cutoff frequency cannot be realized for the given maximum filter order." );
	}

	Solver::normCutoffFreq = realNormCutoffFreq;
	Solver::samplingFreq = samplingFreq;
	Solver::goalTransWidth = goalTransWidth;
	Solver::minFilterOrder = lowerBoundFilterOrder(0);

	// solve nonlinear transfer function problem
	minlmcreatev( 1, filterOrder, diffStep, state );
	minlmsetbc( state, lowerBoundFilterOrder, upperBoundFilterOrder );
	minlmsetcond( state, 0, 0 );
	minlmoptimize( state, GetFuncVal );
    minlmresults( state, filterOrder, optimizationReport );  
	finalFilterOrder = CDataProcessing<double>::MakeEven( filterOrder(0) ); // the result must be always even

	return finalFilterOrder;
}
