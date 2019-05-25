/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>,
                      Jan Pytel <jan.pytel@gmail.com>,
                2010, 2013, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/local/local_revision.h>

using namespace GNU_gama::local;
using namespace std;


bool LocalRevision::direction(const Direction* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::distance(const Distance* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::x(const X* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  return true;
}


bool LocalRevision::y(const Y* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  return true;
}


bool LocalRevision::z(const Z* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  return true;
}


bool LocalRevision::z_angle(const Z_Angle* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());  // station point
  if (s == PD.end()) return false;
  // if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy())   return false;
  if (!(*s).second.active_z())  return false;
  if (!(*s).second.test_z())    return false;

  PointData::const_iterator t = PD.find(obs->to());    // target  point
  if (t == PD.end()) return false;
  // if (!(*t).second.active_xy()) return false;
  if (!(*t).second.test_xy())   return false;
  if (!(*t).second.active_z())  return false;
  if (!(*t).second.test_z())    return false;

  return true;
}


bool LocalRevision::s_distance(const S_Distance* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;
  if (!(*c).second.active_z()) return false;
  if (!(*c).second.test_z()) return false;

  return true;
}


bool LocalRevision::h_diff(const H_Diff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_z()) return false;
  if (!(*c).second.test_z()) return false;

  return true;
}


bool LocalRevision::angle(const Angle* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->bs());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  PointData::const_iterator d = PD.find(obs->fs());
  if (d == PD.end()) return false;
  if (!(*d).second.active_xy()) return false;
  if (!(*d).second.test_xy()) return false;

  return true;
}


bool LocalRevision::xdiff(const Xdiff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::ydiff(const Ydiff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::zdiff(const Zdiff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_z()) return false;
  if (!(*c).second.test_z()) return false;

  return true;
}


bool LocalRevision::azimuth(const Azimuth* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}
