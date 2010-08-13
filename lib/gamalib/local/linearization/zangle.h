/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Jan Pytel <pytel@gama.fsv.cvut.cz>

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
// #include <gamalib/local/pobs/bearing.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::z_angle(const Z_Angle* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   // Double s, d, sd;
   // bearing_distance(PD[obs->from()], PD[obs->to()], s, d);
   // bearing_sdistance(PD[obs->from()], PD[obs->to()], s, sd);

   Double dx = cbod.x() - sbod.x();
   Double dy = cbod.y() - sbod.y();
   Double dz = cbod.z() - sbod.z();

   Double d2 = dx*dx + dy*dy;
   Double d  = sqrt(d2);
   Double sd = sqrt(d2 + dz*dz);
   if (d == 0 || sd == 0)
     throw GaMaLib::Exception(T_POBS_zero_or_negative_zenith_angle);

   Double k  = 10*R2G/(d*sd*sd);

   // Double p = M_0 / stdDev();
   Double px =  k * dz * dx;
   Double py =  k * dz * dy;
   Double pz = -k *  d *  d;
   // Double w = p*p;                  // weight
   
   Double za = acos(dz/sd);

   if (obs->value() > M_PI) za = 2*M_PI - za; 
   Double a  = (obs->value() - za);

   rhs = a * R2CC; // abs. term in cc
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
