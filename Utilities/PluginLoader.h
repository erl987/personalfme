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
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <boost/filesystem.hpp>
#include "FileUtils.h"
#include "Poco/ClassLoader.h"


/*@{*/
/** \ingroup Plugins
*/
namespace Utilities {
	namespace Plugins {
		/**	@brief		Finds all dynamic libraries in a certain directory
		*	@param		pluginPath								Path to the plugin directory
		*	@return 											Container with the file names of all dynamic libraries in the directory (including extension)
		*	@exception 											None
		*	@remarks 											Library files are searched with uppercase file extensions
		*/
		inline std::vector<boost::filesystem::path> FindDynamicLibraries( const boost::filesystem::path& pluginPath )
		{
			using namespace std;
			using namespace boost::filesystem;
			string fileName, currFileExtension, requiredFileExtension, dynamicLibraryExtension;
			std::vector<path> fileList;

			dynamicLibraryExtension = Poco::SharedLibrary::suffix();
			dynamicLibraryExtension = dynamicLibraryExtension.substr( dynamicLibraryExtension.find( '.' ) ); // removes a leading "d" added by Poco in the debug-mode
			requiredFileExtension = dynamicLibraryExtension;
			transform( begin( requiredFileExtension ), end( requiredFileExtension ), begin( requiredFileExtension ), ::toupper );

			// search the directory for all files having the dynamic library extension
			directory_iterator endIt;
			for ( directory_iterator it( pluginPath ); it != endIt; it++ ) {
				currFileExtension = it->path().extension().string();
				transform( begin( currFileExtension ), end( currFileExtension ), begin( currFileExtension ), ::toupper );
				if ( currFileExtension.compare( requiredFileExtension ) == 0 ) {
					fileList.push_back( it->path() );
				}
			}

			return fileList;
		}


		/**	@brief		Loads all plugins with the base class T from the given directory
		*	@param		pluginPath								Path to the plugin directory
		*	@return 											Map containing unique pointers to instances of all plugins in the directory. The keys are the plugin IDs (equivalent to the names of the plugin classes).
		*	@exception 											None
		*	@remarks 											Library files are searched with uppercase file extensions
		*/
		template <typename T>
		std::map< std::string, std::unique_ptr<T> > LoadPlugins( const boost::filesystem::path& pluginPath )
		{
			Poco::ClassLoader<T> classLoader;

			using namespace std;
			using namespace boost::filesystem;

			vector<path> dynLibraries;
			vector<string> availablePlugins;
			map< string, unique_ptr<T> > plugins;
			string libString = "lib";

			// load all available plugins
			dynLibraries = FindDynamicLibraries( pluginPath );
			auto tempPathChanger = Utilities::File::TemporaryPathChange( pluginPath ); // will change back to the current directory when going out of scope
			for ( auto dynLibrary : dynLibraries ) {
				try {
					classLoader.loadLibrary( "./" + dynLibrary.filename().string() );
					availablePlugins.push_back( dynLibrary.stem().string() );
				} catch ( Poco::Exception e ) {
					continue; // shared library, but no plugin
				}
			}

			// create plugin instances
			for ( auto plugin : availablePlugins ) {
				try {
					#ifdef __linux
						// remove the "lib"-prefix
						auto libSubstrPos = plugin.find( libString );
						if ( libSubstrPos == 0 ) { 
							plugin.erase( libSubstrPos, libString.length() );
						}
					#endif
					plugins[plugin] = unique_ptr<T>( classLoader.create( plugin ) );
				} catch ( Poco::Exception e ) {
					continue; // the plugin does not contain the required class
				}
			}

			return plugins;
		}
	}
}
/*@}*/