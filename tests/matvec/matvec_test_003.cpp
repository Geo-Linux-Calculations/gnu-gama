/* matvec_test_003.cpp
   Copyright (C) 2000, 2012, 2018  Ales Cepek <cepek@gnu.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library (see COPYING.LIB); if not, write to the
   Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <iostream>
#include <iomanip>
#include <cmath>
#include <matvec/hilbert.h>

int main()
{
  typedef long double Double;

  using namespace std;
  using namespace GNU_gama;

  int result = 0;
  Double tol = 0;
  
  cout << "\n   Hilbert matrix  ....   test_003  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";

  cout.setf(ios::scientific, ios::floatfield);
  cout.precision(2);

  for (int N=2; N<=15; N++)
    {
      cout << setw(2) << N;

      Mat<Double> E(N,N);
      E.set_identity();
      Mat<Double> H = Hilbert   <Double, int, Exception::matvec>(N);
      Mat<Double> I = InvHilbert<Double, int, Exception::matvec>(N);

      Double fh = 0, fi = 0;
      for (int i=1; i<=N; i++)
        for (int j=1; j<=N; j++)
          {
            fh += H(i,j)*H(i,j);
            fi += I(i,j)*I(i,j);
          }
      cout << setw(14) << sqrt(fh*fi);


      try {
        cout << setw(14);
        Mat<Double> T = Hilbert<Double, int, Exception::matvec>(N);
        T.invert(tol);
        T = H*T - E;

        Double m = 0;
        for (int i=1; i<=N; i++)
          for (int j=1; j<=N; j++)
            if (std::abs(T(i,j)) > std::abs(m))
              m = T(i,j);

        cout << m;
      }
      catch (Exception::matvec&) {
        cout << "failed";
	result++;
      }

      try {
        cout << setw(14);
        Mat<Double> T = InvHilbert<Double, int, Exception::matvec>(N);
        T.invert(tol);
        T = I*T - E;

        Double m = 0;
        for (int i=1; i<=N; i++)
          for (int j=1; j<=N; j++)
            if (std::abs(T(i,j)) > std::abs(m))
              m = T(i,j);

        cout << m;
      }
      catch (Exception::matvec&) {
        cout << "failed";
	result++;
      }

      cout << endl;
    }

  // cout.setf(ios::fixed, ios::floatfield);
  // cout.precision(0);
  // cout << setw(9) << InvHilbert<Double>(15);

  return result;
}
