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
#include <utility>
#include <memory>
#include "AlarmMessage.h"
#include "Validity.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef NETWORKING_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define NETWORKING_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define NETWORKING_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define NETWORKING_API __attribute__ ((visibility ("default")))
	#else
		#define NETWORKING_API
	#endif		
#endif


/*@{*/
/** \ingroup Networking
*/
namespace External {
	namespace Types {
		/** \ingroup Networking
		*	Typedef for the alarm validities set datatype
		*/
		typedef std::pair< std::shared_ptr< External::Validities::CValidity >, std::vector< std::shared_ptr< External::CAlarmMessage > > > BaseType;

		/** \ingroup Networking
		*	Typedef for the alarm validities set type
		*/
		typedef std::vector< BaseType > AlarmValiditiesType;

		/** \ingroup Networking
		*	Typedef for the alarm validities set iterator type
		*/
		typedef AlarmValiditiesType::iterator AlarmValiditiesTypeIterator;

		/** \ingroup Networking
		*	Typedef for the alarm validities set const iterator type
		*/
		typedef AlarmValiditiesType::const_iterator AlarmValiditiesTypeConstIterator;
	}



	/**	\ingroup Networking
	*	Class implementing a dataset containing the exception time validities and the alarm data. The stored validities are guaranteed to be unique.
	*/
	class CAlarmValidities
	{
	public:
		NETWORKING_API CAlarmValidities(void) {};
		template <typename InIt> CAlarmValidities(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd);
		NETWORKING_API CAlarmValidities( const CAlarmValidities& );
		NETWORKING_API CAlarmValidities& operator=( const CAlarmValidities& );
		NETWORKING_API virtual ~CAlarmValidities(void) {};
		template <typename InIt> void AddEntry(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd);
		NETWORKING_API virtual void RemoveEntry(const std::shared_ptr< Validities::CValidity >& validitySet);
		template <typename InIt> void ReplaceEntry(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd);
		template <typename OutIt> OutIt GetEntry(const std::shared_ptr< Validities::CValidity >& validitySet, OutIt alarmMessagesBegin) const;
		NETWORKING_API virtual bool IsEntry(const std::shared_ptr< Validities::CValidity >& validitySet) const;
		NETWORKING_API virtual Types::AlarmValiditiesType GetAllEntries(void) const;
		NETWORKING_API virtual void Clear(void);
		NETWORKING_API virtual int Size(void) const;
		NETWORKING_API friend bool operator==(const CAlarmValidities& lhs, const CAlarmValidities& rhs);
		NETWORKING_API friend bool operator!=(const CAlarmValidities& lhs, const CAlarmValidities& rhs);
	protected:
		NETWORKING_API virtual Types::AlarmValiditiesTypeConstIterator FindEntry(const std::shared_ptr< Validities::CValidity >& validitySet) const;
		NETWORKING_API virtual Types::AlarmValiditiesTypeIterator FindEntry(const std::shared_ptr< Validities::CValidity >& validitySet);
		NETWORKING_API void CopyFrom( const CAlarmValidities& src );
	private:
		Types::AlarmValiditiesType alarmValidities;
	};
}
/*@}*/

namespace External {
	NETWORKING_API bool operator==(const CAlarmValidities& lhs, const CAlarmValidities& rhs);
	NETWORKING_API bool operator!=(const CAlarmValidities& lhs, const CAlarmValidities& rhs);
}


/**	@brief		Constructor	
*	@param		validitySet						Smart pointer to the validity set containing the validity time information
*	@param		alarmMessagesBegin				Iterator to the beginning of the alarm messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@param		alarmMessagesEnd				Iterator past the end of the messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
template <typename InIt>
External::CAlarmValidities::CAlarmValidities(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd)
	: alarmValidities( 1, std::make_pair( validitySet, std::vector< std::shared_ptr< External::CAlarmMessage > >( alarmMessagesBegin, alarmMessagesEnd ) ) )
{
	if ( !alarmValidities.front().first ) {
		throw std::runtime_error( "The validity set is empty." );
	}
}



/**	@brief		Returns the alarm data for a certain validity set
*	@param		validitySet						Smart pointer to the validity set for which the alarm set is required
*	@param		alarmMessagesBegin				Iterator to the beginning of the container holding the alarm data corresponding to the 'validitySet'. Use std::back_inserter.
*	@return										Iterator past the end of the alarm messages container
*	@exception	std::logic_error				Thrown if the requested validity set is not existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
template <typename OutIt>
OutIt External::CAlarmValidities::GetEntry(const std::shared_ptr< Validities::CValidity >& validitySet, OutIt alarmMessagesBegin) const
{
	using namespace std;
	vector< shared_ptr< External::CAlarmMessage > > alarmMessages;

	// search entry
	auto alarmValiditiesIt = FindEntry( validitySet );
	alarmMessages = alarmValiditiesIt->second;

	// set output alarm data
	for (size_t i=0; i < alarmMessages.size(); i++) {
		*(alarmMessagesBegin++) = alarmMessages[i];
	}

	return alarmMessagesBegin;
}



/**	@brief		Add a new entry to the alarm validities set
*	@param		validitySet						Smart pointer to the new validity set
*	@param		alarmMessagesBegin				Iterator to the beginning of the new alarm messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@param		alarmMessagesEnd				Iterator past the end of the new alarm messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@return										None
*	@exception	std::logic_error				Thrown if the validity set is already existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
template <typename InIt>
void External::CAlarmValidities::AddEntry(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd)
{
	using namespace std;
	bool isExisting;
	typename Types::AlarmValiditiesTypeIterator alarmValiditiesIt;
	vector< shared_ptr< External::CAlarmMessage > > alarmMessages( alarmMessagesBegin, alarmMessagesEnd );

	// search entry
	isExisting = true;
	try {
		alarmValiditiesIt = FindEntry( validitySet );
	} catch ( std::logic_error e ) {
		isExisting = false;
	}

	// ensure uniqueness of the datasets
	if ( isExisting ) {
		throw logic_error( "The validity set is already existing" );
	}

	// add the alarm messages
	alarmValidities.push_back( make_pair( validitySet, alarmMessages ) );
}



/**	@brief		Replaces a certain alarm message entry in the alarm validities
*	@param		validitySet						Smart pointer to the validity set whose alarm data should be replaced
*	@param		alarmMessagesBegin				Iterator to the beginning of the new alarm messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@param		alarmMessagesEnd				Iterator past the end of the new alarm messages containing the gateway alarm data (the vector may contain multiple datasets)
*	@return										None
*	@exception	std::logic_error				Thrown if the requested validity set is not existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
template <typename InIt>
void External::CAlarmValidities::ReplaceEntry(const std::shared_ptr< Validities::CValidity >& validitySet, InIt alarmMessagesBegin, InIt alarmMessagesEnd)
{
	using namespace std;
	vector< shared_ptr< External::CAlarmMessage > > alarmMessages( alarmMessagesBegin, alarmMessagesEnd );

	// search entry
	auto alarmValiditiesIt = FindEntry( validitySet );

	// replace alarm data of the entry
	alarmValiditiesIt->second = alarmMessages;
}