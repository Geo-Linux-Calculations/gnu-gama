/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2013  Ales Cepek <cepek@gnu.org>

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

/** \file approx_azimuths.h
 * \brief Approximate coordinates from azimuths and distances
 *
 * \author Ales Cepek
 */

#ifndef gama_local_approximate_coordinates_from_Azimuths_and_Distances_H
#define gama_local_approximate_coordinates_from_Azimuths_and_Distances_H

#include <gnu_gama/local/gamadata.h>
#include <map>
#include <ostream>

namespace GNU_gama { namespace local {

    class ApproximateAzimuths
    {
    public:

      ApproximateAzimuths(PointData& b, ObservationData& m);
      void execute();
      void print(std::ostream&) {}

    private:

      PointData&          PD;
      ObservationData&    OD;

      typedef std::pair<PointID, PointID> Key;
      typedef std::map <Key, double>      Map;
      Map map;

    };

}}

#endif
