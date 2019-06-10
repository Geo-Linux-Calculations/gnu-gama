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

#include <map>
#include <gnu_gama/local/acord/acordtraverse.h>
#include <gnu_gama/local/orientation.h>


using namespace GNU_gama::local;

AcordTraverse::AcordTraverse(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
  try
    {
      for (PointID p : AC.missing_xy_)
        // if (!PD[p].test_xy()) ... AC.missingXY_ cannot have xy
        {
          std::set<PointID> t = get_neighbours(p);
          if (t.size() == 2) candidate_traverse_points_.insert(p);
        }
    }
  catch (...)
    {
      throw;
    }
}


void AcordTraverse::execute() //to keep in line with the acord class
{
  auto etalon_candidate_points = candidate_traverse_points_;
  std::set<PointID>::iterator it = candidate_traverse_points_.begin();
  while (candidate_traverse_points_.size() > 1 && it != candidate_traverse_points_.end())
    {
	  traverse_points_.clear();
	  traverse.clear();
      // If there is one then add pt to traverse_pts and remove from candidates,
      traverse_points_.push_back(*it);
      get_traverse_pts(*it);   // populating the traverse_points_

      if (traverse_points_.size() > 2)
        {
          tr_type = get_connecting_points();
          //if there are at least two points in traverse_pts then I can calculate the polygon and insert into traverses
          bool success = calculate_traverse();
          if (success)
            { 
              AC.traverses.push_back({ traverse, tr_type });

			  for (auto t : traverse_points_)
				  {
				    candidate_traverse_points_.erase(t);
				  }
			  it = candidate_traverse_points_.begin();
            }
          else
            {
              // Computation failed
			  for (auto t : traverse_points_)
				  {
				  auto found = std::find(etalon_candidate_points.begin(), etalon_candidate_points.end(), t);
				  if(found != etalon_candidate_points.end()) candidate_traverse_points_.insert(t);
				  }
			  ++it;
            }

        }
      else
        {
          for (auto t : traverse_points_)
            {
			  auto found = std::find(etalon_candidate_points.begin(), etalon_candidate_points.end(), t);
			  if (found != etalon_candidate_points.end()) candidate_traverse_points_.insert(t);
            }
		  ++it;
        }
    }

  // now we have all traverses we can try to find same points
  std::set<PointID> added_points = add_same_points();
  if (!added_points.empty())
    {
      auto cnt_before = AC.missing_xy_.size();
      AC.get_medians();
      auto cnt_after = AC.missing_xy_.size();
      if (cnt_before != cnt_after)
        {
          /* check which traverses contain the points that are now
           * known and edit them and their traverse type
           */
          for (auto tr : AC.traverses)
            {
              // if it's a closed traverse we already know coords of
              // both points and we can add them
              if (tr.second == AC.closed_traverse) continue;
              PointID tr_pointid = tr.first.back().id;

              // if the point is in missingXY than we know it was not added
              if (AC.in_missingXY(tr_pointid)) continue;
              auto found_it = std::find(added_points.begin(), added_points.end(), tr_pointid);
              if (found_it != added_points.end())
                {
                  tr.first.back().coords = PD[tr_pointid]; //set the newly computed coords
                  tr.second = AC.closed_traverse;

                  // now we can add computed points to PD and remove
                  // them from missingXY
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
                }
            }
        }
    }
  
  // now we just iterate through the traverses one more time and if the
  // last point is known, we can safely remove them from the list
  
  if(!AC.traverses.empty())
    {
	  AC.traverses.erase(std::remove_if(
		  AC.traverses.begin(), 
		  AC.traverses.end(), 
	    [this](std::pair<Acord2::Traverse, Acord2::Traverse_type> tr) {
		  return (!AC.in_missingXY(tr.first.back().id));
		  }), AC.traverses.end());
	}

}

// find same points in traverses and add them to AC.same_points,
// returns true if points are added

std::set<PointID> AcordTraverse::add_same_points()
{
  std::set<PointID> added_points;
  for (auto itr = AC.traverses.begin(); itr!= AC.traverses.end(); ++itr )
    {
	//first check if any points have been computed before
	bool found = false;
	  for (auto pt : (*itr).first)
		{
		  auto fit = AC.same_points_xy_.find (pt.id);
		  auto ait = added_points.find (pt.id);

		  if (fit != AC.same_points_xy_.end() && ait == added_points.end())
			{
			  AC.same_points_xy_.insert({pt.id, pt.coords});
			  found = true;
			}
		}
	  if (found && AC.get_medians())
		{
		  AC.transform_traverse ((*itr).first);
		}
      //we know both endpoints are known - we can add the whole traverse
      //straight to PD
      if ((*itr).second == AC.closed_traverse)
        {
          for (auto pt : (*itr).first)
            {
              if (AC.in_missingXY(pt.id))
                {
                  PD[pt.id].set_xy(pt.coords.x(), pt.coords.y());
                  AC.missing_xy_.erase(pt.id);
                  AC.new_points_xy_++;
                }
            }
        }
      /* tr_type should now only be closed_start_traverse
       * this is the only unknown point that can occur in more traverses
       */
      Acord2::Point comp_point = (*itr).first.back();
      bool found_already = false;
      if (AC.traverses.size() < 2 || itr == AC.traverses.end() - 1) continue;
      for (auto initr = ++itr; initr != AC.traverses.end(); ++initr)
        {
          if ((*itr).second == AC.closed_traverse) continue;
          if (comp_point == (*initr).first.back())
            {
              if (!found_already)
                {
                  AC.same_points_xy_.insert({ comp_point.id, comp_point.coords });
                  found_already = true;
                  added_points.insert(comp_point.id);
                }
              AC.same_points_xy_.insert({ (*initr).first.back().id, (*initr).first.back().coords });
            }
        }

    }
  return added_points;
}

// finds all points that are connected and their coords are unknown
// and adds them to traverse_points in the right order

void AcordTraverse::get_traverse_pts(PointID pt)
{
  std::set<PointID> neighbours = get_neighbours(pt);
  for (auto c : neighbours)
    {
      auto found_traverse_point  = std::find(traverse_points_.begin(), traverse_points_.end(), c);
      auto found_candidate_point = std::find(candidate_traverse_points_.begin(), candidate_traverse_points_.end(), c);

      // if the point is not added in traverse points yet
      if (found_traverse_point == traverse_points_.end())
        {
          // if the point is a candidate traverse point then add it and delete from candidates
          if (found_candidate_point != candidate_traverse_points_.end())
            {
              traverse_points_.push_back(c);
              candidate_traverse_points_.erase(c);
              get_traverse_pts(c);
            }
        }
    }
}

///looks for known points at start or end of traverse, sets type of traverse and add the point to  the right place
Acord2::Traverse_type AcordTraverse::get_connecting_points()
{
  tr_type = AC.open_traverse;
  //find all neighbours of this point and check if they can be added
  std::set<PointID> last_neighbours = get_neighbours(traverse_points_.back());
  bool end_pt = false;
  for (auto pid : last_neighbours)
    {
      auto found_tr = std::find(traverse_points_.begin(), traverse_points_.end(), pid);
      if (found_tr == traverse_points_.end()) //point not yet in traverse
        {
          traverse_points_.push_back(pid);
          if (!AC.in_missingXY(pid)) end_pt = true;
          break;
        }
    }
  //find all neighbours of this point and check if they can be added
  std::set<PointID> first_neighbours = get_neighbours(traverse_points_.front());
  for (auto pid : first_neighbours)
    {
      auto found_tr = std::find(traverse_points_.begin(), traverse_points_.end(), pid);
      if (found_tr == traverse_points_.end()) //point not yet in traverse
        {
          traverse_points_.insert(traverse_points_.begin(), pid);
          if (!AC.in_missingXY(pid))
            {
              if (end_pt) tr_type = AC.closed_traverse;
              else tr_type = AC.closed_start_traverse;
            }
          else //this means the traverse only has a known point at the end
            {
              //would it be better to use reverse iterators later instead?
              std::reverse(traverse_points_.begin(), traverse_points_.end());
              tr_type = AC.closed_start_traverse;
            }
          break;
        }
    }
  return tr_type;
}

/// returns a set of neighbours
std::set<PointID> AcordTraverse::get_neighbours(PointID pt)
{
  std::set<PointID> s;
  auto it = AC.obs_from_.lower_bound(pt);
  auto eit = AC.obs_from_.upper_bound(pt);

  while (it != eit)
    {
      //if (PD[it->second->to()].test_xy()) break;
      s.insert(it->second->to());
      ++it;
    }

  it = AC.obs_to_.lower_bound(pt);
  eit = AC.obs_to_.upper_bound(pt);
  while (it != eit)
    {
      //if (PD[it->second->from()].test_xy()) break;
      s.insert(it->second->from());
      ++it;
    }

  return  s;
}

bool AcordTraverse::candidateTraversePoint(PointID pid)
{
  if (PD[pid].test_xy()) return false;

  return  get_neighbours(pid).size() == 2;
}

///calculates traverse coordinates if traverse is not open and if finds front orientation
bool AcordTraverse::calculate_traverse()
{
  if (tr_type == AC.open_traverse) return false;

  std::pair<double, bool> front_ori = {0, false };
  std::vector<double> angles;
  std::vector<double> distances;

  for (Acord2::size_type i = 0; i<traverse_points_.size(); i++ )
    {
      PointID pid = traverse_points_[i];
      std::vector<double> tmp_dists;
      std::vector<double> tmp_bearings;
      int known_dirs = 0; //11 - both, 10 - front, 01 - back, 00 - none
      double angle_from_dirs = 0;

      //check for distances, directons and angles from this point
      auto it = AC.obs_from_.lower_bound(pid);
      auto eit = AC.obs_from_.upper_bound(pid);
      while (it != eit && i != traverse_points_.size()-1)
        {
          Angle* a = dynamic_cast<Angle*>(it->second);
          if (i != 0 && a != nullptr)
            {
              if (a->bs() == traverse_points_[i - 1] && a->fs() == traverse_points_[i + 1]) tmp_bearings.push_back(a->value());
              if (a->fs() == traverse_points_[i - 1] && a->bs() == traverse_points_[i + 1]) tmp_bearings.push_back(2 * M_PI - (a->value()));
            }
          auto dir = AC.get_dir(it->second);
          if (dir.second)
            {
              if (it->second->to() == traverse_points_[i + 1] && (known_dirs == 00 || known_dirs == 01))
                {
                  known_dirs += 10;
                  angle_from_dirs += dir.first;
                }
              else if ((known_dirs == 00 || known_dirs == 10))
                {
                  if (i == 0 && !AC.in_missingXY(it->second->to()) && !front_ori.second)
                    {
					  StandPoint* sp = AC.find_standpoint(it->second->from());
                      if (sp != nullptr)
                        {
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
                              traverse.push_back({ it->second->to(),PD[it->second->to()] });
                              known_dirs += 1;
                              angle_from_dirs -= (dir.first + sp->orientation());
                              front_ori = { dir.first + sp->orientation(), true };
                            }
                        }
                    }
                  else if (i!=0)
                    {
                      if (it->second->to() == traverse_points_[i - 1])
                        {
                          known_dirs += 1;
                          angle_from_dirs -= dir.first;
                        }
                    }
                }
            }

          auto d = AC.get_dist(it->second);
          if (d.second && it->second->to() == traverse_points_[i + 1]) tmp_dists.push_back(d.first);

          if (known_dirs == 11)
            {
              while (angle_from_dirs > 2 * M_PI)
                angle_from_dirs -= 2 * M_PI;
              while (angle_from_dirs < 0)
                angle_from_dirs += 2 * M_PI;
              tmp_bearings.push_back(angle_from_dirs);
              angle_from_dirs = 0;
              known_dirs = 0;
            }
          ++it;
        }
      //check for directions and distances TO this point
      auto itt = AC.obs_to_.lower_bound(pid);
      auto eitt = AC.obs_to_.upper_bound(pid);
      while (itt != eitt && i != traverse_points_.size() - 1)
        {
          auto d = AC.get_dist(itt->second);
          if (d.second && itt->second->from() == traverse_points_[i + 1]) tmp_dists.push_back(d.first);

          auto dir = AC.get_dir(itt->second);
          if (dir.second)
            {
              if (itt->second->from() == traverse_points_[i + 1] && (known_dirs == 00 || known_dirs == 01))
                {
                  known_dirs += 10;
                  double dirval = M_PI - dir.first;
                  angle_from_dirs -= dirval;
                }
              else if ((known_dirs == 00 || known_dirs == 10))
                {
                  if (i == 0 && !AC.in_missingXY(itt->second->from()) && !front_ori.second)
                    {
                      StandPoint* sp = AC.find_standpoint(itt->second->from());
                      if (sp != nullptr)
                        {
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
                              traverse.push_back({ itt->second->from(),PD[itt->second->from()] });
                              known_dirs += 1;
                              double dirval = M_PI - dir.first;
                              angle_from_dirs += (dirval + sp->orientation());
                              front_ori = { dir.first + sp->orientation(), true };
                            }
                        }
                    }
                  else if (i != 0)
                    {
                      if (itt->second->from() == traverse_points_[i - 1])
                        {
                          known_dirs += 1;
                          double dirval = M_PI - dir.first;
                          angle_from_dirs += dirval;
                        } 
                    }
                }
            }

          if (known_dirs == 11)
            {
              while (angle_from_dirs > 2 * M_PI)
                angle_from_dirs -= 2 * M_PI;
              while (angle_from_dirs < 0)
                angle_from_dirs += 2 * M_PI;
              tmp_bearings.push_back(angle_from_dirs);
              angle_from_dirs = 0;
              known_dirs = 0;
            }
          ++itt;
        }
	  if(tmp_bearings.empty() && known_dirs == 10 && i==0 && tr_type == AC.closed_traverse)
		{
		  front_ori = { 0, true };
		  while (angle_from_dirs > 2 * M_PI)
			  angle_from_dirs -= 2 * M_PI;
		  while (angle_from_dirs < 0)
			  angle_from_dirs += 2 * M_PI;
		  tmp_bearings.push_back(angle_from_dirs);
		  angle_from_dirs = 0;
		  known_dirs = 0;
	    }
      if (!tmp_dists.empty() &&!tmp_bearings.empty())
        {
          distances.push_back(AC.median(tmp_dists));
          angles.push_back( AC.median(tmp_bearings));
        }
      else if(i<traverse_points_.size()-1)
        {
          //if the point is in the first place and traverse
          //type is closed, we can try to switch the order and
          //start again
          if ((i == 0) && tr_type == AC.closed_traverse)
            {
              front_ori = { 0, false };
              tr_type = AC.closed_start_traverse;
              std::reverse(traverse_points_.begin(), traverse_points_.end());
              traverse_points_.pop_back();
              i = -1;
            }
		  else if(i==0)
			{
			  //the point is in the first place, but type is not closed
			  //there is no other choice but to go back
			  return false;
			}
          else
            {
              //if the point is in the middle we can try
              //to end the traverse here and compute the
              //stuff we have got
              tr_type = AC.closed_start_traverse;
              for (auto t = traverse_points_.size()-1; t>i; --t)
                {
                  candidate_traverse_points_.insert(traverse_points_[t]);
                  traverse_points_.pop_back();
                }
              if (traverse_points_.size() < 1) return false;
			  
			  
            }
        }
    }

  //calculate bearings and coord differences
  std::vector<double> bearings;
  std::vector<double> dX;
  std::vector<double> dY;
  std::vector<LocalPoint> XY;
  if (!front_ori.second && tr_type != AC.closed_traverse) return false;
  if (front_ori.second)
    {
      double b = front_ori.first + angles.front();
      while (b > 2*M_PI)
        b -= 2 * M_PI;
      while (b < 0)
        b += 2 * M_PI;
      bearings.push_back(b);
      dX.push_back(distances.front() * std::cos(b));
      dY.push_back(distances.front() * std::sin(b));
    }
  if (distances.size() != angles.size()) return false;
  for (Acord2::size_type j = 1; j < angles.size();++j)
    {
      double b = angles[j] + bearings[j - 1] - M_PI;
      while (b > 2 * M_PI)
        b -= 2 * M_PI;
      while (b < 0)
        b += 2 * M_PI;
      bearings.push_back(b);
      dX.push_back(distances[j] * std::cos(b));
      dY.push_back(distances[j] * std::sin(b));
    }

  //caculate point coordinates
  double X = PD[traverse_points_.front()].x();
  double Y = PD[traverse_points_.front()].y();
  traverse.push_back({ traverse_points_.front(), PD[traverse_points_.front()] });
  for (Acord2::size_type k = 0; k<dX.size();++k)
    {
      X += dX[k];
      Y += dY[k];
      XY.push_back(LocalPoint(X,Y));
      traverse.push_back({ traverse_points_[k + 1], LocalPoint(X,Y) });
    }

  //look for second orientation
  if ((tr_type == AC.closed_traverse || tr_type == AC.closed_start_traverse))
    {
      std::set<PointID> neighbours = get_neighbours(traverse_points_.back());
      std::pair<double, bool> ori_back = { 0, false };
      for (auto n : neighbours)
        {
          if (!AC.in_missingXY(n))
            {
              traverse.push_back({n, PD[n]});
              ori_back.second = true;
			     tr_type = AC.closed_traverse;
            }
        }
    }
  return AC.transform_traverse(traverse);
}
