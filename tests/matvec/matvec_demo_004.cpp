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

#include <iostream>
#include <matvec/matvec.h>

using namespace std;
using namespace GNU_gama;

bool operator==(const Mat<>& A, const Mat<> B)
{
  if (A.rows() != B.rows() || A.cols() != B.cols()) return false;

  for (int i=1; i<=A.rows(); i++)
    for (int j=1; j<=A.cols(); j++)
      if (A(i,j) != B(i,j)) return false;

  return true;
}

bool operator!=(const Mat<>& A, const Mat<> B) { return !(A == B); }

int f(ostream& cout)
{
  cout << "\n   GNU_gama   .........   demo_004  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";

  Vec<> v      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  Vec<> w =    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  Vec<> x; x = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
  cout << "v/w/x = ";
  for (unsigned i=1; i<=v.dim(); i++) {
    cout << v(i) << " ";
    if (i != v(i) || i != w(i) || i != x(i)) {
      cout << "initialization error " << i << " "
	   << v(i) << " " << w(i) << " " << x(i) << "\n";
      return 1;
    }
  }
  cout << "\n\n";

  
  Mat<> X(3, 3), Y, T(3, 3);
  Mat<> M {{ 1,  3,  5},
           { 7, 11, 13},
           {17, 19, 23}};
  T = {{ 1,  7, 17},
       { 3, 11, 19},
       { 5, 13, 23}};

  cout << "M = " << M << "\nT = " << T << "\n";

  X = {{1, 3, 5}, {7, 11, 13}, {17, 19, 23}};
  cout << "M=T";
  Y = M;
  if (X != Y) return 1;
  cout << " ";

  X = {{1, 3, 5}, {7, 11, 13}, {17, 19, 23}};
  cout << "M=T'";
  Y = trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{2, 10, 22}, {10, 22, 32}, {22, 32, 46}};
  cout << "M+T";
  Y = M+T;
  if (X != Y) return 1;
  cout << " ";

  X = {{0, -4, -12}, {4, 0, -6}, {12, 6, 0}};
  cout << "M-T";
  Y = M-T;
  if (X != Y) return 1;
  cout << " ";

  X = {{2, 10, 22}, {10, 22, 32}, {22, 32, 46}};
  cout << "M'+T'";
  Y = trans(M)+trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{0, 4, 12}, {-4, 0, 6}, {-12, -6, 0}};
  cout << "M'-T'";
  Y = trans(M)-trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{2, 6, 10}, {14, 22, 26}, {34, 38, 46}};
  cout << "M+T'";
  Y = M+trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  cout << "M-T'";
  Y = M-trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{2, 14, 34}, {6, 22, 38}, {10, 26, 46}};
  cout << "M'+T";
  Y = trans(M)+T;
  if (X != Y) return 1;
  cout << " ";

  X = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
  cout << "M'-T";
  Y = trans(M)-T;
  if (X != Y) return 1;
  cout << " ";

  X = {{35, 105, 189}, {105, 339, 627}, {189, 627, 1179}};
  cout << "M*T";
  Y = M*T;
  if (X != Y) return 1;
  cout << " ";

  X = {{339, 403, 487}, {403, 491, 595}, {487, 595, 723}};
  cout << "M'*T'";
  Y = trans(M)*trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{107, 131, 159}, {305, 389, 477}, {541, 697, 861}};
  cout << "M*T'";
  Y = M*trans(T);
  if (X != Y) return 1;
  cout << " ";

  X = {{107, 305, 541}, {131, 389, 697}, {159, 477, 861}};
  cout << "M'*T";
  Y = trans(M)*T;
  if (X != Y) return 1;
  cout << " ";

  cout << endl;
  return 0;
}

int main()
{
  if (f(cout))
    {
      cout << "  ******** Bug in matvec\n\n";
      return 1;
    }
}
