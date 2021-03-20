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
		*	Class defining the validities of all alarm responses for a certain alarm code in case no exceptions are valid. This class is not thread-safe.
		*/
		class CDefaultValidity : public Validities::CValidity
		{
		public:
			NETWORKING_API CDefaultValidity( void );
			NETWORKING_API virtual ~CDefaultValidity( void ) {};
			NETWORKING_API virtual std::vector< std::pair<Utilities::CDateTime, Utilities::CDateTime> > GetValidityPeriods( const int& UTCMonth, const int& UTCYear ) const override;
			NETWORKING_API virtual std::unique_ptr<CValidity> Clone() const override;
			NETWORKING_API virtual bool IsValid( void ) const override;
			NETWORKING_API virtual void Invalidate() override;
		protected:
			NETWORKING_API virtual bool CompareDerived( const Validities::CValidity& rhs ) const override;
		};

		const std::shared_ptr<CValidity> DEFAULT_VALIDITY = std::make_shared<CDefaultValidity>();
	}
}
/*@}*/
