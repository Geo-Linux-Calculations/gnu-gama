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

/*
 *  $Id: distance.h,v 1.2 2007/06/26 15:04:06 cepek Exp $
 */

#include <gamalib/local/linearization.h>
#include <gamalib/local/pobs/bearing.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::distance(const Distance* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   Double s, d;
   bearing_distance(PD[obs->from()], PD[obs->to()], s, d);
   // Double p = M_0 / stdDev();
   Double ps = sin(s);
   Double pc = cos(s);

   // Double w = p*p;                  // weight
   rhs = (obs->value() - d)*1e3;       // abs. term in millimetres

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -ps;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = -pc;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = ps;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = pc;
      size++;
   }
}

