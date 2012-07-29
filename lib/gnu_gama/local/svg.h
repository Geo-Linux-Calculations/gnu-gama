/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This program is free software; you can redistribute it and/or modify
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


#ifndef GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h
#define GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h

#include <gnu_gama/local/gamadata.h>
#include <string>
#include <ostream>

namespace GNU_gama { namespace local {

    class GamaLocalSVG {
    public:

    GamaLocalSVG(PointData& pd, ObservationData& od) : PD(pd), OD(od) {}

      std::string string() const;
      void write(std::ostream& str) const;

    private:
      const PointData&       PD;
      const ObservationData& OD;
    };

}}

#endif
