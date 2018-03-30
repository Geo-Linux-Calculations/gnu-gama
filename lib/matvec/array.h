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

#ifndef GNU_gama_gMatVec_Array_h
#define GNU_gama_gMatVec_Array_h

#include <matvec/memrep.h>

namespace GNU_gama {   /** \brief Dynamic array */

  template <typename Type,
            typename Index=int,
            typename Exc=Exception::matvec>
  class Array : public MemRep<Type, Index, Exc>
  {
  public:

    Array(Index dim) : MemRep<Type, Index, Exc>(dim) {}
    Index  operator[](Index i) const { return this->begin()[i]; }
    Index& entry(Index i) { return this->begin()[i]; }
    void swap(Index i, Index j)
    {
      Index *ind = this->begin();
      Index t = ind[i]; ind[i] = ind[j]; ind[j] = t;
    }

  };


}   // namespace GNU_gama

#endif
