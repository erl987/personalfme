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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include "MediaFile.h"

/**	@brief		Standard constructor
*/
Utilities::CMediaFile::CMediaFile()
	: isEmpty( true )
{
}


/**	@brief		Standard constructor
*	@param		filePath			Path of the file (including the file name)
*	@param		MIMEtype			MIME-type of the file
*	@exception						None
*	@remarks						None
*/
Utilities::CMediaFile::CMediaFile( const boost::filesystem::path& filePath, const std::string& MIMEtype )
	: isEmpty( true )
{
	Reset( filePath, MIMEtype );
}


/**	@brief		Destructor
*/
Utilities::CMediaFile::~CMediaFile()
{
}


/**	@brief		Resetting the file
*	@param		filePath			Path of the file (including the file name)
*	@param		MIMEtype			MIME-type of the file
*	@return							None
*	@exception						None
*	@remarks						None
*/
void Utilities::CMediaFile::Reset( const boost::filesystem::path& filePath, const std::string& MIMEtype )
{
	CMediaFile::filePath = filePath;
	CMediaFile::MIMEtype = MIMEtype;
	isEmpty = false;
}


/**	@brief		Obtains the path of the file
*	@return							Path of the file (including the file name)
*	@exception						None
*	@remarks						None
*/
boost::filesystem::path Utilities::CMediaFile::GetFilePath() const
{
	return filePath;
}


/**	@brief		Obtains the MIME-type of the file
*	@return							MIME-type of the file
*	@exception						None
*	@remarks						None
*/
std::string Utilities::CMediaFile::GetMIMEtype() const
{
	return MIMEtype;
}


/**	@brief		Returns if the media file is set or if it is empty
*	@return							True if it is set, false otherwise
*	@exception						None
*	@remarks						None
*/
bool Utilities::CMediaFile::IsEmpty() const
{
	return isEmpty;
}


/**	@brief		Empties the file data
*	@return							None
*	@exception						None
*	@remarks						None
*/
void Utilities::CMediaFile::Clear()
{
	CMediaFile::filePath.clear();
	CMediaFile::MIMEtype.clear();
	isEmpty = true;
}


namespace Utilities {
	/**	@brief		Equality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of equality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator==( const CMediaFile& lhs, const CMediaFile& rhs )
	{
		if ( lhs.IsEmpty() != rhs.IsEmpty() ) {
			return false;
		}
		if ( lhs.GetFilePath() != rhs.GetFilePath() ) {
			return false;
		}
		if ( lhs.GetMIMEtype() != rhs.GetMIMEtype() ) {
			return false;
		}

		return true;
	}


	/**	@brief		Inequality operator
	*	@param		lhs						Left-hand side operand
	*	@param		rhs						Right-hand side operand
	*	@return								True in case of ienquality of the operands, else false
	*	@exception							None
	*	@remarks							None
	*/
	bool operator!=( const CMediaFile& lhs, const CMediaFile& rhs )
	{
		return !( lhs == rhs );
	}
}