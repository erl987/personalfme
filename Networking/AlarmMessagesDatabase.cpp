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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "DefaultValidity.h"
#include "BoostStdTimeConverter.h"
#include "AlarmMessagesDatabase.h"


/**	@brief		Equality operator
*	@param		lhs								Left-hand side of operator
*	@param		rhs								Right-hand side of operator
*	@return										True if left- and right-hand side are identical, otherwise false
*	@exception									None
*	@remarks 									None
*/
bool External::operator==(const External::CAlarmMessageDatabase& lhs, const External::CAlarmMessageDatabase& rhs) {
	auto lhsData = lhs.GetDatabase();
	auto rhsData = rhs.GetDatabase();

	if ( lhsData != rhsData ) {
		return false;
	}

	auto lhsAlarmsForAllCodes = lhs.GetAlarmsForAllCodes();
	auto rhsAlarmsForAllCodes = rhs.GetAlarmsForAllCodes();

	if ( lhsAlarmsForAllCodes != rhsAlarmsForAllCodes ) {
		return false;
	}

	auto lhsFallbackAlarms = lhs.GetFallbackAlarms();
	auto rhsFallbackAlarms = rhs.GetFallbackAlarms();

	if ( lhsFallbackAlarms != rhsFallbackAlarms ) {
		return false;
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
bool External::operator!=(const External::CAlarmMessageDatabase& lhs, const External::CAlarmMessageDatabase& rhs) {
	return !( lhs == rhs );
}


/** \ingroup Networking
*	Constant for invalid time point
*/
const Utilities::CDateTime INVALID_TIMEPOINT = Utilities::CDateTime();

const std::function< Utilities::CDateTime(const boost::posix_time::ptime&) > ToStdTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToStdTime;
const std::function< boost::posix_time::ptime(const Utilities::CDateTime&) > ToBoostTime = &Utilities::Time::CBoostStdTimeConverter::ConvertToBoostTime;


/**	@brief		Constructor	
*	@remarks									None
*/
External::CAlarmMessageDatabase::CAlarmMessageDatabase(void)
	: noneDefaultDatasetsValidityBegin( INVALID_TIMEPOINT ),
	  noneDefaultDatasetsValidityEnd( INVALID_TIMEPOINT )
{
}


/** @brief	Copy constructor.
*	@param	src									Source object
*/
External::CAlarmMessageDatabase::CAlarmMessageDatabase( const CAlarmMessageDatabase& src )
{
	CopyFrom( src );
}


/** @brief	Copy assigment constructor.
*	@param	rhs									Right-hand side object
*/
External::CAlarmMessageDatabase& External::CAlarmMessageDatabase::operator=( const CAlarmMessageDatabase& rhs )
{
	if ( this == &rhs ) {
		return *this;
	}
	Clear(); // invalidate the database settings forcing new calculation in case of the next search call of a client
	CopyFrom( rhs );

	return *this;
}


/** @brief	Helper method for deep copying of the object
*	@param	src									Right-hand side object to be copied into the present object
*	@return										None
*	@exception									None
*	@remarks									This method assumes that the data of the present object is empty
*/
void External::CAlarmMessageDatabase::CopyFrom( const CAlarmMessageDatabase& src )
{
	Types::AlarmDatabaseType srcDB;
	CAlarmValidities validities;

	srcDB = src.GetDatabase();
	for ( auto& entry : srcDB ) {
		validities = entry.second;
		alarmDatabase[entry.first] = validities;
	}

	alarmsForAll = src.GetAlarmsForAllCodes();
	alarmsFallback = src.GetFallbackAlarms();
}


/**	@brief		Destructor	
*	@remarks									None
*/
External::CAlarmMessageDatabase::~CAlarmMessageDatabase(void)
{
}


/**	@brief		Adds a new alarm data subset to the database
*	@param		code							FME-code corresponding to the dataset
*	@param		validitySet						Validity set to be added. If it already exists, the alarm data will be added to the existing data.
*	@param		alarmData						Alarm data corresponding to the validity set to be added
*	@return										None
*	@exception	std::logic_error				Thrown if it is attempted to set an exception validity before setting the default validity
*	@exception	std::length_error				Thrown if the code is empty
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									At first for each code a default alarm dataset must be specified. It may be empty.
*/
void External::CAlarmMessageDatabase::Add(const std::vector<int>& code, const std::shared_ptr<Validities::CValidity>& validitySet, const std::shared_ptr<CAlarmMessage>& alarmData)
{
	using namespace std;
	vector< shared_ptr<External::CAlarmMessage> > currAlarmData;
	CAlarmValidities currDataset;
	bool isDefaultExisting;

	// check if the validity set is valid
	if ( !validitySet ) {
		throw std::runtime_error( "The validity set is empty." );	
	}

	// prevent empty codes
	if ( code.empty() ) {
		throw std::length_error( "The code is empty." );
	}

	// check for existence of the default dataset in the new entry
	if ( *validitySet != *Validities::DEFAULT_VALIDITY ) {
		isDefaultExisting = false;
	} else {
		isDefaultExisting = true;
	}

	// read current entry of the database
	auto alarmDatabaseIt = alarmDatabase.find( code );
	if ( alarmDatabaseIt != alarmDatabase.end() ) {
		currDataset = alarmDatabaseIt->second;
		
		// check for existence of the default dataset
		if ( currDataset.IsEntry( Validities::DEFAULT_VALIDITY ) ) {
			isDefaultExisting = true;
		}

		// load an existing dataset for the given validity time set to prevent deleting
		if ( currDataset.IsEntry( validitySet ) ) {
			currDataset.GetEntry( validitySet, back_inserter( currAlarmData ) );
		}
	}

	if ( !isDefaultExisting ) {
		throw std::logic_error( "No default dataset is existing for the current entry." );
	}

	// add new data to the (possibly exisiting data in the) database
	if ( find_if( currAlarmData.begin(), currAlarmData.end(), [=]( auto const& val ){ return ( *val == *alarmData ); } ) == currAlarmData.end() ) {
		currAlarmData.push_back( alarmData );
	}
	if ( currDataset.IsEntry( validitySet ) ) {
		currDataset.ReplaceEntry( validitySet, currAlarmData.begin(), currAlarmData.end() );	
	} else {
		currDataset.AddEntry( validitySet, currAlarmData.begin(), currAlarmData.end() );
	}
	alarmDatabase[ code ] = currDataset;

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Adds a new complete alarm dataset to the database
*	@param		code							FME-code corresponding to the dataset
*	@param		alarmDataset					New complete dataset containing the alarm data
*	@return										None
*	@exception	std::logic_error				Thrown if it is attempted to set an exception validity set without a default validity
*	@exception	std::length_error				Thrown if the code is empty
*	@remarks									If an entry for the code already exists, it will be replaced
*/
void External::CAlarmMessageDatabase::Add(const std::vector<int>& code, const CAlarmValidities& alarmDataset)
{
	// prevent empty codes
	if ( code.empty() ) {
		throw std::length_error( "The code is empty." );
	}

	// ensure that one default validity is existing in the new dataset
	if ( !alarmDataset.IsEntry( Validities::DEFAULT_VALIDITY ) ) {
		throw std::logic_error( "No default dataset is existing for the current entry." );	
	}

	// add the entry to the database, replace it if an entry for the code already exists
	alarmDatabase[ code ] = alarmDataset;

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Replaces the alarm dataset valid for any code
*	@param		alarmDataset					New complete dataset containing the alarm data
*	@return										None
*	@exception	std::logic_error				Thrown if it is attempted to set an exception validity set without a default validity
*	@remarks									None
*/
void External::CAlarmMessageDatabase::ReplaceForAllCodes( const CAlarmValidities& alarmDataset )
{
	// ensure that one default validity is existing in the new dataset
	if ( !alarmDataset.IsEntry( Validities::DEFAULT_VALIDITY ) ) {
		throw std::logic_error( "No default dataset is existing for the current entry." );
	}

	alarmsForAll = alarmDataset;

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Replaces the alarm dataset valid as fallback if no other alarm data is valid for an alarm
*	@param		alarmDataset					New complete dataset containing the alarm data
*	@return										None
*	@exception	std::logic_error				Thrown if it is attempted to set an exception validity set without a default validity
*	@remarks									None
*/
void External::CAlarmMessageDatabase::ReplaceFallback( const CAlarmValidities& alarmDataset )
{
	// ensure that one default validity is existing in the new dataset
	if ( !alarmDataset.IsEntry( Validities::DEFAULT_VALIDITY ) ) {
		throw std::logic_error( "No default dataset is existing for the current entry." );
	}

	alarmsFallback = alarmDataset;

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Removes a complete dataset for a certain code from the database
*	@param		code							FME-code defining the dataset which should be deleted
*	@return										None
*	@exception	std::logic_error				Thrown if the requested code was not existing in the database
*	@remarks									None
*/
void External::CAlarmMessageDatabase::Remove(const std::vector<int>& code)
{
	if ( alarmDatabase.erase( code ) < 1 ) {
		throw std::logic_error( "Code is not existing in the database." );
	}

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Remove an exception validity set from the dataset for a certain code in the database
*	@param		code							FME-code defining the dataset which should be adjusted
*	@param		validitySet						Validity set that should be deleted
*	@return										None
*	@exception	std::logic_error				Thrown if the code or the validity set are not existing in the database
*	@exception	std::range_error				Thrown if it is attempted to delete the default validity set, when still other validity sets are existing in the entry
*	@exception	std::runtime_error				Thrown if the validity set is empty
*	@remarks									None
*/
void External::CAlarmMessageDatabase::Remove(const std::vector<int>& code, const std::shared_ptr<Validities::CValidity>& validitySet)
{
	// check if the validity set is valid
	if ( !validitySet ) {
		throw std::runtime_error( "The validity set is empty." );	
	}

	// find the required entry in the database
	auto alarmDatabaseIt = alarmDatabase.find( code );
	if ( alarmDatabaseIt == alarmDatabase.end() ) {
		throw std::logic_error( "Code is not existing in the database." );
	}

	// ensure that the default validity set is deleted as the last one
	if ( ( *validitySet == *Validities::DEFAULT_VALIDITY ) && ( alarmDatabaseIt->second.Size() != 1 ) ) {
		throw std::range_error( "The default validity set can only be deleted when no other validity sets exist." );
	}

	// delete the subentry within the dataset
	alarmDatabaseIt->second.RemoveEntry( validitySet );

	// delete complete dataset if the last subentry was removed
	if ( alarmDatabaseIt->second.Size() == 0 ) {
		Remove( code );
	}

	// invalidate the database settings forcing new calculation in case of the next search call of a client
	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Extracts the alarm message datasets valid in a certain month from an alarm validities object
*	@param		alarmValidities					Alarm validities object to be used
*	@param		newGoalTime						Requested month of validity
*	@param		currDefaultDataset				Will contain the default dataset of the alarm validities after the method returned
*	@param		currNoneDefaultDatasets			Will contain the exeception datasets of the alarm validities after the method returned
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::CAlarmMessageDatabase::GetAlarmsFromValidities(const External::CAlarmValidities& alarmValidities, const Utilities::CDateTime& newGoalTime, std::vector< std::shared_ptr<CAlarmMessage> >& currDefaultDataset, std::vector<Types::AlarmExceptionPeriodType>& currNoneDefaultDatasets )
{	
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities;

	vector< pair<CDateTime, CDateTime> > validityPeriods;
	ptime goalTime;

	goalTime = ToBoostTime( newGoalTime );
	currDefaultDataset.clear();
	currNoneDefaultDatasets.clear();

	if ( alarmValidities.Size() > 0 ) {
		alarmValidities.GetEntry( Validities::DEFAULT_VALIDITY, back_inserter( currDefaultDataset ) );

		// determine the exception alarm datasets and UTC-validity time - the datasets might overlap by design
		for ( auto const& validityEntry : alarmValidities.GetAllEntries() ) {
			auto currValidity = validityEntry.first;

			if ( *currValidity != Validities::CDefaultValidity() ) {
				validityPeriods = currValidity->GetValidityPeriods( goalTime.date().month(), goalTime.date().year() ); // these are UTC-times
				for ( auto const& currValidityPeriod : validityPeriods ) {
					currNoneDefaultDatasets.push_back( std::make_tuple( currValidityPeriod.first, currValidityPeriod.second, validityEntry.second ) ); // it is guaranteed that everything is based on seconds without fractional seconds
				}
			}
		}
	}
}


/**	@brief		Recalculates the information on the alarm datasets in dependency of time
*	@param		newGoalTime						The recalculation will be performed for the whole month of 'goalTime' (UTC-time)
*	@return										None
*	@exception									None
*	@remarks									Call this function after any change of the database or if the stored information got invalid
*/
void External::CAlarmMessageDatabase::RecalculateValidityTimes(const Utilities::CDateTime& newGoalTime) const
{
	using namespace std;
	using namespace boost::gregorian;
	using namespace boost::posix_time;
	using namespace Utilities;

	vector<int> code;
	vector< shared_ptr<CAlarmMessage> > currDefaultAlarmDataset;
	vector<Types::AlarmExceptionPeriodType> currNoneDefaultDatasets;
	ptime goalTime;

	// delete the old databases
	defaultDatasets.clear();
	defaultDatasetsForAll.clear();
	defaultDatasetsFallback.clear();
	noneDefaultDatasets.clear();
	noneDefaultDatasetsForAll.clear();
	noneDefaultDatasetsFallback.clear();

	// determine the required time period (always one month)
	goalTime = ToBoostTime( newGoalTime );
	noneDefaultDatasetsValidityBegin = ToStdTime( ptime( goalTime.date().end_of_month() + days(1) - months(1) ) );	// UTC-time
	noneDefaultDatasetsValidityEnd = ToStdTime( ptime( goalTime.date().end_of_month() + days(1) ) );				// UTC-time

	// for specific codes
	for ( auto const& item : alarmDatabase ) {
		code = item.first;		
		auto currDataset = item.second;

		currDefaultAlarmDataset.clear();
		currNoneDefaultDatasets.clear();
		GetAlarmsFromValidities( currDataset, newGoalTime, currDefaultAlarmDataset, currNoneDefaultDatasets );
		defaultDatasets.insert( make_pair( code, currDefaultAlarmDataset ) );
		for ( auto const& currNoneDefaultDataset : currNoneDefaultDatasets ) {
			noneDefaultDatasets.insert( make_pair( code, currNoneDefaultDataset ) );
		}
	}

	// for all codes
	GetAlarmsFromValidities( alarmsForAll, newGoalTime, defaultDatasetsForAll, noneDefaultDatasetsForAll );

	// as fallback if no code is matched
	GetAlarmsFromValidities( alarmsFallback, newGoalTime, defaultDatasetsFallback, noneDefaultDatasetsFallback );
}


/**	@brief		Searches for a dataset of a given FME-code for the given time
*	@param		code							FME-code for which the dataset is required
*	@param		currAlarmTime					UTC time of the alarm - required for determining the correct alarm dataset
*	@return										Datasets corresponding to the code, it may contains serveral datasets if multiple overlapping exceptions are valid or if several datasets were defined in the database for one code exception / default (see External::CAlarmValidities for details regarding the parameters).
*	@exception	std::logic_error				The FME-code was not found in the database
*	@remarks									It is guaranteed that no empty alarm datasets will be returned
*/
std::vector< std::shared_ptr< External::CAlarmMessage > > External::CAlarmMessageDatabase::Search(const std::vector<int>& code, const Utilities::CDateTime& currAlarmTime) const
{
	bool isDefaultDataset;

	return Search( code, currAlarmTime, isDefaultDataset );
}


/**	@brief		Obtains the valid alarms for a certain UTC time
*	@param		currAlarmDataset				Containing all alarms valid for the specified time, the found valid alarms will be added to that container
*	@param		currAlarmTime					UTC time of the alarm - required for determining the correct alarm dataset
*	@param		currDefaultDatasets				Default alarms
*	@param		currNoneDefaultDatasets			Exception alarms
*	@return										True if the default alarm is valid (or no alarm at all), false if an exception alarm is valid
*	@exception									None
*	@remarks									None
*/
bool External::CAlarmMessageDatabase::GetValidAlarmsForTime( std::vector< std::shared_ptr<CAlarmMessage> >& currAlarmDataset, const Utilities::CDateTime& currAlarmTime, const std::vector< std::shared_ptr<CAlarmMessage> >& currDefaultDatasets, const std::vector<Types::AlarmExceptionPeriodType>& currNoneDefaultDatasets )
{
	using namespace std;
	using namespace boost::posix_time;

	ptime alarmTime;
	bool noExceptions = true;

	alarmTime = ToBoostTime( currAlarmTime );

	// set exception datasets if the UTC-alarm time is within any exception time period
	for ( auto exception : currNoneDefaultDatasets ) {
		if ( time_period( ToBoostTime( get<0>( exception ) ), ToBoostTime( get<1>( exception ) ) ).contains( alarmTime ) ) { // searching for UTC-times
			currAlarmDataset.insert( end( currAlarmDataset ), begin( get<2>( exception ) ), end( get<2>( exception ) ) );
			noExceptions = false;
		}
	}

	// set the default dataset if no exceptions have been found
	if ( noExceptions ) {
		currAlarmDataset.insert( end( currAlarmDataset ), begin( currDefaultDatasets ), end( currDefaultDatasets ) );
	}

	return noExceptions;
}


/**	@brief		Searches for a dataset of a given FME-code for the given time
*	@param		code							FME-code for which the dataset is required
*	@param		currAlarmTime					UTC time of the alarm - required for determining the correct alarm dataset
*	@param		isDefaultDataset				Flag that is set to true if the valid dataset is the default dataset, false otherwise. The datasets required for all alarms are not considered.
*	@return										Datasets corresponding to the code, it may contains serveral datasets if multiple overlapping exceptions are valid or if several datasets were defined in the database for one code exception / default (see External::CAlarmValidities for details regarding the parameters).
*	@exception	std::logic_error				The FME-code was not found in the database
*	@remarks									It is guaranteed that no empty alarm datasets will be returned
*/
std::vector< std::shared_ptr<External::CAlarmMessage> > External::CAlarmMessageDatabase::Search(const std::vector<int>& code, const Utilities::CDateTime& currAlarmTime, bool& isDefaultDataset) const
{
	using namespace std;
	using namespace boost::posix_time;
	using namespace Utilities;

	ptime alarmTime;
	bool noExceptionsForCode, noExceptionsAsFallback;
	vector< shared_ptr<CAlarmMessage> > currAlarmDataset, currDefaultDatasetForCode;
	vector<Types::AlarmExceptionPeriodType> currNoneDefaultDatasetForCode;

	// check if the current exception time periods are valid - recalculate for the current month otherwise
	alarmTime = ToBoostTime( currAlarmTime );
	if ( !time_period( ToBoostTime( noneDefaultDatasetsValidityBegin ), ToBoostTime( noneDefaultDatasetsValidityEnd ) ).contains( alarmTime ) ) {
		RecalculateValidityTimes( currAlarmTime );
	}

	// specific for the FME-code
	auto exceptionDatasetIt = noneDefaultDatasets.equal_range( code );
	currNoneDefaultDatasetForCode.resize( noneDefaultDatasets.count( code ) );
	transform( exceptionDatasetIt.first, exceptionDatasetIt.second, begin( currNoneDefaultDatasetForCode ), []( auto const& data ) { return data.second; } );
	
	auto defaultAlarmDatasetIt = defaultDatasets.find( code );
	if ( defaultAlarmDatasetIt != end( defaultDatasets ) ) {
		currDefaultDatasetForCode = defaultAlarmDatasetIt->second;
	}

	noExceptionsForCode = GetValidAlarmsForTime( currAlarmDataset, currAlarmTime, currDefaultDatasetForCode, currNoneDefaultDatasetForCode );

	if ( currAlarmDataset.empty() ) {
		// as a fallback if no alarm has been specified for the code
		noExceptionsAsFallback = GetValidAlarmsForTime( currAlarmDataset, currAlarmTime, defaultDatasetsFallback, noneDefaultDatasetsFallback );
	} else {
		noExceptionsAsFallback = true;
	}

	// it is a "default" alarm if no "exceptions" are valid, if the fallback is used it is handled such as an alarm for a specific code
	isDefaultDataset = noExceptionsForCode && noExceptionsAsFallback;

	// for all alarms
	GetValidAlarmsForTime( currAlarmDataset, currAlarmTime, defaultDatasetsForAll, noneDefaultDatasetsForAll );

	if ( currAlarmDataset.empty() ) {
		throw std::logic_error( "No alarm message valid for this code during the alarm time." );
	}

	// remove all empty datasets from the return list
	auto newEndIt = remove_if( currAlarmDataset.begin(), currAlarmDataset.end(), []( auto const& val ) { return val->IsEmpty(); } );
	currAlarmDataset.erase( newEndIt, currAlarmDataset.end() );

	return currAlarmDataset;
}


/**	@brief		Obtains the size of the database
*	@return										Number of datasets stored in the dabase
*	@exception									None
*	@remarks									None
*/
int External::CAlarmMessageDatabase::Size(void) const
{
	return alarmDatabase.size();
}


/**	@brief		Returns the complete dataset for a certain code
*	@param		code							Code for which the dataset is required
*	@return										Complete dataset for the given code
*	@exception	std::logic_error				The FME-code was not found in the database
*	@remarks									None
*/
External::CAlarmValidities External::CAlarmMessageDatabase::GetDataset(const std::vector<int>& code) const
{
	auto databaseIt = alarmDatabase.find( code );
	if ( databaseIt == alarmDatabase.end() ) {
		throw std::logic_error( "Code is not existing in the database." );
	}

	return databaseIt->second;
}


/**	@brief		Returns the complete database
*	@return										Complete database
*	@remarks									None
*/
const External::Types::AlarmDatabaseType& External::CAlarmMessageDatabase::GetDatabase(void) const
{
	return alarmDatabase;
}


/**	@brief		Returns the alarms valid for all codes (typically infoalarms)
*	@return										Alarm validities valid for all codes
*	@remarks									None
*/
const External::CAlarmValidities& External::CAlarmMessageDatabase::GetAlarmsForAllCodes(void) const
{
	return alarmsForAll;
}


/**	@brief		Returns the alarms that are valid if no other alarm is valid
*	@return										Alarm validities valid if no other alarm is valid
*	@remarks									None
*/
const External::CAlarmValidities& External::CAlarmMessageDatabase::GetFallbackAlarms(void) const 
{
	return alarmsFallback;
}


/**	@brief		Clear the complete database
*	@return										None
*	@remarks									None
*/
void External::CAlarmMessageDatabase::Clear(void)
{

	alarmDatabase.clear();
	alarmsForAll.Clear();
	alarmsFallback.Clear();

	defaultDatasets.clear();
	defaultDatasetsForAll.clear();
	defaultDatasetsFallback.clear();

	noneDefaultDatasets.clear();
	noneDefaultDatasetsForAll.clear();
	noneDefaultDatasetsFallback.clear();

	noneDefaultDatasetsValidityBegin = INVALID_TIMEPOINT;
	noneDefaultDatasetsValidityEnd = INVALID_TIMEPOINT;
}


/**	@brief		Obtains the types of all messages currently present in the database
*	@return										List containing the std::type_index of all message types currently present in the database
*	@exception									None
*	@remarks									None
*/
std::vector<std::type_index> External::CAlarmMessageDatabase::GetAllMessageTypes() const
{
	using namespace std;

	Types::AlarmValiditiesType currAlarmValidities;
	map<type_index, unsigned int> messageTypes;
	vector<type_index> messageTypeList;

	// count all existing message types in the database
	for ( auto currCodeValidities : alarmDatabase ) {
		GetMessageTypesInValidities( currCodeValidities.second, messageTypes );
	}

	// count all existing message types for the alarms always sent out and the fallback alarms
	GetMessageTypesInValidities( alarmsForAll, messageTypes );
	GetMessageTypesInValidities( alarmsFallback, messageTypes );

	// create a list with all existing message types
	for ( auto currMessageType : messageTypes ) {
		messageTypeList.push_back( currMessageType.first );
	}

	return messageTypeList;
}


/**	@brief		Obtains the types of all messages present in the validities object
*	@param		validities						The validities object to be analysed
*	@param		messageTypes					Map where the message types present in the validities object will be added
*	@return										None
*	@exception									None
*	@remarks									None
*/
void External::CAlarmMessageDatabase::GetMessageTypesInValidities( const External::CAlarmValidities& validities, std::map<std::type_index, unsigned int>& messageTypes )
{
	auto currAlarmValidities = validities.GetAllEntries();
	for ( auto currValidity : currAlarmValidities ) {
		for ( auto currMessage : currValidity.second ) {
			messageTypes[currMessage->GetGatewayType()]++;
		}
	}
}


/**	@brief		Obtains the codes of all messages currently present in the database
*	@return										List containing the codes of all messages currently present in the database
*	@exception									None
*	@remarks									None
*/
std::vector< std::vector<int> > External::CAlarmMessageDatabase::GetAllCodes() const
{
	std::vector< std::vector<int> > codesList;

	// find all codes in the database (they are guaranteed to be unique)
	codesList.reserve( alarmDatabase.size() );
	for ( auto currCodeValidities : alarmDatabase ) {
		codesList.push_back( currCodeValidities.first );
	}

	return codesList;
}
