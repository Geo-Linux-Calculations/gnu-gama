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

#ifndef GNU_gama_gMatVec_Vec__h_
#define GNU_gama_gMatVec_Vec__h_

#include <iostream>
#include <cmath>
#include <matvec/vecbase.h>

namespace GNU_gama {   /** \brief Vector */


template <typename Float=double, typename Exc=Exception::matvec>
class Vec : public VecBase<Float, Exc> {

public:

  typedef typename VecBase<Float, Exc>::iterator       iterator;
  typedef typename VecBase<Float, Exc>::const_iterator const_iterator;

  Vec() {}
  Vec(Index nsz) : VecBase<Float, Exc>(nsz) {}
  Vec(const VecBase<Float, Exc>& v) : VecBase<Float, Exc>(v) {}

  Vec operator*(Float f) const {
    Vec t(this->dim()); this->mul(f, t); return t;
  }
  Vec operator+(const Vec &x) const {
    Vec t(this->dim()); this->add(x, t); return t;
  }
  Vec operator-(const Vec &x) const {
    Vec t(this->dim()); this->sub(x, t); return t;
  }

  Vec& operator*=(Float f)      { this->mul(f, *this); return *this; }
  Vec& operator+=(const Vec &x) { this->add(x, *this); return *this; }
  Vec& operator-=(const Vec &x) { this->sub(x, *this); return *this; }

  typename MatVecBase<Float, Exc>::ListInitialiser operator=(Float x)
  {
    return this->list_init(x);
  }

};


template <typename Float, typename Exc>
inline Vec<Float, Exc> operator*(Float f, const Vec<Float, Exc>& V)
  {
    return V*f;
  }


template <typename Float, typename Exc>
Vec<Float, Exc>
operator*(const MatBase<Float, Exc> &A, const Vec<Float, Exc> &b)
  {
    if (A.cols() != b.dim())
      throw Exc(Exception::BadRank, "Vec operator*(const MatBase&, const Vec&)");

    Vec<Float, Exc> t(A.rows());
    Float s;
    for (Index i=1; i<=A.rows(); i++)
      {
        s = 0;
        for (Index j=1; j<=A.cols(); j++)
          s += A(i,j)*b(j);
        t(i) = s;
      }

    return t;
  }


template <typename Float, typename Exc>
Vec<Float, Exc>
operator*(const Mat<Float, Exc> &A, const Vec<Float, Exc> &b)
  {
    if (A.cols() != b.dim())
      throw Exc(Exception::BadRank, "Vec operator*(const Mat&, const Vec&)");

    Vec<Float, Exc> t(A.rows());
    typename Vec<Float, Exc>::iterator ti = t.begin();
    typename Vec<Float, Exc>::const_iterator bb = b.begin();
    typename Vec<Float, Exc>::const_iterator bi;
    typename Vec<Float, Exc>::const_iterator ai = A.begin();
    Float s;
    for (Index i=1; i<=A.rows(); i++)
      {
        s = 0;
        bi = bb;
        for (Index j=1; j<=A.cols(); j++)
          s += *ai++ * *bi++;
        *ti++ = s;
      }

    return t;
  }


}   // namespace GNU_gama

#endif
