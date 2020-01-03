/*
  GNU Gama -- Abstract base class of Acord2 algorithms family
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

#include <gnu_gama/local/acord/acordintersection.h>
#include <cmath>
#include <algorithm>

using namespace GNU_gama::local;
using std::sin;
using std::tan;
using std::sort;

AcordIntersection::AcordIntersection(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_),
    approxy_(PD, OD)
{
  try
    {
    }
  catch (...)
    {
      throw;
    }
}


void AcordIntersection::prepare()
{
  if (prepared_) return;

  completed_ = AC.missing_xy_.empty();
  prepared_  = true;
}


void AcordIntersection::execute()
{
  if (!prepared_) prepare();
  if (completed_) return;

  approxy_.calculation();


  /* the following code is based on Acord::execute() */
  /* *********************************************** */

  for (int loop=1; loop<=2; loop++)
    {
      auto tmp = AC.missing_xy_;
      AC.missing_xy_.clear();
      for (auto pid : tmp)
        {
          if (!AC.PD_[pid].test_xy()) AC.missing_xy_.insert(pid);
        }

      completed_ = AC.missing_xy_.empty();
      if (completed_) return;

      /* all transformed slope distances and azimuths
       * go to a temporary single standpoint */

      StandPoint* standpoint = new StandPoint(&OD);
      standpoint->set_orientation(PD.xNorthAngle());
      for (ObservationData::iterator t=OD.begin(), e=OD.end(); t!=e; ++t)
        {
          Observation* obs = *t;

          // azimuths transformed to directions with the given
          // orientation shift (angle between x-axis and the North)
          if (Azimuth* a = dynamic_cast<Azimuth*>(obs))
            {
              Direction* d = new Direction(a->from(), a->to(), a->value());
              standpoint->observation_list.push_back(d);
              continue;
            }

          S_Distance* s = dynamic_cast<S_Distance*>(obs);
          if (s == nullptr) continue;

          StandPoint* c = dynamic_cast<StandPoint*>(s->ptr_cluster());
          if (c == nullptr) continue;   // this should never happen

          PointID from = s->from();
          PointID to   = s->to();

          // search for a zenith angle corresponding to the given
          // slope distance
          ObservationList::iterator i   = c->observation_list.begin();
          ObservationList::iterator end = c->observation_list.end();
          for ( ;i!=end; ++i)
            if (Z_Angle* z = dynamic_cast<Z_Angle*>(*i))
              if (from == z->from() && to == z->to())
                {
                  // ... and fake a horizontal distance
                  Distance* d = new Distance(from, to,
                                             s->value()*fabs(sin(z->value())));
                  standpoint->observation_list.push_back(d);
                  continue;
                }

          // slope distance reduced to horizontal if heights are available
          LocalPoint a = PD[from];
          LocalPoint b = PD[to];
          if (a.test_z() && b.test_z())
            {
              double dz = a.z() - b.z();
              dz = dz*dz;
              double ds = s->value();
              ds = ds*ds;
              if (ds > dz)
                {
                  ds = std::sqrt(ds - dz);
                  Distance* d = new Distance(from, to, ds);
                  standpoint->observation_list.push_back(d);
                  continue;
                }
            }
        }

      // bind observations to the cluster
      standpoint->update();
      // insert standpoint into 'observation data'
      OD.clusters.push_back(standpoint);

      ApproximateCoordinates ps(PD, OD);
      double limit = ps.small_angle_limit() / 1.5;   // relax from 10 to 6 gon
      ps.set_small_angle_limit(limit);
      ps.calculation();

      OD.clusters.pop_back();
      delete standpoint;
    }
}
