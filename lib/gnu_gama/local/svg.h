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

/** \file svg.h
 * \brief #GNU_gama::local::GamaLocalSVG class header file
 *
 * \author Ales Cepek
 */

#ifndef GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h
#define GAMA_LOCAL_SVG__Gama_Local_Svg__gama_local_svg__h

#include <gnu_gama/xml/localnetworkxml.h>
#include <string>
#include <ostream>

namespace GNU_gama { namespace local {

    /** Simple class for generating schema of a given local network in
     *  SVG subset implemented on Qt platform
     */
    class GamaLocalSVG {
    public:

      /** \param lnet pointer to local network object */
      GamaLocalSVG(LocalNetwork* lnet);

      /** Returns SVG image as a std::string. */
      std::string string() const;
      /** Writes SVG image on to a standard stream */
      void draw(std::ostream& output_stream) const;

      /** Draw point symbols. */
      bool drawPointSymbols() { return tst_draw_point_symbols; }
      /** Set drawing of point symbols. */
      void setDrawPointSymbols(bool p) {tst_draw_point_symbols = p; }

      /** Draw point IDs. */
      bool drawPoinsIDs() const { return tst_draw_point_ids; }
      /** Set drawing of point IDs. */
      void setDrawPointIDs(bool p) {tst_draw_point_ids = p; }

      /** Draw ellipses. */
      bool drawEllipses() const { return tst_draw_ellipses; }
      /** Set drawing of ellipses. */
      void setDrawEllipses(bool p) {tst_draw_ellipses = p; }

      /** Draw observations. */
      bool drawPoinsObservations() const { return tst_draw_observations; }
      /** Set drawing of point symbols. */
      void setDrawObservations(bool p) {tst_draw_observations = p; }

      /** Draw axes. */
      bool drawAxes() const { return tst_draw_axes; }
      /** Set drawing of exes. */
      void setDrawAxes(bool p) {tst_draw_axes = p; }

      /** Font size. */
      int fontSize() const { return fontsize; }
      /** Set font size */
      void setFontSize(int p) { fontsize = p;}

      /** Symbol size. */
      int symbolSize() const { return symbolsize; }
      /** Set symbol size */
      void setSymbolSize(int p) { symbolsize = p;}

      /** SVG stroke width. */
      int strokeWidth() const { return strokewidth; }
      /** Set SVG stroke size */
      void setStrokeWidth(int p) { strokewidth = p;}

      /** Fixed points' symbol. */
      std::string fixedSymbol() const { return fixedsymbol; }
      /** Set symbol for fixed points */
      void setFixedSymbol(std::string p) { fixedsymbol = p;}

      /** Constrained points' symbol. */
      std::string constrainedSymbol() const { return constrainedsymbol; }
      /** Set symbol for fixed points */
      void setConstrainedSymbol(std::string p) { constrainedsymbol = p;}

      /** Free points' symbol. */
      std::string freeSymbol() const { return freesymbol; }
      /** Set symbol for fixed points */
      void setFreeSymbol(std::string p) { freesymbol = p;}

      /** Fixed points' fill. */
      std::string fixedFill() const { return fixedfill; }
      /** Set fill for fixed points */
      void setFixedFill(std::string p) { fixedfill = p;}

      /** Constrained points' fill. */
      std::string constrainedFill() const { return constrainedfill; }
      /** Set fill for constrained points */
      void setConstrainedFill(std::string p) { constrainedfill = p;}

      /** Free points' fill. */
      std::string freeFill() const { return freefill; }
      /** Set symbol for fixed points */
      void setFreeFill(std::string p) { freefill = p;}

    private:
      LocalNetwork&          IS;
      const PointData&       PD;
      const ObservationData& OD;
      const double ysign;    // consistent coordinates +1, inconsistent -1

      mutable std::ostream*  svg;

      // SVG coordinates bounding box and offset
      mutable bool not_in_constructor;
      mutable int  minx, maxx, miny, maxy, offset;
      mutable double ab_median;
      void svg_xy(const LocalPoint& point, double& x, double& y) const;
      void svg_draw_point  (const PointID& pid, const LocalPoint& point) const;
      void svg_point_shape (double x, double y,
                std::string type, std::string shape,
                            std::string fillColor) const;
      void svg_init        () const;
      void svg_axes_xy     () const;
      void svg_points      () const;
      void svg_observations() const;

      mutable int fontsize, symbolsize, strokewidth;
      mutable bool tst_draw_axes, tst_draw_point_symbols, tst_draw_point_ids,
        tst_draw_ellipses, tst_draw_observations;
      mutable std::string  fixedsymbol, fixedfill, constrainedsymbol,
        constrainedfill, freesymbol, freefill;
    };
}}

#endif
