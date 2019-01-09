/*
  GNU Gama C++ library
  Copyright (C) 1999, 2018  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/local/orientation.h>
#include <gnu_gama/local/cluster.h>
#include <gnu_gama/local/gamadata.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace GNU_gama::local;
using Cluster_ = GNU_gama::Cluster<Observation>;


void Orientation::add_all()
{
  ObservationList::const_iterator iterator = OL_.begin();
  double l1;
  int    dir_count;

  while (iterator != OL_.end())
    if (const Direction* direction = dynamic_cast<const Direction*>(*iterator))
      {
        Cluster_* cluster = const_cast<Cluster_*>(direction->ptr_cluster());
        StandPoint* standpoint = static_cast<StandPoint*>(cluster);
        if (standpoint->test_orientation())
          {
            const Cluster_* ca = direction->ptr_cluster();
            const Cluster_* cb = ca;
            while (iterator != OL_.end())
              {
                cb = (*iterator)->ptr_cluster();
                if (ca == cb)
                  ++iterator;
                else
                  break;
              }
          }
        else
          {
            orientation(iterator, l1, dir_count);
            if (dir_count > 0) standpoint->set_orientation(l1);
          }
      }
    else
      ++iterator;
}


void Orientation::orientation(ObservationList::const_iterator& mer,
                              double& z, int& dir_count)
{
  const Cluster_* current = (*mer)->ptr_cluster();
  PointData::const_iterator pa = PD_.find( (*mer)->from() );
  if (pa == PD_.end() || !(*pa).second.test_xy())
    {
      while (mer != OL_.end() && (*mer)->ptr_cluster() == current)  ++mer;

      z = 0;
      dir_count = 0;
      return;
    }

  std::vector<double> sz;

  while ( mer != OL_.end() && (*mer)->ptr_cluster() == current )
    {
      if (const Direction* direction = dynamic_cast<const Direction*>(*mer))
        {
          PointData::const_iterator pb = PD_.find(direction->to());
          if (pb != PD_.end() && (*pb).second.test_xy())
            {
              // gama 1.9.04
              double zn;
              try
              {
                zn = bearing((*pa).second, (*pb).second);
              }
              catch (GNU_gama::local::Exception e)
              {
                std::stringstream s;
                s.precision(5);
                s << e.what() << "\n\n";

                s.precision(5);
                s.setf(std::ios_base::fixed, std::ios_base::floatfield);

                s << std::setw(13) << pa->first;
                s << "  x = " << std::setw(13) << pa->second.x();
                s << "  y = " << std::setw(13) << pa->second.y();
                s << "\n";

                s << std::setw(13) << pb->first;
                s << "  x = " << std::setw(13) << pb->second.x();
                s << "  y = " << std::setw(13) << pb->second.y();
                s << "\n";

                throw GNU_gama::local::Exception(s.str());
              }
              catch (...)
              {
                throw;
              }
              double sn = direction->value();
              double df = zn - sn;
              // if (df < 0) df += 2*M_PI;  ......  gnu_gama/local-1.1.13
              while (df > M_PI)
                df -= 2*M_PI;
              while (df < -M_PI)
                df += 2*M_PI;
              sz.push_back(df);
            }
        }
      ++mer;
    }

  double l1 = 0;
  double d  = 0;          // mean deviation
  auto   n  = sz.size();

  if (n)
    {
      std::sort(sz.begin(), sz.end());
      double l1a = sz[(n-1)/2];
      double l1b = sz[n/2];
      if (std::abs(l1b - l1a) > M_PI/2 && n < 3)
        l1 = l1a;
      else
        l1 = (sz[n/2] + sz[(n-1)/2]) / 2;

      for (decltype(n) i=0; i<n; i++)
        d += std::abs(sz[i] - l1);
      d /= n;
      if (l1 < 0) l1 += 2*M_PI;
    }

  z = l1;
  dir_count = int(n);
  return;
}


void Orientation::orientation(GNU_gama::local::StandPoint* sp,
                              double& z, int& dir_count)
{
  ObservationList& obs = sp->observation_list;
  ObservationList::const_iterator mer = obs.cbegin();
  Orientation orp(PD_, obs);
  orp.orientation(mer, z, dir_count);
}
