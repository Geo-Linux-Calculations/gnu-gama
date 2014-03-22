/*
  GNU Gama --- Geodesy and Mapping C++ library
  Copyright (C) 1999, 2003, 2005, 2011, 2014  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* The VERSION macro is normally defined in <config.h> created by autoconf.

   The #ifndef preprocessor command is used here to enable Qt and MSVC
   builds to define their own VERSION number without explicit use of
   <config.h>
 */
#ifndef VERSION

// macro GNU_GAMA_LOCAL_SQLITE_READER is defined both as a C++ -D
// parameter and in <config.h>. This is the only file where this
// should trigger warning message
#ifdef GNU_GAMA_LOCAL_SQLITE_READER
#undef GNU_GAMA_LOCAL_SQLITE_READER
#endif

#include <config.h>
#endif

#include <gnu_gama/version.h>
#include <iostream>

#define str(s)  # s
#define xstr(s) str(s)

namespace GNU_gama {

  // VERSION is defined in config.h
  const char* GNU_gama_version  = VERSION;

  const char* GNU_gama_compiler =
#if   defined  (__GNUC__)
    "g++ " xstr(__GNUC__) "." xstr(__GNUC_MINOR__) "." xstr(__GNUC_PATCHLEVEL__)
#elif defined  (_MSC_VER)
    "MSVC " xstr(_MSC_VER)
#else
    " unknown compiler"
#error GNU gama - has not been tested with your compiler
#endif
    ;

  const char* GNU_gama_year = "2014";


  int version(const char* program, const char* copyright_holder)
  {
    std::cout
      << program << " (GNU Gama) " << GNU_gama_version
      << " / " << GNU_gama_compiler << "\n"
      << "Copyright (C) " << GNU_gama_year << " "
      << copyright_holder << "\n" <<
      "License GPLv3+: GNU GPL version 3 or later "
      "<http://gnu.org/licenses/gpl.html>\n"
      "This is free software: you are free to change and redistribute it.\n"
      "There is NO WARRANTY, to the extent permitted by law.\n";

    return 0;
  }

}


/* GNU Gama uses James Clark's parser Expat for XML data processing
 *
 *    Expat is subject to the Mozilla Public License Version 1.1.
 *    Alternatively you may use expat under the GNU General Public
 *    License instead.
 *
 *              ftp://ftp.jclark.com/pub/xml/expat.zip
 *
 * Normally GNU Gama is linked with Expat version 1.95.2 (or later)
 * shared library.  It is possible to compile and build Gama with old
 * expat version 1.1.  In such a case scripts for compiling GNU Gama
 * and linking the program gama-local expect Expat 1.1 library to be
 * in the same directory as GNU Gama
 */
