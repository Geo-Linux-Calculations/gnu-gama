/* matvec_test_004.cpp
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

#include <matvec/matvec.h>
#include <matvec/pinv.h>
#include <iostream>
#include <iomanip>

using namespace GNU_gama;

template <class Float, class Exc> void
InitMat_CACM_Alg_52(int n, Mat<Float, Exc>& A, Mat<Float, Exc>& invA)
{
  /* CACM Algorithm 52
   * A set of test matrices by J.R.Herndon, corrected by P. Naur
   * -------------------------------------------------------------------
   *
   * This procedure places in A an n x n matrix whose inverse and
   * eigenvalues are known. The n-th row and the n-th column of the
   * inverse are the set: 1, 2, 3, ..., n. The matrix formed by
   * deleting the n-th row and n-th column of the inverse is the
   * identity matrix of order n-1.
   *
   */

  {
    A.reset(n,n);

    Float c = n * (n+1) * (n+n-5)/6;
    Float d = 1 / c;

    A(n,n) = -d;
    for (int i=1; i<=n-1; i++)
      {
        A(i,n) = A(n,i) = d * i;
        A(i,i) = d * (c - i*i);

        for (int j=1; j<=i-1; j++)
          A(i,j) = A(j,i) = -d*i*j;
      }
  }

  {
    invA.reset(n,n);
    invA.set_identity();
    for (int i=1; i<=n; i++) invA(i,n) = invA(n,i) = i;
  }
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

  cout << "\n   CACM Alg. 52  ......   test_004  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";

  const int MaxDim=70;

  for (int N=3; N<=MaxDim; N++)
    {
      cout << N << "x" << N;

      Mat<> A(N, N), B(N,N), C;
      InitMat_CACM_Alg_52(N, A, B);

      cout.precision(2);
      try {
        Mat<> I(N,N);
        I.set_identity();

        cout << "\tpinv() ";
        C = pinv(A);
        cout << MaxEl(B-C);

        cout << "\tinvert() ";
        C = A;
        C.invert();
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
