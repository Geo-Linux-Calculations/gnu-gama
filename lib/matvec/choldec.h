/*  
    C++ Matrix/Vector templates (GNU Gama / matvec 0.9.25)
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Matrix/Vector template library.
    
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: choldec.h,v 1.1 2006/04/09 16:12:01 cepek Exp $
 *  http://www.gnu.org/software/gama/
 */

#ifndef GNU_gama_gMatVec_Cholesky_Decomposition__h_
#define GNU_gama_gMatVec_Cholesky_Decomposition__h_

#include <matvec/vecbase.h>


namespace GNU_gama {

/* Cholesky Decomposition of Positive Definite Matrix
 * ==================================================
 *
 * Two variants of implementation are possible:
 *
 *    a)   A = L*trans(L)
 *    b)   A = L*D*trans(L),   where D is diagonal and L has 1 on diagonal 
 */


template <typename Float=double, typename Exc=Exception::matvec>
class CholDec {

  Float  tol_;
  
public:

  CholDec(Float t=1e-8) : tol_(t) {}

  Float  cholTol() const  { return tol_; }
  Float  cholTol(Float t) { tol_ = t; return tol_; } 

  virtual void cholDec() = 0;    // `in situ' Cholesky decomposition
  virtual void solve(Vec<Float, Exc>& rhs) const = 0;
};


}   // namespace GNU_gama

#endif



