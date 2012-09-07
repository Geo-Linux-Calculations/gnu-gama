/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 1999, 2006, 2007, 2012  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gMatVec_SymMat__h_
#define GNU_gama_gMatVec_SymMat__h_

#include <matvec/matvec.h>
#include <matvec/choldec.h>


namespace GNU_gama {   /** \brief Symmetrix matrix */

template <typename Float=double,  typename Exc=Exception::matvec>
class SymMat : public MatBase<Float, Exc>, public CholDec<Float, Exc> {

  Index dim_;
  Index idf_;

public:

  typedef typename MatBase<Float, Exc>::iterator       iterator;
  typedef typename MatBase<Float, Exc>::const_iterator const_iterator;

  explicit SymMat(Index d=0) : MatBase<Float, Exc>(d, d, d*(d+1)/2),
    dim_(d), idf_(0) {}
  SymMat(Index r, Index c) : MatBase<Float, Exc>(r, c, r*(r+1)/2),
    dim_(r), idf_(0)
    {
      if (r != c) throw Exc(Exception::BadRank,
                            "SymMat::SymMat(Index, Index, Init)");
    }

  Index dim()  const { return dim_; }
  Index nullity() const { return idf_; }

  void cholDec();
  void solve(Vec<Float, Exc> &rhs) const;

  Float  operator()(Index i, Index j) const
    {
      const Float *p = this->begin();
      return i>=j ? p[i*(i-1)/2+j-1] : p[j*(j-1)/2+i-1];
    }
  Float& operator()(Index i, Index j)
    {
      Float *p = this->begin();
      return i>=j ? p[i*(i-1)/2+j-1] : p[j*(j-1)/2+i-1];
    }
  void reset(Index r, Index c)
    {
      if (r != c || r < 0)
        throw Exc(Exception::BadRank, "SymMat::reset(Index, Index)");
      dim_ = this->row_ = this->col_ = r;
      this->resize(r*(r+1)/2);
    }
  void reset(Index d)
    {
      if (d < 0)
        throw Exc(Exception::BadRank, "SymMat::reset(Index)");
      reset(d, d);
    }

  void invert();

  SymMat operator*(Float f) const {
    SymMat t(dim()); this->mul(f, t); return t;
  }
  SymMat operator+(const SymMat& M) const {
    if (dim() != M.dim())
      throw Exc(Exception::BadRank, "SymMat::operator+(const SymMat&) const");
     SymMat T(dim());
     this->add(M, T);
     return T;
  }
  SymMat operator-(const SymMat& M) const {
    if (dim() != M.dim())
      throw Exc(Exception::BadRank, "SymMat::operator-(const SymMat&) const");
     SymMat T(dim());
     this->sub(M, T);
     return T;
  }

  typename MatVecBase<Float, Exc>::ListInitialiser operator=(Float x)
  {
    return this->list_init(x);
  }

};       // template <Float, Exc> class SymMat;

// ======================================================================

template <typename Float, typename Exc>
std::ostream& operator<<(std::ostream& out, const SymMat<Float, Exc>& S)
  {
    const Index fw = out.width();
    out.width(fw);
    out << S.dim() << "\n\n";
    for (Index i=1; i<=S.dim(); i++)
      {
        for (Index j=1; j<=i; j++) {
          out.width(fw);
          out << S(i,j) << " ";
        }
        out << '\n';
      }
    return out;
  }

template <typename Float, typename Exc>
std::istream& operator>>(std::istream& inp, SymMat<Float, Exc>& S)
  {
    Index n;
    inp >> n;
    S.reset(n);

    for (Index i=1; i<=S.dim(); i++)
        for (Index j=1; j<=i; j++)
          inp >> S(i,j);

    return inp;
  }

// ======================================================================

template <typename Float, typename Exc>
SymMat<Float, Exc>
operator+(const SymMat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.dim() != B.dim())
      throw Exc(Exception::BadRank, "operator+(const SymMat&, const SymMat&)");

    typename SymMat<Float, Exc>::const_iterator a = A.begin();
    typename SymMat<Float, Exc>::const_iterator b = B.begin();
    typename SymMat<Float, Exc>::const_iterator e = A.end();
    SymMat<Float, Exc> M(A.dim());
    typename SymMat<Float, Exc>::iterator m = M.begin();

    while (a != e) *m++ = *a++ + *b++;

    return M;
  }

template <typename Float, typename Exc>
SymMat<Float, Exc>
operator-(const SymMat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.dim() != B.dim())
      throw Exc(Exception::BadRank, "operator-(const SymMat&, const SymMat&)");

    typename SymMat<Float, Exc>::const_iterator a = A.begin();
    typename SymMat<Float, Exc>::const_iterator b = B.begin();
    typename SymMat<Float, Exc>::const_iterator e = A.end();
    SymMat<Float, Exc> M(A.dim());
    typename SymMat<Float, Exc>::iterator m = M.begin();

    while (a != e) *m++ = *a++ - *b++;

    return M;
  }

template <typename Float, typename Exc>
SymMat<Float, Exc>&
operator+=(SymMat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.dim() != B.dim())
      throw Exc(Exception::BadRank, "operator+=(const SymMat&, const SymMat&)");

    typename SymMat<Float, Exc>::iterator a = A.begin();
    typename SymMat<Float, Exc>::iterator e = A.end();
    typename SymMat<Float, Exc>::const_iterator b = B.begin();

    while (a != e) *a++ += *b++;

    return A;
  }

template <typename Float, typename Exc>
SymMat<Float, Exc>&
operator-=(SymMat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.dim() != B.dim())
      throw Exc(Exception::BadRank, "operator-=(const SymMat&, const SymMat&)");

    typename SymMat<Float, Exc>::iterator a = A.begin();
    typename SymMat<Float, Exc>::iterator e = A.end();
    typename SymMat<Float, Exc>::const_iterator b = B.begin();

    while (a != e) *a++ -= *b++;

    return A;
  }

template <typename Float, typename Exc>
inline
SymMat<Float, Exc> operator*(Float d, const SymMat<Float, Exc>& A)
  {
    return A*d;
  }

// ======================================================================

template <typename Float, typename Exc>
Mat<Float, Exc>
Square(const SymMat<Float, Exc>& A)
  {
    Mat<Float, Exc> M(A.dim(), A.dim());
    typename Mat<Float, Exc>::const_iterator m = A.begin();

    for (Index i=1; i<=A.dim(); i++)
      for (Index j=1; j<=i; j++)
        M(i,j) = M(j,i) = *m++;

    return M;
  }

template <typename Float, typename Exc>
Mat<Float, Exc>
Lower(const SymMat<Float, Exc>& A)
  {
    Mat<Float, Exc> M(A.dim(), A.dim());
    typename Mat<Float, Exc>::const_iterator m = A.begin();

    for (Index i=1; i<=A.dim(); i++)
      for (Index j=1; j<=i; j++)
        {
          M(j,i) = 0;
          M(i,j) = *m++;
        }

    return M;
  }

template <typename Float, typename Exc>
Mat<Float, Exc>
Upper(const SymMat<Float, Exc>& A)
  {
    Mat<Float, Exc> M(A.dim(), A.dim());
    typename Mat<Float, Exc>::const_iterator m = A.begin();

    for (Index i=1; i<=A.dim(); i++)
      for (Index j=1; j<=i; j++)
        {
          M(i,j) = 0;
          M(j,i) = *m++;
        }

    return M;
  }

template <typename Float, typename Exc>
SymMat<Float, Exc>
Lower(const Mat<Float, Exc>& A)
  {
    if (A.rows() != A.cols())
      throw Exc(Exception::BadRank, "SymMat<Float, Exc> Lower(const Mat<Float, Exc>& A)");

    SymMat<Float, Exc> M(A.rows());
    typename SymMat<Float, Exc>::iterator m = M.begin();

    for (Index i=1; i<=M.dim(); i++)
      for (Index j=1; j<=i; j++)
          *m++ = A(i,j);

    return M;
  }

template <typename Float, typename Exc>
SymMat<Float, Exc>
Upper(const Mat<Float, Exc>& A)
  {
    if (A.rows() != A.cols())
      throw Exc(Exception::BadRank,
                "SymMat<Float, Exc> Upper(const Mat<Float, Exc>& A)");

    SymMat<Float, Exc> M(A.rows());
    typename SymMat<Float, Exc>::iterator m = M.begin();

    for (Index i=1; i<=M.dim(); i++)
      for (Index j=1; j<=i; j++)
          *m++ = A(j,i);

    return M;
  }

// ======================================================================

// Cholesky decomposition of positive definite matrix A

template <typename Float, typename Exc>
void SymMat<Float, Exc>::cholDec()
  {
    idf_ = 0;
    const Index n = dim();
    Float  x, diag;
    Float* a = this->begin() - 1;

    Index i, ip, iq, ir, j, k;
    ip  = 0;
    for (i=1; i<=n; i++)
      {
        iq = ip + 1;
        ir = 0;
        for (j=1; j<=i; j++)
          {
            x = a[ip+1];
            diag = a[ip+1];
            for (k=iq; k<=ip; k++)
              {
                ir++;
                x -= a[k]*a[ir];
              }
            ir++;
            ip++;
            if (i != j)
              {
                if (a[ir])
                  a[ip] = x/a[ir];
                else
                  a[ip] = 0;
              }
            else if (x > diag*this->cholTol())
              {
                if (x < 0)
                  throw Exc(Exception::BadRank, "void SymMat::cholDec()");
                a[ip] = std::sqrt(x);
              }
            else
              {
                a[ip] = 0;
                idf_++;
              }
          }
      }

  }

template <typename Float, typename Exc>
void SymMat<Float, Exc>::solve(Vec<Float, Exc>& rhs) const
  {
    const_iterator a = this->begin();
    typename Vec<Float, Exc>::iterator b;
    const Index N = dim();
    Float sum;
    Index i, j;

    // forward substitution
    for (i=1; i<=N; i++)
      {
        b = rhs.begin();
        sum = 0;
        for (j=1; j<i; j++)
          sum += *a++ * *b++;
        *b   -= sum;
        *b++ /= *a++;
      }

    // backward substitution
    a = this->begin();
    for (i=N; i>=1; i--)
      {
        b = rhs.end();
        sum = 0;
        for (j=N; j>i; j--)
          sum += a[j*(j-1)/2+i-1] * *(--b);
        --b;
        *b -= sum;
        *b /= a[i*(i-1)/2+i-1];
      }
  }

// ======================================================================

// Inverse of positive definite matrix A

template <typename Float, typename Exc>
void SymMat<Float, Exc>::invert()
  {
    const Index n = dim();
    iterator a = this->begin()-1;
    Vec<Float, Exc> w(n);
    Float p, q;
    Index i, ii, ij, k, m;

    if (n == 1)
      {
        if (a[1] < 0)
          throw Exc(Exception::BadRank, "SymMat<Float, Exc>::invert()");
        a[1] = 1/a[1];
        return;
      }

    for (k=n; k>=1; k--)
      {
        p = a[1];
        if (p < 0)
          throw Exc(Exception::BadRank,
                    "SymMat<Float, Exc>& inv(SymMat<Float, Exc>& A)");

        ii = 1;
        for (i=2; i<=n; i++)
          {
            m   = ii;
            ii += i;
            q   = a[m+1];
            if (i <= k)
              w(i) = -q/p;
            else
              w(i) = q/p;
            for (ij=m+2; ij<=ii; ij++)
              a[ij-i] = a[ij] + q*w(ij-m);
          }
        m--;
        a[ii] = 1/p;
        for (i=2; i<=n; i++)
          a[m+i] = w(i);
      }
  }

template <typename Float, typename Exc>
inline SymMat<Float, Exc> inv(const SymMat<Float, Exc>& A)
  {
    SymMat<Float, Exc> T(A);
    T.invert();
    return T;
  }


// ======================================================================

template <typename Float, typename Exc> inline
const SymMat<Float, Exc>& trans(const SymMat<Float, Exc>& A) { return A; }
template <typename Float, typename Exc> inline
SymMat<Float, Exc>& trans(SymMat<Float, Exc>& A) { return A; }

template <typename Float, typename Exc>
SymMat<Float, Exc>
operator*(const SymMat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.dim() != B.dim())
      throw Exc(Exception::BadRank,
                "operator*(const SymMat& A, const SymMat& B) ");

    const Float *a = A.begin() - 1;
    const Float *b = B.begin() - 1;

    SymMat<Float, Exc> C(A.dim());
    typename SymMat<Float, Exc>::iterator c = C.begin();
    const Index n = C.dim();

    Float cij;
    Index i, j, k, l, m;
    for (i=1; i<=n; i++)
      for (j=1; j<=i; j++)
        {
          cij = 0;
          l = i*(i-1)/2;
          m = j*(j-1)/2;
          for (k=1; k<=n; k++)
            {
              l++;
              m++;
              if (k > i) l += k-2;
              if (k > j) m += k-2;
              cij += a[l]*b[m];
            }
          *c = cij;
          ++c;
        }

    return C;
  }

template <typename Float, typename Exc>
Mat<Float, Exc>
operator*(const Mat<Float, Exc>& A, const SymMat<Float, Exc>& B)
  {
    if (A.cols() != B.rows())
      throw Exc(Exception::BadRank,
                "operator*(const Mat& A, const SymMat& B)");

    const Index m  = A.rows();
    const Index n  = A.cols();
    typename Mat<Float, Exc>::const_iterator a  = A.begin();
    typename Mat<Float, Exc>::const_iterator aj;
    const Float *b = B.begin() - 1;
    Mat<Float, Exc> C(m,n);
    typename Mat<Float, Exc>::iterator c = C.begin();

    Index i, j, k, l;
    Float sum;
    for (i=1; i<=m; i++, a += n)
      {
        for (j=1; j<=n; j++)
          {
            aj = a;
            l = j*(j-1)/2;
            sum = 0;
            for (k=1; k<=j; k++, aj++)
              sum += *aj * b[++l];
            for (k=j+1, l+=j; k<=n; l+=k, k++, aj++)
              sum += *aj * b[l];
            *c = sum;
            ++c;
          }
      }


    return C;
  }

// ======================================================================


}      // namespace GNU_gama

#endif
