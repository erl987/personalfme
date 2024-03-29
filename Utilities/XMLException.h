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
#include <string>
#include <stdexcept>


/*@{*/
/** \ingroup Utilities
*/
namespace Utilities {
	namespace Exception {
		/**	This class provides a XML processing exception inherited from std::exception
		*/
		class xml_error : public std::runtime_error
		{
		public:
			/**	@brief		Provides information on the thrown exception
			*	@return		Information on the XML error
			*	@remarks	None
			*/
			xml_error( const std::string& message )
				: std::runtime_error( message )
			{}
		};
	}
}
/*@}*/
