/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/e3.h>
#include <cmath>

using namespace GNU_gama;

void E_3::operator += (const E_3& v)
{
  e1 += v.e1;  e2 += v.e2; e3 += v.e3;
}
void E_3::operator -= (const E_3& v)
{
  e1 -= v.e1;  e2 -= v.e2; e3 -= v.e3;
}
void E_3::operator *= (double d)
{
  e1 *= d;  e2 *= d;  e3 *= d;
}
void E_3::set(double a, double b, double c)
{
  e1 = a;  e2 = b;  e3 = c;
}
void E_3::add(double a, double b, double c)
{
  e1 += a;  e2 += b;  e3 += c;
}
void E_3::sub(double a, double b, double c)
{
  e1 -= a;  e2 -= b;  e3 -= c;
}
double E_3::dot(const E_3& v) const
{
  return e1*v.e1 + e2*v.e2 + e3*v.e3;
}
void E_3::cross(const E_3& a, const E_3& b)
{
  set(+a.e2*b.e3 - b.e2*a.e3,
      -a.e1*b.e3 + b.e1*a.e3,
      +a.e1*b.e2 - b.e1*a.e2);
}

double GNU_gama::angle(const GNU_gama::E_3& a, const GNU_gama::E_3& b)
{
  double c  = a.dot(b);
  if (c) c /= std::sqrt(a.dot(a) * b.dot(b));

  return std::acos(c);
}

void R_3::set_rotation(double b, double l)
{
  using std::sin;       /* rotation NEU --> XYZ (XYZ = R*NEU) */
  using std::cos;

  r11 = -sin(b)*cos(l);
  r12 = -sin(l);
  r13 =  cos(b)*cos(l);

  r21 = -sin(b)*sin(l);
  r22 =  cos(l);
  r23 =  cos(b)*sin(l);

  r31 = cos(b);
  r32 = 0.0;
  r33 = sin(b);
}

void R_3::rotation(const E_3& c, E_3& x) const     // dif_NEU --> dif_XYZ
{
  x.e1 = r11*c.e1 + r12*c.e2 + r13*c.e3;
  x.e2 = r21*c.e1 + r22*c.e2 + r23*c.e3;
  x.e3 = r31*c.e1 + r32*c.e2 + r33*c.e3;
}

void R_3::inverse (const E_3& c, E_3& x) const     // dif_XYZ --> dif_NEU
{
  x.e1 = r11*c.e1 + r21*c.e2 + r31*c.e3;
  x.e2 = r12*c.e1 + r22*c.e2 + r32*c.e3;
  x.e3 = r13*c.e1 + r23*c.e2 + r33*c.e3;
}
