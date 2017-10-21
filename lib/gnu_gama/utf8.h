/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2011 Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#ifndef Utf8_LENGTH_UTF8_Length_utf8_length_utf8_lengthutf8_h
#define Utf8_LENGTH_UTF8_Length_utf8_length_utf8_lengthutf8_h

#include <string>

namespace GNU_gama {

  class Utf8 {
  public:

    static std::size_t length  (std::string);
    static std::string leftPad (std::string, std::size_t, char=' ');
    static std::string rightPad(std::string, std::size_t, char=' ');

  };

}  // namespace Utf8

#endif
