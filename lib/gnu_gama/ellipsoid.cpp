/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

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
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <cmath>
#include <gnu_gama/ellipsoid.h>
#include <gnu_gama/radian.h>


using namespace std;
using namespace GNU_gama;


double Ellipsoid::M(double b) const
{
  double w = W(b); return AIme2/(w*w*w);
}
double Ellipsoid::N(double b) const
{
  return A/W(b);
}
double Ellipsoid::W(double b) const
{
  double p = sin(b); return sqrt(1 - e2*p*p);
}
double Ellipsoid::V(double b) const
{
  double p = cos(b); return sqrt(1 + e22*p*p);
}
double Ellipsoid::F(double b) const
{
  return sqrt(1 + n*cos(b+b) + n*n);
}

void Ellipsoid::set_abff1(double pa, double pb, double pf, double pf1)
{
  A = pa;
  if      (pb) { B  = pb;    ff = (A - B)/A;  }
  else if (pf) { ff = pf;    B  = A*(1 - ff); }
  else         { ff = 1/pf1; B  = A*(1 - ff); }

  double a2=A*A, b2=B*B;

  n   = (A - B)/(A + B);
  e2  = (a2 - b2)/a2;
  e22 = (a2 - b2)/b2;

  Ime2  = 1 - e2;
  Ipe22 = 1 + e22;
  AIme2 = A*Ime2;
  AB    = A/B;
}

void Ellipsoid::blh2xyz(double  b, double  l, double  h,
                        double& x, double& y, double& z) const
{
  const double sb = sin(b);
  const double cb = cos(b);
  const double sl = sin(l);
  const double cl = cos(l);
  const double nn = N(b);
  const double n1 = nn*Ime2 + h;
  const double nh = nn + h;

  x = nh*cb*cl;
  y = nh*cb*sl;
  z = n1*sb;
}

void Ellipsoid::xyz2blh(double  x, double  y, double  z,
                        double& b, double& l, double& h) const
{


  /* ****************************************************************
   * B. R. Bowring: Transformation from spatial to geographical     *
   * coordinates, Survey Review XXIII, 181, July 1976, pp. 323--327 *
   * ****************************************************************/

  double t, tan_u, cos2_u, cos_u, sin2_u, sin_u;

  l = atan2(y, x);

  x = fabs(x);
  y = fabs(y);
  if (x > y)
    {
      t = y/x;
      x = x * sqrt(1 + t*t);
    }
  else if (y)
    {
      t = x/y;
      x = y * sqrt(1 + t*t);
    }
  else
    {
      l = 0;
      if (z > 0)
        {
          b = M_PI/2;
          h = z - Ime2*N(b);
        }
      else
        {
          b = -M_PI/2;
          h = -z - Ime2*N(b);
        }
      return;
    }

  tan_u  = AB*z/x;
  cos2_u = 1/(1 + tan_u*tan_u);
  cos_u  = sqrt(cos2_u);
  sin2_u = 1 - cos2_u;
  sin_u  = sqrt(sin2_u);
  if (z < 0) sin_u = -sin_u;

  b = atan2(z + e22*B*sin2_u*sin_u, x - e2*A*cos2_u*cos_u);

  /* next iteration is never needed in earth bound region; max error
   * is 0.0018" for H=2a

  sin_u  = Ime2*N(b)/B*sin(b);
  sin2_u = sin_u*sin_u;
  cos2_u = 1 - sin2_u;
  cos_u  = sqrt(cos2_u);
  b = atan2(z + e22*B*sin2_u*sin_u, x - e2*A*cos2_u*cos_u);

  */

  if (x > fabs(z))
    h = x/cos(b) - N(b);
  else
    h = z/sin(b) - Ime2*N(b);
}
