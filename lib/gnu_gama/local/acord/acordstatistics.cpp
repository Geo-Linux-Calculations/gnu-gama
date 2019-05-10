/*
  GNU Gama -- Approximate coordinates by Polar Method
  Copyright (C) 2019  Petra Millarova <petramillarova@gmail.com>

  This file is part of the GNU Gama C++ library.

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/local/acord/acordstatistics.h>

using namespace GNU_gama::local;

AcordStatistics::AcordStatistics(PointData& pd, ObservationData& od)
  : PD(pd), OD(od)
{
  missing_coordinates = false;
  computed_xy = computed_z = computed_xyz = 0;
  given_xy = given_xyz = given_z = 0;
  total_xy = total_xyz = total_z = 0;
  observations = 0;

  for (PointData::const_iterator i = PD.begin(); i != PD.end(); ++i)
    {
      const PointID& c = (*i).first;
      const LocalPoint& p = (*i).second;
      bool cp = p.test_xy();
      bool hp = p.test_z();

      if (cp && hp) given_xyz++;
      else if (cp) given_xy++;
      else if (hp) given_z++;

      if (p.active_xy() && !cp) missing_coordinates = true;
      if (p.active_z() && !hp) missing_coordinates = true;

      if (p.active_xy() && p.active_z()) total_xyz++;
      else if (p.active_xy())  total_xy++;
      else if (p.active_z())  total_z++;
    }

  for (ObservationData::const_iterator
         i = OD.begin(), e = OD.end(); i != e; ++i, ++observations);
}

void AcordStatistics::execute()
{
  int known_now_z, known_now_xy, known_now_xyz;
  known_now_z = known_now_xy = known_now_xyz = 0;
  for (PointData::const_iterator i = PD.begin(); i != PD.end(); ++i)
    {
      // const PointID& c = (*i).first;
      const LocalPoint& p = (*i).second;
      bool cp = p.test_xy();
      bool hp = p.test_z();

      if (cp && hp) known_now_xyz++;
      else if (cp) known_now_xy++;
      else if (hp) known_now_z++;
    }
  computed_xy = known_now_xy - given_xy;
  computed_xyz = known_now_xyz - given_xyz;
  computed_z = known_now_z - given_z;

  if ((total_xy + total_xyz + total_z) ==
      (known_now_xy + known_now_xyz + known_now_z))
    {
      missing_coordinates = false;
    }
}
