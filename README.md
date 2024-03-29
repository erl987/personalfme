# PersonalFME - Gateway linking analog radio selcalls to internet communication services

*PersonalFME* is detecting selcalls ("Fünftonfolgen") within the analog radio network of the German emergency 
services ("BOS") and is forwarding them in various ways such as alarm message apps, e-mail and via external program
calls. It requires an external radio receiver such as a radio device or a radio scanner that needs to be connected to
the applied computer via a sound card.

*PersonalFME* can even run on mini-computers such as RaspberryPi in continuous operation.

*PersonalFME* is developed for the selcalls used by the German BOS, but may be adaptable to selcalls defined by the 
standards in other countries.

**Dies ist der Quellcode von *PersonalFME* und richtet sich an technisch ausgebildete Personen, daher ist der Inhalt 
vollständig in englischer Sprache. Eine ausführliche Anwenderdokumentation für die Allgemeinheit in deutscher Sprache 
findet sich im [Handbuch](documentation/benutzerhandbuch.pdf).**

Detailed documentation about the configuration and operation of the program is available in 
the [handbook](documentation/benutzerhandbuch.pdf) (only in German).


## Supported platforms
* Raspberry Pi OS 11 (Raspberry Pi 2 Model B and higher, only Raspberry Pi 3 Model B is actually tested)
* Debian 11 Bullseye
* Ubuntu 22.04 LTS
* Windows 11, 10, 8.1

Previous versions of Linux are no longer supported, they can only be used with a previous version of *PersonalFME*.
Windows 7 is no longer supported.

## Webpage
https://www.personalfme.de


## Linux

### 1. General

For Linux only the source code is currently available, see below for the compilation instructions.


### 2. Compiling the program including the installer

The program uses a CMake-based build system (requires CMake version 3.5.0 or higher). It is using C++14 and is tested 
to compile with GCC 10.2 and newer under Debian / Ubuntu Linux. The code is written in platform-independent C++ using 
platform-independent libraries. Compilation with other Linux-distributions should be straightforward.

An installer package will be created during the compilation.

Enable the Universe repository (only for Ubuntu):
```shell
sudo add-apt-repository universe
sudo apt update
``` 

#### Install all dependencies:
All dependencies can be conveniently installed via the package manager `apt`:
```shell
sudo apt install build-essential cmake openssl libssl-dev portaudio19-dev libxerces-c-dev libsndfile-dev libalglib-dev ncurses-dev libboost-all-dev libpoco-dev
```

#### Compile the source code
Clone the source code:
```shell
cd source/code/root/path
git clone https://github.com/erl987/personalfme
cd personalfme
git submodule update --init --recursive
git checkout tags/v1.1.0
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

> **Note**
> 
> You may use the option `-jn` to build on n cores in parallel (replace `n` by a number such as `-j2`). This 
> may require a large main memory.

Generate the package by executing:
```shell
sudo make package
```

It is recommended to execute the command as root in order to prevent problems with the Debian maintainer script file 
permissions.

> **Note**
> 
> This requires that a number of revision dependent files as well as the documentation PDF are existing within 
> the source code. If this is not the case, see below for recreating the documentation.


### 3. Installation

The package can be installed using:
```shell
sudo dpkg -i personalfme_#.#.#-#_amd64.deb
```

Further details about the installation can be found in 
the [pdf](documentation/benutzerhandbuch.pdf) or [online handbook](https://personalfme.de/handbuch.html).


## Expert options, not for general usage

## 4. Regenerating the documentation

Normally the documentation PDF is already provided in the Git repository and does not need to be created separately. 
The documentation can be regenerated if you have a running DocBook installation and the external Git submodule has 
been initialized (see previous section).

> **Note**
> 
> The documentation (and some other revision dependent files) can only be created if the source code tree used is 
> under Git-version control.

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


### 5. Building the unit tests

Building the unit tests requires the CMake option `-DOption_BUILD_UNITTESTS=ON`:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DOption_BUILD_UNITTESTS=ON ..
```


## Windows

### 1. General

For Windows, it is recommended to download and install the [pre-compiled binary](https://personalfme.de/download.html). 
**Due to license issues, the required runtime libraries for Microsoft C++ 2022 are not included.** If you have not 
already installed them, you have to download and install the Visual C++ Redistributable for Visual Studio 2022 for 
x86: https://aka.ms/vs/17/release/vc_redist.x86.exe


### 2. Compiling the program including the installer

**Usually it is not recommended to compile the program manually if using Windows, use the pre-compiled binary instead**.

The program uses a CMake-based build system (requires CMake version 3.5.0 or higher). It is using C++14 and is tested 
to compile with Visual Studio 2022 Community under Windows. It may not compile with earlier versions of Visual 
Studio. The code is written in platform-independent C++ using platform-independent libraries.

PersonalFME is usually compiled for *x86* architecture (although *x64* should work as well, but then all libraries 
also need to be provided for *x64*). Note that this is no longer the default in CMake and needs to be changed when 
configuring the build.

An installer package will be created during the compilation by building the project `PACKAGE` in Visual Studio. 

> **Note**
> 
> This requires that a number of revision dependent files as well as the documentation PDF are existing within 
> the source code. If this is not the case, see below for recreating the documentation.


#### Install all dependencies:

The dependencies of *PersonalFME* are on Windows provided using the package manager `vcpkg`. 

`vcpkg` can be installed anywhere on the machine, for instructions see: https://vcpkg.io

Install the following dependencies using `vcpkg` (note that they will be compiled by this command which will take 
quite some time), the required triplet is `x86-windows` which should be the default:

```shell
vcpkg install portaudio boost-asio boost-assign boost-date-time boost-locale boost-math boost-serialization boost-signals2 xerces-c libsndfile poco poco[netssl]
```

> **Note**
>
> At the time of writing this documentation, the installation of `poco[netssl]` required the `vcpkg` package `openssl`
> to be already installed. This package is however only required during the installation and can then be uninstalled.

For the unit tests, these additional dependencies are required:

```shell
vcpkg install boost-accumulators boost-test
```


#### Configure CMake and build the project:

* download the PersonalFME code base, you could either check the GitHub 
* project (https://github.com/erl987/personalfme.git) out using Git or download the `zip`-file of the latest release 
  from Github (https://github.com/erl987/personalfme/)
* start the CMake GUI
* provide the source code path, i.e. the root path of the PersonalFME code base on your machine
* enter the binary path, it should be a folder `./build` being a sub-folder in the code base root directory
* click on the `Configure` button and choose *Win32* as platform and *Specify toolchain file for cross-compiling*
* provide the `vcpkg` toolchain file: `[vcpkg root]/scripts/buildsystems/vcpkg.cmake`
* click on `Generate`
* click on `Open Project` to open the Visual Studio project
* build the `Release` and `Debug` version


### 3. Installation

The program is now simply installed by running the installer `PersonalFME-#.#.#-win32.exe` that has been created by 
Visual Studio building the project `PACKAGE`. It is located in the directory `./build/Release`.

The required CMake option `Option_CREATE_INSTALLER` should be enabled by default.


## Expert options, not for general usage

### 4. Regenerating the documentation

Normally the documentation PDF-file is already provided in the source code ZIP-file and does not need to be created 
separately.
The documentation can be regenerated if you have a running DocBook installation.

> **Note**
> 
> The documentation (and some other revision dependent files) can only be created if the source code tree used is 
> under Git-version control.

* install DocBook, then you have to set the environment variable DOCBOOK_ROOT, 
  for example to: `C:/DocBook/stylesheets/docbook-xsl-ns-1.79.1`
* Java
* the FOP-processor (version 2.0)
* the Saxon XSLT processor (version HE9-7-0)
* the CLASSPATH environment variable for Java has to be set similar 
  to: `C:\Program Files\Saxonica\SaxonHE9-7-0_1J\saxon9he.jar;C:\DocBook\xslthl-2.1.3\xslthl-2.1.3.jar`

It is necessary to update the Git-repository submodule `CMakeModules/externals/CMake-gitinfo` before building the 
project.

Check the options `Option_CREATE_DOCUMENTATION` and `Option_USE_GIT` in the CMake-GUI to generate the documentation 
target. This will create a project `PdfDocumentation` in Visual Studio that can be run to create the documentation.


### 5. Building the unit tests

Building the unit tests requires the CMake option `-DOption_BUILD_UNITTESTS=ON`:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DOption_BUILD_UNITTESTS=ON ..
```


## Unit tests

The tests are grouped by different labels. All tests with a single label can be run like this:

```shell
./Unittests --run_test=@default
```

The following labels are available:

| Label               | Contained Tests                                                         |
|---------------------|-------------------------------------------------------------------------|
| default             | All tests that should always work, even if no audio device is available |
| with_audio          | All tests requiring an active audio device on the test machine          |
| realtime_with_audio | Testing of realtime selcall detection[^1]                               |

[^1]: Requires running the executable `CoreTester` as a counterpart on the same computer.

The label `default` contains the tests that can be run even on a cloud machine without an audio device
and covers all relevant use cases. **Running the tests with this label is sufficient to ensure the
correct functionality of the software.**


# License

Dieses Programm steht unter der GNU General Public License Version 3 oder einer neueren Version. Das Programm darf 
daher frei weitergegeben und verändert werden, solange der Quellcode frei zugänglich bleibt. Der Autor übernimmt 
keinerlei Verantwortung für das korrekte Funktionieren der Software und eventuelle Folgen aus einem Programmfehler.

**Warnhinweis**:
Es wird darauf hingewiesen, dass das willentliche unbefugte Erlangen von Informationen aus nicht-öffentlichen 
Funkdiensten, z.B. der Behörden und Organisationen mit Sicherheitsaufgaben (BOS) strafbar ist. Dies gilt auch für die 
Auswertung von Fünftonfolgen. Die vorschriftsgemäße Nutzung des Programms liegt allein in der Verantwortung des Nutzers.

**Copyright (c) 2010-2023 Ralf Rettig**

**This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public 
License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later 
version.**

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.**

**You should have received a copy of the GNU General Public License along with this program. 
If not, see <https://www.gnu.org/licenses/>.**
