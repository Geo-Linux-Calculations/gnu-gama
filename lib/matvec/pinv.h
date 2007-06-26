/*  
    C++ Matrix/Vector templates (GNU Gama / matvec 0.9.26)
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

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

/*
 *  $Id: pinv.h,v 1.3 2007/06/26 15:04:12 cepek Exp $
 *  http://www.gnu.org/software/gama/
 */

#ifndef GNU_gama_gMatVec_Mat_Inv__h_
#define GNU_gama_gMatVec_Mat_Inv__h_

#include <matvec/svd.h>

namespace GNU_gama {

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
  Float t = svd.tol();
  for (Index k=1; k<=N; k++)
    if (W(k) > t)
      W_inv(k) = 1 / W(k);
    else
      W_inv(k) = 0;
  
  Mat<Float, Exc> pseudo_inverse(M, N);  // V*inv(W)*trans(U);
  
  Float s;
  for (Index i=1; i<=M; i++)
    for (Index j=1; j<=N; j++)
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





