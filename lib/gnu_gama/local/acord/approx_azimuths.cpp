/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2013  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/local/acord/approx_azimuths.h>
#include <cmath>

using namespace std;
using namespace GNU_gama::local;

ApproximateAzimuths::ApproximateAzimuths(PointData& pd, ObservationData& od)
  : PD(pd), OD(od)
{
  for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    if (Azimuth* azimuth = dynamic_cast<Azimuth*>(*i))
      {
        PointID from = azimuth->from();
        PointID to   = azimuth->to();
        PointData::iterator itf = PD.find(from);
        PointData::iterator itt = PD.find(to);
        if (itf != PD.end() && itt != PD.end())
          {
            bool bf = itf->second.test_xy();
            bool bt = itt->second.test_xy();
            if (!bf || !bt) map[Key(from, to)] = azimuth->value();
          }
      }
}

void ApproximateAzimuths::execute()
{
  if (map.size() == 0) return;

  bool repeat;
  do {
    repeat = false;
    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
      if (Distance* distance = dynamic_cast<Distance*>(*i))
        {
          Key key(distance->from(), distance->to());
          Map::iterator iter = map.find(key);
          if (iter == map.end()) continue;

          LocalPoint& from = PD[distance->from()];
          LocalPoint& to   = PD[distance->to()];
          bool bf = from.test_xy();
          bool bt = to.test_xy();
          if (bf == bt) continue;

          double bearing = iter->second + PD.xNorthAngle();
          double dx = distance->value() * cos(bearing);
          double dy = distance->value() * sin(bearing);
          if (bf)
            {
              double x = from.x() + dx;
              double y = from.y() + dy;
              to.set_xy(x,y);
            }
          else
            {
              double x = to.x() - dx;
              double y = to.y() - dy;
              from.set_xy(x,y);
            }
          repeat = true;
        }

  } while (repeat);
}
