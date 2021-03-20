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

#include <boost/serialization/vector.hpp>
#include "SerializableDateTime.h"
#include "SeqData.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef UTILITY_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define UTILITY_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define UTILITY_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define UTILITY_API __attribute__ ((visibility ("default")))
	#else
		#define UTILITY_API
	#endif		
#endif

/*@{*/
/** \ingroup Utilities
*/


namespace Utilities {
	/**	\ingroup Utilities
	*	Serializable class representing the basic data of a code sequence
	*/
	class CSerializableSeqData : public CSeqData
	{
	public:	
		UTILITY_API CSerializableSeqData(void);
		UTILITY_API CSerializableSeqData(const CSeqData& data);
		UTILITY_API CSerializableSeqData(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString);
		UTILITY_API virtual ~CSerializableSeqData(void);
		template <typename Archive> void serialize(Archive & ar, const unsigned int version);
	};
}
/*@}*/



/**	@brief		Serialization using boost::serialize
*	@param		ar						See Boost Serialize documentation for details
*	@param		version					See Boost Serialize documentation for details
*	@return								None
*	@exception							None
*	@remarks							See boost::serialize for details
*/
template <typename Archive> void Utilities::CSerializableSeqData::serialize(Archive& ar, const unsigned int version) {
	using namespace std;

	// convert to serializable data structure
	CSerializableDateTime serializableTime = get<0>( CSeqData::sequenceData );

	ar & serializableTime;
	ar & get<1>( CSeqData::sequenceData );
	ar & get<2>( CSeqData::sequenceData );

	// convert back to original data structure
	CSeqData::sequenceData = make_tuple( serializableTime, get<1>( CSeqData::sequenceData ), get<2>( CSeqData::sequenceData ) );
}
