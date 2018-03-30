/*
  GNU Gama C++ library tests/matvec
  Copyright (C) 2017  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library tests/matvec
  
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

#include <matvec/matvec.h>
#include <matvec/svd.h>
#include <iostream>
#include <string>

using namespace GNU_gama;

int b01()
{
  double ver = std::stod(std::string(matvec_version()));

  Vec<> a, b{ {1,2,3,4,5,6,7,8,9,10} };
  unsigned bd = b.dim();

  a = std::move(b);
  std::cout << "01# matvec_version() " << matvec_version() << " --> "
            << " a.dim() b.dim() : " << a.dim() << " " << b.dim() << "\n";

  if (ver >= 2.0)
    {
      // move constructor
      if (a.dim() != bd || b.dim() != 0 ) return 1;
      for (unsigned i=1; i<=bd; i++)
        if (a(i) != i) return 1;
    }
  else
    {
      // fall back to copy constructor
      if (a.dim() != bd || b.dim() != bd) return 1;
      for (unsigned i=1; i<=bd; i++)
        if (a(i) != i || a(i) != b(i)) return 1;
    }

  return 0;
}


int b02()
{
  int result = 0;

  using std::cout;
  using std::endl;

  // code fragment from matvec_demo_001

  Mat<> A(5, 3);
  Vec<> b(5);

  try    // List initialisation for Mat<> and Vec<> --> move assignment
    {
       A = {{1.001, 0.006, 0.012},
            {0.002, 1.007, 0.013},
            {0.003, 0.008, 1.014},
            {1.004, 0.009, 1.015},
            {0.005, 1.011, 1.016}};

        b = {1.1, 1.9, 3.1, 4, 5.1};
    }
  catch (const Exception::matvec& e)
    {
      cout << e.description << endl;
      return 1;
    }

  return result;
}


int main()
{
  int error = 0;

  error += b01();
  error += b02();

  return error;
}
