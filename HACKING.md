# GNU Gama

## The primary build system

The primary build system is and will remain autotools, therefore any
changes to the project must be consistent with that.

To build Gama run the following steps

1. ./autogen.sh
2. ./configure
3. make
4. make check (optional set of unit tests)

Run ./configure help to get the list of all options and some influential environment variables. Configure script checks for available libraries and if they are available, adds optional fetures to the build:

* support for sqlite3 databases
* xmllint unit tests
* octave adjustment output

Since Gama version 2.08 optional build with legacy expat parser version 1.1 from the source codes is implicitly disabled (the expat sources are not part of Gama project, but they are distributed with Gama).

## Alternative build systems

Alternative builds are Qt project format build of GUI for gama-qt (gama-local adjustment with sqlite3 support) and a secondary CMake build of gama-local and gama-g3 with selected subset of unit tests. Both of alternative builds rely on expat parser compiled from sources. Namely CMake build is aimed to Windows platform where we want to avoid dependencies on external libraries so that it would be possible to distribute executables without an installer. Similarly you can compile Gama alternative build using Visual Studio without explicitely installing cmake tool.

Both alternative builds are simple and (currently) do not exploit conditional ifdefs. We are willing to accept pathes and suggestions to improve CMake build  on the premise that they are consistent with the primary build system.