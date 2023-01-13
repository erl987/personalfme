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
#include "AudioPlugin.h"


/*@{*/
/** \ingroup Plugins
*/

/**	\ingroup Plugins
*	Class representing a Ogg Vorbis-file handler plugin
*/
class OGGAudioPlugin : public Utilities::Plugins::AudioPlugin
{
public:
	OGGAudioPlugin();
	~OGGAudioPlugin();
	virtual void GetLibraryVersionInfo( std::string& pluginID, std::string& libraryName, std::string& versionString, std::string& dateString, std::string& licenseText ) const override;
	virtual void GetPluginVersionInfo( std::string& pluginID, std::string& versionString, std::string& dateString, std::string& licenseText ) const override;
	virtual std::string GetExtension() const override;
	virtual std::string GetMIMEtype() const override;
private:
	virtual void Save( const std::string& fileName, const std::vector<float>& data ) const override;
};
/*@}*/