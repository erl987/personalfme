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
/*@{*/
/** \ingroup UnitTests
*/

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "FileUtils.h"

using boost::unit_test::label;


/*@{*/
/** \ingroup Utility
*/

/**	\defgroup	FileUtils		Unit test for the file utilities
*/

namespace Utilitites {
	/*@{*/
	/** \ingroup FileUtils
	*/
	namespace FileUtils {
		// Test section
		BOOST_AUTO_TEST_SUITE( FileUtils_test_suite, *label("basic") );

		/**	@brief		Testing of of relative path determination
		*/
		BOOST_AUTO_TEST_CASE( GetRelativePath_test_case )
		{
			using namespace boost::filesystem;
			#ifdef _WIN32
				// testing of standard operations in both directions
				path inputPath1 = "C:\\level1\\level2\\level3\\level4\\file.end";
				path basePath1 = "C:\\level1\\level2\\level3\\level4\\level5\\level6";
				path correctRelativePath1 = "..\\..\\file.end";
				path testRelativePath1 = Utilities::File::GetRelativePath( inputPath1, basePath1 );
				BOOST_REQUIRE( testRelativePath1 == correctRelativePath1 );

				path inputPath2 = "C:\\level1\\level2\\level3\\level4\\file.end";
				path basePath2 = "C:\\level1\\level2\\level3\\level4\\level5\\level6\\";
				path correctRelativePath2 = "..\\..\\file.end";
				path testRelativePath2 = Utilities::File::GetRelativePath( inputPath2, basePath2 );
				BOOST_REQUIRE( testRelativePath2 == correctRelativePath2 );

				path inputPath3 = "C:\\level1\\level2\\level3\\level4\\level5\\level6\\file.end";
				path basePath3 = "C:\\level1\\level2\\level3\\level4\\";
				path correctRelativePath3 = ".\\level5\\level6\\file.end";
				path testRelativePath3 = Utilities::File::GetRelativePath( inputPath3, basePath3 );
				BOOST_REQUIRE( testRelativePath3 == correctRelativePath3 );

				path inputPath4 = "C:\\level1\\level2\\level3\\level4\\level5\\level6\\file.end";
				path basePath4 = "C:\\level1\\level2\\level3\\level4";
				path correctRelativePath4 = ".\\level5\\level6\\file.end";
				path testRelativePath4 = Utilities::File::GetRelativePath( inputPath4, basePath4 );
				BOOST_REQUIRE( testRelativePath4 == correctRelativePath4 );
				
				// testing of different drives
				path inputPath5 = "D:\\level1\\level2\\level3\\level4\\level5\\level6\\file.end";
				path basePath5 = "C:\\level1\\level2\\level3\\level4\\";
				path correctRelativePath5 = inputPath5;
				path testRelativePath5 = Utilities::File::GetRelativePath( inputPath5, basePath5 );
				BOOST_REQUIRE( testRelativePath5 == correctRelativePath5 );

				// testing on the level of drives directly
				path inputPath6 = "C:\\level1\\level2\\level3\\level4\\level5\\level6\\file.end";
				path basePath6 = "C:\\";
				path correctRelativePath6 = ".\\level1\\level2\\level3\\level4\\level5\\level6\\file.end";
				path testRelativePath6 = Utilities::File::GetRelativePath( inputPath6, basePath6 );
				BOOST_REQUIRE( testRelativePath6 == correctRelativePath6 );
					
				// testing of a directory as inputPath
				path inputPath7 = "C:\\level1\\level2\\level3\\level4\\level5\\level6\\";
				path basePath7 = "C:\\level1\\level2\\level3\\level4";
				path correctRelativePath7 = ".\\level5\\level6";
				path testRelativePath7 = Utilities::File::GetRelativePath( inputPath7, basePath7 );
				BOOST_REQUIRE( testRelativePath7 == correctRelativePath7 );

				path inputPath8 = "C:\\level1\\level2\\";
				path basePath8 = "C:\\level1\\level2\\level3\\level4";
				path correctRelativePath8 = "..\\..";
				path testRelativePath8 = Utilities::File::GetRelativePath( inputPath8, basePath8 );
				BOOST_REQUIRE( testRelativePath8 == correctRelativePath8 );

				// testing of file as basePath
				path inputPath9 = "C:\\level1\\level2\\";
				path basePath9 = "C:\\level1\\level2\\level3\\level4\\file.end";
				path correctRelativePath9 = "..\\..\\.."; // by design "file.end" must be considered as directory
				path testRelativePath9 = Utilities::File::GetRelativePath( inputPath9, basePath9 );
				BOOST_REQUIRE( testRelativePath9 == correctRelativePath9 );

				// testing of complexe file and directory names
				path inputPath10 = "C:\\level1 part2\\level2\\file.end";
				path basePath10 = "C:\\level1 part2\\level2\\level3\\level4 part 2";
				path correctRelativePath10 = "..\\..\\file.end";
				path testRelativePath10 = Utilities::File::GetRelativePath( inputPath10, basePath10 );
				BOOST_REQUIRE( testRelativePath10 == correctRelativePath10 );
			#else
				// Linux
				// testing of standard operations in both directions
				path inputPath1 = "/level1/level2/level3/level4/file.end";
				path basePath1 = "/level1/level2/level3/level4/level5/level6";
				path correctRelativePath1 = "../../file.end";
				path testRelativePath1 = Utilities::File::GetRelativePath( inputPath1, basePath1 );
				BOOST_REQUIRE( testRelativePath1 == correctRelativePath1 );

				path inputPath2 = "/level1/level2/level3/level4/file.end";
				path basePath2 = "/level1/level2/level3/level4/level5/level6/";
				path correctRelativePath2 = "../../file.end";
				path testRelativePath2 = Utilities::File::GetRelativePath( inputPath2, basePath2 );
				BOOST_REQUIRE( testRelativePath2 == correctRelativePath2 );

				path inputPath3 = "/level1/level2/level3/level4/level5/level6/file.end";
				path basePath3 = "/level1/level2/level3/level4/";
				path correctRelativePath3 = "./level5/level6/file.end";
				path testRelativePath3 = Utilities::File::GetRelativePath( inputPath3, basePath3 );
				BOOST_REQUIRE( testRelativePath3 == correctRelativePath3 );

				path inputPath4 = "/level1/level2/level3/level4/level5/level6/file.end";
				path basePath4 = "/level1/level2/level3/level4";
				path correctRelativePath4 = "./level5/level6/file.end";
				path testRelativePath4 = Utilities::File::GetRelativePath( inputPath4, basePath4 );
				BOOST_REQUIRE( testRelativePath4 == correctRelativePath4 );
				
				// testing on the level of drives directly
				path inputPath6 = "/level1/level2/level3/level4/level5/level6/file.end";
				path basePath6 = "/";
				path correctRelativePath6 = "./level1/level2/level3/level4/level5/level6/file.end";
				path testRelativePath6 = Utilities::File::GetRelativePath( inputPath6, basePath6 );
				BOOST_REQUIRE( testRelativePath6 == correctRelativePath6 );
					
				// testing of a directory as inputPath
				path inputPath7 = "/level1/level2/level3/level4/level5/level6/";
				path basePath7 = "/level1/level2/level3/level4";
				path correctRelativePath7 = "./level5/level6";
				path testRelativePath7 = Utilities::File::GetRelativePath( inputPath7, basePath7 );
				BOOST_REQUIRE( testRelativePath7 == correctRelativePath7 );

				path inputPath8 = "/level1/level2/";
				path basePath8 = "/level1/level2/level3/level4";
				path correctRelativePath8 = "../..";
				path testRelativePath8 = Utilities::File::GetRelativePath( inputPath8, basePath8 );
				BOOST_REQUIRE( testRelativePath8 == correctRelativePath8 );

				// testing of file as basePath
				path inputPath9 = "/level1/level2/";
				path basePath9 = "/level1/level2/level3/level4/file.end";
				path correctRelativePath9 = "../../.."; // by design "file.end" must be considered as directory
				path testRelativePath9 = Utilities::File::GetRelativePath( inputPath9, basePath9 );
				BOOST_REQUIRE( testRelativePath9 == correctRelativePath9 );

				// testing of complexe file and directory names
				path inputPath10 = "/level1 part2/level2/file.end";
				path basePath10 = "/level1 part2/level2/level3/level4 part 2";
				path correctRelativePath10 = "../../file.end";
				path testRelativePath10 = Utilities::File::GetRelativePath( inputPath10, basePath10 );
				BOOST_REQUIRE( testRelativePath10 == correctRelativePath10 );
			#endif
		}

		BOOST_AUTO_TEST_SUITE_END();
	}
}

/*@}*/
/*@}*/