 /*
  C++ Matrix/Vector templates (GNU Gama / matvec)
  Copyright (C) 1999, 2007, 2012, 2017, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GNU_gama_gMatVec_Mat_h
#define GNU_gama_gMatVec_Mat_h

#include <iostream>
#include <initializer_list>
#include <limits>
#include <matvec/matbase.h>
#include <matvec/array.h>

namespace GNU_gama {

  template <typename Float, typename Index, typename Exc> class TransMat;

  /** \brief Matrix class
   */

  template <typename Float=double,
    typename Index=int,
    typename Exc=Exception::matvec>
    class Mat : public MatBase<Float, Index, Exc>
    {
    public:

    typedef typename MatBase<Float, Index, Exc>::iterator       iterator;
    typedef typename MatBase<Float, Index, Exc>::const_iterator const_iterator;

    Mat() {}
    Mat(Index r, Index c) : MatBase<Float, Index, Exc>(r, c, r*c) {}
    Mat(const TransMat<Float, Index, Exc>&);
    Mat(std::initializer_list<std::initializer_list<Float>> init)
    : MatBase<Float, Index, Exc>(init.size(), init.begin()->size(),
                                 init.size() *init.begin()->size())
    {
      Index cols = init.begin()->size();
      Float* f = this->begin();
      for (auto row : init) {
        if (Index(row.size()) != cols)
          throw Exc(Exception::BadRank, "Mat::Initializer list");
        for (auto c : row)
          *f++ = c;
      }
    }

    Float& operator()(Index r, Index c) {
      Float *m = this->begin();
      return m[--r*this->cols() + --c];
    }
    Float  operator()(Index r, Index c) const {
      const Float *m = this->begin();
      return m[--r*this->cols() + --c];
    }

    Mat operator*(Float f) const {
      Mat t(this->rows(), this->cols()); this->mul(f, t); return t;
    }
    Mat operator+(const Mat& M) const {
      if (this->rows() != M.rows() || this->cols() != M.cols())
        throw Exc(Exception::BadRank, "Mat::operator+(const Mat& M) const");

      Mat T(this->rows(), this->cols());
      this->add(M, T);
      return T;
    }
    Mat operator-(const Mat& M) const {
      if (this->rows() != M.rows() || this->cols() != M.cols())
        throw Exc(Exception::BadRank, "Mat::operator-(const Mat& M) const");

      Mat T(this->rows(), this->cols());
      this->sub(M, T);
      return T;
    }

    void transpose() { *this = trans(*this); }
    void invert(Float tol=std::numeric_limits<Float>::epsilon()*1000);

    private:

    Float* pentry;  // not initialized in constructor !!!
    Float& entry(Index i, Index j) { return *(pentry + i*this->col_ + j); }

    };


  template <typename Float, typename Index, typename Exc>
    inline Mat<Float, Index, Exc>
    operator*(Float f, const Mat<Float, Index, Exc> &M)
    {
      return M*f;
    }


  template <typename Float, typename Index, typename Exc>
    Mat<Float, Index, Exc>
    operator* (const MatBase<Float, Index, Exc> &A,
               const MatBase<Float, Index, Exc> &B)
    {
      if (A.cols() != B.rows())
        throw Exc(Exception::BadRank,
                  "Mat operator* (const MatBase&, const MatBase&)");

      Mat<Float, Index, Exc> C(A.rows(), B.cols());
      typename Mat<Float, Index, Exc>::iterator c = C.begin();
      Float s;
      for (Index i=1; i<=C.rows(); i++)
        for (Index j=1; j<=C.cols(); j++)
          {
            s = 0;
            for (Index k=1; k<=B.rows(); k++)
              s += A(i,k) * B(k,j);
            *c++ = s;
          }

      return C;
    }


  template <typename Float, typename Index, typename Exc>
    Mat<Float, Index, Exc>
    operator+(const MatBase<Float, Index, Exc> &A,
              const MatBase<Float, Index, Exc> &B)
    {
      if (A.rows() != B.rows() || A.cols() != B.cols())
        throw Exc(Exception::BadRank,
                  "Mat operator+(const MatBase &A, const MatBase &B)");

      Mat<Float, Index, Exc> C(A.rows(), A.cols());
      for (Index i=1; i<=A.rows(); i++)
        for (Index j=1; j<=A.cols(); j++)
          C(i,j) = A(i,j) + B(i,j);

      return C;
    }


  template <typename Float, typename Index, typename Exc>
    Mat<Float, Index, Exc>
    operator-(const MatBase<Float, Index, Exc> &A,
              const MatBase<Float, Index, Exc> &B)
    {
      if (A.rows() != B.rows() || A.cols() != B.cols())
        throw Exc(Exception::BadRank,
                  "Mat operator-(const MatBase &A,const MatBase &B)");

      Mat<Float, Index, Exc> C(A.rows(), A.cols());
      for (Index i=1; i<=A.rows(); i++)
        for (Index j=1; j<=A.cols(); j++)
          C(i,j) = A(i,j) - B(i,j);

      return C;
    }



  template <typename Float, typename Index, typename Exc>
    Mat<Float, Index, Exc>
    operator*(const Mat<Float, Index, Exc> &A, const Mat<Float, Index, Exc> &B)
    {
      if (A.cols() != B.rows())
        throw Exc(Exception::BadRank, "Mat operator*(const Mat&, const Mat&)");

      Mat<Float, Index, Exc> C(A.rows(), B.cols());
      typename Mat<Float, Index, Exc>::iterator c = C.begin();
      typename Mat<Float, Index, Exc>::const_iterator ab = A.begin();
      typename Mat<Float, Index, Exc>::const_iterator a;
      typename Mat<Float, Index, Exc>::const_iterator bb = B.begin();
      typename Mat<Float, Index, Exc>::const_iterator b;
      Float s;

      for (Index i=1; i<=C.rows(); i++, ab += A.cols())
        for (Index j=0; j<C.cols(); j++)
          {
            s = 0;
            a = ab;
            b = bb + j;
            for (Index k=1; k<=A.cols(); k++, b += B.cols())
              s += *a++ * *b;
            *c++ = s;
          }

      return C;
    }


  template <typename Float, typename Index, typename Exc>
    void Mat<Float, Index, Exc>::invert(Float tol)
    {
      /* Gauss-Jordan elimination */

      if (this->rows() != this->cols())
        throw Exc(Exception::BadRank, "Mat<>::invert()");

      pentry = this->begin();

      const Index N = this->rows();
      Index step, row, p_row {0}, p_col {0}, i, ii, j, jj, r, c, l;

      Array<Index, Index, Exc> indr(N),indc(N);    // row/column permutation
      for (l=0; l<N; l++) indr.entry(l) = indc.entry(l) = l;

      Float pivot = 0, invpivot, e;
      for (step=0; step<N; step++)
        {
          pivot = 0;
          for (ii=step; ii<N; ii++)
            {
              i = indr[ii];
              for (jj=step; jj<N; jj++)
                {
                  e = entry(i, indc[jj]);
                  if (this->Abs(e) > this->Abs(pivot))
                    {
                      pivot = e; p_row = ii; p_col = jj;
                    }
                }
            }
          if (this->Abs(pivot) <= tol)
            throw Exc(Exception::Singular, "Mat<>::invert()");

          if (step != p_row) indr.swap(step, p_row);
          if (step != p_col) indc.swap(step, p_col);

          invpivot = Float(1.0) / pivot;
          entry(indr[step], indc[step]) = Float(1.0);
          i = indr[step];
          for (j=0; j<N; j++) entry(i,j) *= invpivot;

          for (row=0; row<N; row++)
            if (indr[row] != indr[step])
              {
                i = indr[row];
                e = entry(i, indc[step]);
                entry(i, indc[step]) = Float(0.0);
                for (j=0; j<N; j++) entry(i,j) -= e*entry(indr[step],j);
              }
        }

      // inverse row/column permutation

      Array<Index, Index, Exc> invr(N), invc(N);

      for (i=0; i<N; i++)
        {
          invc.entry(indc[i]) = i;
          invr.entry(indr[i]) = i;
        }

      // swap rows

      Array<Index, Index, Exc> perm(N), inv_perm(N);
      for (i=0; i<N; i++)
        {
          perm.entry(i) = indr[invc[i]];
          inv_perm.entry(perm[i]) = i;
        }
      {
        for (i=0; i<N; i++)
          if (i != (r = perm[i]))
            {
              for (j=0; j<N; j++)
                {
                  e = entry(i,j); entry(i,j) = entry(r,j); entry(r,j) = e;
                }
              perm.entry(inv_perm[i]) = perm[i];
              // perm.entry(i) = i;
              inv_perm.swap(i, r);
            }
      }

      // swap columns

      for (i=0; i<N; i++)
        {
          perm.entry(i) = indc[invr[i]];
          inv_perm.entry(perm[i]) = i;
        }
      {
        for (j=0; j<N; j++)
          if (j != (c = perm[j]))
            {
              for (i=0; i<N; i++)
                {
                  e = entry(i,j); entry(i,j) = entry(i,c); entry(i,c) = e;
                }
              perm.entry(inv_perm[j]) = perm[j];
              // perm.entry(j) = j;
              inv_perm.swap(j, c);
            }
      }

    }


  template <typename Float, typename Index, typename Exc>
  inline Mat<Float, Index, Exc>
  inv(const Mat<Float, Index, Exc>& A,
      Float tol=std::numeric_limits<Float>::epsilon()*1000)
    {
      Mat<Float, Index, Exc> t = A;
      t.invert(tol);
      return t;
    }


}   // namespace GNU_gama

#endif
