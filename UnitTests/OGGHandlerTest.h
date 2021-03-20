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
/*@{*/
/** \ingroup UnitTests
*/

#include <random>
#include <chrono>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp> 
#include "PluginLoader.h"
#include "AudioPlugin.h"


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	OGGHandler		Unit test for the class OGGHandler
*/
namespace Utilitites {
	namespace AudioFormats {
		/*@{*/
		/** \ingroup OGGHandler
		*/
		namespace OGGHandler {
			// Test section
			BOOST_AUTO_TEST_SUITE( OGGHandler_test_suite );

			/**	@brief		Testing storage of OGG-files
			*/
			BOOST_AUTO_TEST_CASE( OGGHandler_test_case )
			{
				using namespace std;
				using namespace Utilities::Plugins;

				int samplingFreq = 44100;
				auto testLength = 30s;
				string testFileName = "testFile.ogg";
				#if defined( _WIN32 )
					// Windows
					#ifdef NDEBUG
						boost::filesystem::path pluginPath = "../Release";
					#else
						boost::filesystem::path pluginPath = "../Debug";
					#endif
				#elif defined( __linux )
					// Linux
					boost::filesystem::path pluginPath = ".";
				#endif
				vector<float> data;
				map< string, unique_ptr<Utilities::Plugins::AudioPlugin> > plugins;
				unique_ptr<Utilities::Plugins::AudioPlugin> audioPlugin;

				// load the audio format plugins
				plugins = LoadPlugins<Utilities::Plugins::AudioPlugin>( pluginPath );
				audioPlugin = move( plugins[ AudioPlugin::CompleteID( "OGG" ) ] );
				audioPlugin->SetSamplingFreq( samplingFreq );

				// prevent overwriting of an existing file
				if ( boost::filesystem::exists( testFileName ) ) {
					BOOST_FAIL( testFileName + " already exists. Choose another test file name." );	
				}

				// generate random test data
				random_device rd;
				mt19937 gen( rd() );
				uniform_real_distribution<float> dist( -1.0f, 1.0f );
				auto randomValue = bind( dist, gen );
				data.resize( static_cast<int>( testLength.count() * samplingFreq ) );
				for ( auto& element : data ) {
					element = randomValue();
				}

				// generate OGG-file
				auto startTime = chrono::high_resolution_clock::now();
				audioPlugin->Save( testFileName, begin( data ), end( data ), false );
				auto endTime = chrono::high_resolution_clock::now();
				cout << "Vorbis encoding time: " << chrono::duration_cast<chrono::milliseconds>( endTime - startTime ).count() << " ms" << endl;

				// check for existence of the OGG-file
				BOOST_REQUIRE( boost::filesystem::exists( testFileName ) ); 
				BOOST_REQUIRE( boost::filesystem::file_size( testFileName ) > 0 );

				// delete OGG-file
				if ( boost::filesystem::exists( testFileName ) ) {
					boost::filesystem::remove( boost::filesystem::path( testFileName ) );
				}
			}

			
			BOOST_AUTO_TEST_SUITE_END();
		}
	}
}

/*@}*/
/*@}*/