/* matvec_test_005.cpp
   Copyright (C) 2000, 2012  Ales Cepek <cepek@gnu.org>

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

#include <matvec/symmat.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using namespace GNU_gama;

template <class Float, class Exc> void
InitMat_SymMat(Index n, SymMat<Float, Exc>& A, SymMat<Float, Exc>& I)
{
  SymMat<> U(n);

  for (Index i=1; i<=n; i++)
    for (Index j=1; j<=i; j++)
      if (i == j)
        U(i,i) = 1;
      else
        U(i,j) = rand()/(RAND_MAX+1.0) - 0.5;

  Mat<> M = Upper(U);   // SymMat --> Mat
  M = trans(M)*M;
  A = Upper(M);
  Mat<> Q = M;
  M.invert();
  I = Upper(M);
}


template <class Float, class Exc>
Float MaxEl(const GNU_gama::MatVecBase<Float, Exc>& M)
{
  Float m = 0, x;
  typedef typename GNU_gama::MatVecBase<Float, Exc>::const_iterator cIter;
  for (cIter i=M.begin(); i!=M.end(); ++i)
    {
      x = *i;  if (x < 0) x = -x;
      if (x > m) m = x;
    }
  return m;
}

int main()
{
  using namespace std;
  using namespace GNU_gama;

  cout << "\n   SymMat::invert()  ...  test_005  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";

  const Index MaxDim=70;

  for (Index N=3; N<=MaxDim; N++)
    {
      cout << N << "x" << N;

      SymMat<> A(N, N), B(N,N), C;
      InitMat_SymMat(N, A, B);

      cout.precision(2);
      try {
        Mat<> I(N,N);
        I.set_identity();

        cout << "\tinv() ";
        C = inv(A);
        cout << MaxEl(B-C);
      }
      catch (Exception::matvec e) {
        cout << " err = " << int(e.error) << " : " << e.description;
      }
      catch (...) {
        cout << "?????????????????????????????????\n";
      }
      cout << endl;
    }

  cout <<  "\n------------------------------------------------------\n\n";
 }
