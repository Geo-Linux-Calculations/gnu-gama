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


void LocalLinearization::x(const X* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // Double p = M_0 / stdDev();

   // Double w = p*p;                          // weight
   rhs = (obs->value() - point.x())*1e3;       // abs. term in millimetres  

   size = 0;
   if (point.free_xy())
   {
      if (!point.index_x()) point.index_x() = ++maxn;
      index[ size ] = point.index_x();
      coeff[ size ] = 1;
      size++;
   }
}



void LocalLinearization::y(const Y* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // Double p = M_0 / stdDev();

   // Double w = p*p;                          // weight
   rhs = (obs->value() - point.y())*1e3;       // abs. term in millimetres  

   size = 0;
   if (point.free_xy())
   {
      if (!point.index_y()) point.index_y() = ++maxn;
      index[ size ] = point.index_y();
      coeff[ size ] = 1;
      size++;
   }
}



void LocalLinearization::z(const Z* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // Double p = M_0 / stdDev();

   // Double w = p*p;                          // weight
   rhs = (obs->value() - point.z())*1e3;       // abs. term in millimetres  

   size = 0;
   if (point.free_z())
   {
      if (!point.index_z()) point.index_z() = ++maxn;
      index[ size ] = point.index_z();
      coeff[ size ] = 1;
      size++;
   }
}

