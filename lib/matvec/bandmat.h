/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 1999, 2007, 2012  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gMatVec_Symmetric_Band_Matrix__H_
#define GNU_gama_gMatVec_Symmetric_Band_Matrix__H_

#include <matvec/matvec.h>
#include <matvec/choldec.h>

/*
 * Symmetric Band Matrix
 * =====================
 *
 * Bandwidth is defined as max{ |i-j| | a_ij != 0 }
 *
 * Upper triangular part of the matrix is stored in `diagonal storage scheme'
 *
 */


namespace GNU_gama {   /** \brief Symmetric Band Matrix */


template <typename Float=double, typename Exc=Exception::matvec>
class BandMat : public MatBase<Float, Exc>, public CholDecLD<Float, Exc> {
public:

   BandMat() : band_(0) {}
   BandMat(Index d, Index b)
     : MatBase<Float, Exc>(d,d,d*(b+1)), band_(b) {}

   void    reset() { this->row_ = this->col_ = band_ = 0; this->resize(0); }
   void    reset(Index d, Index b);
   Index   dim() const { return this->row_; }
   Index   bandWidth() const { return band_; }
   Float   operator()(Index, Index) const;
   Float&  operator()(Index, Index);
   Float*  operator[](Index row) { return this->begin() + --row*(band_+1); }
   void    cholDec();
   void    solve(Vec<Float, Exc>&) const;
   void    invBand(BandMat&, Index=0) const;
   Vec<Float, Exc>
           operator*(const Vec<Float, Exc>&) const;
   void    triDiag();
   void    eigenVal(Vec<Float, Exc>&);

   std::istream& read (std::istream&);
   std::ostream& write(std::ostream&) const;

private:

   Index   band_;

   // lower part of symmetric band matrix - internal function for triDiag()
   Float  out_of_band;
   Float *addr_m_;
   Float *addr(Index r, Index s)
     {
       r -= s;

       if (r > band_)
         return &out_of_band;

       return addr_m_ + (--s*(band_+1) + r);
     }

   Float  absa, absb, absq;
   Float  pythag(Float  a, Float  b)
     {
       absa = this->Abs(a);
       absb = this->Abs(b);
       if (absa > absb)
         {
           absq = absb/absa;
           return absa*std::sqrt(Float(1.0) + absq*absq);
         }
       else if (absb)
         {
           absq = absa/absb;
           return absb*std::sqrt(Float(1.0) + absq*absq);
         }

       return Float(0.0);
     }

};      /* class BandMat */


template <typename Float, typename Exc>
void BandMat<Float, Exc>::reset(Index d, Index b)
{
  if (dim() != d || band_ != b)
    {
      this->row_ = this->col_ = d;
      band_ = b;
      this->resize(d*(b+1));
    }
}


template <typename Float, typename Exc>
Float  BandMat<Float, Exc>::operator()(Index r, Index s) const
{
   if (r > s) {
      Index t = r;
      r = s;
      s = t;
   }

   if (s > r+band_)
      return 0;

   const Float *m = this->begin();
   s -= r;
   return m[--r*(band_+1) + s];
}

template <typename Float, typename Exc>
Float& BandMat<Float, Exc>::operator()(Index r, Index s)
{
   if (r > s) {
      Index t = r;
      r = s;
      s = t;
   }

   if (s > r+band_)
      throw Exc(Exception::BadIndex, "Float& BandMat::operator()(Index r, Index s)");

   Float *m = this->begin();
   s -= r;
   return m[--r*(band_+1) + s];
}

template <typename Float, typename Exc>
void BandMat<Float, Exc>::cholDec()
{
  /*
   * Cholesky factorization of positive definite matrix A = L*D*trans(L)
   *
   * L is lower triangular matrix with unity diagonal; D is diagonal matrix.
   * Matrices L and D replace factored band symmetric matrix `in situ'.
   */
   if (dim() == 0)
     throw Exc(Exception::BadRank, "BandMat::cholDec(Float  tol) - zero dim matrix");

   Float *b = this->begin();
   Float *p;
   const Float  Tol = this->Abs(*b*this->cholTol());
   const Index bw1 = band_ + 1;
   Index i, k, l, m, n;
   Float   q, b0;

   for (i=1; i<dim(); i++)
     {
        b0 = b[0];
        if(Tol >= b0)
          throw Exc(Exception::NonPositiveDefinite, "BandMat::cholDec(Float  tol) - "
                                         "Matrix is not positive definite");

        k = band_;
        if(k+i > dim()) k = dim() - i;
        p = b;
        for (n=1; n<=k; n++)
	{
            p += bw1;
            q = b[n]/b0;
	    for (m=0, l=n; m<=band_-n; m++, l++) p[m] -= q*b[l];
	}

        for (m=1; m<=k; m++) b[m] /= b0;
        b += bw1;
     }

     b0 = b[0];
     if(Tol >= b0)
       throw Exc(Exception::NonPositiveDefinite, "BandMat::cholDec(Float  tol) - "
                                      "Matrix is not positive definite");
}

template <typename Float, typename Exc>
void BandMat<Float, Exc>::solve(Vec<Float, Exc>& rhs) const
{
   const Index bw1 = band_ + 1;
   Index i, j, k, l;
   Float s;
   const Float *b, *m;

   // forward substitution
   m = b = this->begin();
   for (i=2; i<=dim(); i++, b += bw1)
   {
     l = i > band_ ? i-band_ : 1;   // l is unsigned
     for (s=0, j=l; j<=i-1; j++)
       s += *( m + (j-1)*bw1+ i - j)*rhs(j);
     rhs(i) -= s;
   }

   // inverse of diagonal
   b = this->begin();
   for (i=1; i<=dim(); i++, b += bw1)
      rhs(i) /= *b;

   // backward substituiton
   b = m + (dim()-2)*bw1;
   for (i=dim()-1; i>0; i--, b -= bw1)
   {
      for (s=0, k=i+1, j=1; j<=band_ && k<=dim(); k++, j++)
         s += rhs(k) * b[j];
      rhs(i) -= s;
   }
}


template <typename Float, typename Exc>
void BandMat<Float, Exc>::invBand(BandMat& Z, Index pbw) const
{
  /*
   * Band subset of inverse matrix. Prior to invBand() must be called
   * function cholDec(). Bandwidth of Z may be explicitly set grater
   * then bandwidth of matrix *this with optional parametr pbw.
   *
   *   BandMat A, Ainv;
   *   cin >> A;
   *   A.cholDec();
   *   A.invBand(Ainv, A.bandWidth()+1);
   */


  if (pbw < bandWidth())
    pbw = bandWidth();
  if (dim() != Z.dim() || Z.bandWidth() != pbw)
    Z.reset(dim(), pbw);

  const Index zbw1 = Z.bandWidth() + 1;
  const Index bw1 = bandWidth() + 1;
  Index i, j, k, l, n, ij;
  const Float *b;
  Float *z, *zm = Z.begin();
  Float s, q;

  z=zm + (dim()-1)*zbw1;
  for (i=dim(); i>0; i--, z -= zbw1)
    {
      l = Z.bandWidth();
      if(l+i > Z.dim()) l = Z.dim()-i;
      b = this->begin() + (i-1)*bw1;
      for (j=l; j>0; j--)
        {
          for (s=0, k=1, n=i+1; n<=Z.dim() && k<=band_; n++, k++)
            {
              // q = Z(n,i+j)
              ij = i + j;
              if (n <= ij)
                q = zm[(n -1)*zbw1 + ij - n];
              else
                q = zm[(ij-1)*zbw1 + n - ij];
              s -= b[k] * q;
            }
          z[j] = s;
        }

      b = this->begin();
      *z = 1/b[(i-1)*bw1];
      l = band_;
      if (i+l > dim()) l = dim()-i;
      b = this->begin() + (i-1)*bw1;
      for (s=0, j=1; j<=l; j++)
        {
          s += z[j]*b[j];
        }
      *z -= s;
    }
}


template <typename Float, typename Exc>
Vec<Float, Exc>
BandMat<Float, Exc>::operator*(const Vec<Float, Exc>& v) const
{
   Vec<Float, Exc> T(dim());
   const Index bw1 = band_ + 1;
   Index i, j, ij, k;
   const Float *b;
   const Float *p;
   Float s;

   for (b=this->begin(), i=1; i<=dim(); i++, b+=bw1)
   {
      s = 0;

      k = (i > band_) ? i - band_ : 1;
      for (p=this->begin()+(k-1)*bw1+i-k, j=k; j<i; j++, p+=band_)
         s += *p * v(j);
      for (ij=i, j=0; j<=band_ && ij<=dim(); j++, ij++)
         s += b[j] * v(ij);

      T(i) = s;
   }

   return T;
}


template <typename Float, typename Exc>
std::istream& BandMat<Float, Exc>::read(std::istream& inp)
{
   int inpd, inpb;
   inp >> inpd >> inpb;
   reset(inpd, inpb);

   Float  *b = this->begin();
   for (Index i=1; i<=dim(); i++)
      for (Index j=i; j<=i+band_; j++)
         if (j <= dim())
	    inp >> *b++;
         else
            *b++ = 0;

   return inp;
}


template <typename Float, typename Exc>
std::ostream& BandMat<Float, Exc>::write(std::ostream& out) const
{
   int w = out.width();
   out.width(w);
   out << dim() << ' ';
   out.width(w);
   out << band_ << "\n\n";

   const Float  *b = this->begin();
   for (Index i=1; i<=dim(); i++, out << '\n')
     for (Index j=i; j<=i+band_; j++)
       if (j <= dim()) {
         out.width(w);
         out << *b++ << ' ';
       }
       else
         b++;

   return out;
}


template <typename Float, typename Exc>
void BandMat<Float, Exc>::triDiag()
{
  Float *a, *b, *z, d, s, c, u, v, w, s2, c2, sc;
  Index band, diag, row, row1, col;

  // init internal inline function addr(Index, Index)
  out_of_band = 0;
  addr_m_ = this->begin();

  for (band=band_; band>1; band--)
    for (diag=1; diag<=dim()-band; diag++)
      {
        row = diag + band;
        col = diag;
        do {
          row1 = row - 1;
          a = addr(row1, col);
          b = addr(row,  col);
          d = pythag(*a, *b);
          if (d)
            {
              c = *a / d;
              s = *b / d;
            }
          else
            {
              c = 1;
              s = 0;
            }
          u = *a*c  + *b*s;
          *a = u;
          *b = 0;

          for (Index k=col+1; k<row1; k++)
            {
              a = addr(row1, k);
              b = addr(row,  k);
              u = *a*c  + *b*s;
              v = *a*-s + *b*c;
              *a = u;
              *b = v;
            }

          a = addr(row1, row1);
          z = addr(row, row1);
          b = addr(row, row);
          s2 = s*s;
          c2 = c*c;
          sc = s*c;
          u = c2**a + s2**b + 2*sc**z;
          v = s2**a + c2**b - 2*sc**z;
          w = c2**z - s2**z +sc**b - sc**a;
          *a = u;
          *b = v;
          *z = w;

          for (Index k=row+1; k<=row+band && k<=dim(); k++)
            {
              a = addr(k, row1);
              b = addr(k, row);
              u = *a*c  + *b*s;
              v = *a*-s + *b*c;
              *a = u;
              *b = v;
            }

          col = row1;
          row += band;
        } while (row <= dim());
      }
}


template <typename Float, typename Exc>
void BandMat<Float, Exc>::eigenVal(Vec<Float, Exc>& eigvals)
{
  triDiag();

  eigvals.reset(dim());
  Vec<Float, Exc> offdiag(dim());
  {
    const Float *d = this->begin();
    for(Index i=1; i<=dim(); i++)
      {
        eigvals(i) = *d++;
        offdiag(i) = *d;
        d += band_;
      }
  }

  /*************************************************************************
  The following text is based on the source of IMTQL1 in EISPACK from NETLIB

  * ======================================================================
  * NIST Guide to Available Math Software.
  * Source for module IMTQL1 from package EISPACK.
  * Retrieved from NETLIB on Thu Oct 29 06:45:00 1998.
  * ======================================================================
      subroutine imtql1(n,d,e,ierr)
  c
      integer i,j,l,m,n,ii,mml,ierr
      double precision d(n),e(n)
      double precision b,c,f,g,p,r,s,tst1,tst2,pythag
  c
  c     this subroutine is a translation of the algol procedure imtql1,
  c     num. math. 12, 377-383(1968) by martin and wilkinson,
  c     as modified in num. math. 15, 450(1970) by dubrulle.
  c     handbook for auto. comp., vol.ii-linear algebra, 241-248(1971).
  c
  c     this subroutine finds the eigenvalues of a symmetric
  c     tridiagonal matrix by the implicit ql method.
  c
  c     on input
  c
  c        n is the order of the matrix.
  c
  c        d contains the diagonal elements of the input matrix.
  c
  c        e contains the subdiagonal elements of the input matrix
  c          in its last n-1 positions.  e(1) is arbitrary.
  c
  c      on output
  c
  c        d contains the eigenvalues in ascending order.  if an
  c          error exit is made, the eigenvalues are correct and
  c          ordered for indices 1,2,...ierr-1, but may not be
  c          the smallest eigenvalues.
  c
  c        e has been destroyed.
  c
  c        ierr is set to
  c          zero       for normal return,
  c          j          if the j-th eigenvalue has not been
  c                     determined after 30 iterations.
  c
  c     calls pythag for  dsqrt(a*a + b*b) .
  c
  c     questions and comments should be directed to burton s. garbow,
  c     mathematics and computer science div, argonne national laboratory
  c
  c     this version dated august 1983.
  c
  **************************************************************************/

  Float *d  = eigvals.begin() - 1;     // 1 based indices
  Float *e  = offdiag.begin() - 1;
  const Index n = dim();

  Index  m, l, j, i;
  Float  s, r, p, g, f, c, b, tst1, tst2;

  // e[n] = 0;   ... not needed

  for (l=1; l<=n; l++)
    {
      j = 0;

    next_iteration:

      // look for small sub-diagonal element

      for (m=l; m<=n; m++)
        {
          if (m == n) break;
          tst1 = this->Abs(d[m])+this->Abs(d[m+1]);
          tst2 = tst1 + this->Abs(e[m]);
          if (tst1 == tst2) break;
        }
      p = d[l];
      if (m == l)
        {
          // we do not order eigenvalues - ordering loop omitted
          continue;
        }

      if (j++ == 30)
        throw Exc(Exception::NoConvergence, "void BandMat::eigenVal(Vec& eigvals) - "
                  "No convergence to an eigenvalue after 30 iterations");

      // form shift

      g = (d[l+1] - p)/(Float(2.0)*e[l]);
      r = pythag(g,Float(1.0));
      g = d[m] - p + e[l]/(g+this->Sign(r,g));
      s = c = Float(1.0);
      p = Float(0.0);
      for (i=m-1; i>=l; i--)
        {
          f = s*e[i];
          b = c*e[i];
          r = pythag(f,g);
          e[i+1] = r;

          if (r == Float(0.0))
            {
              // recover from underflow

              d[i+1] -= p;
              e[m] = Float(0.0);
              goto next_iteration;
            }

          s = f/r;
          c = g/r;
          g = d[i+1] - p;
          r = (d[i] - g)*s + Float(2.0)*c*b;
          p = s * r;
          d[i+1] = g + p;
          g = c*r - b;
        }
      d[l] -= p;
      e[l] = g;
      e[m] = Float(0.0);
      goto next_iteration;
    }

}


}      //  namespace GNU_gama

#endif



