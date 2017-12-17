/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/comb.h>

namespace GNU_gama {

void Comb::reset(int pn, int pk)
{
   delete[] cmb;
   if (pk > pn || pn < 1 || pk < 1)
      {
         n_ = k_ = k__ = 0;
         cmb = 0;
         return;
      }
   n_ = pn;
   k_ = k__ = pk;
   cmb = new int[pk];
   c = cmb - 1;
   begin();
}


void Comb::begin()
{
   k_ = k__;
   for (int i=1; i<=k__; i++) c[i] = i;
}

void Comb::next()
{
   if (k_ == 0) return;

   if (c[k_] < n_)
      {
         c[k_]++;
         return;
      }

   for (int i=k_; i>1; i--)
      if (c[i-1] < n_-k_+i-1)
         {
            c[i-1]++;
            for (int j=i; j<=k_; j++)
               c[j] = c[i-1] + j - i + 1;
            return;
         }

   k_ = 0;
}

}      /* namespace GNU_gama */







