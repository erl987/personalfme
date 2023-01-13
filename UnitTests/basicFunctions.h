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
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Poco/JSON/Array.h>

/*@{*/
/** \ingroup UnitTests
*/

/** @brief		Loading a vector from a file.
*	@param		fileName				File name of test data
*	@param		data					Data container
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void LoadVector(std::string fileName, std::vector<T>& data)
{
	using namespace std;
	T input;

	// read test data
	ifstream in( fileName );
	if ( !in.is_open() ) {
		throw std::runtime_error( "The data file " + fileName + " is not existing." );
	}

	while ( !in.eof() ) {
		if ( in >> input ) {
			data.push_back( input );
		}
	}
	in.close();
}


/** @brief		Convert a Poco array to an standard library vector.
*	@param		pocoArray				The Poco array
*	@return								None
*	@exception							None
*	@remarks							None
*/
std::vector<double> ToVector(const Poco::JSON::Array::Ptr& array)
{
    std::vector<double> vector;

    vector.reserve(array->size());
    for (int i=0; i < array->size(); i++) {
        vector.push_back(array->get(i));
    }

    return vector;
}


/** @brief		Loading a single number from a file.
*	@param		fileName				File name of test data
*	@param		number					Loaded number. The datatype must be identical to that stored in the file.
*	@return								None
*	@exception							None
*	@remarks							None
*/
template <class T> void LoadSingleNumber(std::string fileName, T& number)
{
	std::ifstream in( fileName );
	in >> number;
	in.close();
}



/**	@brief		Generating random test signals
*/
template <class T, class OutIt1, class OutIt2> void GenerateTestData(const int& length, const T& maxSignal, const double& samplingFreq, OutIt1 timeFirst, OutIt2 signalFirst) {
	using namespace std;
	using namespace boost::posix_time;
	typedef mt19937::result_type seed_type;

	ptime startTime;
	vector<ptime> time;
	vector<T> signal;

	// prepare random number generation
	mt19937 generator( static_cast<seed_type>( std::chrono::high_resolution_clock::now().time_since_epoch().count() ) );
	auto random = bind( uniform_real_distribution<T>( -maxSignal, maxSignal ), generator );

	// generate test data
	time.resize( static_cast<size_t>( length ) );
	signal.resize( static_cast<size_t>( length ) );

	startTime = ptime( microsec_clock::universal_time() );
	for (size_t i=0; i < time.size(); i++) {
		time[i] = startTime + microseconds( static_cast<long>( i / samplingFreq * 1.0e6 ) );
		signal[i] = random();
	}

	// move to output
	std::move( time.begin(), time.end(), timeFirst );
	std::move( signal.begin(), signal.end(), signalFirst );
}

/*@}*/