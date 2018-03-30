/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gama_local_Integer_list_GNU_gama_local_Integer_list
#define GNU_gama_gama_local_Integer_list_GNU_gama_local_Integer_list

#include <cstddef>

namespace GNU_gama {

/** Integer list class */

template <typename Index=int>

  class IntegerList {     // Integer List class

    IntegerList (const IntegerList&);
    void operator=(const IntegerList&);

    Index*  m;
    Index*  e;

    public:

    IntegerList() : m(0), e(0)
    {
    }

    IntegerList(Index n)
    {
      m = new Index[n];
      e = m + n;
    }

    ~IntegerList()
    {
      delete[] m;
    }

    Index dim() const { return Index(e-m); }

    void reset()
    {
      delete[] m;
      m = e = 0;
    }

    void reset(Index n)
    {
      delete[] m;
      m = new Index[n];
      e = m + n;
    }

    void set_all(Index f)
    {
      iterator b = m;
      while (b != e)  *b++ = f;
    }

    void set_zero() { set_all(Index()); }


    typedef Index*       iterator;
    typedef const Index* const_iterator;

    iterator begin() { return m; }
    iterator end()   { return e; }

    const_iterator begin() const { return m; }
    const_iterator end()   const { return e; }

    Index  operator()(Index i) const { return m[i]; }
    Index& operator()(Index i)       { return m[i]; }
  };

}   // namespace GNU_gama

#endif



#ifdef GNU_gama_sparse_demo

#include <iostream>

using namespace std;
using namespace GNU_gama;


int main()
{
  cout << "\n---  Integer List demo  ------------------------------------\n\n";

  IntegerList<> ilist;
  ilist.reset(20);
  IntegerList<>::iterator c = ilist.begin();
  IntegerList<>::const_iterator e = ilist.end();
  for (int i=1, j=1, k=1; c!=e; k = i+j, i=j, j=k)
    {
      *c++ = i;
    }

  for (IntegerList<>::const_iterator a=ilist.begin(), b=ilist.end(); a!=b; a++)
    {
      cout << *a << " ";
    }
  cout << "\n\n";
}


#endif
