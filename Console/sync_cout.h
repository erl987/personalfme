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
#include <ostream>
#include <sstream>
#include <memory>
#include <mutex>


/*@{*/
/** \ingroup PersonalFME
*	Thread safe console out class suitable for printing UTF-8 strings in the Windows console
*/
class sync_cout : public std::ostream
{
public:
	static sync_cout& Inst();
	virtual ~sync_cout();
private:
	class CBuffer : public std::stringbuf {
	public:
		CBuffer();
		~CBuffer() {};
		virtual int sync();
	private:
		std::stringstream stream;
		std::mutex outputMutex;
	};

	sync_cout();

	CBuffer buffer;
	static std::once_flag initFlag;
	static std::unique_ptr<sync_cout> instancePtr;
};
/*@}*/

std::ostream& operator<< ( std::ostream& out, const std::string& output );
std::ostream& operator<< ( std::ostream& out, const char* output );
