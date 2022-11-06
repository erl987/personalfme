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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#include "libalglib/interpolation.h"
#include "FIRfilter.h"
#include "SearchTransferFunc.h"

double Core::Processing::Filter::Solver::samplingFreq;
double Core::Processing::Filter::Solver::normCutoffFreq;
double Core::Processing::Filter::Solver::goalTransWidth;
double Core::Processing::Filter::Solver::minFilterOrder;



/**	@brief		Calculation of the transition width residual (in respect to the goal transition width) for finding the corresponding filter order
*	@param		currFilterOrder				Alglib-array containing the current filter order. The dimension of the array must be 1.
*	@param		currTransWidthResidual		Alglib-array containing the transition width residual. The dimension of the array must be 1.
*	@param		ptr							Not used, only for compatibility with the Alglib-library
*	@return									None
*	@exception								None
*	@remarks								This function is intended for use with the Alglib nonlinear equation solvers. The function gives f(x) for the minimization: F = ( f(x) )^2 -> min
*/
void Core::Processing::Filter::Solver::GetFuncVal(const alglib::real_1d_array &currFilterOrder, alglib::real_1d_array &currTransWidthResidual, void *ptr)
{
	using namespace alglib;
	double currTransWidth;
	spline1dinterpolant interpolationModel;
	real_1d_array filterOrders, transWidths;
	const int deltaEven = 2;
	const int numPoints = 2;

	filterOrders.setlength( numPoints );
	filterOrders(0) = CDataProcessing<double>::NextSmallerEven( currFilterOrder(0) );
	// checks if the chosen filter order is lower than the minimum allowed - due to the constraints of the minimizer, choosing the next larger even filter order is always sufficient
	if ( filterOrders(0) < minFilterOrder ) {
		filterOrders(0) += deltaEven;
	}
	for (int i=1; i < filterOrders.length(); i++) {
		filterOrders(i) = filterOrders(i-1) + deltaEven;
	}

	// calculate the corresponding transition widths
	transWidths.setlength( filterOrders.length() );
	for (int i=0; i < filterOrders.length(); i++) {
		transWidths(i) = CFIRfilter<double>::CalcTransitionWidth( static_cast<int>( filterOrders(i) ), normCutoffFreq, samplingFreq );
	}

	// calculate the residual of the transition width
	spline1dbuildlinear( filterOrders, transWidths, interpolationModel );
	currTransWidth = spline1dcalc( interpolationModel, currFilterOrder(0) );
	currTransWidthResidual(0) = std::abs( currTransWidth * samplingFreq - goalTransWidth );
}
