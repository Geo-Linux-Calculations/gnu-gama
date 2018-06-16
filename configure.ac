#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
AC_PREREQ(2.69)
AC_INIT([gama], [2.00], [bug-gama@gnu.org])
AC_CONFIG_SRCDIR([lib/gnu_gama/version.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([scripts/config.aux])

AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])


# You can comment out the following macro if you have the package
# autoconf-archive installed
#
AC_CONFIG_MACRO_DIR([m4])
#
# Install package autoconf-archive or download from
# git://git.sv.gnu.org/autoconf-archive.git
#
# https://www.gnu.org/software/autoconf-archive/ax_cxx_compile_stdcxx_11.html
# https://www.gnu.org/software/autoconf-archive/ax_valgrind_check.html
#
AX_CXX_COMPILE_STDCXX([11], [noext], [mandatory])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_CHECK


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [false])
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_EXPAT_1_1], [false])

AC_ARG_ENABLE([gama-g3],
   [AS_HELP_STRING([--enable-gama-g3],[Turn on build with gama-g3])],
   [AM_CONDITIONAL([GNU_GAMA_G3_ENABLED], [true])
    AC_MSG_NOTICE([*** gama-g3 build enabled])],
   [AM_CONDITIONAL([GNU_GAMA_G3_ENABLED], [false])
    AC_MSG_NOTICE([*** gama-g3 build disabled])]
)


# Checks for libraries libexpat1-dev, libsqlite3-dev

AC_CHECK_LIB([expat], [XML_ParserCreate])
if test "x$ac_cv_lib_expat_XML_ParserCreate" != "xyes"; then
       AC_MSG_WARN([Build with local copy of expat 1.1 XML parser])
       AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_EXPAT_1_1], [true])
       AC_DEFINE([GNU_gama_expat_1_1],1,
                 [Optional support for expat 1.1 from local sources])
       CPPFLAGS="${CPPFLAGS} -DGNU_gama_expat_1_1"
else
       AC_CHECK_HEADER(expat.h, , [AC_MSG_ERROR(
                [Cannot find expat.h ... please install libexpat1-dev] )])
fi

AC_CHECK_LIB([sqlite3], [sqlite3_open])
if test "x$ac_cv_lib_sqlite3_sqlite3_open" = "xyes"; then
       AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [true])
       AC_DEFINE([GNU_GAMA_LOCAL_SQLITE_READER],1,
                 [Conditional support for sqlite3 databases])
       CPPFLAGS="${CPPFLAGS} -DGNU_GAMA_LOCAL_SQLITE_READER"
fi


# Check for xmllint

AC_CHECK_PROG(GNU_GAMA_LOCAL_TEST_XMLLINT, xmllint, yes, no)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_XMLLINT],
               [test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" = "xyes"])

if test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" != "xyes"; then
   AC_MSG_WARN([*** xmllint not found, XML files will not be validated])
fi


# Check for octave

AC_CHECK_PROG(GNU_GAMA_LOCAL_TEST_OCTAVE, octave, yes, no)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_OCTAVE],
               [test "x$GNU_GAMA_LOCAL_TEST_OCTAVE" = "xyes"])

if test "x$GNU_GAMA_LOCAL_TEST_OCTAVE" != "xyes"; then
   AC_MSG_WARN([*** octave not found, .m files will not be validated])
fi


# Checks for header files.
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
   tests/gama-local/Makefile
   tests/gama-local/input/Makefile
   tests/gama-local/scripts/Makefile
   tests/gama-g3/Makefile
   tests/gama-g3/input/Makefile
   tests/gama-g3/scripts/Makefile
])
