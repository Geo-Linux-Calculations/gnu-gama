/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU Gama C++ library.

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

#ifndef gama_local_read_sparse_Ab_h
#define gama_local_read_sparse_Ab_h

#include <matvec/matvec.h>
#include <iostream>

namespace GNU_gama { namespace local {


/*
   Reads A, b and w, sparse matrix of project equations with diagonal weights.

        Ax = b,   w=diag(w1, w2, ..., w_m),   x = inv(At*w*A)*(At*w*b)
*/

template <typename Float, typename Exc>
void Read_Sparse_Abw(std::istream& inp,
                     gMatVec::Mat<Float, Exc>& A,
                     gMatVec::Vec<Float, Exc>& b,
                     gMatVec::Vec<Float, Exc>& w)
{
   int M, N;
   inp >> N >> M;                   // number of unknowns and observations
   A.reset(M, N);
   b.reset(M);
   w.reset(M);

   A.set_zero();

   int* ind = new int[N];
   for (int n, r=1; r<=M; r++)      // all project equations 1, 2, ..., M
      {
         inp >> n;                  // number of nonzero coefficients
         for (int i=0; i<n; i++)
            inp >> ind[i];          // list of indexes of nonzero coefficients
         inp >> w(r) >> b(r);       // weight and rhs
         for (int j=0; j<n; j++)
            inp >> A(r,ind[j]);     // list of nonzero coefficients
      }
      delete[] ind;
}

}}      // namespace GNU_gama::local

#endif







