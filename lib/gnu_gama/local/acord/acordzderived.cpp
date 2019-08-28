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

#include <gnu_gama/local/acord/acordzderived.h>
#include <cmath>
#include <algorithm>

using namespace GNU_gama::local;
using std::sin;
using std::tan;
using std::sort;

AcordZderived::AcordZderived(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
}


void AcordZderived::prepare()
{
  if (prepared_) return;

  completed_ = AC.missing_z_.empty();
  prepared_  = true;
}


void AcordZderived::execute()
{
  if (!prepared_) prepare();

  completed_ = AC.missing_z_.empty();
  if (completed_) return;

  const double pi = std::acos(-1);

  for (StandPoint* spc : AC.SPClusters_)
    {
      PointID station = spc->station;
      LocalPoint point = AC.PD_[station];
      double station_z {};

      if (point.test_z())
        {
          station_z = point.z();
        }
      else
        {
          std::vector<Z_Angle*> zangle;
          std::vector<Distance*> distance;
          std::vector<S_Distance*> sdistance;

          for (Observation* obs : spc->observation_list)
            {
              if (Distance* d = dynamic_cast<Distance*>(obs))
                {
                  if (AC.PD_[d->to()].test_z()) distance.push_back(d);
                }
              else if (S_Distance* s = dynamic_cast<S_Distance*>(obs))
                {
                  if (AC.PD_[s->to()].test_z()) sdistance.push_back(s);
                }
              else if (Z_Angle* z = dynamic_cast<Z_Angle*>(obs))
                {
                  if (AC.PD_[z->to()].test_z()) zangle.push_back(z);
                }
            }

            if (zangle.empty() || (distance.empty() && sdistance.empty())) continue; // next spc

            std::vector<double> sp_height;

            for (Z_Angle* za : zangle)
              {
                double vertical_angle = pi/2 - za->value();
                for (Distance* d : distance)
                  if (za->to() == d->to())
                    {
                      double h = AC.PD_[d->to()].z() - d->value()*tan(vertical_angle);
                      sp_height.push_back(h);
                    }
                for (S_Distance* s : sdistance)
                  if (za->to() == s->to())
                    {
                      double h = AC.PD_[s->to()].z() - s->value()*sin(vertical_angle);
                      sp_height.push_back(h);
                    }

                const LocalPoint& from = AC.PD_[za->from()];
                const LocalPoint& to   = AC.PD_[za->to()];
                if (from.test_xy() && to.test_xy())
                  {
                    double dx = from.x() - to.x();
                    double dy = from.y() - to.y();
                    double d  = std::sqrt(dx*dx + dy*dy);
                    double h  = to.z() - d*tan(vertical_angle);
                    sp_height.push_back(h);
                    std::cerr << __LINE__ << " " << h << "\n";
                  }
              }

            if (sp_height.empty()) continue;  // next spc

            sort(sp_height.begin(), sp_height.end());

            auto N = sp_height.size();
            station_z = (sp_height[(N-1)/2] + sp_height[N/2])/2;

            AC.candidate_z_.insert({station, station_z});
        }

      /*
       * with known standpoint height we compute heights of all targets
       * without known coordinate z
       */

      std::vector<Z_Angle*> zangle;
      std::vector<Distance*> distance;
      std::vector<S_Distance*> sdistance;

      for (Observation* obs : spc->observation_list)
        {
          if (Distance* d = dynamic_cast<Distance*>(obs))
            {
              if (!AC.PD_[d->to()].test_z()) distance.push_back(d);
            }
          else if (S_Distance* s = dynamic_cast<S_Distance*>(obs))
            {
              if (!AC.PD_[s->to()].test_z()) sdistance.push_back(s);
            }
          else if (Z_Angle* z = dynamic_cast<Z_Angle*>(obs))
            {
              if (!AC.PD_[z->to()].test_z()) zangle.push_back(z);
            }
        }

        //if (zangle.empty() /*|| (distance.empty() && sdistance.empty())*/) continue; // next spc

        for (Z_Angle* za : zangle)
          {
            double vertical_angle = pi/2 - za->value();
            for (Distance* d : distance)
              if (za->to() == d->to())
                {
                  double h = station_z + d->value()*tan(vertical_angle);
                  AC.candidate_z_.insert({d->to(), h});
                }
            for (S_Distance* s : sdistance)
              if (za->to() == s->to())
                {
                  double h = station_z + s->value()*sin(vertical_angle);
                  AC.candidate_z_.insert({s->to(), h});
                }

            const LocalPoint& from = AC.PD_[za->from()];
            const LocalPoint& to   = AC.PD_[za->to()];
            if (from.test_xy() && to.test_xy())
              {
                double dx = from.x() - to.x();
                double dy = from.y() - to.y();
                double d  = std::sqrt(dx*dx + dy*dy);
                double h  = station_z + d*tan(vertical_angle);
                AC.candidate_z_.insert({za->to(), h});
              }
          }

    }  // spc loop for all stand point clusters
}
