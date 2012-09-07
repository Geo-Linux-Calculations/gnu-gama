/*
  C++ Matrix/Vector templates (GNU Gama / matvec)
  Copyright (C) 1999, 2001, 2005, 2007  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gMatVec_MatSVD__h_
#define GNU_gama_gMatVec_MatSVD__h_

#include <cmath>
#include <matvec/matvec.h>


namespace GNU_gama {

  /** \brief Singular Value Decomposition  */

  /*  Singular Value Decomposition          A = U*W*trans(V)
      ############################

     SVD is based on the fortran SVD source from package CMLIB

     C * =====================================================================
     C * NIST Guide to Available Math Software.
     C * Source for module SVD from package CMLIB.
     C * Retrieved from ARNO on Thu Oct 29 08:04:40 1998.
     C * =====================================================================
     SUBROUTINE SVD(NM,M,N,A,W,MATU,U,MATV,V,IERR,RV1)
     C***BEGIN PROLOGUE  SVD
     C***REFER TO  EISDOC
     C
     C     This subroutine is a translation of the ALGOL procedure SVD,
     C     NUM. MATH. 14, 403-420(1970) by Golub and Reinsch.
     C     HANDBOOK FOR AUTO. COMP., VOL II-LINEAR ALGEBRA, 134-151(1971).
     C
     C     This subroutine determines the singular value decomposition
     C          T
     C     A=USV  of a REAL M by N rectangular matrix.  Householder
     C     bidiagonalization and a variant of the QR algorithm are used.

     -------------------------------------------------------------------------

.....2011-04-17  (AC) suppressed g++ optimization (volatile variables)

     2001-02-30  (AC) Occasional problems with SVD convergence:

     Revisions marked with  #_LH_#  are made after the fortran subroutine QRBD
     published in: Solving Least Squares Problems by Charles L. Lawson and
     Richard J. Hanson, 1974 Prentice-Hall, Inc., Englewood Cliffs., N.J.,
     ISBN 0-13-822585-0, pp. 341 (QRBD : App. C, 298--300).


     2002-07-05  (AC) problems with SVD convergence:

     Three tests for convergence had to be rewritten to explicitly
     use a temporary variable s2:

        <    if ((s1 + ABS(rv1[L])) == s1) goto test_for_convergence;
        ---
        >    s2 = s1 + ABS(rv1[L]);
        >    if (s1 == s2) goto test_for_convergence;

        <    if (s1 + (ABS(W[L1])) == s1) break;
        ---
        >    s2 = s1 + ABS(W[L1]);
        >    if (s1 == s2) break;

        <    if (s1 + (ABS(f)) == s1) goto test_for_convergence;
        ---
        >    s2 = s1 + ABS(f);
        >    if (s1 == s2) goto test_for_convergence;

     ----------------------------------------------------------------------- */

  template <typename Float=double, typename Exc=Exception::matvec>
    class SVD {

      public:
      SVD() : m(0), n(0), U_(), W_(), V_(),
      decomposed(0), W_tol(0), inv_W_(),  minx(all),
      list_min(0), minV(), U(0), V(0) {}
      SVD(const Mat<Float, Exc>& A) : m(A.rows()), n(A.cols()), U_(A),
      W_(n), V_(n, n), decomposed(0), W_tol(0), inv_W_(n),
      minx(all), list_min(0), minV(), U(0), V(0) {}
      ~SVD()
      {
        delete[] list_min;
        delete[] U;
        delete[] V;
      }

      Float tol() { return W_tol; }
      Float tol(Float t) { W_tol = t; set_inv_W(); return W_tol; }

      SVD& decompose() { svd(); return *this; }
      SVD& reset(const Mat<Float, Exc>& A);
      SVD& reset(const Mat<Float, Exc>& A, const Vec<Float, Exc>& w);
      SVD& clear();

      Float q_xx(Index, Index); // weight coefficient (xi,xj) of adj. unknowns
      Float q_bb(Index, Index); // weight coefficient (bi,bj) of adj. obs.
      Float q_bx(Index, Index); // weight coefficient (bi,xj)

      Index nullity()       { svd(); return defect; }
      bool  lindep(Index i) { svd(); return inv_W_(i)==0; } // linearly dep.

      void  min_x();               // minx = all
      void  min_x(Index, Index[]); // minx = subset

      const Mat<Float, Exc>& SVD_U() { svd(); return U_; }
      const Vec<Float, Exc>& SVD_W() { svd(); return W_; }
      const Mat<Float, Exc>& SVD_V() { svd(); return V_; }

      void solve(const Vec<Float, Exc>& rhs, Vec<Float, Exc>& x);

      private:
      SVD(const SVD&);
      SVD& operator=(const SVD&);

      Index m, n;                // m = A.rows(), n = A.cols()
      Mat<Float, Exc> U_;
      Vec<Float, Exc> W_;
      Mat<Float, Exc> V_;

      Index decomposed;
      void svd();

      volatile Float W_tol;
      Vec<Float, Exc> inv_W_;
      void set_inv_W();

      enum { all, subset } minx;
      Index defect;
      Index  n_min;
      Index* list_min;
      Mat<Float, Exc> minV;
      void min_subset_x();

      Float*  W;
      Float*  inv_W;
      Float** U;
      Float** V;
      void reset_UWV();

    };      /* class SVD */


  // ------ Exception::Singular Value Decompiosition member functions --------


  template <typename T> inline const T ABS(const T& x)
    {
      return (x >= T(0)) ? x : -x ;
    }

  template <typename Float, typename Exc>
    void SVD<Float, Exc>::set_inv_W()
    {
      if (W_tol == 0)
        {             // if not defined set W_tol to 1000*comp_epsilon
          volatile Float  eps, eps_1, eps_min, eps_max, sum;
          const Float one = 1;

          eps_min = Float();
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
          W_tol = 1000*eps;
        }

      volatile Float vmax = Float();
      for (Index k = 1; k <= W_.dim(); k++) if (W[k] > vmax) vmax = W[k];
      const Float vmin = W_tol * vmax;
      defect = 0;
      for (Index i=1; i<=inv_W_.dim(); i++)
        if (ABS(W[i]) > vmin)
          inv_W[i] = 1/W[i];
        else
          {
            inv_W[i] = Float();
            defect++;
          }

    }      /* void SVD<Float, Exc>::set_inv_W() */


  template <typename Float> inline Float PYTHAG( Float a, Float b )
    {
      volatile Float at, bt, ct;

      return
        (( at = ABS(a) )  > ( bt = ABS(b) ) )     ?
        (       ct = bt/at, at * std::sqrt( (Float)1 + ct * ct ) ) :
        (bt ? ( ct = at/bt, bt * std::sqrt( (Float)1 + ct * ct ) ) : (Float)0);
    }


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::svd()
    {
      if (decomposed)
        return;

      const Float ZERO = 0;
      const Float ONE  = 1;
      const Float TWO  = 2;

      Index  i, i1, its, j, k, k1, L, L1;
      volatile Float  c, f, h, s, x, y, z ;
      volatile Float  s1=ZERO, g=ZERO, scale=ZERO, r, s2;
      volatile Float  tmp1;
      Index  mn;

      reset_UWV();
      Vec<Float, Exc> rv1_(n);
      Float* rv1 = rv1_.begin() - 1;

      /* Householder reduction to bidiagonal form */
      for (i=1; i<=n; i++) {
        L = i+1;
        rv1[i] = scale*g ;
        g = s = scale = ZERO ;
        if (i <= m) {
          for (k=i; k<=m; k++) scale += ABS(U[k][i]);
          if (scale) {
            for (k=i; k<=m; k++) {
              tmp1 = (U[k][i] /= scale);
              s += tmp1*tmp1;
            }
            f = U[i][i];
            g = std::sqrt(s); if (f >= ZERO) g = -g;   // g = -SIGN(sqrt(s),f)
            h = f*g - s;
            U[i][i] = f - g;
            if (i != n)
              {
                for (j=L; j<=n; j++) {
                  s = ZERO;
                  for (k=i; k<=m; k++) s += U[k][i] * U[k][j];
                  f = s/h;
                  for (k=i; k<=m; k++) U[k][j] += f*U[k][i];
                }
              }
            for ( k = i ; k <= m ; k++ ) U[k][i] *= scale ;
          }
        }
        W[i] = scale*g;
        g = s = scale = ZERO;
        if (i <= m && i != n) {
          for (k=L; k<=n; k++) scale += ABS(U[i][k]);
          if (scale) {
            for (k=L; k<=n; k++) {
              tmp1 = (U[i][k] /= scale);
              s += tmp1*tmp1;
            }
            f = U[i][L];
            g = std::sqrt(s); if ( f >= ZERO ) g = -g; // g = -SIGN(sqrt(s),f)
            h = f*g - s;
            U[i][L] = f - g;
            for (k=L; k<=n; k++) rv1[k] = U[i][k] / h;
            if (i != m)
              {
                for (j=L; j<=m; j++) {
                  s = ZERO;
                  for (k=L; k<=n; k++) s += U[j][k] * U[i][k];
                  for ( k = L ; k <= n ; k++ ) U[j][k] += s * rv1[k];
                }
              }
            for (k=L; k<=n; k++) U[i][k] *= scale;
          }
        }
        // s1 = FMAX(s1, (FABS(W[i]+FABS(rv1[i])) );
        r = ABS(W[i]) + ABS(rv1[i]); if (r > s1) s1=r;
      }

      /* Accumulation of right-hand transformations */
      for (i=n; i>=1; i--) {
        if (i != n) {
          if (g) {
            for (j=L; j<=n; j++) // double division avoids possible underflow
              V[j][i] = (U[i][j] / U[i][L]) / g;
            for (j=L; j<=n; j++) {
              s = ZERO;
              for (k=L; k<=n; k++) s += U[i][k] * V[k][j];
              for (k=L; k<=n; k++) V[k][j] += s*V[k][i];
            }
          }
          for (j=L; j<=n; j++) V[i][j] = V[j][i] = ZERO;
        }
        V[i][i] = ONE;
        g = rv1[i];
        L = i;
      }

      /* Accumulation of left-hand transformations */
      mn = (m < n) ? m : n;
      for (i=mn; i>=1; i--) {    // i=IMIN(m,n);
        L = i+1;
        g = W[i];
        if (i != n)
          {
            for (j=L; j<=n; j++) U[i][j]=ZERO;
          }
        if (g)
          {
            if (i != mn)
              {
                for (j=L; j<=n; j++) {
                  s=ZERO;
                  for (k=L; k<=m; k++) s += U[k][i] * U[k][j];
                  f = (s / U[i][i]) / g;
                  for (k=i; k<=m; k++) U[k][j] += f * U[k][i];
                }
              }
            for (j=i; j<=m; j++) U[j][i] /= g;
          }
        else
          {
            for (j=i; j<=m; j++) U[j][i] = ZERO;
          }
        U[i][i] += ONE;
      }

      /* Diagonalization of the bidiagonal form */

      /* test for splitting */
      for (k=n; k>=1; k--)
        {
          k1  = k - 1;
          its = 0;

          /* test for splitting */
          for (;;)     // label 520 in fortran source
            {
              for (L=k; L>=1; L--)
                {
                  s2 = s1 + ABS(rv1[L]);
                  if (s1 == s2) goto test_for_convergence;

                  /* rv[1] is always zero, so there is no exit
                   * through the bottom of the loop */
                  L1 = L - 1;
                  s2 = s1 + ABS(W[L1]);
                  if (s1 == s2) break;
                }

              /* cancellation of rv1[L], if L greater then 1 */
              c = ZERO;
              s = ONE;
              for (i=L; i<=k; i++)
                {
                  f = s * rv1[i];
                  rv1[i] = c * rv1[i];
                  s2 = s1 + ABS(f);
                  if (s1 == s2) goto test_for_convergence;
                  g = W[i];
                  h = PYTHAG(f,g);
                  W[i] = h;
                  c =  g / h;
                  s = -f / h;
                  for (j=1; j<=m; j++)
                    {
                      y = U[j][L1];
                      z = U[j][i];
                      U[j][L1] =  y*c + z*s;
                      U[j][i]  = -y*s + z*c;
                    }
                }

            test_for_convergence:

              z = W[k];
              if (L == k) {
                /* W[k] is made nonnegative */
                if (z < ZERO)
                  {
                    W[k] = -z;
                    for (j=1; j<=n; j++) V[j][k] = -V[j][k];
                  }
                break;   /******   leaving for (;;) loop here   ******/
              }

              /* shift from bottom 2 by 2 minor */

              if ( its++ == 30)
                throw Exc(Exception::NoConvergence, "No convergence in SVD");

              x = W[L];
              y = W[k1];
              g = rv1[k1];
              h = rv1[k];
              // f = (Float)0.5*( ((g + z)/h)*((g - z)/y) + y/h -h/y);
              f = ((y-z)*(y+z)+(g-h)*(g+h))/(TWO*h*y);              // #_LH_#
              g = PYTHAG(f,ONE);
              s = (f >= ZERO) ? g : - g;         // SIGN(g,f)
              // f = x - (z/x)*z + (h/x)*(y/(f+s) - h);
              f = ((x-z)*(x+z) + h*(y/(f+s) - h))/x;                // #_LH_#

              /* next QR transformation */
              c = s = ONE;
              for (i1=L; i1<=k1; i1++) {
                i = i1 + 1;
                g = rv1[i];
                y = W[i];
                h = s*g;
                g = c*g;
                z = PYTHAG(f,h);
                rv1[i1] = z;
                c = f/z;
                s = h/z;
                f =  x*c + g*s;
                g = -x*s + g*c;
                h = y*s;
                y = y*c;
                for (j=1; j<=n; j++) {
                  x = V[j][i1];
                  z = V[j][i];
                  V[j][i1] =  x*c + z*s;
                  V[j][i]  = -x*s + z*c;
                }
                z = PYTHAG(f,h);
                W[i1] = z;

                /* rotation can be arbitrary if z is zero */
                if (z) {
                  c = f/z;
                  s = h/z;
                }
                f =  c*g + s*y;
                x = -s*g + c*y;
                for (j=1; j<=m; j++) {
                  y = U[j][i1];
                  z = U[j][i];
                  U[j][i1] =  y*c + z*s;
                  U[j][i]  = -y*s + z*c;
                }
              }
              rv1[L] = ZERO;
              rv1[k] = f;
              W[k] = x;
            }   // for (;;)

        }   // for k

      decomposed = 1;
      set_inv_W();
      if (defect > 0)
        {
          minV = V_;
          if (minx == subset) min_subset_x();
        }

    }      /* void SVD<Float, Exc>::svd() */


  template <typename Float, typename Exc> SVD<Float, Exc>&
    SVD<Float, Exc>::reset(const Mat<Float, Exc>& A)
    {
      const Index R = A.rows();
      const Index C = A.cols();
      if (R != U_.rows() || C != U_.cols()) {
        m = R;
        n = C;
        W_.reset(C);
        V_.reset(C, C);
        inv_W_.reset(C);
      }
      U_ = A;
      reset_UWV();

      decomposed = 0;

      return *this;
    }      /* SVD& reset(const Mat<Float, Exc>& A) */


  template <typename Float, typename Exc> SVD<Float, Exc>&
    SVD<Float, Exc>::reset(const Mat<Float, Exc>& A,
                           const Vec<Float, Exc>& w)
    {
      reset(A);
      volatile Float wi;
      for (Index i = 1; i <= A.rows(); i++)
        {
          wi = w(i);
          for (Index j = 1; j <= A.cols(); j++) U[i][j] *= wi;
        }
      return *this;
    }


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::reset_UWV()
    {
      delete[] U;
      delete[] V;

      W = W_.begin() - 1;             // w[i] == W_(i)
      inv_W = inv_W_.begin() - 1;
      U = new Float*[m+1];
      U[1] = U_.begin() - 1;          // U[i][j] == U_(i,j)
      for (Index i=2; i<=m; i++)
        U[i] = U[i-1] + n;
      V = new Float*[n+1];
      V[1] = V_.begin() - 1;          // V[i][j] == V_(i,j)
      {   // for ...
        for (Index i=2; i<=n; i++)
          V[i] = V[i-1] + n;
      }   // for ...

    }      /* void SVD<Float, Exc>::reset_UWV() */


  template <typename Float, typename Exc>
    SVD<Float, Exc>& SVD<Float, Exc>::clear()
    {
      m = 0;
      n = 0;
      W_.reset();
      V_.reset();
      inv_W_.reset();
      U_.reset();
      minV.reset();

      delete[] U;
      delete[] V;
      U = V = 0;

      decomposed = 0;

      return *this;
    }      /* SVD& SVD<Float, Exc>::clear() */


  template <typename Float, typename Exc>
    Float SVD<Float, Exc>::q_xx(Index i, Index j)
    {
      if (!(1 <= i && i <= n && 1 <= j && j <= n))
        throw Exc(Exception::BadRank, "Float SVD::q_xx(Index, Index)");
      // A = U*W*trans(V)
      // Covariance = V * inv_W * trans(inv_W) * trans(V);
      if (!decomposed) svd();
      volatile Float c = Float();
      for (Index k = 1; k <= n; k++)
        c += V[i][k] * inv_W[k] * inv_W[k] * V[j][k];
      return c;
    }


  template <typename Float, typename Exc>
    Float SVD<Float, Exc>::q_bb(Index i, Index j)
    {
      if (!(1 <= i && i <= m && 1 <= j && j <= m))
        throw Exc(Exception::BadRank, "Float SVD::q_bb(Index, Index)");
      // A = U*W*trans(V)
      // Covariance = U * trans(U);
      if (!decomposed) svd();
      volatile Float c = Float();
      for (Index k = 1; k <= n; k++)
        if (inv_W[k] != 0)
          c += U[i][k] * U[j][k];
      return c;
    }


  template <typename Float, typename Exc>
    Float SVD<Float, Exc>::q_bx(Index i, Index j)
    {
      if (!(1 <= i && i <= m && 1 <= j && j <= n))
        throw Exc(Exception::BadRank, "Float SVD::q_bx(Index, Index)");
      // A = U*W*trans(V)
      // Covariance = U * trans(V);
      if (!decomposed) svd();
      volatile Float c = Float();
      for (Index k = 1; k <= n; k++)
        c += U[i][k] * inv_W[k] * V[j][k];
      return c;
    }


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::min_x()
    {
      if (decomposed && minx != all)
        {
          V_ = minV;
          delete[] V;
          V = new Float*[n+1];
          V[1] = V_.begin() - 1;        // V[i][j] == V_(i,j)
          for (Index i=2; i<=n; i++)
            V[i] = V[i-1] + n;
        }
      minx = all;
    }


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::min_x(Index n, Index list[])
    {
      minx = subset;
      if (list_min != 0) delete[] list_min;
      n_min = n;
      list_min = new Index[n_min];
      for (Index i = 0; i < n; i++)
        list_min[i] = list[i];

      if (decomposed && defect != 0) {
        V_ =  minV;
        delete[] V;
        V = new Float*[n+1];
        V[1] = V_.begin() - 1;        // V[i][j] == V_(i,j)
        for (Index i=2; i<=n; i++)
          V[i] = V[i-1] + n;
        min_subset_x();
      }
    }      /* void SVD<Float, Exc>::min_x(Index n, Index list[]) */


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::min_subset_x()
    {
      using namespace std;

      if (defect == 0) return;
      if (defect > n_min)
        throw Exc(Exception::BadRegularization, "void SVD::min_subset_x()");

      Index im;
      volatile Float s;
      for (Index k = 1; k <= n; k++)
        if (inv_W[k] == 0)
          {
            volatile Float Vimk;
            s = Float();
            for (Index i = 0; i < n_min; i++) {
              im = list_min[i];
              Vimk = V[im][k];
              s += Vimk*Vimk;
            }
            s = std::sqrt(s);
            if (s == 0)
              throw Exc(Exception::BadRegularization,
                        "void SVD::min_subset_x()");
            { for (Index i = 1; i <= n; i++) V[i][k] /= s; }   // for ...

            for (Index j = 1; j <= n; j++)
              if (j != k)
                {
                  s = Float();
                  for (Index i = 0; i < n_min; i++) {
                    im = list_min[i];
                    s += V[im][j] * V[im][k];
                  }
                  { for (Index i = 1; i <= n; i++) V[i][j] -= s * V[i][k]; }
                }
          }
    }      /* void SVD<Float, Exc>::min_subset_x() */


  template <typename Float, typename Exc>
    void SVD<Float, Exc>::solve(const Vec<Float, Exc>& rhs,
                                Vec<Float, Exc>& x_)
    {
      svd();
      if (n != x_.dim()) x_.reset(n);

      Vec<Float, Exc> t_(n);
      typename Vec<Float, Exc>::const_iterator b = rhs.begin();
      typename Vec<Float, Exc>::iterator t = t_.begin();
      volatile Float s;

      // t = trans(U)*b*inv(W);
      {   // for ...
        for (Index i=1; i<=n; ++i, ++t)
          {
            s = Float();
            b = rhs.begin();
            for (Index k=1; k<=m; k++, ++b) s += U[k][i] * (*b);
            *t = s * inv_W[i];
          }
      }   // for ...

      // x = V*t;
      typename Vec<Float, Exc>::iterator x = x_.begin();
      for (Index i=1; i<=n; ++i, ++x)
        {
          s = Float();
          t = t_.begin();
          for (Index j=1; j<=n; ++j, ++t) s += V[i][j] * (*t);
          *x = s;
        }
    }      /* void solve(const VecVec& rhs, Vec& x_) */



}   // namespace GNU_gama

#endif
