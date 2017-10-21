/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>
                 2014  Maxime Le Moual <maxime.le-moual@ensg.eu>

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
 * \author Maxime Le Moual
 */

#ifndef GAMA_LOCAL_SVG_Gama_Local_Svg_gama_local_svg_h
#define GAMA_LOCAL_SVG_Gama_Local_Svg_gama_local_svg_h

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
      bool drawPointIDs() const { return tst_draw_point_ids; }
      /** Set drawing of point IDs. */
      void setDrawPointIDs(bool p) {tst_draw_point_ids = p; }

      /** Draw ellipses. */
      bool drawEllipses() const { return tst_draw_ellipses; }
      /** Set drawing of ellipses. */
      void setDrawEllipses(bool p) {tst_draw_ellipses = p; }

      /** Draw observations. */
      bool drawObservations() const { return tst_draw_observations; }
      /** Set drawing of point symbols. */
      void setDrawObservations(bool p) {tst_draw_observations = p; }

      /** Draw axes. */
      bool drawAxes() const { return tst_draw_axes; }
      /** Set drawing of exes. */
      void setDrawAxes(bool p) {tst_draw_axes = p; }

      /** Font size. */
      double fontSize() const { return fontsize; }
      /** Set font size */
      void setFontSize(double p) { fontsize = p; tst_implicit_size = false; }

      /** Symbol size. */
      double symbolSize() const { return symbolsize; }
      /** Set symbol size */
      void setSymbolSize(double p) { symbolsize = p; tst_implicit_size = false; }

      /** SVG stroke width. */
      double strokeWidth() const { return strokewidth; }
      /** Set SVG stroke size */
      void setStrokeWidth(double p) { strokewidth = p; tst_implicit_size = false; }

      /** Fixed points' symbol. */
      std::string fixedSymbol() const { return fixedsymbol; }
      /** Set symbol for fixed points */
      void setFixedSymbol(std::string p) { fixedsymbol = p; }

      /** Constrained points' symbol. */
      std::string constrainedSymbol() const { return constrainedsymbol; }
      /** Set symbol for fixed points */
      void setConstrainedSymbol(std::string p) { constrainedsymbol = p; }

      /** Free points' symbol. */
      std::string freeSymbol() const { return freesymbol; }
      /** Set symbol for fixed points */
      void setFreeSymbol(std::string p) { freesymbol = p; }

      /** Fixed points' fill. */
      std::string fixedFill() const { return fixedfill; }
      /** Set fill for fixed points */
      void setFixedFill(std::string p) { fixedfill = p; }

      /** Constrained points' fill. */
      std::string constrainedFill() const { return constrainedfill; }
      /** Set fill for constrained points */
      void setConstrainedFill(std::string p) { constrainedfill = p; }

      /** Free points' fill. */
      std::string freeFill() const { return freefill; }
      /** Set symbol for fixed points */
      void setFreeFill(std::string p) { freefill = p; }

      /** Restores default program settings derived from given coordinates set. */
      void restoreDefaults();

      /** Minimal size of font size and stroke width. */
      double minimalSize() const { return minimalsize; }

      /** Minimal size of font size and stroke width. */
      void setMinimalSize(double p = 0.0001) const { minimalsize = p > 0 ? p : 0.0001; }

      /** Error ellipse scale. */
      double ellipsesScale() const { return ellipsescale; }

      /** Error ellipse scale. */
      void setEllipsesScale(double p) const { ellipsescale = p; tst_implicit_size = false; }

      /** Transformation from geodetic to SVG coordinates (x, y)
       * \code{.cpp}
       * xsvg = t11*x + t12*y + tx;
       * ysvg = t21*x + t22*y + ty;
       * \endcode
       */
      void SvgCoordinates(double& t11, double& t12,
                          double& t21, double& t22,
                          double& tx,  double& ty) const;

    private:
      LocalNetwork&          IS;
      const PointData&       PD;
      const ObservationData& OD;
      const double y_sign;   // consistent coordinates +1, inconsistent -1

      mutable std::ostream*  svg;

      mutable double T11, T12, T21, T22, Tx, Ty;

      // SVG coordinates bounding box and offset
      mutable bool tst_implicit_size;
      mutable double  minx, maxx, miny, maxy, offset;
      mutable double ab_median, minimalsize, ellipsescale;
      void svg_xy(const LocalPoint& point, double& x, double& y) const;
      void svg_draw_point  (const PointID& pid, const LocalPoint& point) const;
      void svg_point_shape (double x, double y,
                std::string type, std::string shape,
                            std::string fillColor) const;
      void svg_init        () const;
      void svg_axes_xy     () const;
      void svg_points      () const;
      void svg_observations() const;
      void svg_ellipse(const PointID& pid, double& a, double &b, double& alpha) const;

      mutable double fontsize, symbolsize, strokewidth;
      mutable bool tst_draw_axes, tst_draw_point_symbols, tst_draw_point_ids,
        tst_draw_ellipses, tst_draw_observations;
      mutable std::string  fixedsymbol, fixedfill, constrainedsymbol,
        constrainedfill, freesymbol, freefill;


      /* helper svg point class */

      class Point {
      public:
        double x, y;

        Point() {}
        Point(double a, double b) : x(a), y(b) {}

        friend std::ostream& operator<< (std::ostream& ostr, const Point& p) {
            return ostr << p.x << "," << p.y << " ";
        }
        friend Point operator+(const GamaLocalSVG::Point& a, const Point& b) {
          return Point(a.x+b.x, a.y+b.y);
        }
        friend Point operator-(const Point& a, const Point& b)
        {
          return Point(a.x-b.x, a.y-b.y);
        }
        friend Point operator*(const Point& a, double q)
        {
          return Point(a.x*q, a.y*q);
        }
      };

      mutable Point TX, TY;

      void sett(double tr11, double tr12, double tr21, double tr22) const
      {
        TX = Point(tr11, tr12);
        TY = Point(tr21, tr22);
      }
    };
}}

#endif
