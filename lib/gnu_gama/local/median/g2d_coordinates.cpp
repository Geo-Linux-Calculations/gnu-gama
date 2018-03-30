/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                2001, 2013, 2014, 2016  Ales Cepek  <cepek@fsv.cvut.cz>
                2016, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

/*************************************************************
 * computation of approximate coordinates:                   *
 *************************************************************/

#include <gnu_gama/local/median/g2d_coordinates.h>
#include <gnu_gama/local/median/g2d_point.h>

using namespace std;
using namespace GNU_gama::local;

// private

void ApproximateCoordinates::
copy_horizontal(const ObservationData& from, ObservationList& to)
{
  for (ObservationData::
         const_iterator i=from.begin(), e=from.end(); i!=e; ++i)
    {
        Observation* obs = const_cast<Observation*>(*i);

        if      (dynamic_cast<Direction*>(obs))  to.push_back(obs);
        else if (dynamic_cast<Angle*    >(obs))  to.push_back(obs);
        else if (dynamic_cast<Distance *>(obs))  to.push_back(obs);
    }
}

void ApproximateCoordinates::reset()
{

  state = calculation_not_done;
  selected.erase(selected.begin(), selected.end());
  solved_pd.erase(solved_pd.begin(), solved_pd.end());

  known_coordinates_ = 0;
  for (PointData::const_iterator i=SB.begin(); i!=SB.end(); ++i)
    if ((*i).second.test_xy())
      ++known_coordinates_;

}


// to be solvable, at least 2 points with known coordinates must exist

bool ApproximateCoordinates::solvable_data(PointData& b)
{

  bool first = false, second = false;
  bool tmp;
  PointData::iterator i = b.begin();
  if (i == b.end()) return false;
  do
    {
      tmp = (*i).second.test_xy() && absent((*i).first);
      if(first)
        second = tmp;
      else
        first = tmp;
      i++;
    }
  while(!(second || i == b.end()));

  // give it still a try if there are vectors, observed coordinates or azimuts
  if (first && !second)
  {
      for (ObservationData::
             const_iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
        {
            if (dynamic_cast<const Xdiff*  >(*i)) return true;
            if (dynamic_cast<const X*      >(*i)) return true;
            if (dynamic_cast<const Azimuth*>(*i)) return true;
        }
  }

  return second;

}  // bool ApproximateCoordinates::solvable_data(PointData& b)


bool ApproximateCoordinates::necessary_observations(PointID id)
{

  // for coordinates to be solvable, at least 2 observations are needed
  // for details see ApproxPoint

  bool first = false, second = false;
  bool tmp;
  ObservationList::iterator i = SM.begin();
  if (i == SM.end()) return false;
  do
    {
      tmp = (((*i)->from() == id) || ((*i)->to() == id));
      if(first)
        second = tmp;
      else
        first = tmp;
      // is second target available?
      Angle* u = dynamic_cast<Angle*>(*i);
      if( u && (u->fs() == id))
      { // added { to avoid dangling else
        if(first)
          second = true;
        else
          first = true;
      } // added } to avoid dangling else
      i++;
    }
  while(!(second || i == SM.end()));
  return second;

}       // bool ApproximateCoordinates::necessary_observations()


void ApproximateCoordinates::find_missing_coordinates()
{

  selected.erase(selected.begin(), selected.end());

  // from observation list points we fetch points that are not in SB
  for(ObservationList::iterator i = SM.begin(); i != SM.end(); i++)
    {
      if((SB.find((*i)->from()) == SB.end()) && absent((*i)->from()))
        selected.push_back((*i)->from());
      if((SB.find((*i)->to()) == SB.end()) && absent((*i)->to()))
        selected.push_back((*i)->to());
      // is second target available?
      Angle*u = dynamic_cast<Angle*>(*i);
      if(u && (SB.find(u->fs()) == SB.end()) && absent(u->fs()))
        selected.push_back(u->fs());
    }

  // from point list we fetch the points with test_xy() == false
  for(PointData::iterator i = SB.begin(); i != SB.end(); i++)
    if((!(*i).second.test_xy()) && absent((*i).first))
      selected.push_back((*i).first);

  // final sort and removal of duplicities in the list of seleted points
  selected.sort();
  selected.unique();

}       // void ApproximateCoordinates::find_missing_coordinates()


void ApproximateCoordinates::move_point(PointData& from, PointData& to,
                                        PointID& what)
{

  PointData::iterator i = from.find(what);
  PointData::iterator j = to.find(what);
  if(i != from.end())
  { // added { to avoid dangling else
    if(j != to.end())
      (*j).second.set_xy((*i).second.x(), (*i).second.y());
    else
      to[what] = LocalPoint((*i).second.x(), (*i).second.y());
  } // added } to avoid dangling else

}    // ApproximateCoordinates::move_point(PointData&, PointData&, PointID&)


bool ApproximateCoordinates::solve_intersection(PointData& points,
                                                PointIDList& what)
{
  if(what.empty()) return false;

  ApproxPoint PB(&points,&SM);
  bool finished = false;
  bool success = false;
  LocalPoint bb;
  PointIDList::iterator i;
  PointData::iterator j;
  while (!finished)
    {
      finished = true;
      i = what.begin();
      while(i != what.end())
        {
          PB.calculation(*i);
          if(PB.state() == unique_solution)
            {
              finished = false;
              success = true;
              bb = PB.Solution();
              j = points.find(*i);
              if(j != points.end())
                (*j).second.set_xy(bb.x(), bb.y());
              else
                points[*i] = LocalPoint(bb.x(), bb.y());
              solved_pd[*i] = bb;
              i = what.erase(i);
            }
          else
            i++;
        }
    }

  return success;

}  // ApproximateCoordinates::solve_intersection(PointData&, PointIDList&)


// points that cannot be solved by intersections - eg inserted traverse

bool ApproximateCoordinates::solve_insertion()
{

  const int max_depth = 5;
  if(selected.empty() || (depth >= max_depth)) return false;

  // building a point list in local coordinate system.  during loop
  // through all observations connecting selected points the involved
  // points are fetched

  PointIDList obs_points;
  bool prv_distance = true;
  bool prv_observation = true;
  ObservationList::iterator first_distance = SM.end();
  ObservationList::iterator first_observation = SM.end();
  for(PointIDList::iterator i = selected.begin(); i != selected.end(); i++)
    for(ObservationList::iterator j = SM.begin(); j != SM.end(); j++)
      if(observation_hasID(j,i))
        // all point IDs are stored and then duplicities are removed
        // - it's faster
        {
          if(Angle *u = dynamic_cast<Angle*>(*j))
            {
              obs_points.push_back((*j)->from());
              obs_points.push_back((*j)->to());
              obs_points.push_back(u->fs());
              if(prv_observation)
                {
                  first_observation = j;
                  prv_observation = false;
                }
            }
          else
            {
              if(dynamic_cast<Distance*>(*j) && prv_distance)
                {
                  first_distance = j;
                  prv_distance = false;
                }
              else
                if(prv_observation)
                  {
                    first_observation = j;
                    prv_observation = false;
                  }
              obs_points.push_back((*j)->from());
              obs_points.push_back((*j)->to());
            }
        }

  obs_points.sort();
  obs_points.unique();
  /*
   * // removing selected with less the 2 observations - is it correct ???
   * for(PointIDList::iterator i = selected.begin(); i != selected.end(); i++)
   * if(!necessary_observations(*i))
   * {
   * PointIDList::iterator pozice = find(obs_points.begin(),
   *                                     obs_points.end(), *i);
   * obs_points.erase(pozice);
   * };
   */

  int number_of_given = 0;
  for(PointIDList::iterator i = obs_points.begin(); i != obs_points.end(); i++)
    if(absent(*i))
      number_of_given++;

  // not enough given points needed for transformation from local
  // coordinate system (cs)
  if(number_of_given < 2)
    return false;

  PointData local_s;
  // removed in 1.7.14   for(PointIDList::iterator i = obs_points.begin();
  // removed in 1.7.14       i != obs_points.end(); i++)
  // removed in 1.7.14     local_s[*i] = LocalPoint();

  ObservationData OD_local = OD;                // ... deep copy
  /* ObservationList& SM_local(OD_local.OL);   // gnu_gama/local-1.1.13 (AC)
   *   for(ObservationList::iterator i = SM.begin(); i != SM.end(); i++)
   *     if(Local_observation(i, obs_points))
   *       SM_local.push_back(*i);
   */

  // local coordinate system (cs) must be defined now
  const double pom_Y = 1000;
  const double pom_X = 5000;
  const double const_distance = 1000;
  PointID local_cs_1, local_cs_2;
  if(first_distance != SM.end())
    {
      local_cs_1 = (*first_distance)->from();
      local_cs_2 = (*first_distance)->to();
      local_s[local_cs_1].set_xy(pom_X, pom_Y);
      local_s[local_cs_2].set_xy(pom_X, pom_Y + (*first_distance)->value());
    }
  else
    {
      local_cs_1 = (*first_observation)->from();
      local_cs_2 = (*first_observation)->to();
      local_s[local_cs_1].set_xy(pom_X, pom_Y);
      local_s[local_cs_2].set_xy(pom_X, pom_Y + const_distance);
    }
  obs_points.remove(local_cs_1);
  obs_points.remove(local_cs_2);

  // calculation of coordinates in local coordinate system
  ApproximateCoordinates local_solution(local_s, OD_local, depth + 1);
  local_solution.calculation();
  if(local_solution.solved().empty())
    return false;

  // computing in local coordinate system is done, now follows
  // transformation into the current coordinate system
  PointIDList vypoctene_urc;
  PointData::iterator i;
  for(PointIDList::iterator id = selected.begin(); id != selected.end(); id++)
    {
      i = local_s.find(*id);
      if((i != local_s.end()) && (*i).second.test_xy())
        vypoctene_urc.push_back(*id);
    }
  PointData local_s_yx;
  for(PointData::iterator sb_it = local_s.begin();
      sb_it != local_s.end(); sb_it++)
    if((*sb_it).second.test_xy())
      local_s_yx[(*sb_it).first] = (*sb_it).second;

  SimilarityTr2D transf(SB, local_s_yx, vypoctene_urc);
  transf.calculation();
  if(transf.state() < unique_solution)
    return false;
  PointData vysledek = transf.transf_points();
  if(vysledek.empty())
    return false;

  // moving solved points to the point list
  for(PointData::iterator bod = vysledek.begin(); bod != vysledek.end(); bod++)
    {
      if((i = SB.find((*bod).first)) == SB.end())
        SB[(*bod).first] = (*bod).second;
      else
        (*i).second.set_xy((*bod).second.x(), (*bod).second.y());
      solved_pd[(*bod).first] = (*bod).second;
    }

  // removing solved points from "selected" list
  for(PointIDList::iterator id = vypoctene_urc.begin();
      id != vypoctene_urc.end(); id++)
    selected.remove(*id);

  return true;

}  // ApproximateCoordinates::solve_insertion()


void ApproximateCoordinates::computational_loop()
{

  PointIDList unsolvable;
  PointIDList::iterator i = selected.begin();
  if (i==selected.end()) return;
  do
    {
      if(!necessary_observations(*i))
        unsolvable.push_back(*i);
      i++;
    }
  while(i != selected.end());
  for(i = unsolvable.begin(); i != unsolvable.end(); i++)
    {
      PointIDList::iterator pozice = std::find(selected.begin(),
                                               selected.end(), *i);
      selected.erase(pozice);
    }

  bool finished = false;
  while(!finished)
    {
      finished = !solve_intersection(SB, selected);
      // compute insertion
      if(!selected.empty())
        finished = finished && (!solve_insertion());
    }

  // return unsolvable points back
  selected.insert(selected.end(), unsolvable.begin(), unsolvable.end());
  unsolvable.erase(unsolvable.begin(), unsolvable.end());

}       // ApproximateCoordinates::computational_loop()


// public


// single point (even if it has coordinates already)

bool ApproximateCoordinates::calculation(PointID id)
{

  state = calculation_done;
  selected.erase(selected.begin(), selected.end());
  selected.push_back(id);
  if(!necessary_observations(id) || !solvable_data(SB))
    return false;
  solve_intersection(SB, selected);
  solve_insertion();
  if(!selected.empty())
    {
      find_missing_coordinates();
      // id already has coordinates
      if(std::find(selected.begin(), selected.end(), id) == selected.end())
        selected.push_back(id);
      PointData SB_puv = SB;
      computational_loop();
      if(std::find(selected.begin(), selected.end(), id) == selected.end())
        {
          move_point(SB, SB_puv, id);
          selected.erase(selected.begin(), selected.end());
        }
      else
        {
          selected.erase(selected.begin(), selected.end());
          selected.push_back(id);
        }
      SB = SB_puv;
    }

  return all_is_solved();

}       // bool ApproximateCoordinates::calculation(PointID id)


// point list (even the points with coordinates)

bool ApproximateCoordinates::calculation(PointIDList id)
{

  state = calculation_done;
  selected = id;
  // removing duplicities
  selected.sort();
  selected.unique();
  if(selected.empty())
    return true;
  if(!solvable_data(SB))
    return false;
  PointIDList unsolvable;
  PointIDList::iterator i = selected.begin();
  do
    {
      if(!necessary_observations(*i))
        {
          unsolvable.push_back(*i);
          i = selected.erase(i);
        }
      else
        i++;
    }
  while(i != selected.end());
  computational_loop();
  if(!selected.empty())
    {
      PointIDList selected_puv = selected;
      find_missing_coordinates();
      // id already has coordinates
      for(i = selected_puv.begin(); i != selected_puv.end(); i++)
        if(std::find(selected.begin(), selected.end(), (*i)) == selected.end())
          selected.push_back(*i);
      PointData SB_puv = SB;
      computational_loop();
      i = selected_puv.begin();
      do
        if(std::find(selected.begin(), selected.end(), (*i)) == selected.end())
          {
            move_point(SB, SB_puv, *i);
            i = selected_puv.erase(i);
          }
        else
          i++;
      while(i != selected_puv.end());
      selected = selected_puv;
      SB = SB_puv;
    }

  // return unsolvable points back
  selected.insert(selected.end(), unsolvable.begin(), unsolvable.end());

  return all_is_solved();

}       // bool ProblSour::calculation(PointIDList id)
