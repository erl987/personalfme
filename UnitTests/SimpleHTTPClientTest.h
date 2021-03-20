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

#include <string>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include "SimpleHTTPClient.h"

/*@{*/
/** \ingroup UnitTests
*/


namespace Networking {
	/*@{*/
	/** \ingroup HTTPTests
	*/
	namespace HTTPTest {
		std::string server = "www.groupalarm.de";
		std::string path = "/webin.php?log_user=user&log_pass=pass&listcode=4&text=5&flash=1";
		
		// Test section
		BOOST_AUTO_TEST_SUITE( HTTP_test_suite );

		/**	@brief		Testing of HTTP without proxies
		*/
		BOOST_AUTO_TEST_CASE( Direct_HTTP_test_case )
		{
			std::string header, information;

			Networking::HTTP::CSimpleHTTPClient httpClient( server, path );
			BOOST_CHECK_NO_THROW( httpClient.Run() );
			httpClient.GetData(header, information);
                        std::cout << header << std::endl;
                        std::cout << information << std::endl;
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
	/*@}*/
}
/*@}*/
