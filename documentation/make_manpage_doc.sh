#!/bin/sh

# PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)

# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.If not, see <http://www.gnu.org/licenses/>

# generates the UNIX-manpage from the XML-source
xsltproc -xinclude /usr/share/xml/docbook/stylesheet/docbook-xsl/manpages/docbook.xsl manpage.xml

# removes all leading tabs from the manpage file (causing wrong interpretation on Ubuntu 16.04 LTS)
sed "s/^[ \t]*//" -i personalfme.1

