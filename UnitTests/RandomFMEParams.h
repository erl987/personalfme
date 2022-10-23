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
#include <functional>

/*@{*/
/** \ingroup UnitTests
*/

/*@{*/
/** \ingroup FMEdetectionTests
*/

namespace FMEdetectionTests {

	/**	\ingroup FMEdetectionTests
	*	Class producing random FME code deviations (used for generating random FME codes)
	*/
	class CRandomFMEParams
	{
	public:
		CRandomFMEParams(bool isAllTonesIdentical, int lengthCode, int minCodeDigit, int maxCodeDigit, float minToneAmp, float maxToneAmp, float minDeltaF, float maxDeltaF, float minDeltaLength, float maxDeltaLength, float minDeltaCycle, float maxDeltaCycle, float minSNR, float maxSNR);
		virtual ~CRandomFMEParams(void) {};
		template <class OutIt1, class OutIt2, class OutIt3, class OutIt4, class OutIt5> void DesignParams(OutIt1 codeFirst, OutIt2 toneAmpFirst, OutIt3 deltaFfirst, OutIt4 deltaLengthFirst, OutIt5 deltaCycleFirst, float& SNR);
	protected:
		std::function< int() > randomCode;
		std::function< float() > randomToneAmp;
		std::function< float() > randomDeltaF;
		std::function< float() > randomDeltaLength;
		std::function< float() > randomDeltaCycle;
		std::function< float() > randomSNR;
		bool isAllTonesIdentical;
		int lengthCode;
	};
}
/*@}*/
/*@}*/



/** @brief		Producing random FME code deviations
*/
template <class OutIt1, class OutIt2, class OutIt3, class OutIt4, class OutIt5> void FMEdetectionTests::CRandomFMEParams::DesignParams(OutIt1 codeFirst, OutIt2 toneAmpFirst, OutIt3 deltaFfirst, OutIt4 deltaLengthFirst, OutIt5 deltaCycleFirst, float& SNR)
{
	using namespace std;
	float currDeltaLength, currDeltaCycle;
	vector<int> code;
	vector<float> toneAmp, deltaF, deltaLength, deltaCycle;

	// set with random values
	if ( !isAllTonesIdentical ) {
		for (int i=0; i < lengthCode; i++) {
			toneAmp.push_back( randomToneAmp() );
			deltaF.push_back( randomDeltaF() );
			currDeltaLength = randomDeltaLength();
			currDeltaCycle = randomDeltaCycle();
			// ensure that the cycle length is always at least as long as the tone length
			if ( currDeltaCycle > currDeltaLength ) {
				deltaLength.push_back( currDeltaLength );
				deltaCycle.push_back( currDeltaCycle );
			} else {
				deltaLength.push_back( currDeltaLength );
				deltaCycle.push_back( currDeltaLength );			
			}
		}
	} else {
		toneAmp.assign( lengthCode, randomToneAmp() );
		deltaF.assign( lengthCode, randomDeltaF() );
		currDeltaLength = randomDeltaLength();
		currDeltaCycle = randomDeltaCycle();
		// ensure that the cycle length is always at least as long as the tone length
		if ( currDeltaCycle > currDeltaLength ) {
			deltaLength.assign( lengthCode, currDeltaLength );
			deltaCycle.assign( lengthCode, currDeltaCycle );
		} else {
			deltaLength.assign( lengthCode, currDeltaLength );
			deltaCycle.assign( lengthCode, currDeltaLength );			
		}
	}
	for (int i=0; i < lengthCode; i++) {
		code.push_back( randomCode() );
	}
	SNR = randomSNR();

	// set output values
	move( code.begin(), code.end(), codeFirst );
	move( toneAmp.begin(), toneAmp.end(), toneAmpFirst );
	move( deltaF.begin(), deltaF.end(), deltaFfirst );
	move( deltaLength.begin(), deltaLength.end(), deltaLengthFirst );
	move( deltaCycle.begin(), deltaCycle.end(), deltaCycleFirst );
}
