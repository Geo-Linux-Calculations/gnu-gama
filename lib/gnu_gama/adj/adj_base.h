/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999, 2006  Ales Cepek <cepek@gnu.org>

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
 *  $Id: adj_base.h,v 1.10 2006/08/30 13:52:09 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h

#include <matvec/matvec.h>

namespace GNU_gama {


  template <typename Float, typename Exc>
  class AdjBase {

  public:

    virtual ~AdjBase() {}
 
    virtual const Vec<Float, Exc>& unknowns()  = 0;   // unknown parameters
    virtual const Vec<Float, Exc>& residuals() = 0;   // adjusted residuals
    virtual Index defect()                     = 0;
 
    virtual Float q_xx(Index, Index)           = 0;   // w. coeff. (xi,xj)
    virtual Float q_bb(Index, Index)           = 0;   //           (bi,bj)
    virtual Float q_bx(Index, Index)           = 0;   //           (bi,xj)
                                              
    virtual bool lindep(Index)                 = 0;   // lin. dep. column
    virtual void min_x()                       = 0;
    virtual void min_x(Index, Index[])         = 0;

    virtual Float cond() { return 0; }                // 0 if not available

  };



  template <typename Float, typename Exc>
  class AdjBaseFull : public AdjBase<Float, Exc> 
  {    
  public:

    AdjBaseFull() : pA(0), pb(0), is_solved(false)
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

    const Vec<Float, Exc>& unknowns() 
    { 
      if (!is_solved) solve(); 
      return x; 
    }

    const Vec<Float, Exc>& residuals() 
    { 
      if (!is_solved) solve(); 
      return r; 
    }
    
    // solve() must compute vectors x, r  and set is_solved=true
    virtual void solve() = 0;


  protected:

    const Mat<Float, Exc>* pA;
    const Vec<Float, Exc>* pb;

    Vec<Float, Exc> x;
    Vec<Float, Exc> r;
    bool is_solved;

  };
  

}
#endif

