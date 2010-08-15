/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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

#include <gnu_gama/local/local/revision.h>

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
