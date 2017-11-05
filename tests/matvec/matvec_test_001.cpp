/* matvec_test_001.cpp
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

   ----------------------------------------------------------------------

   Computes maximal error in an element of inv(const Mat& U), where U
   is an upper triangular ill-conditioned matrix with known inverse.

   The structure of elements of the upper triangular matrix U is defined
   in the array invbmat as follows:

          | u1 u2 ... un 0  ... 0 |
          | 0  u1 u2 ... un ... 0 |
      U = | 0  0  u1 u2 ... ... 0 |
          |        ...            |
          | 0      ...       0  u1|


   Condition number of the matrix U is estimated as norm(U)*norm(inv(U)),
   where norm(U) = \sqrt{ \sum_{i=1}^N \sum_{j=1}^N u_{ij}*u_{ij} }.
*/

#include <matvec/matvec.h>
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
  /*
  Mat X = I*trans(M);
  Mat Y = trans(I)*M;
  M = X;
  I = Y;
  */

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


template<class Float> void InverseTest()
{
  using namespace GNU_gama;
  using namespace std;

  const  Index   MaxN = 50;
  double inibmat[MaxN] = {1, 2, 3};
  double inverse[MaxN];
  cout.setf(ios::scientific, ios::floatfield);
  cout.precision(3);
  for (Index N=3; N<=MaxN; N++)
    {
      cout << setw(2) << sizeof(Float) << " " << setw(2) << N;
      Mat<Float> M(N,N);
      Mat<Float> I(N,N);
      double cond, maxe;
      InitMat(I, inibmat, M, inverse, cond, maxe);
      cout << "  cond. <= " << cond;
      Mat<Float> invM = inv(M);
      double maxdif = 0;
      for (Index i=1; i<=N; i++)
        for (Index j=1; j<=N; j++)
          if (fabs(invM(i,j) - I(i,j)) > fabs(maxdif))
              maxdif = invM(i,j) - I(i,j);
      cout << "  max. relative error = "
           << fabs(maxdif)/maxe << endl;
    }
  cout << endl;
}

int main()
{
  std::cout << "\n   inv(Mat)   .........   test_001  matvec "
	    << GNU_gama::matvec_version() << "\n"
            << "------------------------------------------------------\n\n";

  // InverseTest<float>();
  InverseTest<double>();
  InverseTest<long double>();
 }
