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

#include <string>
#include <regex>
#include "AlarmMessage.h"


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
		/**	@brief	Maximum length allowed for a SMS-message */
		const int maxSMSLength = 160;

		/**	@brief	Phone number, allowed formats: "+492 343 3434-345", "(03434) 3434 234-342" or "23434 34234-34" */
		const std::regex regExPhoneNumber( "([+][\\d]{1,}[\\d\\s\\-]{1,})|([(][0][\\d\\s]{1,}[)][\\s]?[\\d\\s\\-]{1,})|([\\d][\\d\\s\\-]{0,})" );
		
		/**	@brief	Groupalarm (permanent x-)text code, allowed format: "5", "4", ... */
		const std::regex regExGroupalarmTextCode( "\\d{1,}" );

		/**	@brief	Find all minus, whitespace and bracket characters in the phone number */
		const std::regex regExReplacePhoneNumber( "[\\-\\s()]" );

		/**	\ingroup Networking
		*	Class implementing a dataset for the www.Groupalarm.de calling parameters.
		*/
		class CGroupalarmMessage : public CAlarmMessage
		{
		public:
			NETWORKING_API CGroupalarmMessage();
			NETWORKING_API CGroupalarmMessage( const std::string& alarmPhoneNum, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax );
			NETWORKING_API CGroupalarmMessage( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax );
			NETWORKING_API virtual ~CGroupalarmMessage() {};
			NETWORKING_API std::unique_ptr< External::CAlarmMessage > Clone() const override;
			NETWORKING_API void Set( const std::string& alarmPhoneNum, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax );
			NETWORKING_API void Set( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax );
			NETWORKING_API virtual void SetEmpty() override;
			NETWORKING_API virtual bool IsEmpty() const override;		
			NETWORKING_API bool GetAlarmString( std::string& alarmString ) const;
			NETWORKING_API bool GetAlarmData( std::string& alarmPhoneNum, std::vector<int>& alarmLists, std::vector<int>& alarmGroups ) const;
			NETWORKING_API bool GetAlarmMessage( std::string& messageText ) const;
			NETWORKING_API void GetSettings( bool& isLoadFeedback, bool& isFlashSMS, bool& isNoFax ) const;
			NETWORKING_API virtual std::type_index GetGatewayType() const override;
			NETWORKING_API static std::string GetSMSMessageWithValidLength( const std::string& text );
		protected:
			NETWORKING_API virtual bool IsEqual( const CAlarmMessage& rhs ) const override;
			NETWORKING_API virtual bool IsSmaller( const CAlarmMessage& rhs ) const override;
			NETWORKING_API void Set( const std::string& alarmPhoneNum, const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups, const bool& isFreeText, const std::string& messageText, const bool& isLoadFeedback, const bool& isFlashSMS, const bool& isNoFax );
			NETWORKING_API static std::string FormatPhoneNumber( const std::string& phoneNumberString );
			NETWORKING_API static void CheckPhoneNumberFormat( const std::string& alarmPhoneNum );
			NETWORKING_API static void CheckMessageFormat( const bool& isFreeText, const std::string& messageText );
			NETWORKING_API static void CheckListGroupFormat( const std::vector<int>& alarmLists, const std::vector<int>& alarmGroups );
			NETWORKING_API static bool CheckSMSMessageLength( std::string text );
		private:
			std::string alarmPhoneNum;
			std::vector<int> alarmLists;
			std::vector<int> alarmGroups;
			bool isFreeText;
			std::string messageText;
			bool isLoadFeedback;
			bool isFlashSMS;
			bool isNoFax;
			bool isEmpty;
		};

		
		namespace Exception {
			/**	This class provides an exception for Groupalarm.de list code errors
			*/
			class GroupalarmListCodeIncorrect : public virtual std::logic_error
			{
			public:
				GroupalarmListCodeIncorrect( const char* errorMessage )
					: logic_error( errorMessage )
				{}
			};


			/**	This class provides an exception for Groupalarm.de phone number errors
			*/
			class GroupalarmPhoneNumberIncorrect : public virtual std::logic_error
			{
			public:
				GroupalarmPhoneNumberIncorrect(const char* errorMessage)
				: logic_error( errorMessage )
				{}
			};


			/**	This class provides an exception for Groupalarm.de text code errors
			*/
			class GroupalarmTextCodeIncorrect : public virtual std::logic_error
			{
			public:
				GroupalarmTextCodeIncorrect(const char* errorMessage)
				: logic_error( errorMessage )
				{}
			};


			/**	This class provides an exception for Groupalarm.de free text message length errors
			*/
			class GroupalarmFreeTextLength : public virtual std::logic_error
			{
			public:
				GroupalarmFreeTextLength(const char* errorMessage)
				: logic_error( errorMessage )
				{}
			};
		}
	}
}
/*@}*/
