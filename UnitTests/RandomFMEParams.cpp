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
#include <random>
#include <chrono>
#include "RandomFMEParams.h"

/** @brief		Constructor
*/
FMEdetectionTests::CRandomFMEParams::CRandomFMEParams(bool isAllTonesIdentical, int lengthCode, int minCodeDigit, int maxCodeDigit, float minToneAmp, float maxToneAmp, float minDeltaF, float maxDeltaF, float minDeltaLength, float maxDeltaLength, float minDeltaCycle, float maxDeltaCycle, float minSNR, float maxSNR)
	: lengthCode( lengthCode ),
	  isAllTonesIdentical( isAllTonesIdentical )
{
	using namespace std;
	typedef mt19937::result_type seed_type;
	mt19937 generator;

	auto init_seed = static_cast<seed_type>( std::chrono::high_resolution_clock::now().time_since_epoch().count() );

	// initialize random number generators
	generator.seed( init_seed );
	randomCode = bind( uniform_int_distribution<int>( minCodeDigit, maxCodeDigit ), generator );
	generator.seed( init_seed + static_cast<seed_type>( 1 ) );
	randomToneAmp = bind( uniform_real_distribution<float>( minToneAmp, maxToneAmp ), generator );
	generator.seed( init_seed + static_cast<seed_type>( 2 ) );
	randomDeltaF = bind( uniform_real_distribution<float>( minDeltaF, maxDeltaF ), generator );
	generator.seed( init_seed + static_cast<seed_type>( 3 ) );
	randomDeltaLength = bind( uniform_real_distribution<float>( minDeltaLength, maxDeltaLength ), generator );
	generator.seed( init_seed + static_cast<seed_type>( 4 ) );
	randomDeltaCycle = bind( uniform_real_distribution<float>( minDeltaCycle, maxDeltaCycle ), generator );
	generator.seed( init_seed + static_cast<seed_type>( 5 ) );
	randomSNR = bind( uniform_real_distribution<float>( minSNR, maxSNR ), generator );
}
