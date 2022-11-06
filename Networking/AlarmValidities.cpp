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
		#define NETWORKING_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define NETWORKING_API __declspec(dllexport)
	#endif
#endif

#include <algorithm>
#include <stdexcept>
#include "AlarmValidities.h"



/** @brief	Copy constructor.
*	@param	src									Source object
*/
External::CAlarmValidities::CAlarmValidities( const CAlarmValidities& src )
{
	CopyFrom( src );
}



/** @brief	Copy assigment constructor.
*	@param	rhs									Right-hand side object
*/
External::CAlarmValidities& External::CAlarmValidities::operator=( const CAlarmValidities& rhs )
{
	if ( this == &rhs ) {
		return *this;
	}
	Clear();
	CopyFrom( rhs );

	return *this;
}



/** @brief	Helper method for deep copying of the object
*	@param	src									Right-hand side object to be copied into the present object
*	@return										None
*	@exception									None
*	@remarks									This method assumes that the data of the present object is empty
*/
void External::CAlarmValidities::CopyFrom( const CAlarmValidities& src )
{
	Types::AlarmValiditiesType srcDB;
	std::shared_ptr<External::Validities::CValidity> validity;
	std::vector< std::shared_ptr< External::CAlarmMessage > > entries;

	srcDB = src.GetAllEntries();
	alarmValidities.clear();
	for ( auto& entry : srcDB ) {
		validity = entry.first->Clone();

		entries.clear();
		for ( auto& message : entry.second ) {
			entries.push_back( message->Clone() );
		}
		alarmValidities.push_back( std::make_pair( validity, entries ) );
	}
}



/**	@brief		Determines if a certain validity set is existing in the alarm validities set
*	@param		validitySet						Smart pointer to the validity set for which the existence is checked
*	@return										True if the alarm validity set exists, false otherwise
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
bool External::CAlarmValidities::IsEntry(const std::shared_ptr< Validities::CValidity >& validitySet) const
{
	using namespace std;

	// determine if the entry is existing
	try {
		auto alarmValiditiesIt = FindEntry( validitySet );
	} catch ( std::logic_error e ) {
		return false;
	}

	return true;
}



/**	@brief		Clear the alarm validities set completely
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::CAlarmValidities::Clear(void)
{
	alarmValidities.clear();
}



/**	@brief		Returns all entries stored in the alarm validities set
*	@return										Container of the type AlarmValiditiesType<T>::Type holding all entries stored
*	@exception									None
*	@remarks									None
*/
External::Types::AlarmValiditiesType External::CAlarmValidities::GetAllEntries(void) const
{
	using namespace std;
	vector< Types::BaseType > allValidities;

	// obtain all entries
	allValidities.reserve( alarmValidities.size() );
	for (auto allValiditiesIt = alarmValidities.begin(); allValiditiesIt != alarmValidities.end(); allValiditiesIt++) {
		allValidities.push_back( make_pair( allValiditiesIt->first, allValiditiesIt->second ) );
	}

	return allValidities;
}



/**	@brief		Removes a certain entry from the alarm validities set
*	@param		validitySet						Smart pointer to the validity set to be removed
*	@return										None
*	@exception	std::logic_error				Thrown if the requested validity set is not existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
void External::CAlarmValidities::RemoveEntry(const std::shared_ptr< Validities::CValidity >& validitySet)
{
	using namespace std;

	// search entry
	auto alarmValiditiesIt = FindEntry( validitySet );

	// remove entry
	alarmValidities.erase( alarmValiditiesIt );
}



/**	@brief		Finds a certain entry in the alarm validities set
*	@param		validitySet						Smart pointer to the validity to be found
*	@return										Const iterator to the found alarm validity
*	@exception	std::logic_error				Thrown if the requested validity set is not existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
External::Types::AlarmValiditiesTypeConstIterator External::CAlarmValidities::FindEntry(const std::shared_ptr< Validities::CValidity >& validitySet) const
{
	using namespace std;

	// check if the validity set is valid
	if ( !validitySet ) {
		throw std::runtime_error( "The validity set is empty." );	
	}

	// search entry	
	auto allValiditiesIt = find_if( alarmValidities.cbegin(), alarmValidities.cend(), [=]( Types::BaseType val ){ return ( *( val.first ) == *validitySet ); } );
	
	// determine if the entry was found
	if ( allValiditiesIt == alarmValidities.end() ) {
		throw std::logic_error( "The validity set is not existing." );
	}

	return allValiditiesIt;
}



/**	@brief		Finds a certain entry in the alarm validities set
*	@param		validitySet						Smart pointer to the validity to be found
*	@return										Iterator to the found alarm validity
*	@exception	std::logic_error				Thrown if the requested validity set is not existing
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
External::Types::AlarmValiditiesTypeIterator External::CAlarmValidities::FindEntry(const std::shared_ptr< Validities::CValidity >& validitySet)
{
	using namespace std;

	// check if the validity set is valid
	if ( !validitySet ) {
		throw std::runtime_error( "The validity set is empty." );	
	}

	// search entry
	auto allValiditiesIt = find_if( alarmValidities.begin(), alarmValidities.end(), [=]( Types::BaseType val ){ return ( *( val.first ) == *validitySet ); } );
	
	// determine if the entry was found
	if ( allValiditiesIt == alarmValidities.end() ) {
		throw std::logic_error( "The validity set is not existing." );
	}

	return allValiditiesIt;
}



/**	@brief		Returns number of stored entries in the alarm validities set
*	@return										Number of stored alarm validities entries
*	@exception									None
*	@remarks									None
*/
int External::CAlarmValidities::Size(void) const
{
	return alarmValidities.size();
}



/**	@brief		Equality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::operator==(const External::CAlarmValidities& lhs, const External::CAlarmValidities& rhs) {
	auto lhsAlarmValidities = lhs.GetAllEntries();
	auto rhsAlarmValidities = rhs.GetAllEntries();
	
	// compare for each element separately
	if ( lhsAlarmValidities.size() != rhsAlarmValidities.size() ) {
		return false;
	}

	for (size_t i=0; i < lhsAlarmValidities.size(); i++) {
		if( *( lhsAlarmValidities[i].first ) != *( rhsAlarmValidities[i].first ) ) {
			return false;
		}
		if ( lhsAlarmValidities[i].second.size() != rhsAlarmValidities[i].second.size() ) {
			return false;
		}
		for ( size_t j = 0; j < lhsAlarmValidities[i].second.size(); j++ ) {
			if ( *( ( lhsAlarmValidities[i].second )[j] ) != *( ( rhsAlarmValidities[i].second )[j] ) ) {
				return false;
			}
		}
	}

	return true;
}



/**	@brief		Unequality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are not identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::operator!=(const External::CAlarmValidities& lhs, const External::CAlarmValidities& rhs) {
	return !( lhs == rhs );
}