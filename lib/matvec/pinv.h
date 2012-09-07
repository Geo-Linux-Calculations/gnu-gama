/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 1999, 2007, 2009  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Matrix/Vector template library.

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

#ifndef GNU_gama_gMatVec_Mat_Inv__h_
#define GNU_gama_gMatVec_Mat_Inv__h_

#include <matvec/svd.h>


namespace GNU_gama {

  /** \brief Moore-Penrose pseudoinverse of a real M x N matrix.

  \param  A input matrix
  \return pseudoinverse of A

  Definition:

  \f{eqnarray*}
  AA^+A    &=& A     \\
  A^+A^A   &=& A^+   \\
  (AA^+)^T &=& AA^+  \\
  (A^+A)^T &=& A^+A  \\
  \f}

  Pseudoinverse is computed by using singular value decomposition
  (template class SVD).

  */

template <typename Float, typename Exc>
Mat<Float, Exc> pinv(const Mat<Float, Exc>& A)
{
  const Index M = A.rows();
  const Index N = A.cols();
  SVD<Float, Exc> svd(A);
  svd.decompose();

  const Mat<Float, Exc>& U = svd.SVD_U();
  const Vec<Float, Exc>& W = svd.SVD_W();
  const Mat<Float, Exc>& V = svd.SVD_V();

  Vec<Float,Exc> W_inv(N);
  for (Index k=1; k<=N; k++)
    if (!svd.lindep(k))
      W_inv(k) = 1 / W(k);
    else
      W_inv(k) = 0;

  Mat<Float, Exc> pseudo_inverse(N, M);  // V*inv(W)*trans(U);

  Float s;
  for (Index i=1; i<=N; i++)
    for (Index j=1; j<=M; j++)
      {
        s = 0;
        for (Index k=1; k<=N; k++)
          s += V(i,k)*W_inv(k)*U(j,k);
        pseudo_inverse(i,j) = s;
      }

  return pseudo_inverse;

}       /* Mat<Float, Exc> pinv(const Mat<Float, Exc>& A) */


}   // namespace GNU_gama

#endif







