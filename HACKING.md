# GNU Gama

This file describes how gama is developed.  People that are using
gama, including compiling from source, should not need to read it.


## clean changesets

Proposed changes should be structured as branches which make a
coherent set of logically related changes, with commits that each make
one logical change.  This is simply the git "clean changeset" notion
and not intended to be special.

In particular, commits that clean up whitespace, or that make any
other kind of cleanup or structural changes, should be entirely
separate from functional changes.

Commits that make functional changes should not make any whitespace
changes on lines not already intentionally modified.


## Release process

The logical flow of a release is a tag in git, and then "make dist" of
a checkout of that tag, with the resulting tarball made available.

Before tagging,
  - choose a version number
  - update NEWS
  - update the version in configure.ac and lib/gnu_gama/version.cpp
  - run "make distcheck" on a system with sqlite3 available, to ensure
    that the distfile includes all necessary files for building, and
    that the tests pass

Apply the tag, and ask other active contributors to test.  They can
each run "make distcheck" on their platforms, and test the resulting
distfile as the source for packaging systems (but should NOT publish
the packaging changes).

If there are problems, fix with a commit and move the tag.  If there
are no problems, the distfile from make distcheck or make dist can be
published.

Once a gama-x.y.tar.gz is made available in the download area, it may
not be changed, because some packaging systems store hashes and a
maintainer-replaced tarball looks the same as an attacker changing the
distfile.  We have an infinite number of integers, so a new release
can always be made if there is a critical flaw.  It is OK to remove a
bad tarball after a replacement one has been published, but this is a
practice probably reserved for truly problematic situations.

## Build System issues

### The primary build system

The primary build system is and will remain autotools; therefore any
changes to the project must be consistent with that.

To build Gama run the following steps

1. ./autogen.sh
2. ./configure
3. make
4. make check         # optional set of unit tests
5. make dist          # create distribution tarball
6. make distcheck     # build from tarball and run tests

Run ./configure --help to get the list of all options and some
influential environment variables. Configure script checks for
available libraries and if they are available, adds optional features
to the build:

* support for sqlite3 databases
* xmllint unit tests
* octave adjustment output

The Gama repository and distfile contains a copy of expat 1.1, as an
aid to those who do not have expat installed on their systems.  The
standard approach is to use expat from the system, and this copy is
provided only to enable those who cannot deal with that to build gama
anyway.


### Alternative build systems

Alternative builds are Qt project format build of GUI for gama-qt
(gama-local adjustment with sqlite3 support) and a secondary CMake
build of gama-local and gama-g3 with selected subset of unit
tests. Both of alternative builds rely on expat parser compiled from
sources. Namely CMake build is aimed to Windows platform where we want
to avoid dependencies on external libraries so that it would be
possible to distribute executables without an installer. Similarly you
can compile Gama alternative build using Visual Studio without
explicitly installing cmake tool.

Both alternative builds are simple and (currently) do not exploit
conditional ifdefs. We are willing to accept patches and suggestions to
improve CMake build on the premise that they are consistent with the
primary build system.


### Contributions to gama and build systems

All contributions to gama must work with autotools.  Specifically,
after a patch, the process of autogen, configure, and make distcheck
must work at least as well as before, and support any features in the
new code that should be supported by a build system.   It should be
possible to cross-compile gama, and contributions should not impair
that property.

It is nice if contributions that need build adjustments also modify
the secondary build systems, but that is not necessary.  Patches to
the secondary build systems are welcome as separate contributions.

In all build systems, it is not appropriate to have ifdefs based on
operating system (within the POSIX family) unless absolutely
necessary.  Instead, the system should use feature tests, following
autoconf doctrine, so that the build system is likely to work on new
systems.
