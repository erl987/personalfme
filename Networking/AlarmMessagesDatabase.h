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

#include <map>
#include <vector>
#include <memory>
#include <tuple>
#include "DateTime.h"
#include "AlarmValidities.h"

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
		*	Typedef for the alarm database type
		*/
		using AlarmDatabaseType = std::map< std::vector<int>, CAlarmValidities >;

		/** \ingroup Networking
		*	Typedef for the alarms during an exception peroid
		*/
		using AlarmExceptionPeriodType = std::tuple< Utilities::CDateTime, Utilities::CDateTime, std::vector< std::shared_ptr< CAlarmMessage > > >;
	}


	/**	\ingroup Networking
	*	Class implementing a database connecting alarm codes with the stored alarm messages (for different gateways) considering the alarm timepoint. Any entry has at least a default dataset. 
	*	This class is not thread-safe.
	*/
	class CAlarmMessageDatabase
	{
	public:
		NETWORKING_API CAlarmMessageDatabase();
		NETWORKING_API CAlarmMessageDatabase( const CAlarmMessageDatabase& );
		NETWORKING_API CAlarmMessageDatabase& operator=( const CAlarmMessageDatabase& );
		NETWORKING_API virtual ~CAlarmMessageDatabase();
		NETWORKING_API void Add(const std::vector<int>& code, const CAlarmValidities& alarmDataset);
		NETWORKING_API void Add(const std::vector<int>& code, const std::shared_ptr<Validities::CValidity>& validitySet, const std::shared_ptr<CAlarmMessage>& alarmData);
		NETWORKING_API void ReplaceForAllCodes(const CAlarmValidities& alarmDataset);
		NETWORKING_API void ReplaceFallback(const CAlarmValidities& alarmDataset);
		NETWORKING_API void Remove(const std::vector<int>& code);
		NETWORKING_API void Remove(const std::vector<int>& code, const std::shared_ptr<Validities::CValidity>& validitySet);
		NETWORKING_API CAlarmValidities GetDataset(const std::vector<int>& code) const;
		NETWORKING_API const Types::AlarmDatabaseType& GetDatabase() const;
		NETWORKING_API const CAlarmValidities& GetAlarmsForAllCodes() const;
		NETWORKING_API const CAlarmValidities& GetFallbackAlarms() const;
		NETWORKING_API std::vector<std::type_index> GetAllMessageTypes() const;
		NETWORKING_API std::vector< std::vector<int> > GetAllCodes() const;
		NETWORKING_API std::vector< std::shared_ptr<CAlarmMessage> > Search(const std::vector<int>& code, const Utilities::CDateTime& currAlarmTime) const;
		NETWORKING_API std::vector< std::shared_ptr<CAlarmMessage> > Search(const std::vector<int>& code, const Utilities::CDateTime& currAlarmTime, bool& isDefaultDataset) const;
		NETWORKING_API void Clear(void);
		NETWORKING_API int Size(void) const;
		NETWORKING_API friend bool operator==(const CAlarmMessageDatabase& lhs, const CAlarmMessageDatabase& rhs);
		NETWORKING_API friend bool operator!=(const CAlarmMessageDatabase& lhs, const CAlarmMessageDatabase& rhs);
	protected:
		void RecalculateValidityTimes(const Utilities::CDateTime& newGoalTime) const;
		void CopyFrom( const CAlarmMessageDatabase& src );		
		static void GetMessageTypesInValidities(const External::CAlarmValidities& validities, std::map<std::type_index, unsigned int>& messageTypes);
		static void GetAlarmsFromValidities(const External::CAlarmValidities& alarmValidities, const Utilities::CDateTime& newGoalTime, std::vector< std::shared_ptr<CAlarmMessage> >& currDefaultDataset, std::vector<Types::AlarmExceptionPeriodType>& currNoneDefaultDataset );
		static bool GetValidAlarmsForTime( std::vector< std::shared_ptr<CAlarmMessage> >& currAlarmDataset, const Utilities::CDateTime& currAlarmTime, const std::vector< std::shared_ptr<CAlarmMessage> >& currDefaultDatasets, const std::vector<Types::AlarmExceptionPeriodType>& currNoneDefaultDatasets );
	private:
		Types::AlarmDatabaseType alarmDatabase;
		CAlarmValidities alarmsForAll;
		CAlarmValidities alarmsFallback;
		mutable std::map< std::vector<int>, std::vector< std::shared_ptr<CAlarmMessage> > > defaultDatasets;
		mutable std::multimap< std::vector<int>, Types::AlarmExceptionPeriodType > noneDefaultDatasets;
		mutable std::vector< std::shared_ptr<CAlarmMessage> > defaultDatasetsForAll;
		mutable std::vector< std::shared_ptr<CAlarmMessage> > defaultDatasetsFallback;
		mutable std::vector<Types::AlarmExceptionPeriodType> noneDefaultDatasetsForAll;
		mutable std::vector<Types::AlarmExceptionPeriodType> noneDefaultDatasetsFallback;
		mutable Utilities::CDateTime noneDefaultDatasetsValidityBegin;
		mutable Utilities::CDateTime noneDefaultDatasetsValidityEnd;
	};

	NETWORKING_API bool operator==(const CAlarmMessageDatabase& lhs, const CAlarmMessageDatabase& rhs);
	NETWORKING_API bool operator!=(const CAlarmMessageDatabase& lhs, const CAlarmMessageDatabase& rhs);
}
/*@}*/
