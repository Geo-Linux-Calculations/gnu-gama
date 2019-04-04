/*
  GNU Gama C++ library
  Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

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

/*
 * A helper program for generating gama-local.pro (a qmake project file)
 * from GNU Gama lib/Makefile.am.
 *
 * Usage: gama-local-pro  < Makefile.am  > gama-local.pro
 *
 *****************************************************
 *                                                   *
 *    If possible use cmake CMakeLists.txt instead   *
 *                                                   *
 *****************************************************
 */


#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::getline;
using std::isspace;

int main()
{
  cout << "# qmake project file for building gama-local"
          " (generated from GNU Gama lib/Makefile.am)\n\n"

          "TEMPLATE = app\n"
          "CONFIG += console c++11\n"
          "CONFIG -= app_bundle\n"
          "CONFIG -= qt\n"
          "CONFIG -= warn_on\n\n" // can qmake suppress CFLAGS warnings only ?

          "DEFINES     += GNU_gama_expat_1_1\n"
          "INCLUDEPATH += lib\n"
          "INCLUDEPATH += lib/expat/xmltok\n\n"

          "SOURCES += \\\n"
          "    lib/expat/xmltok/xmltok.c \\\n"
          "    lib/expat/xmltok/xmlrole.c \\\n"
          "    lib/expat/xmlwf/codepage.c \\\n"
          "    lib/expat/xmlparse/xmlparse.c \\\n"
          "    lib/expat/xmlparse/hashtable.c \\\n"

          "    bin/gama-local.cpp";

    std::vector<std::string> sources, headers;

    string s;
    auto ends_with = [&s](std::string const & ending)
    {
      if (ending.size() > s.size()) return false;
      return std::equal(ending.rbegin(), ending.rend(), s.rbegin());
    };

    while (cin >> s && s != "libgama_src");
    getline(cin, s);

    while (getline(cin, s))
      {
        if (s.empty()) break;
        while (isspace(s.back()) || s.back() == '\\') s.pop_back();
        if (s.empty()) break;

        while (isspace(s.front())) s.erase(s.begin());

        if (ends_with(".cpp")) sources.push_back(s);
        if (ends_with(".h"))   headers.push_back(s);
      }

    for (auto t : sources) cout << " \\\n    lib/" << t;
    cout << "\n\n";

    cout << "HEADERS =";
    for (auto t : headers) cout << " \\\n    lib/" << t;
    cout << "\n";

    return 0;
}
