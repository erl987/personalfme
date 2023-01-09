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
#include <memory>
#include <typeindex>
#include "GatewayLoginData.h"

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
	/**	\ingroup Networking
	*	Class implementing an abstract message for the alarm gateways
	*/
	class CAlarmMessage
	{
	public:
		NETWORKING_API CAlarmMessage(const bool& requiredState);
		~CAlarmMessage(void) {};
		NETWORKING_API friend bool operator==(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
		NETWORKING_API friend bool operator!=(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
		NETWORKING_API friend bool operator<(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
		NETWORKING_API virtual bool RequiredState(void) const;
		NETWORKING_API virtual void SetRequiredState(const bool& requiredState);
		/**	@brief		Checking if the dataset is empty.
		*	@return									True if the dataset is empty, if it contains valid data it is false
		*	@exception								None		
		*	@remarks								An empty dataset is interpreted as the suppression of the gateway call
		*/
		NETWORKING_API virtual bool IsEmpty(void) const = 0;

		/**	@brief		Setting the dataset empty.
		*	@return									None
		*	@exception								None		
		*	@remarks								An empty dataset is interpreted as the suppression of the gateway call
		*/		
		NETWORKING_API virtual void SetEmpty(void) = 0;

		/**	@brief		Cloning method duplicating the object
		*	@return									Duplicate base class pointer of the object
		*	@exception								None
		*	@remarks								NOne
		*/
		NETWORKING_API virtual std::unique_ptr< CAlarmMessage > Clone() const = 0;

		/**	@brief		Obtains the type index of the gateway class corresponding to the present message class.
		*	@return										Type index of the gateway class corresponding to the present message class
		*	@exception									None		
		*	@remarks									None
		*/
		NETWORKING_API virtual std::type_index GetGatewayType() const = 0;
	protected:
		NETWORKING_API virtual bool IsEqual(const CAlarmMessage& rhs) const = 0;
		NETWORKING_API virtual bool IsSmaller(const CAlarmMessage& rhs) const = 0;
	private:
		bool requiredState;
	};
}
/*@}*/



namespace External {
	NETWORKING_API bool operator==(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
	NETWORKING_API bool operator!=(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
	NETWORKING_API bool operator<(const CAlarmMessage& lhs, const CAlarmMessage& rhs);
}
