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
#include "TimeDatatypes.h"
#include <stdexcept>
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
	namespace Validities {
		/**	\ingroup Networking
		*	Class defining the validities of all alarm responses for a certain alarm code in case of weekly repeated alarm exceptions.
		*/
		class CWeeklyValidity : public Validities::CValidity
		{
		public:
			NETWORKING_API CWeeklyValidity();
			template <typename InIt> CWeeklyValidity( InIt weeksInMonthBegin, InIt weeksInMonthEnd, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API CWeeklyValidity( const Utilities::Time::WeekType& weekInMonth, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API CWeeklyValidity( const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API virtual ~CWeeklyValidity() {};
			NETWORKING_API virtual void SetValidityDates( const std::vector< Utilities::Time::WeekType >& weeksInMonth, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			template <typename OutIt> void GetValidityDates( OutIt weeksInMonthBegin, Utilities::Time::DayType& weekDay, Utilities::CTime& beginTimeOfDay, Utilities::CTime& endTimeOfDay ) const;
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const override;
			NETWORKING_API virtual void Invalidate() override;
			NETWORKING_API virtual bool IsValid() const override;
			NETWORKING_API virtual std::unique_ptr<CValidity> Clone() const override;
		protected:
			NETWORKING_API virtual bool CompareDerived( const Validities::CValidity& rhs ) const override;
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > GetValiditiesStartingInLocalMonth( const int& localMonth, const int& localYear ) const;
		private:
			std::vector< Utilities::Time::WeekType > validWeeksInMonth;
			Utilities::Time::DayType validWeekDay;
			Utilities::CTime beginTimeOfDay;
			Utilities::CTime endTimeOfDay;
			bool isValid;
		};
	}
}
/*@}*/



/**	@brief		Constructor
*	@param		weeksInMonthBegin				Iterator to the beginning of the array containing all valid weeks of the month. If all weeks are valid use rather the third constructor.
*	@param		weeksInMonthEnd					Iterator past the end of the array containing all valid weeks of the month
*	@param		weekDay							Valid day of week
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if an input is invalid
*	@remarks 									None
*/
template <typename InIt>
External::Validities::CWeeklyValidity::CWeeklyValidity( InIt weeksInMonthBegin, InIt weeksInMonthEnd, const Utilities::Time::DayType& weekDay, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	std::vector< Utilities::Time::WeekType > weeksInMonth( weeksInMonthBegin, weeksInMonthEnd );

	SetValidityDates( weeksInMonth, weekDay, beginTimeOfDay, endTimeOfDay );
}



/**	@brief		Getting the validity parameters
*	@param		weeksInMonthBegin				Array containing all valid weeks of the month
*	@param		weekDay							Valid day of week
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@return										False if the object was not yet initialized, otherwise true
*	@exception	std::runtime_error				Thrown if the object is invalid
*	@remarks 									None
*/
template <typename OutIt>
void External::Validities::CWeeklyValidity::GetValidityDates(OutIt weeksInMonthBegin, Utilities::Time::DayType& weekDay, Utilities::CTime& beginTimeOfDay, Utilities::CTime& endTimeOfDay ) const
{
	if ( !IsValid() ) {
		throw std::runtime_error( "The weekly validity exception object is invalid." );
	}

	weekDay = CWeeklyValidity::validWeekDay;
	for (size_t i=0; i < validWeeksInMonth.size(); i++) {
		*( weeksInMonthBegin++ ) = CWeeklyValidity::validWeeksInMonth.at(i);
	}
	beginTimeOfDay = CWeeklyValidity::beginTimeOfDay;
	endTimeOfDay = CWeeklyValidity::endTimeOfDay;
}
