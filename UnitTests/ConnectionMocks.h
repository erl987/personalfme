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

#include <chrono>
#include <thread>
#include <random>
#include <boost/test/unit_test.hpp>
#include "AlarmGateway.h"
#include "GatewayLoginData.h"
#include "AlarmMessage.h"

/*@{*/
/** \ingroup UnitTests
*/
namespace Networking {
	/*@{*/
	/** \ingroup ConnectionMocks
	*/
	namespace ConnectionMocks {
		/**	@brief	Mock class implementing a test gateway
		*/
		class CMockGateway : public External::CAlarmGateway {
		public:
			CMockGateway() : CAlarmGateway() {};
			virtual void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<External::CGatewayLoginData> loginData, std::unique_ptr<External::CAlarmMessage> message, const Utilities::CMediaFile& audioFile ) override {
				using namespace std::literals;

				// wait for some time before finishing
				std::this_thread::sleep_for( std::chrono::milliseconds{ timeDist( eng ) } );

				// generate some (non-fatal) failures
				if ( failureDist( eng ) <= 9 ) {
					throw std::domain_error( "Non-fatal failure" );
				}
			};
			virtual bool IsEqual( const CAlarmGateway& rhs ) const override
			{
				try {
					dynamic_cast< const CMockGateway& >( rhs );
				} catch ( std::bad_cast e ) {
					return false;
				}

				return true;
			};

			virtual std::unique_ptr<External::CAlarmGateway> Clone() const override { return std::make_unique<CMockGateway>(); };
		private:
			std::mt19937_64 eng{ std::random_device{}( ) };
			std::uniform_int_distribution<> timeDist{ 0, 100 }; // in ms
			std::uniform_int_distribution<> failureDist{ 1, 10 };
		};



		/**	@brief	Mock class implementing the login for the test gateway
		*/
		class CMockLoginData : public External::CGatewayLoginData {
		public:
			virtual std::unique_ptr< External::CGatewayLoginData > Clone( void ) const override { 
				return std::make_unique<CMockLoginData>( *this );
			};
			virtual bool IsEqual( const CGatewayLoginData& rhs ) const override {
				try {
					dynamic_cast<const CMockLoginData&>( rhs );
				} catch ( std::bad_cast e ) {
					return false;
				}
				
				return true;
			};
		};



		/**	@brief	Mock class implementing a message for the test gateway
		*/
		class CMockMessage : public External::CAlarmMessage
		{
		public:
			CMockMessage(const std::string& messageID) : CAlarmMessage( true ), messageID( messageID ) {};
			virtual bool IsEmpty( void ) const override { return false; };
			virtual void SetEmpty( void ) override {};
			virtual std::unique_ptr< CAlarmMessage > Clone() const override { return std::make_unique< CMockMessage >( *this ); };
			virtual std::type_index GetGatewayType() const override { return typeid( ConnectionMocks::CMockGateway ); };
		protected:
			virtual bool IsEqual( const CAlarmMessage& rhs ) const override { return true; };
			virtual bool IsSmaller( const CAlarmMessage& rhs ) const override { return true; };
		private:
			std::string messageID;
		};
	}
}
/*@}*/