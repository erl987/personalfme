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
	/** \ingroup Networking
	*	Abstract class representing the login data for gateway alarms. The derived classes must store the information on the connection trials in the base class.
	*/
	class CGatewayLoginData
	{
	public:
		NETWORKING_API CGatewayLoginData(void);
		virtual ~CGatewayLoginData(void);
		NETWORKING_API virtual void SetConnectionTrialInfos(const unsigned int& numTrials, const float& timeDistTrial, const unsigned int& maxNumConnections);
		NETWORKING_API virtual void GetConnectionTrialInfos(unsigned int& numTrials, float& timeDistTrial, unsigned int& maxNumConnections ) const;
		NETWORKING_API friend bool operator==( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs );
		NETWORKING_API friend bool operator!=( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs );

		/**	@brief		Clone method
		*	@return										Deep-copy of the object
		*	@exception									None
		*	@remarks									You should implement the method using the copy constructor of the derived (and the base) class
		*/
		NETWORKING_API virtual std::unique_ptr< CGatewayLoginData > Clone(void) const = 0;
	protected:
		bool IsBaseClassValid(void) const;

		/**	@brief		Equality comparison method
		*	@param		rhs								Object to be compared with the present object
		*	@return										True if the objects are equal, false otherwise
		*	@exception									None
		*	@remarks									This method is required for the equality and unequality operators
		*/
		virtual bool IsEqual( const CGatewayLoginData& rhs ) const = 0;
	private:
		int numTrials;
		float timeDistTrial;
		unsigned int maxNumConnections;
		bool isBaseClassValid;
	};

	NETWORKING_API bool operator==( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs );
	NETWORKING_API bool operator!=( const CGatewayLoginData& lhs, const CGatewayLoginData& rhs );
}
/*@}*/
