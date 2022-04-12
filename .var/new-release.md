# New release

* Update files ```NEWS``` and ```ChangeLog```

* Update release version in the following three files

      configure.ac
      lib/gnu_gama/version.cpp
      CMakeLists.txt

  Unit test ```tests/gama-local/gama-local-version``` checks that the
  release version is identical in all these files (run ```make check```).

* Run full set of unit tests with *extra tests* enabled

      ./autogen.sh
      ./configure  --enable-extra-tests
      make check

   Script ```autogen.sh``` usually only needs to be run for the first time after
   substantial changes in the project configuration.

* GNU Gama comes with an alternative ```cmake``` build system
  defined in CMakeLists.txt with a limited set of unit tests.
  This *unofficial*   build is available for users who need to build Gama on Windows without the MinGW development enviroment, neither directly with Visual Studio or within Windows Subsystem for
  Linux (WSL).

  ```gama-local-version``` unit test cannot check if all file names are
  properly defined in ```CMakeLists.txt``` and you need to check the
  *cmake build*, for example like this

      mkdir build      # temporary directory
      cd build
      cmake ..
      make
      ctest

  or if you want to use the Ninja build system for a faster run

      cmake .. -G Ninja
      ninja
      ctest

  The following steps work on Windows as well as Linux:

      mkdir build
      cd build
      cmake ..
      cmake --build . --config Release
      ctest -C Release

* Now we need to check if all the files have been committed to the Gama
  git server. Create a *working/temporary* directory, clone the project and repeat all the steps above

      git clone https://git.savannah.gnu.org/git/gama.git temp-gama
      cd temp-gama

* If everything passed, you can generate the new distribution tarball

      ./autogen.sh
      ./configure
      make distcheck

* Now when you have the release tarball ready (for example
  ```gama-2.18.tar.gz```), you tag the git repository, for example

      git tag gama-2.18

  Push the tag to the repository (set the origin repo to ssh first):

      git remote set-url origin ssh://user@git.sv.gnu.org/srv/git/gama.git
      git push origin --tags

   where *user* is your login name.

* You also have to send an email about the new release to
  ```info-gama@@gnu.org``` and ```info-gnu@@gnu.org```, describing what
  is new in the release and what bugs were fixed (if any). Here is an
  example text sent for version 2.18

      Subject: GNU gama 2.18 released

      We are pleased to announce the release of GNU Gama 2.18.

      There is one major fix visible to end users. Implicit value of XML
      parameter 'update_constrained_coordinates' was changed to "yes". The
      behavior of adjustment calculation was well and clearly described in
      the documentation, but in some cases the old implicit value ("no")
      might had lead to poor numerical results. The input data XML parameter
      'update_constrained_coordinates' is preserved only for backward
      compatibility and is likely to be removed in some future release.

      Several other minor internal issues were fixed, none of them visible to
      end users.

      About

      GNU Gama package is dedicated to adjustment of geodetic networks. It
      is intended for use with traditional geodetic surveyings which are
      still used and needed in special measurements (e.g., underground or
      high precision engineering measurements) where the Global Positioning
      System (GPS) cannot be used.

      Adjustment in local coordinate systems is fully supported by a
      command-line program gama-local that adjusts geodetic (free) networks
      of observed distances, directions, angles, height differences, 3D
      vectors and observed coordinates (coordinates with given
      variance-covariance matrix). Adjustment in global coordinate systems
      is supported only partly as a gama-g3 program.

      https://www.gnu.org/software/gama/

## Generate an announcement message

The announce message can and should be generated with
```announce-gen``` script from directory ```build-aux``` of GNU
Gnulib https://www.gnu.org/software/gnulib/manual/gnulib.html

      ./announce-gen --help
      Usage: announce-gen [OPTIONS]
      Generate an announcement message.  Run this from builddir.

      OPTIONS:

      These options must be specified:

      --release-type=TYPE          TYPE must be one of alpha beta stable
      --package-name=PACKAGE_NAME
      --previous-version=VER
      --current-version=VER
      --gpg-key-id=ID         The GnuPG ID of the key used to sign the tarballs
      --url-directory=URL_DIR

      The following are optional:

      --news=NEWS_FILE             include the NEWS section about this release
                                   from this NEWS_FILE; accumulates.
      ......

   For example:

      ./announce-gen --package-name gama \
         --release-type stable \
         --current-version 2.17 \
         --previous-version 2.18 \
         --gpg-key-id 1b77fc09 \
         --url https://ftp.gnu.org/gnu/gama \
         --news NEWS

   Including the ```NEWS``` file (the NEWS section about this release)
is highly recommended, ```NEWS``` are generally more interesting to
users rather than ChangeLog, which is of interest mainly to developpers.

## Builds on WSL

This section is given with no guarantee, GNU Gama is not tested on
Windows Subsystem Linux. It seems that on WSL you need to install

      sudo apt install automake
      sudo apt install build-essential

and it seems that you also need to explicitly enable build with local copy
of expat parser

      ./autogen
      ./configure --enable-expat_1_1  --enable-extra-tests

    make check    # build all and run all tests including the extra tests

## Distributing the new release

For uploading new release tarball to the GNU FTP server, use script
```gnupload``` from directory ```build-aux``` from *GNU
portability library* (gnulib, see
https://savannah.gnu.org/git/?group=gnulib)

      git clone git://git.sv.gnu.org/gnulib.git
      ln -s gnulib/build-aux/gnupload     # create a symlink or copy ...

To upload your new release tarball, simply run

      ./gnupload --to ftp.gnu.org:gama gama-2.18.tar.gz

You will be asked for your gpg passphrase (twice) and you will get
an email when your upload is finished.

Similarly you can replace the tarball, in case it is needed

      ./gnupload --replace --to ftp.gnu.org:gama gama-2.18.tar.gz

Use ```gnupload --help``` for the full list of options with examples, you
can always try running ```gnupload``` with the option ```--dry-run```

* Note: If you encounter this error when running gnulib

      gpg: signing failed: Inappropriate ioctl for device

Simply run in the command line:

      export GPG_TTY=$(tty)

## Online documentation

To update Gama webpages and online documentation, you need to checkout
its repository from CVS (Concurrent Version System)

      cvs -z3 -d:ext:user@cvs.savannah.gnu.org:/web/gama co -P gama

where *user* is your login name.

* For generating documentation in various formats from texinfo sources,
  download and run ```gendocs` (see ```doc/Makefile.am``` for details)

      cd gama/doc
      make download-gendocs.sh
      make run-gendocs.sh

* Copy the manual generated by the gendocs script from gama 
  repository to CVS web

* Run script
      ./update-manual.sh

      #!/bin/sh

      find . -type f -name "*.html" -print0 | xargs -0 cvs add
      find . -type f -name "*.gz"   -print0 | xargs -0 cvs add
      find . -type f -name "*.pdf"  -print0 | xargs -0 cvs add
      find . -type f -name "*.txt"  -print0 | xargs -0 cvs add

      for m in html_chapter html_node html_section
      do
          echo
          echo manual/$m/.symlinks :
          cat manual/$m/.symlinks
      done

* add ```gama.html index.html``` to ```manual/html_*/.symlinks```

* remove all files index.html from manual ```find manual -name index.html```

  Run

      cvs commit .   # only cvs is available for GNU web pages, no git