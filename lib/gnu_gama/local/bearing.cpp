/*
  GNU Gama C++ library
  Copyright (C) 1999, 2019  Ales Cepek <cepek@fsv.cvut.cz>

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

#include <cmath>
#include <gnu_gama/local/bearing.h>
#include <gnu_gama/local/float.h>
#include <gnu_gama/local/gamadata.h>

namespace GNU_gama { namespace local {


void bearing_distance(const LocalPoint& a, const LocalPoint& b,
                      double& br, double& d)
{
  bearing_distance(a.y(), a.x(), b.y(), b.x(), br, d);
}


void bearing_distance(double ya, double xa, double yb, double xb,
                      double& b, double& d)
{
   double dy = yb - ya;
   double dx = xb - xa;

   d  = std::sqrt(dy*dy + dx*dx);

   // avoid exception from std::atan2
   if (d < 1e-6)
     {
       b = d = 0;
       return;
     }

   double s  = std::atan2( dy , dx );
   b = s >= 0 ? s : s + 2*M_PI;
}


double bearing(const LocalPoint& a, const LocalPoint& b)
{
   return bearing(a.y(), a.x(), b.y(), b.x());
}


double bearing(double ya, double xa, double yb, double xb)
{
  double b, d;
  bearing_distance(ya, xa, yb, xb, b, d);

  return b;
}


double distance(const LocalPoint& a, const LocalPoint& b)
{
  double dy = b.y() - a.y();
  double dx = b.x() - a.x();

  return std::sqrt(dy*dy + dx*dx);
}

}}   // namespace GNU_gama::local
