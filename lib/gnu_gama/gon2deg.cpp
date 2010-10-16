/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004, 2010  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.

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

#include <gnu_gama/gon2deg.h>
#include <gnu_gama/intfloat.h>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cmath>

using namespace std;

namespace GNU_gama {

  string gon2deg(double gon, int sign, int prec)
  {
    bool negative = (gon < 0);
    if (negative) gon = -gon;

    gon *= 0.9;   // 400 ==> 360
    int d = int(gon);
    gon -= d;
    gon *= 60;
    int m = int(gon);
    gon -= m;
    gon *= 60;

    ostringstream dms;
    if (sign == 1 || sign == 2) dms << " ";
    dms.setf(ios_base::fixed, ios_base::floatfield);
    if (sign == 3)
      {
        if (negative) dms << "-";
      }
    else
      {
        dms << setw(3);
      }
    dms << d << "-";
    dms.fill('0');
    dms << setw(2) << m << "-"
        << setprecision(prec) << setw(3+prec) << gon;

    string deg = dms.str();
    if (negative)
      {
        if (sign == 1)
          deg[0] = '-';
        else if (sign == 2)
          {
            int k = 0;
            if (deg[1] == ' ') k = 1;
            if (deg[2] == ' ') k = 2;
            deg[k] = '-';
          }
      }

    return deg;
  }


  bool deg2gon(string deg, double& gon)
  {
    string::const_iterator b=deg.begin();
    string::const_iterator e=deg.end();

    TrimWhiteSpaces(b, e);
    if (b == e) return false;

    bool negative = (*b == '-');
    if (*b == '-' || *b == '+')  ++b;
    if (b == e) return false;

    int     d,  m;
    double  s;
    istringstream dms(string(b, e));

    if (!(dms >> d))             return false;
    if (  dms.get() != '-')      return false;
    if (! isdigit(dms.peek()))   return false;
    if (!(dms >> m))             return false;
    if (  dms.get() != '-')      return false;
    if (! isdigit(dms.peek()))   return false;
    if (!(dms >> s))             return false;
    if (d < 0 || m < 0 || s < 0) return false;
    if (! dms.eof())             return false;

    gon = (d/360.0 + m/21600.0 + s/1296000)*400.0;

    if (gon && negative) gon = -gon;

    return true;
  }

double dms2rad(double dms)
{
    double sgn = 1;
    if (dms < 0)
        {
            dms = -dms;
            sgn = -1;
        }

    double d = int(dms);  dms -= d;  dms *= 100;
    double m = int(dms);  dms -= m;  dms *= 100;

    double r = sgn*(d/180.0 + m/10800.0 + dms/648000.0) * M_PI;
    double z = 2*M_PI;
    while (r >= z) r -= z;
    while (r <  0) r += z;

    return r;
}

double rad2dms(double rad)
{
    rad *= 180/M_PI;
    while (rad >= 360) rad -= 360;
    while (rad <   0 ) rad += 360;

    double d = int(rad);  rad -= d;  rad *= 60;
    double m = int(rad);  rad -= m;  rad *= 60;

    return d + m/100 + rad/10000;
}

}
