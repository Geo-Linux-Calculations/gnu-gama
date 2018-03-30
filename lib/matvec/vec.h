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

#ifndef GNU_gama_gMatVec_Vec_h
#define GNU_gama_gMatVec_Vec_h

#include <iostream>
#include <cmath>
#include <initializer_list>
#include <matvec/vecbase.h>

namespace GNU_gama {   /** \brief Vector */


  template <typename Float=double,
            typename Index=int,
            typename Exc=Exception::matvec>
  class Vec : public VecBase<Float, Index, Exc> {

  public:

    typedef typename VecBase<Float, Index, Exc>::iterator       iterator;
    typedef typename VecBase<Float, Index, Exc>::const_iterator const_iterator;

    Vec() {}
    Vec(Index nsz) : VecBase<Float, Index, Exc>(nsz) {}
    Vec(const VecBase<Float, Index, Exc>& v)
      : VecBase<Float, Index, Exc>(v)
    {
    }
    Vec(std::initializer_list<Float> init)
      : VecBase<Float, Index, Exc>(init.size())
    {
      Float* f = this->begin();
      for (auto p : init)
        *f++ = p;
    }

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
  };


  template <typename Float, typename Index, typename Exc>
  inline Vec<Float, Index, Exc>
  operator*(Float f, const Vec<Float, Index, Exc>& V)
  {
    return V*f;
  }


  template <typename Float, typename Index, typename Exc>
  Vec<Float, Index, Exc>
  operator*(const MatBase<Float, Index, Exc> &A,
            const Vec<Float, Index, Exc> &b)
  {
    if (A.cols() != b.dim())
      throw Exc(Exception::BadRank,
                "Vec operator*(const MatBase&, const Vec&)");

    Vec<Float, Index, Exc> t(A.rows());
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


  template <typename Float, typename Index, typename Exc>
  Vec<Float, Index, Exc>
  operator*(const Mat<Float, Index, Exc> &A, const Vec<Float, Index, Exc> &b)
  {
    if (A.cols() != b.dim())
      throw Exc(Exception::BadRank, "Vec operator*(const Mat&, const Vec&)");

    Vec<Float, Index, Exc> t(A.rows());
    typename Vec<Float, Index, Exc>::iterator ti = t.begin();
    typename Vec<Float, Index, Exc>::const_iterator bb = b.begin();
    typename Vec<Float, Index, Exc>::const_iterator bi;
    typename Vec<Float, Index, Exc>::const_iterator ai = A.begin();
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
