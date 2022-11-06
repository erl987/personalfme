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
#if defined( _WIN32 )
	#include "stdafx.h"
#endif
#include <iostream>
#include "sync_cout.h"



/**	@brief		Access to the singleton instance
*	@return								Singleton instance that can be used for writing the stream
*	@remarks							None
*/
sync_cout& sync_cout::Inst()
{
	std::call_once( initFlag, [] { instancePtr.reset( new sync_cout() ); } );

	// return the singleton instance
	return *instancePtr;
}


/**	@brief		Constructor of the buffer class
*	@remarks							None
*/
sync_cout::CBuffer::CBuffer()
{
}


/**	@brief		Destructor
*/
sync_cout::~sync_cout()
{
}


/**	@brief		Constructor
*	@exception							None
*	@remarks							The constructor is private and can therefore only be used for instantiating the singleton
*/
sync_cout::sync_cout()
	: std::ostream( &buffer )
{
}


/**	@brief		Synchronizes the controlled character sequence (the buffers) with the associated character sequence
*	@return								Always 0 (meaning success)
*	@exception							Output stream
*	@remarks							Overloaded method of std::strinbuf
*/
int sync_cout::CBuffer::sync() {
	std::lock_guard<std::mutex> lock( outputMutex );

	// the Windows Console does not support UTF-8 strings with std::cout, therefore printf is required instead on Windows
	#ifdef _WIN32
		printf( str().c_str() );
	#else
		std::cout << str();
	#endif
	
	str( "" );

	return 0;
}


/**	@brief		Outstream operator for strings
*	@param		out						Output stream
*	@param		output					UTF-8 encoded output string to be written to console
*	@return								Output stream
*	@exception							None
*	@remarks							This stream operator is thread-safe regarding access to the console output
*/
std::ostream& operator<< ( std::ostream& out, const std::string& output )
{
	for ( auto character : output ) {
		out.put( character );
	}
	out.flush();

	return out;
}


/**	@brief		Outstream operator for C-strings
*	@param		out						Output stream
*	@param		output					UTF8 encoded output string to be written to console
*	@return								Output stream
*	@exception							None
*	@remarks							This stream operator is thread-safe regarding access to the console output
*/
std::ostream& operator<< ( std::ostream& out, const char* output )
{
	size_t length =	std::char_traits<char>::length( output );
	for ( size_t i = 0; i < length; i++ ) {
		out.put( output[i] );
	}
	out.flush();

	return out;
}


// instantiation of static class members
std::once_flag sync_cout::initFlag;
std::unique_ptr<sync_cout> sync_cout::instancePtr = nullptr;
