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
#include <tuple>
#include <vector>
#include <string>
#include "DateTime.h"

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
	*	Class representing the basic data of a code sequence
	*/
	class CSeqData
	{
	public:	
		UTILITY_API CSeqData(void);
		UTILITY_API CSeqData(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString);
		UTILITY_API virtual ~CSeqData(void);
		UTILITY_API void Set(const CDateTime& startTime, const std::vector<int>& code, const std::string& infoString);
		UTILITY_API void Get(CDateTime& startTime, std::vector<int>& code, std::string& infoString) const;
		UTILITY_API const CDateTime& GetStartTime(void) const;
		UTILITY_API const std::vector<int>& GetCode(void) const;
		UTILITY_API const std::string& GetInfoString(void) const;
		UTILITY_API friend bool operator==(const CSeqData& lhs, const CSeqData& rhs);
		UTILITY_API friend bool operator!=(const CSeqData& lhs, const CSeqData& rhs);
	protected:
		std::tuple< CDateTime, std::vector<int>, std::string > sequenceData;
	};
}
/*@}*/
