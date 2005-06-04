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
 *  $Id: adj_gso.h,v 1.6 2005/06/04 21:02:04 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_OLS_gso_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_OLS_gso_h

#include <matvec/gso.h>
#include <gnu_gama/adj/adj_base.h>
#include <cmath>

namespace GNU_gama {
  
template <typename Float, typename Exc>
class AdjGSO : public virtual AdjBase<Float, Exc> {

public:

  AdjGSO() {}
  AdjGSO(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b)
    : AdjBase<Float, Exc>(A, b) {}
  AdjGSO(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b,
         const Vec<Float, Exc>& w) : AdjBase<Float, Exc>(A, b, w) {}
  
  void reset(const Mat<Float, Exc>& A, 
             const Vec<Float, Exc>& b)
    {
      AdjBase<Float, Exc>::reset(A, b);
    }
  void reset(const Mat<Float, Exc>& A, 
             const Vec<Float, Exc>& b,
             const Vec<Float, Exc>& w)
    {
      AdjBase<Float, Exc>::reset(A, b, w);
    }
  
  const Vec<Float, Exc>& solve(Vec<Float, Exc>& x)
    {
      return x = AdjBase<Float, Exc>::solve();
    }
  const Vec<Float, Exc>& solve() 
    { 
      return AdjBase<Float, Exc>::solve(); 
    }
  
  Index defect() { return gso.defect(); }
  bool  lindep(Index i) { return gso.lindep(i); }
  
  void  q_xx(Mat<Float, Exc>& C) { AdjBase<Float, Exc>::q_xx(C); }
  Float q_xx(Index i, Index j);
  Float q_bb(Index i, Index j);
  Float q_bx(Index i, Index j);
  
  void min_x() { gso.min_x(); }
  void min_x(Index n, Index x[]) { gso.min_x(n, x); }
  
  
protected:
  
  void solve_me();
   
private:

   Mat<Float, Exc> A_;
   GSO<Float, Exc> gso;
   Vec<Float, Exc> sqrt_w;

   void init_gso_();
};

// ...................................................................


template <typename Float, typename Exc>
void AdjGSO<Float, Exc>::solve_me()
{
  if (this->is_solved) return;

  const Index M = this->pA->rows();
  const Index N = this->pA->cols();
  
  A_.reset(M+N, N+1);
  this->sqrt_w.reset(M);

  if (this->pw)
    {
      using namespace std;
      const Vec<Float, Exc>& w_ = *this->pw;
      for (Index i=1; i<=M; i++) this->sqrt_w(i) = sqrt(w_(i));
    }
  else
    {
      for (Index i=1; i<=M; i++) this->sqrt_w(i) = 1;
    }

  const Mat<Float, Exc>& A1 = *this->pA;
  const Vec<Float, Exc>& b1 = *this->pb;

  {  // redundant curly braces needed by MS VC++ 
  for (Index i=1; i<=M; i++)
    {
      A_(i, N+1) = -b1(i)*this->sqrt_w(i);
      for (Index j=1; j<=N; j++) A_(i, j) = A1(i, j)*this->sqrt_w(i);
    }
  }
  {
  for (Index i=1; i<=N; i++) 
    for (Index j=1; j<=N+1; j++)
      A_(M+i, j) = (i==j) ? 1 : 0;
  }

  gso.reset(A_, M, N);
  gso.gso1(); 
  gso.gso2(); 

  this->x.reset(N);
  for (Index i=1; i<=N; i++)
    this->x(i) = A_(M+i, N+1);

  this->r.reset(M);
  for (Index j=1; j<=M; j++)
    this->r(j) = A_(j, N+1)/this->sqrt_w(j);
  
  this->is_solved = true; 
}


template <typename Float, typename Exc>
Float AdjGSO<Float, Exc>::q_xx(Index i, Index j)
  {
    if(!this->is_solved) solve_me();
    const Index M = this->pA->rows();
    const Index N = this->pA->cols();
    i += M;
    j += M;
    Float s = 0;                        
    for (Index k=1; k<=N; k++) 
      s += A_(i,k)*A_(j,k);              // cov x_i x_j
    return s;
  }


template <typename Float, typename Exc>
Float AdjGSO<Float, Exc>::q_bb(Index i, Index j)
  {
    if(!this->is_solved) solve_me();

    const Index N = this->pA->cols();
    Float s = 0;                        
    for (Index k=1; k<=N; k++) 
      s += A_(i,k)*A_(j,k);              // cov b_i b_j
    return s/this->sqrt_w(i)/this->sqrt_w(j);
  }


template <typename Float, typename Exc>
Float AdjGSO<Float, Exc>::q_bx(Index i, Index j)
  {
    if(!this->is_solved) solve_me();

    const Index M = this->pA->rows();
    const Index N = this->pA->cols();
    j += M;
    Float s = 0;                        
    for (Index k=1; k<=N; k++) 
      s += A_(i,k)*A_(j,k);              // cov b_i x_j
    return s/this->sqrt_w(i);
  }


}   // GaMaLib

#endif



















