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

#include <Poco/Util/AbstractConfiguration.h>
#include "SettingsParam.h"
#include "XMLSerializable.h"

#if defined _WIN32 || defined __CYGWIN__
	#ifdef Middleware_API
		// All functions in this file are exported
	#else
		// All functions in this file are imported
		// Windows
		#ifdef __GNUC__
			// GCC
			#define Middleware_API __attribute__ ((dllimport))
		#else
			// Microsoft Visual Studio
			#define Middleware_API __declspec(dllimport)
		#endif
	#endif
#else
	// Linux
	#if __GNUC__ >= 4
		#define Middleware_API __attribute__ ((visibility ("default")))
	#else
		#define Middleware_API
	#endif		
#endif


/*@{*/
/** \ingroup Middleware
*/
namespace Middleware {
	/** \ingroup Middleware
	*	XML-serializable class representing user parameters for real-time FME-sequence detection
	*/
	class CXMLSerializableSettingsParam : public CSettingsParam, public Utilities::XML::CXMLSerializable
	{
	public:
		Middleware_API CXMLSerializableSettingsParam() {};
		Middleware_API virtual ~CXMLSerializableSettingsParam() {};
		Middleware_API CXMLSerializableSettingsParam( const CSettingsParam& other ) : CSettingsParam( other ) {};
		Middleware_API virtual void SetFromXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile );
		Middleware_API virtual void GenerateXML( Poco::AutoPtr<Poco::Util::AbstractConfiguration> xmlFile ) const;
	};
}
/*@}*/

