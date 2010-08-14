/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

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

#include <sstream>
#include <gnu_gama/latlong.h>
#include <gnu_gama/radian.h>

namespace
{
  std::string latlong(double rad, int prec)
  {
    bool neg = rad < 0;
    if (neg) rad = -rad;

    int d, m;
    rad *= RAD_TO_DEG;
    d    = int(rad);

    rad -= d;
    rad *= 60.0;
    m    = int(rad);
    rad -= m;

    rad *= 60.0;

    std::ostringstream ostr;
    ostr.width(4);
    ostr << d << "-";
    ostr.fill('0');
    ostr.width(2);
    ostr << m << "-";
    ostr.width(3+prec);
    ostr.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ostr.precision(prec);
    ostr << rad;

    std::string s = ostr.str();
    if (neg)
      {
        if      (s[2] == ' ') s[2] = '-';
        else if (s[1] == ' ') s[1] = '-';
        else                  s[0] = '-';
      }

    return s;
  }
}


namespace GNU_gama {


std::string latitude (double rad, int prec)
{
  return latlong(rad, prec);
}


std::string longitude(double rad, int prec)
{
  return latlong(rad, prec);
}

}

