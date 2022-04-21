dnl GNU General Public license v3 or later
dnl Copyright 2019, 2022 Ales Cepek <cepek@gnu.org>

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

dnl GNU Gama version is also defined in CMakeLists.txt and
dnl lib/gnu_gama.version.cpp

AC_INIT([gama],[2.18.4],[bug-gama@gnu.org])

AC_CONFIG_SRCDIR([lib/gnu_gama/version.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts/config.aux])

AM_INIT_AUTOMAKE(subdir-objects)
AM_SILENT_RULES([yes])

dnl You can comment out the following macro if you have the package
dnl autoconf-archive installed

AC_CONFIG_MACRO_DIR([m4])

dnl Install package autoconf-archive or download from
dnl git://git.sv.gnu.org/autoconf-archive.git
dnl
dnl https://www.gnu.org/software/autoconf-archive/ax_valgrind_check.html

AC_LANG(C++)
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK


dnl Checks for programs.

AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB


dnl Optional extra tests

AC_ARG_ENABLE([extra-tests],
   [AS_HELP_STRING([--enable-extra-tests[[yes|=no]]],
                   [Turn on/off extra tests] (default is no))],
   [AM_CONDITIONAL([GNU_GAMA_LOCAL_EXTRA_TESTS],
                   [AS_IF([test "x$enable_extra_tests" != "xyes"],
                          enable_extra_tests=no)]
                   [test "x$enable_extra_tests" = "xyes"])],
   [AM_CONDITIONAL([GNU_GAMA_LOCAL_EXTRA_TESTS], [false])
    enable_extra_tests=no]
)
AC_MSG_NOTICE([checking whether extra tests are enabled... $enable_extra_tests])


dnl Optional build of gama-g3

AC_ARG_ENABLE([gama-g3],
   [AS_HELP_STRING([--enable-gama-g3[[=yes|no]]],
                   [Turn on/off build with gama-g3] (default is yes))],
   [AM_CONDITIONAL([GNU_GAMA_G3_ENABLED],
                   [AS_IF([test "x$enable_gama_g3" != "xyes"],
                          enable_gama_g3=no)]
                   [test "x$enable_gama_g3" = "xyes"])],
   [AM_CONDITIONAL([GNU_GAMA_G3_ENABLED], [true])
    enable_gama_g3=yes]
)
AC_MSG_NOTICE([checking whether gama-g3 build is enabled... $enable_gama_g3])


dnl Check for makeinfo

AC_CHECK_PROG([gnu_gama_makeinfo],[makeinfo], [yes], [no])
AM_CONDITIONAL([GNU_GAMA_MAKEINFO], [test "x$gnu_gama_makeinfo" = xyes])
AM_COND_IF([GNU_GAMA_MAKEINFO],,
   AC_MSG_WARN([makeinfo is missing on your system])
   MAKEINFO=true
)

dnl Optional build with legacy expat 1.1 (local copy)

AC_ARG_ENABLE(
   [expat_1_1],
   [AS_HELP_STRING([--enable-expat_1_1[[=yes|no]]],
   [Enable build with legacy expat parser version 1.1 (default is yes)])]
)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_EXPAT_1_1], [false])
AS_IF([test x$enable_expat_1_1 != xyes], enable_expat_1_1=no)
if test x$enable_expat_1_1 = xyes
then
   AC_MSG_WARN([build with local copy of expat 1.1 XML parser])
   AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_EXPAT_1_1], [true])
   AC_SUBST([CPPFLAGS], "${CPPFLAGS} -DGNU_gama_expat_1_1")
else
   m4_define([install_expat], [please install libexpat1-dev])
   AC_CHECK_LIB([expat], [XML_ParserCreate],,
      [AC_MSG_ERROR([XML expat parser is missing,] install_expat)])
   AC_CHECK_HEADERS([expat.h],,
      [AC_MSG_ERROR([Cannot find expat.h,] install_expat)])
fi


dnl Check for yaml-cpp library

AC_MSG_CHECKING([for yaml-cpp])
AC_LINK_IFELSE(
 [AC_LANG_PROGRAM([#include <yaml-cpp/yaml.h>],[YAML::Node node;])],

 [AC_MSG_RESULT([yes])]
 [AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_YAML_CPP],[true])],

 [AC_MSG_RESULT([no])]
 [AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_YAML_CPP],[false])]
 [AC_MSG_WARN([libyaml-cpp-dev is not installed.])]
 )


dnl Check for sqlite3 support

AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [false])
AC_CHECK_LIB([sqlite3], [sqlite3_open])
if test "x$ac_cv_lib_sqlite3_sqlite3_open" = "xyes";
then
       AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [true])
       AC_DEFINE([GNU_GAMA_LOCAL_SQLITE_READER],1,
                 [Conditional support for sqlite3 databases])
       AC_SUBST([CPPFLAGS], "${CPPFLAGS} -DGNU_GAMA_LOCAL_SQLITE_READER")
fi


dnl Check for xmllint

AC_CHECK_PROG(GNU_GAMA_LOCAL_TEST_XMLLINT, xmllint, yes, no)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_XMLLINT],
               [test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" = "xyes"])

if test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" != "xyes";
then
   AC_MSG_WARN([xmllint not found, XML files will not be validated])
fi


dnl Check for octave

AC_CHECK_PROG(GNU_GAMA_LOCAL_TEST_OCTAVE, octave, yes, no)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_OCTAVE],
               [test "x$GNU_GAMA_LOCAL_TEST_OCTAVE" = "xyes"])

if test "x$GNU_GAMA_LOCAL_TEST_OCTAVE" != "xyes";
then
   AC_MSG_WARN([octave not found, .m files will not be validated])
fi


dnl AC_SUBST([CPPFLAGS], "${CPPFLAGS} -DDEBUG_ACORD2")
dnl AC_SUBST([CPPFLAGS], "${CPPFLAGS} -DA2G_DEBUG")
dnl AC_SUBST([CPPFLAGS], "${CPPFLAGS} -DDEBUG_REDUCED_OBS")


dnl Checks for header files.

AC_HEADER_STDC

AC_CONFIG_FILES([
   Makefile
   lib/Makefile
   src/Makefile
   scripts/Makefile
   doc/Makefile
   xml/Makefile
   tests/Makefile
   tests/matvec/Makefile
   tests/statan/Makefile
   tests/acord2/Makefile
   tests/acord2/input/a2g/Makefile
   tests/acord2/input/a2diff/Makefile
   tests/gama-local/Makefile
   tests/gama-local/input/Makefile
   tests/gama-local/src/Makefile
   tests/gama-g3/Makefile
   tests/gama-g3/input/Makefile
   tests/gama-g3/src/Makefile
   tests/extra-tests/Makefile
   tests/extra-tests/input/Makefile
   tests/krumm/Makefile
])
AC_OUTPUT
