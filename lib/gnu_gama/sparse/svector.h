/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.

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

#ifndef GNU_gama___sparse__vector__h___sparsevectorh_sparsevectorh
#define GNU_gama___sparse__vector__h___sparsevectorh_sparsevectorh

#include <cstddef>


namespace GNU_gama {

/** Sparse vector class */

template <typename Float=double, typename Index=std::size_t>

  class SparseVector {
  private:

  Index  vdim, flts, used;
  Float* nonz;
  Index* indx;

  SparseVector  (const SparseVector&);
  void operator=(const SparseVector&);

  enum { min_buffer_size=10 };

  public:

  SparseVector(Index floats=min_buffer_size, Index dimension=0)
  {
    if (floats < min_buffer_size) floats = min_buffer_size;

    vdim = dimension;            // zero if not used
    nonz = new Float[floats];
    indx = new Index[floats];
    flts = floats;
    used = 0;
  }

  ~SparseVector()
  {
    delete[] nonz;
    delete[] indx;
  }

  Index  dim      () const { return vdim; }
  Index  nonzeroes() const { return used; }
  Float* begin    () const { return nonz;        }
  Float* end      () const { return nonz + used; }
  Index* ibegin   () const { return indx;        }
  Index* iend     () const { return indx + used; }

  /** resets the number of vector elements to zero */
  void reset()
  {
    used = 0;
  }

  /** add index-value pair into the vector */
  void add(Index ind, Float flt)
  {
    if (used == flts)
    {
      Float* nonz_ = new Float[2*flts];
      Index* indx_ = new Index[2*flts];

      for (Index i=0; i<flts; i++)
      {
        nonz_[i] = nonz[i];
        indx_[i] = indx[i];
      }

      delete[] nonz;
      delete[] indx;
      nonz  = nonz_;
      indx  = indx_;
      flts *= 2;
    }

    nonz[used] = flt;
    indx[used] = ind;
    used++;
  }

  };

}   // namespace GNU_gama

#endif



#ifdef GNU_gama_sparse_demo

#include <iostream>

int main()
{
  using namespace std;
  using namespace GNU_gama;

  cout << "\n---  Sparse Vector demo  -----------------------------------\n\n";

  SparseVector<> svec;

  for (int i=1; i<=9; i++)
    {
      svec.reset();

      for (int j=1; j<=3+i/3; j++) svec.add(j, 10.0*i + 0.01*j);

      cout << i << " (nonz " << svec.nonzeroes() << ") : ";
      double* mm = svec.begin ();
      double* me = svec.end   ();
      std::size_t* ii = svec.ibegin();
      std::size_t* ie = svec.iend  ();
      while (mm != me && ii != ie)
        {
          cout << *ii++ << " " << *mm++ << "   ";
        }
      cout << endl;
    }

  return 0;
}

#endif
