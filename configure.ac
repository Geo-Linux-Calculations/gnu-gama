#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
AC_PREREQ(2.65)
AC_INIT([gama], [1.15], [bug-gama@gnu.org])
AC_CONFIG_SRCDIR([lib/gnu_gama/version.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([scripts/config.aux])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [false])

# Checks for libraries.
AC_CHECK_LIB([expat], [XML_ParserCreate])
AC_CHECK_LIB([sqlite3], [sqlite3_open])
AC_CHECK_LIB([sqlite3], [sqlite3_open],
                AC_DEFINE([GNU_GAMA_LOCAL_SQLITE_READER],,
                          [Conditional support for sqlite3 databases])
                AC_SUBST([CPPFLAGS],[-DGNU_GAMA_LOCAL_SQLITE_READER $CPPFLAGS])
                AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_SQLITE_READER], [true])
            )

# Check for xmllint
AC_CHECK_PROG(GNU_GAMA_LOCAL_TEST_XMLLINT, xmllint, yes, no)
AM_CONDITIONAL([GNU_GAMA_LOCAL_TEST_XMLLINT],
               [test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" = "xyes"])

if test "x$GNU_GAMA_LOCAL_TEST_XMLLINT" != "xyes"; then
   AC_MSG_WARN([*** xmllint not found, XML files will not be validated])
fi


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER(expat.h, , [AC_MSG_ERROR(
                [Couldn't find expat.h...try installing libexpat-dev] )])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([memmove memset munmap pow sqrt])

AC_OUTPUT([Makefile
           lib/Makefile
           bin/Makefile
           scripts/Makefile
           doc/Makefile
           xml/Makefile
           tests/Makefile
           tests/gama-local/Makefile
           tests/gama-local/input/Makefile
           tests/gama-local/scripts/Makefile
          ])
