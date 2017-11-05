/* matvec_test_002.cpp
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

template <class Mat> void
InitMat(Mat& M, double ini[], Mat& I, double inv[], double& cond, double& maxe)
{
  typedef size_t Index;
  long double inv0 = 1.0L/ini[0];
  inv[0] = inv0;
  for (Index i=1; i<M.rows(); i++)
    {
      long double s = 0;
      for (Index j=1; j<=i; j++)
        s -= ini[j]*inv[i-j];
      inv[i] = s * inv0;
    }
  // for (Index i=0; i<M.rows(); i++) cout << " " << long(inv[i]); cout<<endl;

  for (Index i=1; i<=M.rows(); i++)
    for (Index j=1; j<=M.cols(); j++)
      if (i > j)
        I(i,j) = M(i,j) = 0;
      else
        {
          M(i,j) = ini[j-i];
          I(i,j) = inv[j-i];
        }

  Mat X = I*trans(M);
  Mat Y = trans(I)*M;
  M = X;
  I = Y;

  maxe = 0;   // max. element
  double a = 0, b = 0, e;
  for (Index i=1; i<=M.rows(); i++)
    for (Index j=1; j<=M.cols(); j++)
      {
        e = fabs(M(i,j));
        if (e > maxe) maxe = e;
        a += e*e;
        b += I(i,j)*I(i,j);
      }
  cond =sqrt(a*b);
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

  cout << "\n   inv(Mat)   .........   test_002  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";

  const Index MaxDim=70;
  double dini[MaxDim] = {2.7, 3.1, 3.3, -0.4};
  double dinv[MaxDim];
  double cond, maxe;

  for (Index N=3; N<=MaxDim; N++)
    {
      cout << N << "x" << N;

      Mat<> A(N, N), B(N,N), C(N,N);
      InitMat(A, dini, B, dinv, cond, maxe);

      cout.precision(2);
      try {
        Mat<> I(N,N);
        I.set_identity();

        cout << "\tcond: " << cond;
        cout << "\tpinv() ";
        C = pinv(A);
        cout << MaxEl(C-B)/maxe << "\t" << MaxEl(C*A - I);

        cout << "\tinvert() ";
        C = A;
        C.invert();
        cout << MaxEl(C-B)/maxe << "\t" << MaxEl(C*A - I);
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
