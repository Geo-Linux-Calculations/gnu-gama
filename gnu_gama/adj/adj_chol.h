/*
  GNU Gama is a package for adjustment and analysis of geodetic observations
  Copyright (C) 2005  Ales Cepek

  This file is part of the GNU Gama C++ library.
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * $Id: adj_chol.h,v 1.3 2005/05/21 20:17:11 cepek Exp $
 */

#ifndef GNU_gama_adjustment_cholesky_decomposition_gnu_gama_adj_chol__h
#define GNU_gama_adjustment_cholesky_decomposition_gnu_gama_adj_chol__h

#include <gnu_gama/exception.h>
#include <gnu_gama/adj/adj_base.h>
#include <gnu_gama/sparse/intlist.h>
#include <matvec/inderr.h>
#include <matvec/symmat.h>

namespace GNU_gama {

  template <typename Float=double,  
            typename Exc=Exception::matvec> 
  class AdjCholDec 
    : virtual public AdjBase<Float, Exc>
  {
  public:
    
    AdjCholDec() { init(); }
    
    
    Index defect  ();
    Float q_xx    (Index, Index);
    Float q_bb    (Index, Index);
    Float q_bx    (Index, Index);
    bool  lindep  (Index);
    void  min_x   ();
    void  min_x   (Index, Index[]);

  protected:

    void  solve_me();
    
  private:
    
    Vec   <Index>       perm;
    Vec   <Index>       invp;    // inverse permutation : invp(perm(i)) = i
    SymMat<Float, Exc>  mat;
    Vec   <Float, Exc>  rhs;

    Float               s_tol;   // tolerance for linearly dependent vectors
    Index               nullity; 
    Index               N0;      // last linearly independent column
    Vec   <Float, Exc>  x0;      // a particular solution 'x0'
    SymMat<Float, Exc>  Q0;      // cofactor matrix (inverse of mat(:N0,:N0))

    void init()
    {
      s_tol   = Float();
      nullity = Index();
    }

  };

}
      
#include <gnu_gama/adj/adj_chol_implementation.h>
  
#endif
