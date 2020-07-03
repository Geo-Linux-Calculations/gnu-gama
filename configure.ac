dnl GNU General Public license v3 or later
dnl Copyright 2019 Ales Cepek <cepek@gnu.org>

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

AC_INIT([gama], [2.09.3], [bug-gama@gnu.org])
AC_CONFIG_SRCDIR([lib/gnu_gama/version.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([scripts/config.aux])

AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])


dnl You can comment out the following macro if you have the package
dnl autoconf-archive installed

AC_CONFIG_MACRO_DIR([m4])

dnl Install package autoconf-archive or download from
dnl git://git.sv.gnu.org/autoconf-archive.git
dnl
dnl https://www.gnu.org/software/autoconf-archive/ax_valgrind_check.html

AX_CXX_COMPILE_STDCXX([14], [noext], [mandatory])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK


dnl Checks for programs.

AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_RANLIB


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
   AC_CHECK_LIB([expat], [XML_ParserCreate],,
      [AC_MSG_ERROR([Expat XML parser library is not installed])])
   AC_CHECK_HEADERS([expat.h],,
      [AC_MSG_ERROR([Cannot find expat.h, please install libexpat1-dev])])
fi


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


dnl Checks for header files.

AC_HEADER_STDC

AC_OUTPUT([
   Makefile
   lib/Makefile
   bin/Makefile
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
   tests/gama-local/scripts/Makefile
   tests/gama-g3/Makefile
   tests/gama-g3/input/Makefile
   tests/gama-g3/scripts/Makefile
])
