/*
  GNU Gama -- Approximate coordinates by Polar Method
  Copyright (C) 2019  Petra Millarova <petramillarova@gmail.com>

  This file is part of the GNU Gama C++ library.

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMA_LOCAL_ACORDSTATISTICS_H
#define GAMA_LOCAL_ACORDSTATISTICS_H

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama { namespace local
{
  class AcordStatistics
  {
  public:
    AcordStatistics(PointData&, ObservationData&);
    void execute();
    
    int observations;
    int  given_xy, given_z, given_xyz;
    int  computed_xy, computed_z, computed_xyz;
    int  total_xy, total_z, total_xyz;
    bool missing_coordinates;
    PointData & PD;
    ObservationData& OD;
  };

}} //namespace GNU_gama::local


#endif
