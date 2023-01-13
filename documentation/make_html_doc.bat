@REM PersonalFME - Gateway linking analog radio selcalls to internet communication services
@REM Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

@REM This program is free software: you can redistribute it and / or modify
@REM it under the terms of the GNU General Public License as published by
@REM the Free Software Foundation, either version 3 of the License, or
@REM (at your option) any later version.

@REM This program is distributed in the hope that it will be useful,
@REM but WITHOUT ANY WARRANTY; without even the implied warranty of
@REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
@REM GNU General Public License for more details.

@REM You should have received a copy of the GNU General Public License
@REM along with this program.If not, see <http://www.gnu.org/licenses/>-->

@REM ---------------------------------------------------------------------
@REM ATTENTION: Before running this script you need to update / generate all XML-files from the templates using CMake!

java net.sf.saxon.Transform -s:benutzerhandbuch.xml -xsl:"benutzerhandbuch_html.xsl" -o:benutzerhandbuch.html -xi
