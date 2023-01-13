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
#include <memory>
#include "DateTime.h"

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
		*	Virtual base class defining the validities of all alarm responses for a certain alarm code in dependence of time
		*/
		class CValidity
		{
		public:
			NETWORKING_API CValidity() {};
			NETWORKING_API virtual ~CValidity() {};
			NETWORKING_API friend bool operator==( const CValidity& lhs, const CValidity& rhs );
			NETWORKING_API friend bool operator!=( const CValidity& lhs, const CValidity& rhs );
			/**	@brief		Returns all validities for the given month in the given year (defined in UTC time)
			*	@param		UTCMonth						Month for which the validities are calculated (UTC-time)
			*	@param		UTCYear							Year for which the validities are calculated (UTC-time)
			*	@return 									Vector container storing all validity UTC-start and end times in the given month
			*	@exception	std::runtime_error				Thrown if the object was initialized before use
			*	@exception	std::out_of_range				Thrown if either given month or year are invalid
			*	@remarks 									This method needs to handle correctly validities from other months that interset with the month
			*/
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const = 0;

			/**	@brief		Cloning method duplicating the object
			*	@return									Duplicate base class pointer of the object
			*	@exception								None
			*	@remarks								None
			*/
			NETWORKING_API virtual std::unique_ptr<CValidity> Clone() const = 0;

			/**	@brief		Determines if the object is valid (i.e. correctly set)
			*	@return									True if the object is valid, false otherwise
			*	@exception								None
			*	@remarks								None
			*/
			NETWORKING_API virtual bool IsValid() const = 0;


			/**	@brief		Invalidates the object
			*	@return									None
			*	@exception								None
			*	@remarks								None
			*/
			NETWORKING_API virtual void Invalidate() = 0;
		protected:
			NETWORKING_API virtual bool CompareDerived( const CValidity& rhs ) const = 0;
		};

		NETWORKING_API bool operator==( const CValidity& lhs, const CValidity& rhs );
		NETWORKING_API bool operator!=( const CValidity& lhs, const CValidity& rhs );
	}
}
/*@}*/
