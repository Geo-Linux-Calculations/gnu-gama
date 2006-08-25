/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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
 *  $Id: adj_base.h,v 1.8 2006/08/25 15:52:35 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h

#include <matvec/matvec.h>

namespace GNU_gama {


  template <typename Float, typename Exc>
  class AdjBase {
    
  public:

    AdjBase() : is_solved(false)
    {
    }
    virtual ~AdjBase() 
    {
    }
    
    void unknowns(Vec<Float, Exc>& parameters) 
    { 
      if (!is_solved) solve(); 
      parameters = x; 
    }
    const Vec<Float, Exc>& unknowns() 
    { 
      if (!is_solved) solve(); 
      return x; 
    }
    void residuals(Vec<Float, Exc>& res) 
    { 
      if (!is_solved) solve(); 
      res = r; 
    }
    const Vec<Float, Exc>& residuals() 
    { 
      if (!is_solved) solve(); 
      return r; 
    }
    
    virtual Index defect() = 0;
    
    virtual Float q_xx(Index, Index) = 0; // w. coeff. (xi,xj)
    virtual Float q_bb(Index, Index) = 0; //           (bi,bj)
    virtual Float q_bx(Index, Index) = 0; //           (bi,xj)
    
    virtual bool lindep(Index) = 0;       // linearly dependent column/unknown
    virtual void min_x() = 0;
    virtual void min_x(Index, Index[]) = 0;
    
    virtual Float cond() { return 0; }    // 0 if not available
    
    // solve() must compute vectors x, r  and set is_solved=true
    virtual void solve() = 0;
    
  protected:
    
    Vec<Float, Exc> x;
    Vec<Float, Exc> r;
    bool is_solved;
    
  };


  
  template <typename Float, typename Exc>
  class AdjBaseFull : public AdjBase<Float, Exc> 
  {    
  public:

    AdjBaseFull() : pA(0), pb(0)
    {
    }
    AdjBaseFull(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b)
      : pA(&A), pb(&b)
    {
    }
    virtual ~AdjBaseFull() 
    {
    }

    virtual void reset(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b) 
    {
      pA = &A;
      pb = &b;
      this->is_solved = false;
    }

  protected:
    
    const Mat<Float, Exc>* pA;
    const Vec<Float, Exc>* pb;
    
  };
  

}
#endif

