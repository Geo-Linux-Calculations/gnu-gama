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

#ifndef GNU_gama_gMatVec_MatVecBase__h_
#define GNU_gama_gMatVec_MatVecBase__h_

#include <matvec/memrep.h>


namespace GNU_gama {   /** \brief Matrix/vector base class */

template <typename Float=double, typename Exc=Exception::matvec>
class MatVecBase : public MemRep<Float, Exc> {

public:

  typedef typename MemRep<Float, Exc>::iterator       iterator;
  typedef typename MemRep<Float, Exc>::const_iterator const_iterator;

  void operator*=(Float f)
  {
    iterator b = this->begin();
    iterator e = this->end();
    while (b != e)
    *b++ *= f;
  }
  void operator/=(Float f) { operator*=(1/f); }

  void set_all(Float f)
  {
    iterator b = this->begin();
    iterator e = this->end();
    while (b != e)
      *b++ = f;
  }

  void set_zero() { set_all(Float()); }

protected:

  MatVecBase() {}
  MatVecBase(Index nsz) : MemRep<Float, Exc>(nsz) {}
  MatVecBase(const MemRep<Float, Exc>& mem) : MemRep<Float, Exc>(mem) {}

  void mul(Float f, MatVecBase& X) const
    {
      if (this->size() != X.size())
        throw Exc(Exception::BadRank, "MatVecBase::mul(Float f, MatVecBase& X)");

      const_iterator a = this->begin();
      iterator x = X.begin();
      iterator e = X.end();
      while (x != e)
        *x++ = *a++ * f;
    }

  void add(const MatVecBase& B, MatVecBase& X) const
    {
      if (this->size() != B.size() || this->size() != X.size())
        throw Exc(Exception::BadRank, "MatVecBase::add(const MatVecBase&, MatVecBase&)");

      const_iterator a = this->begin();
      const_iterator b = B.begin();
      iterator x = X.begin();
      iterator e = X.end();
      while (x != e)
        *x++ = *a++ + *b++;
    }

  void sub(const MatVecBase& B, MatVecBase& X) const
    {
      if (this->size() != B.size() || this->size() != X.size())
        throw Exc(Exception::BadRank, "MatVecBase::sub(const MatVecBase&, MatVecBase&)");

      const_iterator a = this->begin();
      const_iterator b = B.begin();
      iterator x = X.begin();
      iterator e = X.end();
      while (x != e)
        *x++ = *a++ - *b++;
    }

    const Float Abs(Float x) const
    {
       return (x >= Float()) ? x : -x ;
    }
    const Float Sign(Float a, Float b) const
    {
       return b >= Float() ? Abs(a) : -Abs(a);
    }



  // List initialiser helper class

  class ListInitialiser {

    iterator x, e, first;

  public:

  ListInitialiser(iterator begin,
                  iterator end) : x(begin), e(end)
    {
      first = x;
      if (x != e) ++first;
    }
  ~ListInitialiser()
    {
      if (x != first && x != e)
        throw Exc(Exception::BadRank, "ListInitialiser : "
                           "not enough elements in the initialisation list");
    }

  void add(Float p)
    {
      if (x == e)  throw Exc(Exception::BadRank, "ListInitialiser : "
                             "too many elements in the initialisation list");
      *x = p;
      ++x;
    }

  ListInitialiser& operator,(Float x)
    {
      add(x);
      return *this;
    }

  };

  ListInitialiser list_init(Float p)
    {
      ListInitialiser linit( this->begin(), this->end() );
      linit.add(p);
      return linit;
    }

};

}   // namespace GNU_gama

#endif








