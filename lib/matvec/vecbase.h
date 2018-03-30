/*
  C++ Matrix/Vector templates (GNU Gama / matvec)
  Copyright (C) 1999, 2007, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gMatVec_VecBase_h
#define GNU_gama_gMatVec_VecBase_h

#include <iostream>
#include <cmath>
#include <matvec/matvecbase.h>

namespace GNU_gama {   /** \brief Vector base class. */


  template <typename Float=double,
            typename Index=int,
            typename Exc=Exception::matvec>
  class VecBase : public MatVecBase<Float, Index, Exc> {

  protected:
  public:

    VecBase() {}
    VecBase(Index nsz) :  MatVecBase<Float, Index, Exc>(nsz) {}

  public:

    typedef typename MatVecBase<Float, Index, Exc>::iterator       iterator;
    typedef typename MatVecBase<Float, Index, Exc>::const_iterator const_iterator;

    Index dim() const { return this->size(); }

    Float& operator()(Index n)
    {
      Float* m = this->begin(); return m[--n];
    }
    Float  operator()(Index n) const
    {
      const Float* m = this->begin(); return m[--n];
    }

    void reset(Index n=0) { this->resize(n); }

    Float dot(const VecBase<Float, Index, Exc> &B) const;

    Float norm_L1()   const;
    Float norm_L2()   const { return std::sqrt(dot(*this)); }
    Float norm_Linf() const;

  };


  template <typename Float, typename Index, typename Exc>
  Float
  VecBase<Float, Index, Exc>::dot(const VecBase<Float, Index, Exc> &B) const
  {
    if (dim() != B.dim())
      throw Exc(Exception::BadRank,
                "Float VecBase::dot(const VecBase&) const");

    const_iterator a = this->begin();
    const_iterator e = this->end();
    const_iterator b = B.begin();

    Float sum = 0;
    while (a != e) sum += *a++ * *b++;

    return sum;
  }


  template <typename Float, typename Index, typename Exc>
  Float VecBase<Float, Index, Exc>::norm_L1() const
  {
    const_iterator a = this->begin();
    const_iterator e = this->end();

    Float sum = 0;
    while (a != e) { sum += *a >= 0 ? *a : -(*a); ++a; }

    return sum;
  }


  template <typename Float, typename Index, typename Exc>
  Float VecBase<Float, Index, Exc>::norm_Linf() const
  {
    const_iterator a = this->begin();
    const_iterator e = this->end();

    Float norm = 0, x;
    while (a != e)
      {
        x = *a >= 0 ?  *a : -(*a);
        ++a;
        if (x > norm) norm = x;
      }

    return norm;
  }


  template <typename Float, typename Index, typename Exc>
  std::istream& operator>>(std::istream& inp, VecBase<Float, Index, Exc>& v)
  {
    Index size;
    if (inp >> size)
      {
        if (size != v.dim())
          v.reset(size);

        typename MatVecBase<Float, Index, Exc>::iterator b = v.begin();
        typename MatVecBase<Float, Index, Exc>::iterator e = v.end();
        while (b != e)
          {
            inp >> *b;
            ++b;
          }
      }

    return inp;
  }


}   // namespace GNU_gama

#endif
