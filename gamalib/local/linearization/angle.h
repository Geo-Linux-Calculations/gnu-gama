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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: angle.h,v 1.1 2001/12/07 12:44:55 cepek Exp $
 */

#include <gamalib/local/linearization.h>
#include <gamalib/local/pobs/bearing.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::angle(const Angle* obs) const
{
   Point& sbod  = PD[obs->from()];
   Point& cbod1 = PD[obs->to()];
   Point& cbod2 = PD[obs->rs()];
   Double s1, d1, s2, d2;
   bearing_distance(PD[obs->from()], PD[obs->to()], s1, d1);
   bearing_distance(PD[obs->from()], PD[obs->rs()], s2, d2);
   // Double p = m0 / obs->stdDev();
   const Double K1 = 10*R2G/d1;
   const Double K2 = 10*R2G/d2;
   const Double ps1 = K1*sin(s1);
   const Double pc1 = K1*cos(s1);
   const Double ps2 = K2*sin(s2);
   const Double pc2 = K2*cos(s2);

   // Double w = p*p;                          // weight
   Double ds = s2 - s1;
   if (ds < 0) ds += 2*M_PI;
   Double a = (obs->value() - ds)*R2CC;        // rhs
   // "big" positive/negative angle transformed to "lesser" positive/negative
   while (a > 200e4)
      a -= 400e4;
   while (a < -200e4)
      a += 400e4;
   rhs = a;          // rhs in cc

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] =  sbod.index_y();
      coeff[ size ] = -pc2 + pc1;
      size++;
      index[ size ] =  sbod.index_x();
      coeff[ size ] =  ps2 - ps1;
      size++;
   }
   if (cbod1.free_xy())
   {
      if (!cbod1.index_x()) cbod1.index_x() = ++maxn;
      if (!cbod1.index_y()) cbod1.index_y() = ++maxn;
      index[ size ] =  cbod1.index_y();
      coeff[ size ] = -pc1;
      size++;
      index[ size ] =  cbod1.index_x();
      coeff[ size ] =  ps1;
      size++;
   }
   if (cbod2.free_xy())
   {
      if (!cbod2.index_x()) cbod2.index_x() = ++maxn;
      if (!cbod2.index_y()) cbod2.index_y() = ++maxn;
      index[ size ] =  cbod2.index_y();
      coeff[ size ] =  pc2;
      size++;
      index[ size ] =  cbod2.index_x();
      coeff[ size ] = -ps2;
      size++;
   }
}
