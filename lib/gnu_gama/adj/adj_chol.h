/*
  GNU Gama is a package for adjustment and analysis of geodetic observations
  Copyright (C) 2005, 2006, 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GNU_gama_adjustment_cholesky_decomposition_gnu_gama_adj_chol_h
#define GNU_gama_adjustment_cholesky_decomposition_gnu_gama_adj_chol_h

#include <gnu_gama/exception.h>
#include <gnu_gama/adj/adj_basefull.h>
#include <gnu_gama/sparse/intlist.h>
#include <matvec/inderr.h>
#include <matvec/symmat.h>

namespace GNU_gama {

  template <typename Float=double,
	    typename Index=int,
	    typename Exc=Exception::matvec>
  class AdjCholDec : public AdjBaseFull<Float, Index, Exc>
  {
  public:

    AdjCholDec()  { init();          }
    ~AdjCholDec() { delete[] minx_i; }

    Index defect  ();
    Float q_xx    (Index, Index);
    Float q_bb    (Index, Index);
    Float q_bx    (Index, Index);
    bool  lindep  (Index);
    void  min_x   ();
    void  min_x   (Index, Index[]);
    void  solve   ();

  private:

    Index                     M, N; // number of observations, parameters
    Vec<Index, Index, Exc>    perm;
    Vec<Index, Index, Exc>    invp; // inverse permutation : invp(perm(i)) = i
    SymMat<Float, Index, Exc> mat;
    Vec   <Float, Index, Exc> rhs;

    Float s_tol;                    // tolerance for linearly dependent vectors
    Index nullity;
    Index N0;                       // last linearly independent column
    Vec   <Float, Index, Exc>  x0;  // a particular solution 'x0'
    SymMat<Float, Index, Exc>  Q0;  // cofactor matrix (inverse of mat(:N0,:N0))

    enum {ALL, SUBSET}  minx_t;     // parameters of regularization
    Index               minx_n;
    Index*              minx_i;

    void init()
    {
      s_tol   = Float();
      nullity = Index();
      minx_t  = ALL;
      minx_i  = 0;
      N0      = 0;
    }

    Mat<Float, Index, Exc> G;
    Float dot(const Mat<Float,Index,Exc>& M, Index i, Index j) const;
    Float T(Index, Index) const;

  };

}

#include <gnu_gama/adj/adj_chol_implementation.h>

#endif
