/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

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


#ifndef GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h
#define GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h

#include <gnu_gama/xml/localnetwork.h>
#include <string>
#include <ostream>

namespace GNU_gama { namespace local {

    class GamaLocalSVG {
    public:

      GamaLocalSVG(LocalNetwork* is);

      std::string string() const;
      void draw(std::ostream& output_stream) const;

    private:
      LocalNetwork&          IS;
      const PointData&       PD;
      const ObservationData& OD;
      const double ysign;    // consistent coordinates +1, inconsistent -1

      mutable std::ostream*  svg;

      // SVG coordinates bounding box and offset
      mutable int  minx, maxx, miny, maxy, offset, fontsize_;
      mutable double ab_median;
      void svg_xy(const LocalPoint& point, double& x, double& y) const;
      void svg_draw_point  (const PointID& pid, const LocalPoint& point) const;
      void svg_point_shape (std::string type, int shape,
                            std::string fillColor) const;
      void svg_init        () const;
      void svg_symbols     () const;
      void svg_axes_xy     () const;
      void svg_points      () const;
      void svg_observations() const;

    protected:
      mutable int fontsize;
      mutable bool tst_draw_axes, tst_draw_point_symbols, tst_draw_point_ids,
        tst_draw_ellipses, tst_draw_observations;
    };
}}

#endif
