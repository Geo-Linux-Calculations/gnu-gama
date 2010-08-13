/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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

#include <gamalib/local/linearization.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::h_diff(const H_Diff* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   Double h = cbod.z() - sbod.z();
   // Double p = M_0 / stdDev();

   // Double w = p*p;                  // weight
   rhs = (obs->value() - h)*1e3;       // abs. term in millimetres

   size = 0;
   if (sbod.free_z())
   {
      if (!sbod.index_z())  sbod.index_z() = ++maxn;
      index[ size ] = sbod.index_z();
      coeff[ size ] = -1;
      size++;
   }
   if (cbod.free_z())
   {
      if (!cbod.index_z())  cbod.index_z() = ++maxn;
      index[ size ] = cbod.index_z();
      coeff[ size ] = 1;
      size++;
   }
}
