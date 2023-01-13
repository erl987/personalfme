/*	Ogg Vorbis audio file handler plugin for the software PersonalFME
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
#include <string>


/*@{*/
/** \ingroup Plugins
*	Class for handling the Ogg Vorbis-audio file format.
*/
class COGGHandler {
public:
	COGGHandler( const int& samplingFreq, const int& channels );
	virtual ~COGGHandler(){};
	static void GetLibsndfileVersion( std::string& versionString, std::string& dateString, std::string& licenseText );
	void Save( const std::string& fileName, const std::vector<float>& data );
private:
	int samplingFreq;
	int channels;
	int format;
};
/*@}*/
