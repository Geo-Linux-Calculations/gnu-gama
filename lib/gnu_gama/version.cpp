/*
    GNU Gama --- Geodesy and Mapping C++ library
    Copyright (C) 1999, 2003, 2005  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/version.h>
#include <config.h>
#include <iostream>

namespace GNU_gama {

  // const char* GNU_gama_version  = "1.9.01a";
  // VERSION is defined in config.h
  const char* GNU_gama_version  = VERSION;

  const char* GNU_gama_compiler =
              #if   defined (__GNUC__)
              "GNU g++"             // g++ 3.3 / 3.4
              #elif defined (__BORLANDC__)
              "win32-borland"       // 5.6
              #elif defined (_MSC_VER)
              "win32-msvc"          // 1300
              #else
              #error GNU_gama - has not been tested with your compiler
              #endif
              ;

  const char* GNU_gama_year = "2010";


  int version(const char* program, const char* copyright_holder)
  {
    std::cout << program << " (GNU Gama) " << GNU_gama_version << "\n";
    std::cout << "Copyright (C) " << GNU_gama_year << " " << copyright_holder << "\n";
    std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n";
    std::cout << "This is free software: you are free to change and redistribute it.\n";
    std::cout << "There is NO WARRANTY, to the extent permitted by law.\n";

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

