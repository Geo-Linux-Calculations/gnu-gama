/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>
                 2014  Maxime Le Moual <maxime.le-moual@ensg.eu>
                 2018  Ales Cepek <cepek@gnu.org>

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

/** \file svg.cpp
 * \brief #GNU_gama::local::GamaLocalSVG class implementation
 *
 * \author Ales Cepek
 * \author Maxime Le Moual
 */

#include <gnu_gama/local/svg.h>
#include <gnu_gama/radian.h>
#include <algorithm>
#include <sstream>
#include <utility>
#include <set>

using namespace GNU_gama::local;

GamaLocalSVG::GamaLocalSVG(LocalNetwork* is)
  : IS(*is), PD(is->PD), OD(is->OD),
    y_sign(is->y_sign())
{
  restoreDefaults();
}

void GamaLocalSVG::restoreDefaults()
{
  tst_implicit_size = true;
  tst_draw_axes = true;
  tst_draw_point_symbols = true;
  tst_draw_point_ids = true;
  tst_draw_ellipses = true;
  tst_draw_observations = true;

  fixedsymbol = "triangle";         fixedfill = "blue";
  constrainedsymbol = "circle";     constrainedfill = "green";
  freesymbol = "circle";            freefill = "yellow";

  svg_init();
}

void GamaLocalSVG::svg_init() const
{
  /* Original coordinate unit vectors x and y (EN, NW, ...) expressed
     in SVG coordinate system (x right, y down).

     For example in NE coordinate system axis vector x (1, 0) {points
     north} is expressed in SVG as (0, -1) {x points up}; similarly
     canonical coordinate vector y (0, 1) {pointing east} in SVG
     corresponds to (1, 0) {points right}; and parameters of are
     sett(0,-1,1,0) */
  switch (PD.local_coordinate_system)
    {
      // right handed
    case LocalCoordinateSystem::EN: sett( 1, 0, 0,-1); break;
    case LocalCoordinateSystem::NW: sett( 0,-1,-1, 0); break;
    case LocalCoordinateSystem::SE: sett( 0, 1, 1, 0); break;
    case LocalCoordinateSystem::WS: sett(-1, 0, 0, 1); break;
      // left handed
    case LocalCoordinateSystem::NE: sett( 0,-1, 1, 0); break;
    case LocalCoordinateSystem::SW: sett( 0, 1,-1, 0); break;
    case LocalCoordinateSystem::ES: sett( 1, 0, 0, 1); break;
    case LocalCoordinateSystem::WN: sett(-1, 0, 0,-1); break;
    default:
      sett(0,0,0,0);
    }


  // clear global transformation parameters
  minx = maxx = miny = maxy = offset = 0;


  // median of error ellipsoid semiaxes is needed for computing main bounding box
  std::vector<double> abmed;
  for (PointData::const_iterator i=PD.begin(), e=PD.end(); i!=e; ++i)
    {
      PointID    pid   = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy()) continue;

      if (tst_draw_ellipses && IS.is_adjusted() && !point.fixed_xy())
      {
          double a, b, alpha;
          IS.std_error_ellipse(pid, a, b, alpha);
          abmed.push_back(a);
          abmed.push_back(b);
       }
  }

  ab_median = 0;
  if (unsigned n = abmed.size())
    {
      std::sort(abmed.begin(), abmed.end());
      if (n % 2) ab_median =  abmed[n/2];
      else       ab_median = (abmed[n/2] + abmed[n/2-1])/2;
    }
  if (ab_median <= 0) ab_median = 1;   // handle unrealistic data


  // main bounding box
  if (tst_implicit_size) ellipsescale = 1.0;

  bool first_point = true;
  // tminx, tmaxx, tminy, tmaxy are set for the first point
  // initialization here is just to remove compiler warning
  double x, y, tminx {0}, tmaxx {0}, tminy {0}, tmaxy {0};
  for (int iter=1; iter<=2; iter++)
  {
  for (PointData::const_iterator i=PD.begin(), e=PD.end(); i!=e; ++i)
    {
      PointID    pid   = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy()) continue;

      T11 = TX.x;   T12 = TY.x*y_sign;
      T21 = TX.y;   T22 = TY.y*y_sign;
      Tx  = 2*offset - minx;
      Ty  = 2*offset - miny;

      svg_xy(point, x, y);

      double dx = 0, dy = 0;
      if (tst_draw_ellipses && IS.is_adjusted() && !point.fixed_xy())
      {
          double a, b, alpha;
          svg_ellipse(pid, a, b, alpha);

          // bounding box with 10% extra space for huge ellipses
          double t  = atan2(-b*sin(alpha), a*cos(alpha));
          double u  = atan2( b*cos(alpha), a*sin(alpha));
          dx = 1.1*abs(a*cos(t)*cos(alpha) - b*sin(t)*sin(alpha));
          dy = 1.1*abs(b*sin(u)*cos(alpha) + a*cos(u)*sin(alpha));
      }

      if (first_point)
        {
          tminx = tmaxx = x;
          tminy = tmaxy = y;
          first_point = false;
        }
      else
        {
          if (x - dx < tminx) tminx = x - dx;
          if (x + dx > tmaxx) tmaxx = x + dx;
          if (y - dy < tminy) tminy = y - dy;
          if (y + dy > tmaxy) tmaxy = y + dy;

          maxx = std::abs(tmaxx-tminx);
          maxy = std::abs(tmaxy-tminy);
          offset = (maxx + maxy)/2*0.05;
        }

    }
  }  // iter

  minx = tminx;
  miny = tminy;
  maxx = std::abs(tmaxx-tminx);
  maxy = std::abs(tmaxy-tminy);
  offset = (maxx + maxy)/2*0.05;

  if (offset == 0) offset = 100;

  T11 = TX.x;   T12 = TY.x*y_sign;
  T21 = TX.y;   T22 = TY.y*y_sign;
  Tx  = 2*offset - minx;
  Ty  = 2*offset - miny;

  // font and symbol sizes must be initialized only once
  // int the constructer, otherwise they could not be setup
  // by the interface

  if (tst_implicit_size)
  {
    setMinimalSize();
    ellipsescale = 1.0;

    fontsize = offset*0.4;
    if (fontsize < minimalsize) fontsize = minimalsize;
    symbolsize  = fontsize;
    strokewidth = offset*0.01;
    if (strokewidth < minimalsize) strokewidth = minimalsize;
  }

#if 0
  std::cerr << "### initial implicit SVG units\n";
  std::cerr << "### minx        = " << minx << "\n";
  std::cerr << "### maxx        = " << maxx << "\n";
  std::cerr << "### miny        = " << miny << "\n";
  std::cerr << "### maxy        = " << maxy << "\n";
  std::cerr << "### offset      = " << offset << "\n";
  std::cerr << "### fontsize    = " << fontsize << "\n";
  std::cerr << "### symbolsize  = " << symbolsize << "\n";
  std::cerr << "### strokewidth = " << strokewidth << "\n";
#endif
}

void GamaLocalSVG::SvgCoordinates(double &t11, double &t12,
                                  double &t21, double &t22,
                                  double &tx,  double &ty) const
{
  t11 = T11;  t12 = T12;  t21 = T21;  t22 = T22;  tx = Tx;  ty = Ty;
}

void GamaLocalSVG::svg_xy(const LocalPoint& point, double& x, double& y) const
{
  x = T11*point.x() + T12*point.y() + Tx;
  y = T21*point.x() + T22*point.y() + Ty;
}

std::string GamaLocalSVG::string() const
{
  std::ostringstream stream;
  draw(stream);
  return stream.str();
}

void GamaLocalSVG::draw(std::ostream& output_stream) const
{
  svg = &output_stream;

  svg_init();

  const double wmaxx = 1.0*(maxx + 4*offset);
  const double wmaxy = 1.0*(maxy + 4*offset);

  *svg << "<?xml version='1.0' encoding='utf-8' standalone='no'?>\n";
  *svg <<
    "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'\n"
    "  'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
    "<svg version='1.1' "
    " width='"  << wmaxx << "'"
    " height='" << wmaxy << "'"
    " xmlns='http://www.w3.org/2000/svg'"
    " xmlns:xlink='http://www.w3.org/1999/xlink' >\n";

#if 0
  *svg << "<rect x='0' y='0' "
       << "width ='" << wmaxx << "' "
       << "height='" << wmaxy << "' "
       << "style='fill:none;stroke:blue;stroke-width:"
       << strokewidth << ";' />\n";

  *svg << "<rect x='" << 2*offset << "' y='" << 2*offset << "' "
       << "width ='" << maxx << "' " << "height='" << maxy << "' "
       << "style='fill:grey;stroke:black;stroke-width:"
       << strokewidth << ";opacity:0.1' />\n";
#endif

  svg_axes_xy();
  svg_observations();
  svg_points();

  *svg << "</svg>\n";
}

void GamaLocalSVG::svg_point_shape (double x, double y,
                                    std::string /*type*/,  // Fixed Constrained Free
                                    std::string shape,     // triangle, circle
                                    std::string fillColor
                                    ) const
{
   if (shape == "triangle")
   {
      Point P(x, y);

      *svg << "<polyline points='"
           << P + Point(-0.5, 0.28868) * (1.2*symbolsize)
           << P + Point( 0.5, 0.28868) * (1.2*symbolsize)
           << P + Point( 0.0,-0.57735) * (1.2*symbolsize)
           << P + Point(-0.5, 0.28868) * (1.2*symbolsize) << "' ";
   }
   else if (shape == "circle")
   {
      *svg <<  "<circle cx='" << x << "' cy='" << y << "' "
           << "r='" << 0.5*symbolsize << "' ";
   }

   *svg << " style='" << "stroke:black;fill:" << fillColor
        << ";stroke-width:" << strokewidth<< ";'/>\n";
}

void GamaLocalSVG::svg_axes_xy() const
{
  if (!tst_draw_axes) return;

  Point  P, X, Y, CX, X1, X2, CY, Y1, Y2;
  std::string alignx, aligny;
  const double arrowlength = 4*offset;
  const double caption = 0.4*offset;
  const double arrowheadlong  = 1.5*0.3*offset;
  const double arrowheadshort = 0.3*0.3*offset;

  const double left   = offset;
  const double right  = maxx + 3*offset;
  const double top    = offset;
  const double bottom = maxy + 3*offset;

  switch (PD.local_coordinate_system)
    {
      // right handed
    case LocalCoordinateSystem::EN: P = Point(left,  bottom);  break;
    case LocalCoordinateSystem::NW: P = Point(right, bottom);  break;
    case LocalCoordinateSystem::SE: P = Point(left,  top);     break;
    case LocalCoordinateSystem::WS: P = Point(right, top);     break;

      // left handed
    case LocalCoordinateSystem::NE: P = Point(left,  bottom);  break;
    case LocalCoordinateSystem::SW: P = Point(right, top);     break;
    case LocalCoordinateSystem::ES: P = Point(left,  top);     break;
    case LocalCoordinateSystem::WN: P = Point(right, bottom);  break;

    default:
      return;
    }

  X  = P + TX*arrowlength;
  Y  = P + TY*arrowlength;
  CX = X + TX*caption;
  CY = Y + TY*caption;
  X1 = X - TX*arrowheadlong + TY*arrowheadshort;
  X2 = X - TX*arrowheadlong - TY*arrowheadshort;
  Y1 = Y - TY*arrowheadlong + TX*arrowheadshort;
  Y2 = Y - TY*arrowheadlong - TX*arrowheadshort;
  if (TX.x)
    {
      alignx = "dominant-baseline: central;";
      aligny = "text-anchor: middle;";
    }
  else
    {
      aligny = "dominant-baseline: central;";
      alignx = "text-anchor: middle;";
    }

  *svg << "<g style='stroke:black;stroke-width:" << strokewidth << ";'>\n";

  // axes
  *svg << "<polyline points='"
       << X << P << Y
       << "' style='fill:none;' />\n";

  // arrows
  *svg << "<polyline points='"
       << X1 << X << X2
       << "' style='fill:none;' />\n";
  *svg << "<polyline points='"
       << Y1 << Y << Y2
       << "' style='fill:none;' />\n";

  *svg << "</g>\n";

  // captions
  *svg << "<text font-family='sans-serif' "
       << "font-size='" << fontsize <<  "' "
       << "x='" << CX.x << "' y='" << CX.y << "' "
       << "style='" << alignx << "'>X</text>\n";
  *svg << "<text font-family='sans-serif' "
       << "x='" << CY.x << "' y='" << CY.y << "' "
       << "font-size='" << fontsize <<  "' "
       << "style='" << aligny << "'> Y</text>\n";
}

void GamaLocalSVG::svg_points() const
{
  for (PointData::const_iterator i=PD.begin(), e=PD.end(); i!=e; ++i)
    {
      PointID    pid   = i->first;
      LocalPoint point = i->second;

      // skip points that are not part of the adjustment or do not have xy
      if (!point.active_xy() || !point.test_xy()) continue;

      svg_draw_point(pid, point);
   }
}

void GamaLocalSVG::svg_observations() const
{
  if (!tst_draw_observations) return;

  typedef std::pair<PointID, PointID> PairID;
  typedef std::set<std::pair<PointID, PointID> > Pairs;

  Pairs pairs;

  typedef std::list<GNU_gama::Cluster<Observation>*> ClusterList;
  const ClusterList& clusters = OD.clusters;
  for (ClusterList::const_iterator
         c=clusters.begin(), ce=clusters.end(); c!=ce; ++c)
    {
      for (ObservationList::const_iterator
             i=(*c)->observation_list.begin(),
             e=(*c)->observation_list.end(); i!=e; ++i)
        {
          if (const Observation* b = dynamic_cast<const Observation*>(*i))
            {
              if (b->active())
                {
                  PointID from = b->from();
                  PointID to   = b->to();
                  if (from < to) pairs.insert(PairID(from, to));
                  else           pairs.insert(PairID(to, from));
                }

              if (const Angle* b = dynamic_cast<const Angle*>(*i))
                {
                  PointID from = b->from();
                  PointID to   = b->fs();
                  if (from < to) pairs.insert(PairID(from, to));
                  else           pairs.insert(PairID(to, from));
                }
            }
        }
    }

  for (Pairs::const_iterator i=pairs.begin(), e=pairs.end(); i!=e; ++i)
    {
      // operator[] cannot be used for a const map
      PointData::const_iterator a = PD.find(i->first);
      PointData::const_iterator b = PD.find(i->second);
      if (a == PD.end()) continue;
      if (b == PD.end()) continue;

      const LocalPoint& A = a->second;
      const LocalPoint& B = b->second;
      if (!A.test_xy() || !B.test_xy()) continue;

      double x1, y1, x2, y2;
      svg_xy(A, x1, y1);
      svg_xy(B, x2, y2);

      *svg << "<line x1='"<<x1<<"' y1='"<<y1<<"' x2='"<<x2<<"' y2='"<<y2<<"' "
           << "style='stroke:black;stroke-width:" << strokewidth << ";' />\n";
    }
}

void GamaLocalSVG::svg_draw_point(const PointID& pid,
                                  const LocalPoint& point) const
{
      const double psize = 0.4*fontsize;

      double x, y;
      svg_xy(point, x, y);

      if (tst_draw_point_symbols)
        {
          if (point.fixed_xy())
            svg_point_shape(x, y, "Fixed", fixedsymbol, fixedfill);
          else if (point.constrained_xy())
            svg_point_shape(x, y, "Constrained",
                            constrainedsymbol, constrainedfill);
          else
            svg_point_shape(x, y, "Free",  freesymbol,  freefill);
        }

      if (tst_draw_point_ids)
        {
          *svg << "<text font-family='sans-serif' "
               << "transform='translate(" << 2*psize << " " << 2*psize <<")' "
               << "font-size='" << fontsize <<  "' "
               << "x='" << x << "' y='" << y << "' "
               << ">" << pid.str() << "</text>\n";
        }

      if (tst_draw_ellipses && IS.is_adjusted() && !point.fixed_xy())
        {
          double a, b, alpha;
          svg_ellipse(pid, a, b, alpha);
#if 0
           double t  = atan2(-b*sin(alpha), a*cos(alpha));
           double u  = atan2( b*cos(alpha), a*sin(alpha));
           double dx = abs(a*cos(t)*cos(alpha) - b*sin(t)*sin(alpha));
           double dy = abs(b*sin(u)*cos(alpha) + a*cos(u)*sin(alpha));
           *svg << "<polygon points='"
                << x-dx <<"," << y-dy << " "
                << x+dx <<"," << y-dy << " "
                << x+dx <<"," << y+dy << " "
                << x-dx <<"," << y+dy << " "
                << "' style='fill-opacity:0;stroke:lime;stroke-width:"
                << 0.5 << "' />";
#endif
          alpha *= RAD_TO_DEG;   // see gnu_gama/radian.h

          *svg << "<ellipse  " //cx='" << x << "' cy='" << y << "' "
               << "rx='" << a << "' ry='" << b << "' "
               << "transform='translate(" << x << " " << y << ") "
               << "rotate("<< alpha << ")' "
               << "style='stroke:grey;stroke-width:"
               << strokewidth << ";fill:none;' />\n";
        }
}

void GamaLocalSVG::svg_ellipse(const PointID& pid, double &a, double &b, double &alpha) const
{
    IS.std_error_ellipse(pid, a, b, alpha);

    if (PD.right_handed_angles()) alpha = -alpha;

    a *= offset/ab_median*ellipsescale;
    b *= offset/ab_median*ellipsescale;

    switch (PD.local_coordinate_system)
      {
        // right handed
      case LocalCoordinateSystem::EN: ; break;
      case LocalCoordinateSystem::NW: alpha += M_PI/2; break;
      case LocalCoordinateSystem::SE: alpha += M_PI/2; break;
      case LocalCoordinateSystem::WS: ; break;
        // left handed
      case LocalCoordinateSystem::NE: alpha += M_PI/2; break;
      case LocalCoordinateSystem::SW: alpha += M_PI/2; break;
      case LocalCoordinateSystem::ES: break;
      case LocalCoordinateSystem::WN: break;
      default:
        ;
      }

     while (alpha < 0)      alpha += 2*M_PI;
     while (alpha > 2*M_PI) alpha -= 2*M_PI;
}
