/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Jan Pytel <pytel@gama.fsv.cvut.cz>

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

#include <gnu_gama/local/local/linearization.h>
// #include <gnu_gama/local/local/pobs/bearing.h>

using namespace GNU_gama::local;
using namespace std;


void LocalLinearization::s_distance(const S_Distance* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   // Double s, sd;
   // bearing_sdistance(PD[obs->from()], PD[obs->to()], s, sd);
   // Double p = M_0 / stdDev();
   Double dx = cbod.x() - sbod.x();
   Double dy = cbod.y() - sbod.y();
   Double dz = cbod.z() - sbod.z();
   Double sd = sqrt(dx*dx + dy*dy + dz*dz);
   if (sd == 0)
     throw GNU_gama::local::Exception(T_POBS_zero_or_negative_slope_distance);

   Double px = dx / sd;
   Double py = dy / sd;
   Double pz = dz / sd;

   // Double w = p*p;                // weight
   rhs = (obs->value() - sd)*1e3;    // abs. term in millimetres

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -py;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = -px;
      size++;
   }
   if (sbod.free_z())
   {
      if (!sbod.index_z()) sbod.index_z() = ++maxn;
      index[ size ] = sbod.index_z();
      coeff[ size ] = -pz;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = py;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = px;
      size++;
   }
   if (cbod.free_z())
   {
      if (!cbod.index_z()) cbod.index_z() = ++maxn;
      index[ size ] = cbod.index_z();
      coeff[ size ] = pz;
      size++;
   }
}

