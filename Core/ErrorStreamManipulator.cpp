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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define AUDIOSP_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define AUDIOSP_API __declspec(dllexport)
	#endif
#endif

#ifdef __linux
	#include <unistd.h>
	#include <fcntl.h>
#else
	#include <io.h>
#endif
#include "ErrorStreamManipulator.h"
#include <iostream>


/** @brief		Standard constructor.
*	@remarks	The original state is restored after executing the destructor.
*/
Core::Processing::Helper::CErrorStreamManipulator::CErrorStreamManipulator()
{
	std::lock_guard<std::mutex> lock( mutex );

	if ( numInstances == 0 ) {
#ifdef __linux
		if ( isatty(STDERR_FILENO) == 1 ) {
			isReferringToTerminal = true;

			// close the C-error stream
			int fd = open( "/dev/null", O_RDWR );
			dup2( fd, STDERR_FILENO );
		}
		else {
			isReferringToTerminal = false;
		}
#else
		if ( _isatty(_fileno(stderr)) != 0 ) {
			isReferringToTerminal = true;

			// close the C-error stream
			fclose( stderr );
		} else {
			isReferringToTerminal = false;
		}
#endif		
	}
	numInstances++;
}


/** @brief		Destructor.
*/
Core::Processing::Helper::CErrorStreamManipulator::~CErrorStreamManipulator()
{
	std::lock_guard<std::mutex> lock( mutex );

	numInstances--;
	if ( numInstances == 0 ) {
		if ( isReferringToTerminal ) {
			// reopen the C-error stream
#ifdef __linux
			int fd = open( "/dev/tty", O_RDWR );
			dup2( fd, STDERR_FILENO );
#else
			FILE* stderrFile;
			freopen_s( &stderrFile, "CON", "w", stderr );
#endif	
		}
	}
}


// instantiation of the static class members
std::mutex Core::Processing::Helper::CErrorStreamManipulator::mutex;
unsigned int Core::Processing::Helper::CErrorStreamManipulator::numInstances = 0;
bool Core::Processing::Helper::CErrorStreamManipulator::isReferringToTerminal = false;