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
#include <memory>
#include <mutex>
#include <string>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/NetException.h>
#ifndef _WIN32
	#include <openssl/err.h>
	#include <openssl/engine.h>
	#include <openssl/conf.h>
#endif
#include "EmailMessage.h"
#include "Groupalarm2Message.h"
#include "ExternalProgramMessage.h"
#include "GatewayLoginData.h"
#include "InfoalarmMessageDecorator.h"
#include "EmailGateway.h"

/*@{*/
/**		\ingroup Networking
*		String used in the alarm e-mail for identifying a default alarm
*/
const std::string DefaultAlarmString = u8"Einsatzalarmierung";

/**		\ingroup Networking
*		String used in the alarm e-mail for identifying a test alarm
*/
const std::string TestAlarmString = u8"Probealarm";

const std::string InfoAlarmString = u8"Infoalarm";


namespace External {
	namespace Email {
		/** \ingroup Networking
		*	Wrapper for the proper initialization of the SSL-library
		*/
		class SSLInitializer
		{
		public:
			SSLInitializer()
			{
				Poco::Net::initializeSSL();
			}

			~SSLInitializer()
			{
				Poco::Net::uninitializeSSL();

				#ifndef _WIN32
					// manual memory freeing (fixing a bug of Poco) - however by design 36 bytes cannot be freed by the OpenSSL-library
				#if OPENSSL_VERSION_NUMBER >= 0x30000000L
					EVP_default_properties_enable_fips(nullptr, 0);
				#else
					FIPS_mode_set(0);
				#endif
					ENGINE_cleanup();
					CONF_modules_unload( 1 );
					EVP_cleanup();
					CRYPTO_cleanup_all_ex_data();
					ERR_free_strings();
				#endif
			}
		};
	}
}


/**	\ingroup Networking
*	Private implementation class for the e-mail gateway
*/
class External::Email::CEmailGateway::CEmailGatewayImpl
{
public:
	CEmailGatewayImpl();
	virtual ~CEmailGatewayImpl() {};
	void Send( const std::vector<int>& code, const Utilities::CDateTime& alarmTime, const bool& isRealAlarm, std::unique_ptr<CGatewayLoginData> loginData, std::unique_ptr<CAlarmMessage> message, const Utilities::CMediaFile& audioFile );
protected:
	static void LoadRootCertificatesFromOSTrustStore();
	void SendSMTP( const Poco::Net::MailMessage& message, std::unique_ptr<CGatewayLoginData> loginData );
	bool IsTemporarySMTPServerError( const Poco::Net::SMTPException& e );
	static std::string CreateOtherMessagesInfo( const Infoalarm::CInfoalarmMessageDecorator& infoalarmMessage );
	static std::string CreateEmailMessageInfo( const External::Email::CEmailMessage& message );
	static std::string CreateGroupalarmMessageInfo( const External::Groupalarm::CGroupalarm2Message& message );
	static std::string CreateExternalProgramMessageInfo( const External::ExternalProgram::CExternalProgramMessage& message );
private:
	static std::unique_ptr<SSLInitializer> sslInitializer;
	static std::once_flag rootCertificatesLoadOnceFlag;
};
/*@}*/
