/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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
 *  $Id: direction.h,v 1.1 2006/04/09 16:40:24 cepek Exp $
 */

#include <gamalib/local/linearization.h>
#include <gamalib/local/pobs/bearing.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::direction(const Direction* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   Double s, d;
   bearing_distance(sbod, cbod, s, d);
   // const Double p = m0 / obs->stdDev();
   const Double K = 10*R2G/d;
   const Double ps = K*sin(s);
   const Double pc = K*cos(s);

   const StandPoint*  csp = static_cast<const StandPoint*>(obs->ptr_cluster());
   StandPoint* sp = const_cast<StandPoint*>(csp);

   // Double w = p*p;                                       // weight
   Double a = (obs->value() + sp->orientation() - s)*R2CC;  // rhs

   while (a > 200e4)
      a -= 400e4;
   while (a < -200e4)
      a += 400e4;
   rhs = a;          // absolute termm is in cc

   size = 0;
   if (!sp->index_orientation()) sp->index_orientation(++maxn);
   index[ size ] = sp->index_orientation();
   coeff[ size ] = -1;
   size++;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -pc;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = ps;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = pc;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = -ps;
      size++;
   }
}

