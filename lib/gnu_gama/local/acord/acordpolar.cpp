/*
  GNU Gama -- Approximate coordinates by Polar Method
  Copyright (C) 2018  Petra Millarova <petramillarova@gmail.com>

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

#include <numeric>
#include <algorithm>
#include <gnu_gama/local/acord/acordpolar.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/orientation.h>
#include <gnu_gama/local/median/g2d_point.h>

using namespace GNU_gama::local;

AcordPolar::AcordPolar(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
  try
    {
    }
  catch (...)
    {
      throw;
    }
}

void AcordPolar::execute()
{
  if (AC.missing_xy_.size() == 0) return;

  execute_counter_++;
  if (execute_counter_ > 2)   // heuristic
    {
      enable_slope_observations();

      if (AC.median_max_norm_ < 0.2) AC.median_max_norm_ += 0.02;
    }

  do
    {
      AC.new_points_xy_ = 0;

      /*for (auto& cluster : AC.SPClusters_)
        {
          if (!points_from_SPCluster(cluster)) cluster = nullptr;
        }
      AC.SPClusters_.erase( std::remove(AC.SPClusters_.begin(),
                                        AC.SPClusters_.end(),
                                        nullptr),
                            AC.SPClusters_.end() );*/
      int N = AC.SPClusters_.size()-1;
      int i = 0;
      while (i <= N && N > 0)
        {
          if (!points_from_SPCluster(AC.SPClusters_[i]))
            {
              std::swap(AC.SPClusters_[i], AC.SPClusters_[N]);
              --N;
            }
          else ++i;
        }

      AC.get_medians();
    }
  while (AC.new_points_xy_ > 0  &&
         AC.missing_xy_.size()  > 0);
}

// points_from_SPCluster: goes through a StandPoint cluster and if it
// finds enough information, calls the calculate_polar function

bool AcordPolar::points_from_SPCluster(StandPoint* sp)
{
  bool res = true;
  if (AC.in_missingXY(sp->station)) return res;

  // local observation list
  ObservationList lol = sp->observation_list;   // deep copy

  // orientation shift can be defined explicitly in the input XML
  // if this is the case it has the precedence over computed values
  if (!sp->test_orientation())
    {
      Orientation ori(PD, lol);
      double orientation_shift;
      int n;
      ori.orientation(sp, orientation_shift, n);
      if (n > 0) sp->set_orientation(orientation_shift);
    }

  std::map<PointID, double> sp_distances;   // <to where, median value>
  std::vector<Angle*> sp_angles;
  std::map<PointID, double> sp_directions;

  lol.sort([](Observation* a, Observation* b) { return a->to() < b->to(); });
  for (PointID pid : AC.missing_xy_)
    {
      std::vector<double> tmp_dists;
      bool skip = true;
      for (Observation* obs : lol)
        {
          if (Angle* a = dynamic_cast<Angle*>(obs))
            {
              sp_angles.push_back(a);
              continue;
            }

          if (obs->to() != pid)
            {
              if (skip) continue;    // skip leading obs not targeting to pid
              else      break;       // ignore trailing obs ...
            }

          skip = false;

          auto dir = AC.get_dir(obs);
          if (dir.second)
            {
              sp_directions.insert({ pid, dir.first });
            }
          else
            {
              auto dist = AC.get_dist(obs);
              if (dist.second)
                {
                  tmp_dists.push_back(dist.first);
                }
            }
        }
      if (!tmp_dists.empty())
        {
          sp_distances.insert({ pid, AC.median(tmp_dists) });
        }

    }

  // go through directions and compute
  if (sp->test_orientation())
    {
      for (auto o : sp_directions)
        {
          auto dist_it = sp_distances.find(o.first);
          if (dist_it != sp_distances.end())
            {
              Measurement m(sp->station, o.first, o.second,
                            dist_it->second, sp);
              LocalPoint calc_pt = calculate_polar(m);
              if (stub(o.first))
                {
                   //only setting the coords, leaving other info as is
                  PD[o.first].set_xy(calc_pt.x(), calc_pt.y());
                  AC.missing_xy_.erase(o.first);
                  AC.new_points_xy_++;
                  res = false;
                }
              else
                {
                  AC.candidate_xy_.insert({ o.first, calc_pt });
                  res = false;
                }
            }
        }
    }

  // add coordinates computed from angle observation
  if (sp_angles.size() == 0) return res;

  std::map<PointID, LocalPoint> secondary_points;
  Acord2::size_type N = sp_angles.size();
  Acord2::size_type N_before {};

  do
    {
      N_before = N;
      for (Acord2::size_type i=0; i<N; i++)
        {
          Angle* a = sp_angles[i];
          if (a == nullptr) continue;

          PointID bsid = a->bs();
          PointID fsid = a->fs();
          LocalPoint bs_local_point, fs_local_point;
          int known_xy = 0;   // bitmap 00, 10, 01, 11

          { // find xy coordinates for bs ray if exists
            auto bsiter = PD.find(bsid);
            if (bsiter != PD.end() && bsiter->second.test_xy())
              {
                known_xy += 1; // set 1st bit
                bs_local_point = bsiter->second;
              }
            else
              {
                auto bsiter = secondary_points.find(bsid);
                if (bsiter != secondary_points.end() &&
                    bsiter->second.test_xy())
                  {
                    known_xy += 1; // set 1st bit
                    bs_local_point = bsiter->second;
                  }
              }
          }

          { // find xy coordinates for fs ray if exists
            auto fsiter = PD.find(fsid);
            if (fsiter != PD.end() && fsiter->second.test_xy())
              {
                known_xy += 2; // set 2nd bit
                fs_local_point = fsiter->second;
              }
            else
              {
                auto fsiter = secondary_points.find(fsid);
                if (fsiter != secondary_points.end() &&
                    fsiter->second.test_xy())
                  {
                    known_xy += 2; // set 2nd bit
                    fs_local_point = fsiter->second;
                  }
              }
          }

          if (known_xy != 1 && known_xy != 2) continue;

          // now we know that for the given angle there is exactly one
          // ray with known xy
          LocalPoint target;
          LocalPoint stand_point = PD[a->from()];
          if (bs_local_point.test_xy())
            {
              double b = bearing(stand_point,  bs_local_point);
              b += a->value();
              //double d = sp_distances[fsid];
              auto dist_it = sp_distances.find(fsid);
              if (dist_it != sp_distances.end())
                {
                  double d = dist_it->second;
                  double x = stand_point.x() + d * std::cos(b);
                  double y = stand_point.y() + d * std::sin(b);
                  target.set_xy(x, y);
                  AC.candidate_xy_.insert({ fsid,target });
                  secondary_points.insert({ fsid,target });
                }

            }
          if (fs_local_point.test_xy())
            {
              double b = bearing(stand_point,  fs_local_point);
              b -= a->value();
              //double d = sp_distances[bsid];
              auto dist_it = sp_distances.find(bsid);
              if (dist_it != sp_distances.end())
                {
                  double d = dist_it->second;
                  double x = stand_point.x() + d * std::cos(b);
                  double y = stand_point.y() + d * std::sin(b);
                  target.set_xy(x, y);
                  AC.candidate_xy_.insert({ bsid,target });
                  secondary_points.insert({ bsid,target });
                }

            }

          // remove processed angle from the list
          sp_angles[i] = nullptr;
        }
    }
  while(N > 0 && N_before > N);

  return res;
}

bool AcordPolar::stub(PointID p) //returns true if point is a stub
{
  auto it = AC.obs_to_.lower_bound(p);
  auto eit = AC.obs_to_.upper_bound(p);
  PointID comp_pt = it->second->from();
  while (it != eit)
    {
      if (comp_pt != it->second->from()) return false;
      ++it;
    }
  return true;
}

// calculate_polar: calculates coords from measurement

LocalPoint AcordPolar::calculate_polar(Measurement m)
{
  const LocalPoint& sp_xy = PD[m.standpoint];
  double bearing = m.sp->orientation() + (m.dir);
  double x = sp_xy.x() + (m.dist*std::cos(bearing));
  double y = sp_xy.y() + (m.dist*std::sin(bearing));
  return LocalPoint(x, y);
}
