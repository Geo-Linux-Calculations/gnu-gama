/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local_Bod_Mer_VYPORPOS_H
#define gama_local_Bod_Mer_VYPORPOS_H

#include <functional>
#include <vector>
#include <gnu_gama/local/pobs/bearing.h>
#include <gnu_gama/local/gamadata.h>

namespace GNU_gama { namespace local {

class Orientation {

   PointData&       PL;
   ObservationList& OL;

public:

   Orientation(PointData& p, ObservationList& o) : PL(p), OL(o) {}

   // L1 estimate of the standpoint orientation (iter. to the first direction)
   void orientation(ObservationList::const_iterator&, double&, int&);

   // add all possible orientations for the observation list
   void add_all();
};

}}
#endif
