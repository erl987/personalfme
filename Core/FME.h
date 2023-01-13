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
#include <deque>
#include <map>
#include <tuple>
#include <fstream>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "BoostStdTimeConverter.h"
#include "FMEAnalysisParam.h"
#include "FMESequenceSearch.h"
#include "Search.h"

/*@{*/
/** \ingroup Core
*/
namespace Core {
	namespace FME {
		/**	\ingroup Core
		*	Class for searching for five tone sequences according to TR-BOS in a audio signal stream, implementation of abstract interface CSearch.
		*/	
		template <class T> class CFME :
			public Core::General::CSearch<T>
		{
		public:
			CFME(void);
			CFME(double samplingFreq, std::string parameterFileName, std::string specializedParameterFileName, std::function<void(const std::string&)> runtimeErrorCallback);
			~CFME(void);
			virtual std::deque< Utilities::CSeqDataComplete<T> > GetSequencesDebug(void) override;
			virtual std::deque< Utilities::CSeqData > GetSequences(void) override;
			void SaveFMEParameters(std::string fileName, FME::CFMEAnalysisParam params);
		private:
			void PerformSpecializedCalculation(std::vector< std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, T, T > > tones) override;
			void LoadFMEParameters(std::string fileName, FME::CFMEAnalysisParam &params);

			FME::CFMESequenceSearch<T> fmeSearch;
			CFMEAnalysisParam fmeParams;
		};
	}
}
/*@}*/



/**
* 	@brief		Default constructor.
*/
template <class T> Core::FME::CFME<T>::CFME()
{
}



/**
* 	@brief		Default destructor.
*/
template <class T> Core::FME::CFME<T>::~CFME()
{
	// manual stopping of the analysis thread in the base class - required for prevention of "pure virtual function calls"
	Core::General::CSearch<T>::StopThread();
}



/**	@brief		Constructor for initialization of all parameters.
*	@param		samplingFreq						Sampling frequency [Hz]
*	@param		parameterFileName					File name of parameter file for general sequence search (*.dat), it can be given relative to the current path. It is assumed that all underlying parameter files are located in the same path.
*	@param		specializedParameterFileName		File name of parameter file for FME sequence search (*.dat), it can be given relative to the current path.
*	@param		runtimeErrorCallback				Function, which is called in case of a runtime error during execution. The execution will not stop automatically!
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T> Core::FME::CFME<T>::CFME(double samplingFreq, std::string parameterFileName, std::string specializedParameterFileName, std::function<void(const std::string&)> runtimeErrorCallback)
	: Core::General::CSearch<T>(samplingFreq, parameterFileName, runtimeErrorCallback)
{
	using namespace std;

	int codeLength;
	double excessTime;
	double deltaTMaxTwice;
	double minLength;
	double maxLength;
	double maxToneLevelRatio;
	map<int,T> foundTones;

	// load parameters for FME evaluation from file
	LoadFMEParameters( specializedParameterFileName, fmeParams );

	// obtain code sequences from the tone stream
	fmeParams.Get( codeLength, excessTime, deltaTMaxTwice, minLength, maxLength, maxToneLevelRatio );
	fmeSearch.SetParameters( codeLength, excessTime, deltaTMaxTwice, minLength, maxLength, maxToneLevelRatio, foundTones.begin(), foundTones.end(), runtimeErrorCallback );
	
	// manual starting of analysis thread in the base class - required for prevention of "pure virtual function calls"
	Core::General::CSearch<T>::StartThread();
}



/**	@brief		Putting new tones into the sequence search stream thread
*	@param		tones								Queue container with all newly found tones (tone index, reference start time, calculated start time, calculated stop time tone frequency [Hz], absolute tone level)
*	@return 										None
*	@exception 										None
*	@remarks 										This is the implementation of the virtual base class function
*/
template <class T> void Core::FME::CFME<T>::PerformSpecializedCalculation(std::vector< std::tuple< int, boost::posix_time::ptime, boost::posix_time::ptime, boost::posix_time::ptime, T, T > > tones)
{
	// push data into thread for FME sequence search
	fmeSearch.PutTonesStream( tones.begin(), tones.end() );
}



/**	@brief		Implementation of abstract access function for searching for FME sequences in a signal stream
*	@return 										Queue containing a stream of: ( start time of sequence (DD, MM, YYYY, HH, MM, SS), ( tone number, tone length [s], tone period [s], tone frequency [Hz] ), string containing additional information on the sequence (not used for FME sequences) ).
*	@exception 										None
*	@remarks 										Use this function for obtaining precise information on the detected sequences, otherwise use CFME::GetSequences
*/
template <class T> std::deque< Utilities::CSeqDataComplete<T> > Core::FME::CFME<T>::GetSequencesDebug(void)
{
	using namespace boost::posix_time;
	using namespace std;
	
	Utilities::CDateTime timeset;
	deque< Utilities::CSeqDataComplete<T> > convertedFoundCodes;
	deque< tuple < ptime, Utilities::CCodeData<T> > > foundCodes;

	// obtain all newly found sequences
	fmeSearch.GetSequences( back_inserter( foundCodes ) );

	// convert from Boost Time format
	for (auto itMap = foundCodes.begin(); itMap != foundCodes.end(); itMap++) {
		timeset = Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime( get<0>( *itMap ) );
		convertedFoundCodes.push_back( Utilities::CSeqDataComplete<T>( timeset, get<1>( *itMap ), string() ) ); // the info string is not required for FME codes
	}

	return convertedFoundCodes;
}



/**	@brief		Implementation of abstract access function for searching for FME sequences in a signal stream
*	@return 										Queue containing a stream of start times (DD, MM, YYYY, HH, MM, SS),  corresponding sequences and a string containing additional information on the sequence (not used for FME sequences)
*	@exception 										None
*	@remarks 										Use this function for operational purposes and CMFE::GetSequencesDebug for obtaining precise information on the detected sequences
*/
template <class T> std::deque< Utilities::CSeqData > Core::FME::CFME<T>::GetSequences(void)
{
	using namespace std;

	deque< Utilities::CSeqDataComplete<T> > code;
	deque< Utilities::CSeqData > returnCode;

	// obtain code
	code = GetSequencesDebug();

	// simplify return data
	for (auto itMap = code.begin(); itMap != code.end(); itMap++) {
		returnCode.push_back( Utilities::CSeqData( itMap->GetStartTime(), itMap->GetCodeData().GetTones(), itMap->GetInfoString() ) );
	}

	return returnCode;
}



/**	@brief		Loading FME analysis parameters from file
*	@param		fileName							Parameter file name with the full absolute path
*	@param		params								Parameters
*	@return 										None
*	@exception 				std::ios_base::failure	Thrown if the parameter file cannot be read
*	@remarks 										None
*/
template <class T> void Core::FME::CFME<T>::LoadFMEParameters(std::string fileName, FME::CFMEAnalysisParam &params)
{
	// deserialize parameter data from file
	std::ifstream ifs( fileName );
    boost::archive::text_iarchive ia( ifs );
	
	// read parameters
	if ( !ifs.eof() ) {
		ia >> params;
	} else {
		throw std::ios_base::failure("Parameter file cannot be read.");
	}

	ifs.close();
}



/**	@brief		Saving FME analysis parameters from file
*	@param		fileName							Parameter file name with the full absolute path
*	@param		params								Parameters
*	@return 										None
*	@exception 										None
*	@remarks 										None
*/
template <class T> void Core::FME::CFME<T>::SaveFMEParameters(std::string fileName, FME::CFMEAnalysisParam params)
{
	// initialize serialization
	std::ofstream ofs( fileName );
	boost::archive::text_oarchive oa( ofs );

	// serialize parameter object
	const FME::CFMEAnalysisParam constParams = params; // workaround
	oa << constParams;

	ofs.close();
}
