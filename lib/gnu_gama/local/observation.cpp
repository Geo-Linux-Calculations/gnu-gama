/*
    GNU Gama C++ library
    Copyright (C) 2000, 2010  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU Gama C++ library

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

#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/cluster.h>

namespace GNU_gama { namespace local
{
  // the static member Observation::gons was introduced in 1.7.09 to
  // enable selection between grades and degrees in virtual functions
  // Observation::write() with minimal changes in existing old code of
  // GNU_gama::local

  bool Observation::gons = true;
}}

using namespace GNU_gama::local;
using namespace std;

bool  Observation::check_std_dev() const
{
  return cluster && cluster->covariance_matrix.dim();
}


Double Observation::stdDev() const
{
   return cluster->stdDev(cluster_index);
}

Double Direction::orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->orientation();
}

void Direction::set_orientation(Double p)
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->set_orientation(p);
}

bool Direction::test_orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->test_orientation();
}

void Direction::delete_orientation()
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->delete_orientation();
}

void Direction::index_orientation(int n)
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->index_orientation(n);
}

int Direction::index_orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->index_orientation();
}


