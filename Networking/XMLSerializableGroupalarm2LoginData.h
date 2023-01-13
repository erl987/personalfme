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

#include <Poco/Util/AbstractConfiguration.h>
#include "Groupalarm2LoginData.h"
#include "XMLSerializable.h"

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
	namespace Groupalarm {
		/** \ingroup Networking
		*	XML-serializable class representing the login data for sending alarms via the gateway www.groupalarm.de
		*/
		class CXMLSerializableGroupalarm2LoginData : public CGroupalarm2LoginData, public Utilities::XML::CXMLSerializable
		{
		public:
			NETWORKING_API CXMLSerializableGroupalarm2LoginData() {};
			NETWORKING_API CXMLSerializableGroupalarm2LoginData( const CGroupalarm2LoginData& other ) : CGroupalarm2LoginData( other ) {};
			NETWORKING_API virtual ~CXMLSerializableGroupalarm2LoginData() {};
			NETWORKING_API virtual void SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile );
			NETWORKING_API virtual void GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const;
		};
	}
}
/*@}*/

