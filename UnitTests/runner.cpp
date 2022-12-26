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
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "Unit Tests for PersonalFME"

#include <boost/test/unit_test.hpp>

// test suites
#include "XMLTests.h"
#include "ConnectionThreadTest.h"
#include "ConnectionManagerTest.h"
#include "ExternalProgramGatewayTest.h"
#include "ExternalProgramLoginDataTest.h"
#include "ExternalProgramMessageTest.h"
#include "EmailGatewayTest.h"
#include "EmailLoginDataTest.h"
#include "EmailMessageTest.h"
#include "AlarmGatewaysManagerTest.h"
#include "Groupalarm2GatewayTest.h"
#include "Groupalarm2LoginDataTest.h"
#include "Groupalarm2MessageTest.h"
#include "GatewayLoginDatabaseTest.h"
#include "FileUtilsTest.h"
#include "AlarmValiditiesTest.h"
#include "AlarmMessageDatabaseTest.h"
#include "GeneralStatusMessageTest.h"
#include "DetectorStatusMessageTest.h"
#include "SendStatusMessageTest.h"
#include "InfoalarmMessageDecoratorTest.h"
#include "MonthlyValidityTest.h"
#include "WeeklyValidityTest.h"
#include "SingleTimeValidityTest.h"
#include "DefaultValidityTest.h"
#include "fmeDetectionTest.h"
#include "filterTest.h"
#include "fftTest.h"
#include "audioSignalReaderTest.h"
#include "sequencePasserTest.h"
#include "sequencePasserDebugTest.h"
#include "audioFullDownsamplerTest.h"
#include "audioSignalPreserverTest.h"
#include "SeqDataCompleteTest.h"
#include "SeqDataTest.h"
#include "SettingsParamTest.h"
#include "TimeTest.h"
#include "DateTimeTest.h"
#include "CodeDataTest.h"
#include "BoostStdTimeConverterTest.h"
#include "germanLocalDateTimeTest.h"
#include "SerializableCodeDataTest.h"
#include "SerializableTimeTest.h"
#include "SerializableDateTimeTest.h"
#include "SerializableSeqDataTest.h"
#include "SerializableSeqDataCompleteTest.h"
#include "dataProcessingTest.h"
#include "SimpleHTTPClientTest.h"
#include "portaudioTest.h"
#include "OGGHandlerTest.h"
