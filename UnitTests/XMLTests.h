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
/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include "XMLGatewayLoginDatabaseTest.h"
#include "XMLSettingsParamTest.h"
#include "XMLEmailLoginDataTest.h"
#include "XMLEmailMessageTest.h"
#include "XMLGroupalarm2LoginDataTest.h"
#include "XMLGroupalarm2MessageTest.h"
#include "XMLExternalProgramLoginDataTest.h"
#include "XMLExternalProgramMessageTest.h"
#include "XMLAlarmValiditiesTest.h"
#include "XMLWeeklyValidityTest.h"
#include "XMLMonthlyValidityTest.h"
#include "XMLSingleTimeValidityTest.h"
#include "XMLAlarmMessagesDatabaseTest.h"

using boost::unit_test::label;


// the XML headers require wrapping into a cpp-file because the Poco AbstractConfiguration.hpp seems to expose windows.h which causes problems


/*@{*/
/** \ingroup Utility
*/
namespace Utilitites {
	/*@{*/
	/** \ingroup XMLTest
	*/
	namespace XMLTest {
		// Test section
		BOOST_AUTO_TEST_SUITE( XML_Test_test_suite, *label("basic") );

		/**	@brief		Testing XML serialization and deserialization of the CGatewayLoginDatabase class
		*/
		BOOST_AUTO_TEST_CASE( XML_GatewayLoginDatabase_test_case )
		{
			BOOST_REQUIRE( XMLGatewayLoginDatabaseTest::EmptyTest() );
			BOOST_REQUIRE( XMLGatewayLoginDatabaseTest::Test() );
		}


		/**	@brief		Testing XML serialization and deserialization of the CSettingsParam class
		*/
		BOOST_AUTO_TEST_CASE( XML_SettingsParam_test_case )
		{
			BOOST_REQUIRE( Middleware::XMLTest::XMLSettingsParamTest::Test() );
		}


		/**	@brief		Testing XML serialization and deserialization of the CEmailLoginData class
		*/
		BOOST_AUTO_TEST_CASE( XML_EmailLoginData_test_case )
		{
			BOOST_REQUIRE( XMLEmailLoginDataTest::Test() );
		}


		/**	@brief		Testing XML serialization and deserialization of the CGroupalarmLoginData class
		*/
		BOOST_AUTO_TEST_CASE( XML_GroupalarmLoginData_test_case )
		{
			BOOST_REQUIRE( XMLGroupalarmLoginDataTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CGroupalarmMessage class
		*/
		BOOST_AUTO_TEST_CASE( XML_GroupalarmMessageTest_test_case )
		{
			BOOST_REQUIRE( XMLGroupalarmMessageTest::TestDefinedUsers() );
			BOOST_REQUIRE(XMLGroupalarmMessageTest::TestAllUsers());
			BOOST_REQUIRE(XMLGroupalarmMessageTest::TestAlarmTemplate());
		}

		/**	@brief		Testing XML serialization and deserialization of the CEmailMessage class
		*/
		BOOST_AUTO_TEST_CASE( XML_EmailMessageTest_test_case )
		{
			BOOST_REQUIRE( XMLEmailMessageTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CExternalProgramLoginData class
		*/
		BOOST_AUTO_TEST_CASE( XML_ExternalProgramLoginData_test_case )
		{
			BOOST_REQUIRE( XMLExternalProgramLoginDataTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CExternalProgramMessage class
		*/
		BOOST_AUTO_TEST_CASE( XML_ExternalProgramMessageTest_test_case )
		{
			BOOST_REQUIRE( XMLExternalProgramMessageTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CAlarmValidities class
		*/
		BOOST_AUTO_TEST_CASE( XML_AlarmValiditiesTest_test_case )
		{
			BOOST_REQUIRE( XMLAlarmValiditiesTest::EmptyTest() );
			BOOST_REQUIRE( XMLAlarmValiditiesTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CWeeklyValidity class
		*/
		BOOST_AUTO_TEST_CASE( XML_WeeklyValidityTest_test_case )
		{
			BOOST_REQUIRE( XMLWeeklyValidityTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CMonthlyValidity class
		*/
		BOOST_AUTO_TEST_CASE( XML_MonthlyValidityTest_test_case )
		{
			BOOST_REQUIRE( XMLMonthlyValidityTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CSingleTimeValidity class
		*/
		BOOST_AUTO_TEST_CASE( XML_SingleTimeValidityTest_test_case )
		{
			BOOST_REQUIRE( XMLSingleTimeValidityTest::Test() );
		}

		/**	@brief		Testing XML serialization and deserialization of the CAlarmMessagesDatabase class
		*/
		BOOST_AUTO_TEST_CASE( XML_AlarmMessagesDatabaseTest_test_case )
		{
			BOOST_REQUIRE( XMLAlarmMessagesDatabaseTest::Test() );
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/
/*@}*/
