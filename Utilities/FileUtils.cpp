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
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define UTILITY_API __attribute__ ((dllexport))
	#else
		// Microsoft Visual Studio
		#define UTILITY_API __declspec(dllexport)
	#endif
#endif

#include <string>
#include <vector>
#include "FileUtils.h"


/**	@brief		Obtains the path of a file or directory relative to another one
*	@param		path								File or directory for which the relative path is required (may be with trailing "\" or without)
*	@param		base								Base path for comparison. Any input is considered as a directory path.
*	@return											Path of 'path' relative to the 'base' path
*	@exception	std::logic_error					Thrown if the base path is invalid
*	@remarks 										This function does not handle symbolic links correctly
*/
boost::filesystem::path Utilities::File::GetRelativePath(const boost::filesystem::path& path, const boost::filesystem::path& base) {
	using namespace std;
	using namespace boost::filesystem;

	boost::filesystem::path result;
	vector<string> baseLevels, pathLevels;

	if ( path.has_root_path() ) {		
        if ( path.root_path() != base.root_path() ) {
            // in case of different directories the original path is returned
			return path;
        } else {
			// the directory name is removed and the function is iteratively called
            return GetRelativePath( path.relative_path(), base.relative_path() );
        }
    } else {
        if ( base.has_root_path() ) {
            throw std::logic_error( "Base path is not valid." );
        } else {
			// perform the comparison of the two paths
            path::const_iterator pathIt = path.begin();
            path::const_iterator baseIt = base.begin();

			// iterate from the root into the directories until the paths are differing
            while ( ( pathIt != path.end() ) && ( baseIt != base.end() ) ) {
                if ( *pathIt != *baseIt ) {
					break;
				}
                pathIt++;
				baseIt++;
            }

			// obtain the different parts of the path
			while ( baseIt != base.end() ) {
				baseLevels.push_back( baseIt->string() );
				baseIt++;
			}

            while ( pathIt != path.end() ) {
				pathLevels.push_back( pathIt->string() );
				pathIt++;
            }

			// remove any dots from the paths (in order to handle trailing "\" in the paths
			auto removeBaseIt = remove_if( baseLevels.begin(), baseLevels.end(), [](std::string val){ return ( val == "." ); } );
			baseLevels.erase( removeBaseIt, baseLevels.end() );
			auto removePathIt = remove_if( pathLevels.begin(), pathLevels.end(), [](std::string val){ return ( val == "." ); } );
			pathLevels.erase( removePathIt, pathLevels.end() );

			// replace the common part of the path by single or double dots
			if ( baseLevels.size() == 0 ) {
				result /= ".";
			} else {
				for (size_t i = 0; i < baseLevels.size(); i++) {
					result /= "..";
				}
			}

			// add the final parts of the relative path
			for (size_t i=0; i < pathLevels.size(); i++) {
				result /= pathLevels[i];
			}

            return result;
        }
    }
}


/**	@brief		Changes the current path and changes back to the previous path as soon as the object goes out of scope
*	@param		newPath								New current path
*	@exception										None
*	@remarks 										None
*/
Utilities::File::TemporaryPathChange::TemporaryPathChange( const boost::filesystem::path& newPath )
	: previousPath( boost::filesystem::current_path() )
{
	boost::filesystem::current_path( newPath );
}


/**	@brief		Destructor.
*	@remarks 										Changes the current path back to the previous path
*/
Utilities::File::TemporaryPathChange::~TemporaryPathChange() {
	boost::filesystem::current_path( previousPath );
}