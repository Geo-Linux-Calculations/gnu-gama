/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2013, 2015  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/** \file gamadata.cpp
 * \brief #GNU_gama::local::PointData class implementation
 *
 * \author Ales Cepek
 */

#include <gnu_gama/local/gamadata.h>

double GNU_gama::local::PointData::xNorthAngle() const
{
  int lh = 0;
  switch (local_coordinate_system)
    {
    case EN: case ES:  lh = 300; break;
    case NW: case NE:  lh = 400; break;
    case SE: case SW:  lh = 200; break;
    case WS: case WN:  lh = 100; break;
    default:;
    }

  if (right_handed_angles()) lh = 400 - lh;
  if (lh == 400) lh = 0;

  return lh*G2R;
}



using PointID = GNU_gama::local::PointID;
using LocalPoint = GNU_gama::local::LocalPoint;

std::map <PointID, LocalPoint>::iterator
GNU_gama::local::PointData::begin()
{
  return points.begin();
}

std::map <PointID, LocalPoint>::iterator
GNU_gama::local::PointData::end()
{
  return points.end();
}

std::map <PointID, LocalPoint>::const_iterator
GNU_gama::local::PointData::begin() const
{
  return points.begin();
}

std::map <PointID, LocalPoint>::const_iterator
GNU_gama::local::PointData::end() const
{
  return points.end();
}

GNU_gama::local::LocalPoint&
GNU_gama::local::PointData::operator[](const PointID& id)
{
  return points[id];
}

std::map <PointID, LocalPoint>::iterator
GNU_gama::local::PointData::find(const PointID& id)
{
  return points.find(id);
}

std::map <PointID, LocalPoint>::const_iterator
GNU_gama::local::PointData::find(const PointID& id) const
{
  return points.find(id);
}

std::map <PointID, LocalPoint>::iterator
GNU_gama::local::PointData::erase(
    std::map <PointID, LocalPoint>::const_iterator first,
    std::map <PointID, LocalPoint>::const_iterator last)
{
  return points.erase(first, last);
}

unsigned
GNU_gama::local::PointData::size() const
{
  return points.size();
}

bool
GNU_gama::local::PointData::empty() const
{
  return points.empty();
}
