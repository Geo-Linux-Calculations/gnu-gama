/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.

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

#ifndef GNU_gama_CheckNum_IntFloat__h_
#define GNU_gama_CheckNum_IntFloat__h_

#include <cctype>

namespace GNU_gama {


template <typename Iterator> void SkipWhiteSpaces(Iterator& b, Iterator e)
  {
    using namespace std;
    while ((b != e) && isspace(*b)) ++b;
  }

template <typename Iterator> void TrimWhiteSpaces(Iterator& b, Iterator& e)
  {
    using namespace std;
    SkipWhiteSpaces(b, e);

    Iterator i = b, z = e;
    while (i != e)
      if (!isspace(*(i++))) z = i;
    e = z;
  }

template <typename Iterator> bool IsInteger(Iterator& b, Iterator e)
  {
    using namespace std;

    TrimWhiteSpaces(b, e);
    if (b == e) return false;

    switch (*b) {
    case '+':
    case '-':
      ++b;
    };

    while (b != e)
      switch (*b) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        ++b;
        break;
      default:
        return false;
      }

    return true;
  }


template <typename Iterator> bool IsFloat(Iterator& b, Iterator e)
  {
    using namespace std;

    TrimWhiteSpaces(b, e);
    if (b == e) return false;

    switch (*b) {
    case '+':
    case '-':
      ++b;
    };

    bool hasdigit = false;

    if    (b != e && isdigit(*b)) hasdigit = true, ++b;
    while (b != e && isdigit(*b)) ++b;

    if    (b != e && *b == '.')   ++b;

    if    (b != e && isdigit(*b)) hasdigit = true, ++b;
    while (b != e && isdigit(*b)) ++b;

    if (b != e)
      {
        if (*b != 'e' && *b != 'E') return false;
        ++b;
        if (b == e) return false;
        switch (*b) {
        case '+':
        case '-':
          ++b;
        };
        if (b == e) return false;
        while (b != e && isdigit(*b)) ++b;
        if (b != e) return false;
      }

    return hasdigit;
  }


template <typename String> bool IsInteger(const String& s)
  {
    typename String::const_iterator b = s.begin();
    typename String::const_iterator e = s.end();
    return IsInteger(b, e);
  }


template <typename String> bool IsFloat(const String& s)
  {
    typename String::const_iterator b = s.begin();
    typename String::const_iterator e = s.end();
    return IsFloat(b, e);
  }


}   // namespace GNU_gama

// --------------------------------------------------------------------------

#ifdef GNU_gama_CheckNum_IntFloat_demo

#include <iostream>
#include <string>

void test(std::string s)
{
  using namespace std;
  using namespace GNU_gama;

  cout << (IsInteger(s) ? " Y " : " N ") << " "
       << (IsFloat  (s) ? " Y " : " N ") << " " << s << endl;
}

int main()
{
  using namespace std;
  using namespace GNU_gama;

  cout << "\nGNU_gama_CheckNum_IntFloat_demo\n"
       <<   "*******************************\n\n";

  cout << "int flt string\n\n";


  test("");
  test("  ");
  test(" 1   ");
  test(" 1 1 ");
  test(" +1  ");
  test(" -1  ");
  test(" 1.0 ");
  test(" 1.0 0 ");
  test("e");
  test("+1.2g");
  test("+1.2e");
  test("+1.2e3");
  test("+1.2e+");
  test("+1.2e-3");
  test("+1.2e-3x");

  return 0;
}

#endif

#endif
