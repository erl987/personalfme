# PersonalFME - Gateway linking analog radio selcalls to internet communication services

*PersonalFME* is detecting selcalls ("Fünftonfolgen") within the analogue radio network of the German emergency services ("BOS") and is forwarding them in various ways such as SMS, e-mail and via external program calls. It requires an external radio receiver such as a radio device or a radio scanner that needs to be connected to the applied computer via a sound card.

*PersonalFME* can even run on mini-computers such as RaspberryPi in continuous operation.

*PersonalFME* is developed for the selcalls used by the German BOS, but may be adaptable to selcalls defined by the standards in other countries.

**Dies ist der Quellcode von *PersonalFME* und richtet sich an technisch ausgebildete Personen, daher ist der Inhalt vollständig in englischer Sprache. Eine ausführliche Anwenderdokumentation für die Allgemeinheit in deutscher Sprache findet sich hier:** `./documentation/benutzerhandbuch.pdf`

Detailed documentation about the configuration and operation of the program is available in the handbook (only in German): `./documentation/benutzerhandbuch.pdf`


## Supported platforms:
* Raspberry Pi OS 10 (Raspberry Pi 2 Model B and higher, only Raspberry Pi 3 Model B is actually tested)
* Debian 10 Buster
* Ubuntu 20.04 LTS
* Windows 10, 8.1 and 7

Previous versions of Linux are no longer supported. They can only be used with a previous version of *PersonalFME*.


## Linux

### 1. General

For Linux only the source code is currently available, see below for the compilation instructions.


### 2. Compiling the program including the installer

The program uses a CMake-based build system (requires CMake version 3.5.0 or higher). It is using C++14 and is tested to compile with GCC 8.3 and newer under Debian / Ubuntu Linux. The code is written in platform-independent C++ using platform-independent libraries. Compilation with other Linux-distributions should be straightforward.

An installer package will be created during the compilation.

Enable the Universe repository (only for Ubuntu):
```shell
sudo add-apt-repository universe
sudo apt update
``` 

#### Install most dependencies:
Most dependencies can be conveniently installed via the package manager `apt`:
```shell
sudo apt install build-essential cmake openssl libssl-dev portaudio19-dev libxerces-c-dev libsndfile-dev libalglib-dev ncurses-dev libboost-all-dev
```

#### Install the Poco version 1.8.1 manually:
The dependency Poco (1.9.0) shipped with Debian 10 / Ubuntu is containing a critical unfixed bug that prevents e-mail sending. The last working version of this library needs therefore to be compiled and installed manually. This will take some time. The installation procedure is described in detail on this webpage: https://github.com/pocoproject/poco

The general installation procedure is as follows (it is important to checkout the tag of the last working version):
```shell
sudo apt-get -y update && sudo apt-get -y install git g++ make cmake libssl-dev
git clone -b master https://github.com/pocoproject/poco.git
cd poco
git checkout tags/poco-1.8.1-release
mkdir cmake-build
cd cmake-build
cmake ..
cmake --build . --config Release -j n
sudo cmake --build . --target install
```

By specifying `-j n` in the build command, more than one core can be used which can speed up the compilation on multi-core machines.

Note that you should **not** have `libpoco-dev` being installed via `apt` on this machine.

#### Compile the source code
Clone the source code:
```shell
cd source/code/root/path
git clone -b master https://github.com/erl987/personalfme
cd personalfme
git checkout tags/personalfme-1.0.0-release
```

Configure your build. Note that you should set the `CMAKE_BUILD_TYPE` `Release` (or `Debug` in special cases):	
```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Now you can build *personalfme*:
```shell
make -jn
```

**Note:** You may use the option `-jn` to build on n cores in parallel. This may require a large main memory.

Generate the package by executing:
```shell
sudo make package
```

It is recommended to execute the command as root in order to prevent problems with the Debian maintainer script file permissions.

**Note:** This requires that a number of revision dependent files as well as the documentation PDF are existing within the source code. If this is not the case, see below for recreating the documentation.


### 3. Installation

The package can be installed using:
```shell
sudo dpkg -i personalfme_#.#.#-#_amd64.deb
```

Further details about the installation can be found in the PDF or online handbook.


## Expert options, not for general usage

### 4. Initializing Git submodules

An external Git submodule needs to be initialized before any regeneration of templated strings such as revision number and commit date within the source code is possible. This is required for regenerating the documentation for example:
```shell
cd path/to/personalfme
git submodule update --init --recursive
```


## 5. Regenerating the documentation

Normally the documentation PDF is already provided in the Git repository and does not need to be created separately. The documentation can be regenerated if you have a running DocBook installation and the external Git submodule has been initialized (see previous section).

Note: The documentation (and some other revision dependent files) can only be created if the source code tree used is under Git-version control.

* install the required programs:
```shell
sudo apt install xsltproc fop docbook-xsl default-jre
```

* generate the documentation target by configuring CMake accordingly:
```shell
cmake -DCMAKE_BUILD_TYPE=Release -DOption_USE_GIT=ON -DOption_CREATE_DOCUMENTATION=ON ..
```

* run the documentation targets:
```shell
make PdfDocumentation
make ManpageDocumentation
```
	

### 6. Running the unit tests

The unit tests are only built if the CMake option `-DOption_BUILD_UNITTESTS=ON` is set. Some unit tests require the numerical calculation software Octave for comparison purposes. 
You may deactivate these tests if not required  by commenting them out in the file `UnitTests/runner.cpp`.

On Linux you can install Octave from the package sources:
```shell
sudo apt install octave liboctave-dev
```

Then you can install the required signal-package and its dependencies within Octave. This may take a while.
```shell
octave --no-gui
pkg install -forge control
pkg install -forge signal
```
	
	
## Windows

### 1. General

For Windows it is recommended to download and install the pre-compiled binary. **Due to license issues, the required runtime libraries for Microsoft C++ 2019 are not included.** If you have not already installed them, you have to download and install the Visual C++ Redistributable for Visual Studio 2019 for x86: https://support.microsoft.com/de-de/help/2977003/the-latest-supported-visual-c-downloads


### 2. Compiling the program including the installer

**Usually it is not recommended to compile the program manually if using Windows, use the pre-compiled binary instead**.

The program uses a CMake-based build system (requires CMake version 3.5.0 or higher). It is using C++14 and is tested to compile with Visual Studio 2019 Community under Windows. It will not compile with earlier versions of Visual Studio. The code is written in platform-independent C++ using platform-independent libraries.

PersonalFME is usually compiled for *x86* architecture (although *x64* should work as well, but then all libraries also need to be provided for *x64*). Note that this is no longer the default in CMake and needs to be changed when configuring the build.

An installer package will be created during the compilation by building the project `PACKAGE` in Visual Studio. Note: This requires that a number of revision dependent files as well as the documentation PDF are existing within the source code. If this is not the case, see below for recreating the documentation.


#### Install most dependencies:

The dependencies of *PersonalFME* are on Windows primarily provided using the package manager `vcpkg`. **This is not possible for the Poco library because of a regression bug regarding e-mail sending being present in all later versions of the library.**

`vcpkg` can be installed anywhere on the machine, for instructions see: https://github.com/microsoft/vcpkg

**Note:** The usage of `vcpkg` can be problematic, the Boost library is for example not fully compiling on Windows 7 anymore when using this package manager. Usually such issues can be solved by reverting to an earlier version of `vcpkg` or by manually adapting the port-files of the library in question. Please refer to the resources provided by `vcpkg` (for example on Github) in order to resolve any problems related to the packager manager.

Install the following dependencies using `vcpkg` (note that they will be compiled by this command which will take quite some time), the required triplet is `x86-windows` which should be the default:

```shell
vcpkg install portaudio boost-asio boost-assign boost-date-time boost-serialization boost-signal2 xerces-c libsndfile
```

For the unit tests, these additional dependencies are required:

```shell
vcpkg install boost-accumulators boost-test
```


#### Install the Poco Complete version 1.7.7 manually:

* download the ZIP-file at: https://github.com/pocoproject/poco/archive/refs/tags/poco-1.7.7-release.zip
* fix one file in the source code according to this pull request: https://github.com/pocoproject/poco/pull/3191/files
* enable the option `NetSSL Windows` in CMake
* build the project for *Win32* using CMake in a subfolder `./build` (both `Debug` and `Release` build)
* move everything in the root directory of Poco into a new directory `MSVC-14.0` on the root level (i.e. the directory 
  structure should then be such as `Poco_DIR/MSVC-14.0/ApacheConnector` and so on)
* inform CMake about this root directory of Poco in one of the following ways:
	- define the Poco-library root path in the evironment variable `POCO_ROOT`
	- specify the path of `Poco_DIR` directly in CMake (the others paths are chosen automatically)

	
#### Configure CMake and build the project:

* download the PersonalFME code base, you could either check the Github project out using Git or download the `zip`-file from Github
* start the CMake GUI
* provide the source code path, i.e. the root path of the PersonalFME code base on your machine
* enter the binary path, it should be a folder `./build` being a subfolder in the code base root directory
* click on the `Configure` button and choose *Win32* as platform and *Specify toolchain file for cross-compiling*
* provide the `vcpkg` toochain file: `[vcpkg root]/scripts/buildsystems/vcpkg.cmake`
* the configuration will fail now, provide the root path of the Poco-library in the variable `Poco_DIR`
* click on `Configure` again, now the configuration should succeed
* click on `Generate`
* click on `Open Project` to open the Visual Studio project
* build the `Release` and `Debug` version


### 3. Installation

The program is now simply installed by running the installer `PersonalFME-#.#.#-win32.exe` that has been created by Visual Studio
building the project `PACKAGE`. It is located in the directory "./build/Release".

The required CMake option `Option_CREATE_INSTALLER` should be enabled by default.



## Expert options, not for general usage

### 4. Regenerating the documentation

Normally the documentation PDF-file is already provided in the source code ZIP-file and does not need to be created separately. 
The documentation can be regenerated if you have a running DocBook installation.

Note: The documentation (and some other revision dependent files) can only be created if the source code tree used is under Git-version control.

* install DocBook, then you have to set the environment variable DOCBOOK_ROOT, for example to: `C:/DocBook/stylesheets/docbook-xsl-ns-1.79.1`
* Java
* the FOP-processor (version 2.0) 
* the Saxon XSLT processor (version HE9-7-0)
* the CLASSPATH environment variable for Java has to be set similar to: `C:\Program Files\Saxonica\SaxonHE9-7-0_1J\saxon9he.jar;C:\DocBook\xslthl-2.1.3\xslthl-2.1.3.jar`

Check the options `Option_CREATE_DOCUMENTATION` and `Option_USE_GIT` in the CMake-GUI to generate the documentation target. This will create a project `PdfDocumentation` in Visual Studio that can be run to create the documentation.


### 5. Running the unit tests

The unit tests are only built if the CMake option `Option_BUILD_UNITTESTS` is set to ON. Some unit tests require the numerical calculation software Octave for comparison purposes. 
You may deactivate these tests if not required by commenting them out in the file `UnitTests/runner.cpp`.

You have to install the required signal-package and its dependencies within Octave. This may take a while.
```shell
octave
pkg install -forge control
pkg install -forge signal
```


# License

Dieses Programm steht unter der GNU General Public License Version 3 oder einer neueren Version. Das Programm darf daher frei weitergegeben und verändert werden, solange der Quellcode frei zugänglich bleibt. Der Autor übernimmt keinerlei Verantwortung für das korrekte Funktionieren der Software und eventuelle Folgen aus einem Programmfehler.

**Warnhinweis**:
Es wird darauf hingewiesen, dass das willentliche unbefugte Erlangen von Informationen aus nicht-öffentlichen Funkdiensten, z.B. der Behörden und Organisationen mit Sicherheitsaufgaben (BOS) strafbar ist. Dies gilt auch für die Auswertung von Fünftonfolgen. Die vorschriftsgemäße Nutzung des Programms liegt allein in der Verantwortung des Nutzers.

**Copyright (c) 2010-2021 Ralf Rettig**

**This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.**

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.**

**You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.**
