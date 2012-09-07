/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 1999, 2007  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gMatVec_GSO__h_
#define GNU_gama_gMatVec_GSO__h_

#include <matvec/matvec.h>
#include <cmath>

/* Gram-Schmidt Ortogonalization
 * =============================
 *
 * Template class GSO is meant only as a tool for testing purposes.
 * Both initial scaling and pivoting with euklidian norms are not
 * suitable for practical computations.
 *
 * gso1()
 * ------
 *
 * Modified Gram-Schmidt ortogonalization of a given block matrix
 *
 *   ( A1, A2 ) => (W1, W2) == (    W1,     A2 - W1*trans(W1)*A2 )
 *   ( A3, A4 )    (W3, W4)    ( A3*inv(R), A4 - W3*trans(W1)*A2 )
 *
 * where
 *
 *   A1 = W1*R,  in other words R is upper triangular matrix of
 *               Cholesky decomposition of the matrix trans(A1)*A1
 *
 * gso2()
 * ------
 *
 * Regularization for the case of A1 with linearly dependent columns
 * (algorithm GSO).
 *
 * Frantisek Charamza: GSO - An Algorithm for Solving Linear Least
 * Squares Problems with Possibly Rank Deficient Matrices, referat
 * VUGTK, Praha 1977.
 *
 * Frantisek Charamza: An Algorithm for the Minimum-Length
 * Least-Squares Solution of a Set of Observation Equations, Studia
 * geoph. et. geod., 22 (1978), 129-139.
 *
 *
 *  */

namespace GNU_gama {   /** \brief Gram-Schmidt Ortogonalization */

template <typename Float=double, typename Exc=Exception::matvec>
class GSO {

public:

  GSO(): pA(0), M(0), N(0), sc(true), tol_(0),
    minx_n(0), minx(0), clist(0), rlist(0) {}
  ~GSO() { delete[] minx; delete[] clist; delete[] rlist; }
  GSO(Mat<Float, Exc>& a, Index m, Index n)
    : pA(0), M(0), N(0), sc(true), tol_(0),
    minx_n(0), minx(0), clist(0), rlist(0)
  {
    reset(a, m, n);     // where m, n are dimensions of A1(m, n)
  }
  void reset(Mat<Float, Exc>& a, Index m, Index n);

  void gso1();
  void gso2();

  Index defect()        { gso1(); return defect_; }
  bool  lindep(Index i) { gso1(); return norm(i)==0; } // dependent column
  bool  scaling() const { return sc; }
  void  scaling(bool s) { sc = s; }
  Float tol() const     { return tol_; }
  void  tol(Float t)    { tol_ = t; }

  void  min_x();               // minx = all
  void  min_x(Index, Index[]); // minx = subset

private:

  GSO(const GSO&);
  void operator=(const GSO&);

  void modified_gso(Index r_first, Index r_last,
                    Index c_last, Index r_dim, bool first);

  Mat<Float, Exc> *pA;
  Index M, N;
  Index defect_;
  bool  solved;
  bool  sc;
  Vec<Float, Exc> norm;
  Float tol_;

  Index  minx_n;
  Index *minx;
  Index *clist;
  Index *rlist;

  template <typename T> inline const T ABS(const T& x)
    {
      return (x >= T(0)) ? x : -x ;
    }
};


template <typename Float, typename Exc>
void GSO<Float, Exc>::reset(Mat<Float, Exc>& a, Index m, Index n)
{
  pA = &a;
  M = m;
  N = n;
  defect_ = 0;
  delete[] clist;
  clist = new Index[pA->cols()+1];
  delete[] rlist;
  rlist = new Index[M+1];
  solved = false;
  sc = true;
}

template <typename Float, typename Exc>
void GSO<Float, Exc>::min_x()
{
  minx_n = N;
  delete[] minx;
  minx = new Index[N];
  for (Index i=0; i<N; i++) minx[i] = i+1;
}


template <typename Float, typename Exc>
void GSO<Float, Exc>::min_x(Index N, Index nx[])
{
  minx_n = N;
  delete[] minx;
  minx = new Index[N];
  for (Index i=0; i<N; i++) minx[i] = nx[i];
}


template <typename Float, typename Exc>
void GSO<Float, Exc>::gso1()
{
  if (pA==0)  return;
  if (solved) return;

  for (Index i=1; i<=pA->cols(); i++) clist[i] = i;
  for (Index j=1; j<=M;          j++) rlist[j] = j;
  norm.reset(N);
  defect_ = 0;
  modified_gso(1, pA->rows(), N, M, true);
  for (Index t, l=1, u=N; l<=defect_; l++, u--)
    {
      t = clist[l];
      clist[l] = clist[u];
      clist[u] = t;
    }
  solved = true;
}


template <typename Float, typename Exc>
void GSO<Float, Exc>::gso2()
{
  if (pA==0)  return;
  if (!solved) gso1();
  if (defect() == 0) return;

  for (Index j=1; j<=minx_n; j++) rlist[j] = M+minx[j-1];
  modified_gso(M+1, pA->rows(), defect(), minx_n, false);

  Mat<Float, Exc> &A = *pA;
  for (Index c, column=1; column<=defect(); column++)
    {
      c = clist[column];
      for (Index r=1; r<=A.rows(); r++)
        A(r,c) = 0;
    }
}

template <typename Float, typename Exc>
void GSO<Float, Exc>::modified_gso(Index r_first, Index r_last,
                               Index c_last,  Index r_dim, bool first)
{
  if (tol_ <= 0)
    {
      Float  eps, eps_1, eps_min, eps_max, sum;
      const Float one = 1;

      eps_min = 0;
      eps_max = eps = 1e-5;
      do
        {
          eps_1 = eps;
          eps = (eps_min + eps_max) / 2;
          sum = one + eps;
          if (sum == one)
            eps_min = eps;
          else
            eps_max = eps;
        } while (ABS(eps - eps_1)/eps > 0.1);
      tol_ = std::sqrt(eps);
    }

  Mat<Float, Exc> &A = *pA;
  Float a;

  if (sc)   // initial scaling
    {
      Float s;
      for (Index r, c, k, column=1; column<=c_last; column++)
        {
          c = clist[column];
          s = 0;
          for (k=1; k<=r_dim; k++)
            {
              r = rlist[k];
              a = A(r,c);
              s += a*a;
            }
          using namespace std;
          s = std::sqrt(s);
          if (s)
            for (r=r_first; r<=r_last; r++)
              A(r,c) /= s;
        }
    }

  for (Index r, c, column=1; column<=c_last; column++)
    {
      // column pivoting
      Float maxd = 0;
      Index maxi = column;
      for (Index pivot=column; pivot<=c_last; pivot++)
        {
          c = clist[pivot];
          Float s = 0;
          for (Index k=1; k<=r_dim; k++)
            {
              r = rlist[k];
              a = A(r,c);
              s += a*a;
            }
          if (s > maxd)
            {
              maxd = s;
              maxi = pivot;
            }
        }

      Index t = clist[column];
      clist[column] = clist[maxi];
      clist[maxi] = t;

      c = clist[column];
      maxd = std::sqrt(maxd);
      if (first)
        norm(c) = maxd;

      if (maxd < tol_)
        {
          for (Index d=column; d<=c_last; d++)
            {
              defect_++;
              Index c = clist[d];
              norm(c) = 0;
            }
          return;
        }

      for (Index k=r_first; k<=r_last; k++)
        A(k, c) /= maxd;

      for (Index n, ncol=column+1; ncol<=A.cols(); ncol++)
        {
          n = clist[ncol];
          Float dotp = 0;
          for (Index k=1; k<=r_dim; k++)
            {
              Index i = rlist[k];
              dotp += A(i, c)*A(i, n);
            }

          for (Index m=r_first; m<=r_last; m++)
            A(m, n) -= dotp*A(m, c);
        }
    }
}

}   // namespace GNU_gama

#endif












