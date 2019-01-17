/*
  GNU Gama C++ library
  Copyright (C) 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/simplified.h>
#include <cctype>

// Returns a string that has whitespace removed from the start and the
// end, and that has each sequence of internal whitespace replaced
// with a single space.

using std::string;

string GNU_gama::simplified(string s)
{
  std::string str;
  //if (str.empty()) return str;

  while (!s.empty() && isspace(s.back())) s.pop_back();
  string::const_iterator i = s.begin();
  string::const_iterator e = s.end();
  while (i != e && std::isspace(*i)) i++;

  while (i != e)
    {
      if (!std::isspace(*i))
        {
          str.push_back(*i++);
        }
      else
        {
          str.push_back(' ');
          while (i != e && std::isspace(*i)) i++;
        }
    }

  return str;
}
