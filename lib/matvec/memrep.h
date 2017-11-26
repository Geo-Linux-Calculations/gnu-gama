/*
  C++ Matrix/Vector templates (GNU Gama / matvec)
  Copyright (C) 1999, 2007, 2014, 2017  Ales Cepek <cepek@gnu.org>

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef GNU_gama_gMatVec_MemRep_h_
#define GNU_gama_gMatVec_MemRep_h_

#include <cstring>
#include <matvec/inderr.h>

namespace GNU_gama {   /** \brief Memory repository for matvec objects */

  template <typename Float=double, typename Exc=Exception::matvec>
    class MemRep {

  public:

  using iterator = Float*;
  iterator begin() { return rep; }
  iterator end()   { return rep + sz; }

  using const_iterator = const Float*;
  const_iterator begin() const { return rep; }
  const_iterator end()   const { return rep + sz; }

  protected:

  MemRep() : rep(nullptr), sz(0) {}

  MemRep(Index nsz)
  {
    if (nsz > 0)
      {
        sz  = nsz;
        rep = new Float[sz];
      }
    else if (nsz == 0)
      {
        sz  = 0;
        rep = nullptr;
      }
    else
      {
        throw Exc(Exception::BadRank, "MemRep::MemRep(Index nsz)");
      }
  }

  MemRep(const MemRep& x)
  {
    sz = x.sz; rep = new Float[sz];
    std::memcpy(rep, x.rep, sz*sizeof(Float));
  }

  MemRep(MemRep&& x)
  {
    sz = x.sz;  rep = x.rep;
    x.sz = 0;   x.rep = nullptr;
  }

  MemRep& operator = (const MemRep& x)
  {
    if (&x == this) return *this;

    if (sz == x.sz) {
      std::memcpy(rep, x.rep, sz*sizeof(Float));
      return *this;
    }

    sz = x.sz;
    if (sz > 0)
      {
        rep = new Float[sz];
        std::memcpy(rep, x.rep, sz*sizeof(Float));
      }
    else
      {
        rep = nullptr;
      }

    return *this;
  }

  MemRep& operator = (MemRep&& x)
  {
    if (&x != this)
      {
        if (rep != nullptr) delete[] rep;

        sz = x.sz;  rep = x.rep;
        x.sz = 0;   x.rep = nullptr;
      }

    return *this;
  }

  ~MemRep() { delete[] rep; }

  void resize(Index nsz)
  {
    if (nsz == sz) return;

    sz = nsz;
    delete[] rep;

    if (sz > 0)
      rep = new Float[sz];
    else
      rep = nullptr;
  }

  Index size() const { return sz; }


  private:

  Float* rep;
  Index  sz;

  };      /* class MemRep; */


}   // namespace GNU_gama

#endif
