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
		*	Class defining the validities of all alarm responses for a certain alarm code in case of monthly repeated alarm exceptions. This class is not thread-safe.
		*/
		class CMonthlyValidity : public Validities::CValidity
		{
		public:
			NETWORKING_API CMonthlyValidity();
			template <typename InIt> CMonthlyValidity( const int& day, InIt monthsBegin, InIt monthsEnd, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API CMonthlyValidity( const int& day, const int& month, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API CMonthlyValidity( const int& day, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			NETWORKING_API virtual ~CMonthlyValidity() {};
			NETWORKING_API virtual void SetValidityDays( const int& day, const std::vector<int>& months, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay );
			template <typename OutIt> void GetValidityDays( int& day, OutIt months, Utilities::CTime& beginTimeOfDay, Utilities::CTime& endTimeOfDay ) const;
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const override;
			NETWORKING_API virtual void Invalidate( void ) override;
			NETWORKING_API virtual std::unique_ptr<CValidity> Clone() const override;
			NETWORKING_API virtual bool IsValid( void ) const override;
		protected:
			NETWORKING_API virtual bool CompareDerived( const Validities::CValidity& rhs ) const override;
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> >  GetValiditiesStartingInLocalMonth( const int& localMonth, const int& localYear ) const;
		private:
			std::vector<int> validMonths;
			int validDay;
			Utilities::CTime beginTimeOfDay;
			Utilities::CTime endTimeOfDay;
			bool isValid;
		};
	}
}
/*@}*/



/**	@brief		Constructor
*	@param		day								Valid day of the month
*	@param		monthsBegin						Iterator to the beginning of the array with all valid months. If all weeks are valid use the appropriate constructor.
*	@param		monthsEnd						Iterator past the end of the array with all valid months
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@exception 	std::out_of_range				Thrown if a the given day of the year is not valid (i.e. either day or months)
*	@remarks 									None
*/
template <typename InIt>
External::Validities::CMonthlyValidity::CMonthlyValidity( const int& day, InIt monthsBegin, InIt monthsEnd, const Utilities::CTime& beginTimeOfDay, const Utilities::CTime& endTimeOfDay )
	: isValid( false )
{
	std::vector<int> months( monthsBegin, monthsEnd );

	SetValidityDays( day, months, beginTimeOfDay, endTimeOfDay );
}



/**	@brief		Getting the validity dates
*	@param		day								Valid day of the month
*	@param		monthsBegin						Iterator to the beginning of the array of all valid months
*	@param		beginTimeOfDay					Begin time of the validity exception (local time)
*	@param		endTimeOfDay					End time of the validity exception [begin time, end time) (local time)
*	@return										None
*	@exception 	std::runtime_error				Thrown if the object is invalid
*	@remarks 									None
*/
template <typename OutIt>
void External::Validities::CMonthlyValidity::GetValidityDays( int& day, OutIt monthsBegin, Utilities::CTime& beginTimeOfDay, Utilities::CTime& endTimeOfDay ) const
{
	std::vector<int> months;

	if ( !IsValid() ) {
		throw std::runtime_error( "The object is invalid." );
	}

	day = static_cast<int>( CMonthlyValidity::validDay );	
	months = CMonthlyValidity::validMonths;
	for (size_t i=0; i < months.size(); i++) {
		*( monthsBegin++ ) = static_cast<int>( months[i] );
	}
	beginTimeOfDay = CMonthlyValidity::beginTimeOfDay;
	endTimeOfDay = CMonthlyValidity::endTimeOfDay;
}