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
#include <vector>
#include <deque>
#include <tuple>
#include <string>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include "SeqData.h"
#include "StatusMessage.h"
#include "AlarmGatewaysManager.h"
#include "SettingsParam.h"

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
	*	Abstract class implementing the runtime environment of the program
	*/
	class CExecutionRuntime
	{
	public:
		Middleware_API CExecutionRuntime( const CSettingsParam& newParams, const boost::filesystem::path& appSettingsDir, std::function<void( const std::string& )> runtimeErrorCallback, std::function<void( std::unique_ptr<Utilities::Message::CStatusMessage> )> messageFromDetectorCallback );
		Middleware_API virtual ~CExecutionRuntime(void);
		Middleware_API virtual void Run(void) = 0;
	protected:
		Middleware_API virtual void OnProcessingSequence(const Utilities::CSeqData& sequence) = 0;
		Middleware_API virtual void OnRuntimeError(const std::string& errorString);
		Middleware_API virtual void OnRuntimeException( const std::exception_ptr& error );
		Middleware_API virtual void OnStatusMessage( std::unique_ptr<Utilities::Message::CStatusMessage> message);
		Middleware_API virtual void BasicSequenceProcessing(const Utilities::CSeqData& sequenceData);
		Middleware_API virtual CSettingsParam GetParams(void);
		boost::filesystem::path appSettingsDir;
		template <typename OutIt> OutIt GetCurrentBlacklist(OutIt sequencesBlacklist);

		std::function<void(const Utilities::CSeqData&)> onFoundSequenceCallback;
		External::CAlarmGatewaysManager gateways;
	private:
		CSettingsParam params;
		std::function< void( const std::string& ) > runtimeErrorCallback;
		std::function< void( std::unique_ptr<Utilities::Message::CStatusMessage> ) > statusMessageCallback;
		std::mutex sequencesBlacklistMutex;
		std::deque< std::tuple< boost::posix_time::ptime, std::vector<int> > > sequencesBlacklist;
		std::mutex lastSequenceMutex;
		std::tuple< boost::posix_time::ptime, std::vector<int> > lastSequence;
	};
}
/*@}*/



/**	@brief		Obtain current list of not allowed sequences
*	@param		sequencesBlacklistIt				Begin iterator to blacklist with all currently not allowed sequences
*	@return 										Iterator past the end of the blacklist
*	@exception 										None
*	@remarks 										None
*/
template <typename OutIt>
OutIt Middleware::CExecutionRuntime::GetCurrentBlacklist(OutIt sequencesBlacklistIt)
{
	using namespace std;

	std::lock_guard<std::mutex> lock( sequencesBlacklistMutex );
	for_each( sequencesBlacklist.cbegin(), sequencesBlacklist.cend(), [&](tuple< boost::posix_time::ptime, vector<int> > currSequence){
		*(sequencesBlacklistIt++) = currSequence;
	} );

	return sequencesBlacklistIt;
}
