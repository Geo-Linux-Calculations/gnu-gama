/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999, 2017  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.

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

#ifndef GNU_gama_Comb_h
#define GNU_gama_Comb_h

#include <cmath>

namespace GNU_gama {

// K-combinations of N positive integers <1, N>

class Comb
{
   int n_, k_, k__;
   int* cmb;
   int* c;

public:
   const int& n;
   const int& k;

   Comb(int pn=0, int pk=0) : cmb(0), n(n_), k(k_) { reset(pn, pk); }
   ~Comb() { delete[] cmb; }
   void reset(int pn=0, int pk=0);
   void begin();
   void next();
   int operator[](int i) const { return c[i]; }   // i=1,2,3...
   void operator++(int) { next(); }
   void operator++()    { next(); }
};

}      /* namespace GNU_gama */

#endif
//----------------------------------------------------------------------








