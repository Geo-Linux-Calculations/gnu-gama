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

#include <gnu_gama/local/local/linearization.h>

using namespace GaMaLib;
using namespace std;


void LocalLinearization::xdiff(const Xdiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];               // stand point
  LocalPoint& tpoint = PD[obs-> to() ];               // target
  Double df = tpoint.x() - spoint.x();
  // Double p  = M_0 / stdDev();

  // Double w = p*p;                             // weight
  rhs = (obs->value() - df)*1e3;                 // abs. term in millimetres

  size = 0;
  if (spoint.free_xy())
    {
      if (!spoint.index_x()) spoint.index_x() = ++maxn;
      index[ size ] =  spoint.index_x();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_xy())
    {
      if (!tpoint.index_x()) tpoint.index_x() = ++maxn;
      index[ size ] =  tpoint.index_x();
      coeff[ size ] = +1;
      size++;
    }
}


void LocalLinearization::ydiff(const Ydiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];
  LocalPoint& tpoint = PD[obs-> to() ];
  Double df = tpoint.y() - spoint.y();
  // Double p = M_0 / stdDev();

  // Double w = p*p;
  rhs = (obs->value() - df)*1e3;

  size = 0;
  if (spoint.free_xy())
    {
      if (!spoint.index_y()) spoint.index_y() = ++maxn;
      index[ size ] =  spoint.index_y();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_xy())
    {
      if (!tpoint.index_y()) tpoint.index_y() = ++maxn;
      index[ size ] =  tpoint.index_y();
      coeff[ size ] = +1;
      size++;
    }
}


void LocalLinearization::zdiff(const Zdiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];
  LocalPoint& tpoint = PD[obs-> to() ];
  Double df = tpoint.z() - spoint.z();
  // Double p = M_0 / stdDev();

  // Double w = p*p;
  rhs = (obs->value() - df)*1e3;

  size = 0;
  if (spoint.free_z())
    {
      if (!spoint.index_z()) spoint.index_z() = ++maxn;
      index[ size ] =  spoint.index_z();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_z())
    {
      if (!tpoint.index_z()) tpoint.index_z() = ++maxn;
      index[ size ] =  tpoint.index_z();
      coeff[ size ] = +1;
      size++;
    }
}







