/*  
    C++ Matrix/Vector templates (GNU GaMa / gMatVec 0.9.18)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the gMatVec C++ Matrix/Vector template library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: pinv.h,v 1.5 2002/04/10 16:19:59 cepek Exp $
 *  http://www.gnu.org/software/gama/
 */

#ifndef gMatVec_Mat_Inv__h_
#define gMatVec_Mat_Inv__h_

#include <gmatvec/svd.h>

namespace gMatVec {

template <class Float, class Exc>
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


}   // namespace gMatVec

#endif





