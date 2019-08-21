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

#include <gnu_gama/local/acord/acordweakchecks.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/bearing.h>
#include <gnu_gama/local/orientation.h>

using namespace GNU_gama::local;

AcordWeakChecks::AcordWeakChecks(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
}

void AcordWeakChecks::execute()
{
  // iterate through AC.traverses which now contains only unchecked
  // traverses
  for (auto tr : AC.traverses)
    {
      if (tr.second == AC.closed_start_traverse)
        // they should all be of this type by now but just in case...
        {
          auto check_res = check_traverse_endpoint(tr.first.back());
          if (check_res.first)
            {
              tr.first.push_back(Acord2::Point(check_res.second,
                                               PD[check_res.second]));
              // transform points first
              if (AC.transform_traverse(tr.first))
                {
                  for (auto pt : tr.first)
                    {
                      if (AC.in_missingXY(pt.id))
                        {
                          // only setting the coords, leaving other info as is
                          PD[pt.id].set_xy(pt.coords.x(), pt.coords.y());
                          AC.missing_xy_.erase(pt.id);
                          AC.new_points_xy_++;
                            }
                    }
                }
              tr.first.pop_back(); //popping back the point we added
                                   //earlier for simpler erasing
            }
        }
    }
  if (!AC.traverses.empty())
    {
      // now we just iterate through the traverses one more time and
      // if the last point is known, we can safely remove them from
      // the list
      AC.traverses.erase(std::remove_if(
          AC.traverses.begin(),
          AC.traverses.end(),
          [this](std::pair<Acord2::Traverse, Acord2::Traverse_type> tr) {
            return (!AC.in_missingXY(tr.first.back().id));
          }), AC.traverses.end());
    }

  // similar procedure for points in same_points
  for (auto pt : AC.candidate_xy_)
    {
      if (AC.in_missingXY(pt.first)
        && check_point(Acord2::Point(pt.first, pt.second)))
        {
          PD[pt.first].set_xy(pt.second.x(), pt.second.y());
          AC.missing_xy_.erase(pt.first);
          AC.new_points_xy_++;
        }
    }
  std::set<PointID> keys;
  for (auto i : AC.candidate_xy_) keys.insert(i.first);
  for (auto p : keys)
  {
	  if (!AC.in_missingXY(p))
	  {
		  AC.candidate_xy_.erase(p);
	  }
  }
}

// check traverse endpoint: find if there are any observations from
// known points to the free end point if there are, try to compute the
// value from your computed cocrdinates, if the difference is
// acceptable, add traverse to PD returns true if point passes the
// check, along with pointID of the point that connects to the
// traverse

std::pair<bool, PointID>
AcordWeakChecks::check_traverse_endpoint(Acord2::Point pt)
{
  auto it = AC.obs_to_.lower_bound(pt.id);
  auto eit = AC.obs_to_.upper_bound(pt.id);

  while (it != eit)
    {
      Observation* obs = (*it).second;
      if (!AC.in_missingXY(obs->from()))
        // this means there is an observation from a known point to our
        // endpoint
        {
          auto dist = AC.get_dist(obs);
          if (dist.second)
            {
              double calc_dist = distance(PD[obs->from()], pt.coords);
              // if the point is within tolerance then we can add it
              if (std::abs(dist.first - calc_dist) <= AC.median_max_norm_)
                return {true, obs->from()};
            }
          auto dir = AC.get_dir(obs);
          if (dir.second)
            {
              StandPoint* sp = AC.find_standpoint(obs->from());
              if (sp == nullptr)
                {
                  ++it;
                  continue;
                }
              if (!sp->test_orientation())
                {
                  Orientation ori(PD, sp->observation_list);
                  double orientation_shift;
                  int n;
                  ori.orientation(sp, orientation_shift, n);
                  if (n > 0) sp->set_orientation(orientation_shift);
                }
              if (sp->test_orientation())
                {
                  double calc_dir = bearing(PD[obs->from()], pt.coords);
                  double calc_dist = distance(PD[obs->from()], pt.coords);
                  while (calc_dir > 2 * M_PI)
                    calc_dir -= 2 * M_PI;
                  while (calc_dir < 0)
                    calc_dir += 2 * M_PI;
                  if (std::abs(std::sin(std::abs(calc_dir - dir.first-
					  sp->orientation()))*calc_dist) <
                      AC.median_max_norm_)
                    {
                      return { true, obs->from() };
                    }
                }
            }
        }
      ++it;
    }
  it = AC.obs_from_.lower_bound(pt.id);
  eit = AC.obs_from_.upper_bound(pt.id);

  while (it != eit)
  {
	  Observation* obs = (*it).second;
	  // this means there is an observation from a known point to our
	  // point
	  if (!AC.in_missingXY(obs->to()))
	  {
		  auto dist = AC.get_dist(obs);
		  if (dist.second)
		  {
			  double calc_dist = distance(PD[obs->to()], pt.coords);
			  if (std::abs(dist.first - calc_dist) <= AC.median_max_norm_)
				  return {true, obs->to()};
		  }
		  //no point in testing direction if we would not know the orientation anyway
	  }
	  ++it;
  }

  return {false, PointID()};
}

// check point: find if there are any observations from/to known
// points to/from a point from same_points if there are, try to
// compute the value from your computed cocrdinates, compare the
// biggest diff with NORM_MAX returns true if point passes the check

bool AcordWeakChecks::check_point(Acord2::Point pt)
{
  double maxdiff = -1;
  auto it = AC.obs_to_.lower_bound(pt.id);
  auto eit = AC.obs_to_.upper_bound(pt.id);

  while (it != eit)
    {
      Observation* obs = (*it).second;
      // this means there is an observation from a known point to our
      // point
      if (!AC.in_missingXY(obs->from()))
        {
          auto dist = AC.get_dist(obs);
          if (dist.second)
            {
              double calc_dist = distance(PD[obs->from()], pt.coords);
              double diff = std::abs(dist.first - calc_dist);
              if (diff > maxdiff) maxdiff = diff;
            }
          auto dir = AC.get_dir(obs);
          if (dir.second)
            {
              StandPoint* sp = AC.find_standpoint(obs->from());
              if (sp == nullptr)
                {
                  ++it;
                  continue;
                }
              if (!sp->test_orientation())
                {
                  Orientation ori(PD, sp->observation_list);
                  double orientation_shift;
                  int n;
                  ori.orientation(sp, orientation_shift, n);
                  if (n > 0) sp->set_orientation(orientation_shift);
                }
              if (sp->test_orientation())
                {
                  double calc_dir = bearing(PD[obs->from()], pt.coords);
                  double calc_dist = distance(PD[obs->from()], pt.coords);
                  while (calc_dir > 2 * M_PI)
                    calc_dir -= 2 * M_PI;
                  while (calc_dir < 0)
                    calc_dir += 2 * M_PI;
                  double diff = std::abs(std::sin(std::abs(calc_dir - dir.first -
                                         sp->orientation()))*calc_dist);
                  if (diff > maxdiff) maxdiff = diff;
                }
            }
        }
      ++it;
    }

  it = AC.obs_from_.lower_bound(pt.id);
  eit = AC.obs_from_.upper_bound(pt.id);

  while (it != eit)
    {
      Observation* obs = (*it).second;
      // this means there is an observation from a known point to our
      // point
      if (!AC.in_missingXY(obs->to()))
        {
          auto dist = AC.get_dist(obs);
          if (dist.second)
            {
              double calc_dist = distance(PD[obs->to()], pt.coords);
              double diff = std::abs(dist.first - calc_dist);
              if (diff > maxdiff) maxdiff = diff;
            }
          auto dir = AC.get_dir(obs);
          if (dir.second)
            {
              StandPoint* sp = AC.find_standpoint(obs->from());
              if (sp == nullptr)
                {
                  ++it;
                  continue;
                }
              if (!sp->test_orientation())
                {
                  Orientation ori(PD, sp->observation_list);
                  double orientation_shift;
                  int n;
                  ori.orientation(sp, orientation_shift, n);
                  if (n > 0) sp->set_orientation(orientation_shift);
                }
              if (sp->test_orientation())
                {
                  double calc_dir = bearing(pt.coords, PD[obs->from()]);
                  double calc_dist = distance(pt.coords, PD[obs->from()]);
                  while (calc_dir > 2 * M_PI)
                    calc_dir -= 2 * M_PI;
                  while (calc_dir < 0)
                    calc_dir += 2 * M_PI;
                  double diff = std::abs(std::sin(std::abs(calc_dir - dir.first -
                                         sp->orientation()))*calc_dist);
                  if (diff > maxdiff) maxdiff = diff;
                }
            }
        }
      ++it;
    }

  if (maxdiff < 0) return false;

  return (maxdiff<AC.median_max_norm_);
}
