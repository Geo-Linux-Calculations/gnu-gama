/* GNU Gama C++ library
   Copyright (C) 2022  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/xml/str2xml.h>

namespace GNU_gama {

  std::string str2xml(const std::string &str)
  {
    std::string t;
    for (std::string::const_iterator i=str.begin(), e=str.end(); i!=e; ++i)
      {
        char c = *i;
        if      (c == '<') t += "&lt;";
        else if (c == '>') t += "&gt;";
        else if (c == '&') t += "&amp;";
        else if (c =='\'') t += "&quot;";
        else               t += c;
      }
    return t;
  }

  std::string str2xml(char c)
  {
    std::string t {c};
    return str2xml(t);
  }
}
