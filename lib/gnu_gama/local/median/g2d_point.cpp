/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                2001, 2014  Ales Cepek  <cepek@gama.fsv.cvut.cz>
                2018  Ales Cepek <cepek@gnu.org>

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

/*************************************************************
 * approximate coordinates of a single point                 *
 *************************************************************/


#include <gnu_gama/local/median/g2d_point.h>
#include <gnu_gama/local/median/g2d_cogo.h>
#include <gnu_gama/local/orientation.h>
#include <list>

using namespace std;

namespace GNU_gama { namespace local {

  // private

  void ApproxPoint::ClearLists()
  {
    for(ObservationList::iterator i = SM.begin(); i != SM.end(); i++)
      delete(*i);
    SM.clear();
    solved_points.clear();
  }



  void ApproxPoint::reset(PointData* sb, ObservationList* sm,
                          const PointID& cb)
  {
    state_ = calculation_not_done;
    ClearLists();  // clearin lists (if a distance is used more then once)
    SB = *sb;
    CB = cb;
    ObservationList sm_s;
    ObservationList sm_pom;

    // computing orientation shift again - solved points are considered as well
    Orientation ors(SB,*sm);
    ors.add_all();

    // selecting observations related to the computed point
    for(ObservationList::iterator i = sm->begin(); i != sm->end(); i++)
      {
        if(((*i)->from() == CB) && KnownTarget(i))
	{ // added { to avoid dangling else 
          if(Direction* s = dynamic_cast<Direction*>(*i))
            sm_s.push_back(s);
          else
            sm_pom.push_back(*i);
	} // added } to avoid dangling else 
        else
          if(knownStandpoint(i))
	  { // added { to avoid dangling else 
            if(Angle* u = dynamic_cast<Angle*>(*i))
              {
                if((u->to() == CB && KnownTarget2(u)) ||
                   (u->fs() == CB && KnownTarget1(i)))
                  SM_S.push_back(makeBearing(u,CB));
              }
            else
              if((*i)->to() == CB)
              { // added { to avoid dangling else 
                if(Direction* s = dynamic_cast<Direction*>(*i))
                  SM_S.push_back(makeBearing(s,CB));
                else
                  sm_pom.push_back(*i);
              } // added { to avoid dangling else 
	  } // added } to avoid dangling else 
      }

    // transforming directions on the computed standpoint to inner angels
    // enable processing of standard C++ code with MS compiler
    for(ObservationList::iterator j, i = sm_s.begin(); i != sm_s.end(); i++)
      for(j=i, ++j; j != sm_s.end(); j++)
        if ((*i)->ptr_cluster() == (*j)->ptr_cluster())
          SM_U.push_back(makeAngle(i,j));
    sm_s.clear();

    // now putting selected observations in good order - both distances
    // from and to the stanpoint, identical angles, ... etc.
    ArrangeObservations(sm_pom);

  }  /* ApproxPoint::reset(PointData& sb, const ObservationList& sm,
         const PointID& cb) */


  // public

  void ApproxPoint::calculation()
  {
    CoordinateGeometry2D* GU {nullptr};
    Select_solution_g2d* VR = new Select_solution_g2d(&SB,&SM);
    bool two_solutions = false;
    LocalPoint prv, dru;
    for(ObservationList::iterator j, i = SM.begin(); i != SM.end(); i++)
      for(j=i, ++j; j != SM.end(); j++)
        {
          switch (ObservationType(*i) + ObservationType(*j))
            {
            case 2*is_Distance :
              {
                Distance_distance* V = new Distance_distance(*i, *j, &SB, CB);
                V->calculation();
                GU = V;
              }
              break;
            case is_Distance + is_Direction :
              {
                Direction_distance* V = new Direction_distance(*i, *j, &SB);
                V->calculation();
                GU = V;
              }
              break;
            case is_Distance + is_Angle :
              {
                Distance_angle* V = new Distance_angle(*i, *j, &SB);
                V->calculation();
                GU = V;
              }
              break;
            case 2*is_Direction :
              {
                Direction_direction* V = new Direction_direction(*i, *j, &SB);
                V->calculation();
                GU = V;
              }
              break;
            case is_Direction + is_Angle :
              {
                Direction_angle* V = new Direction_angle(*i, *j, &SB);
                V->calculation();
                GU = V;
              }
              break;
            case 2*is_Angle :
              {
                Angle_angle* V = new Angle_angle(*i, *j, &SB);
                V->calculation();
                GU = V;
              }
              break;
            }
          switch (GU->number_of_solutions())
            {
            case 2 :
              {
                VR->calculation(GU->solution_1(),GU->solution_2());
                if(VR->state() == 1)         // unique solution selection
                  {
                    solved_points.push_back(VR->Solution());
                  }
                if(solved_points.empty() &&
                   (VR -> state() == 0)  &&  (!two_solutions))
                  {
                    prv = GU->solution_1();
                    dru = GU->solution_2();
                    two_solutions = true;
                  }
              }
              break;
            case 1 :
              {
                solved_points.push_back(GU->solution_1());
              }
              break;
            default : // void solution remains - nothing else could be done
              break;
            }
          delete GU;
        }
    // making final coordinates
    if(solved_points.size() > 0)
      {
        state_ = unique_solution;
        Statistics_g2d* ST = new Statistics_g2d(&solved_points);
        ST->calculation();
        v_point = ST->Median();
      }
    else
      state_ = no_solution;
    if(two_solutions && solved_points.empty())
      {
        v_point = prv;
        v_point2 = dru;
        state_ = ambiguous_solution;   // two solutions
      }

  }      // void ApproxPoint::calculation()



  void ApproxPoint::ArrangeObservations(ObservationList& psm_pom)
  {
    // ----------------------------------------------------------------------
    // in this function we use std:list instead of GNU_gama::List
    // tu allow 'erase' of list elements

    typedef std::list<Observation*> ObservationList;

    ObservationList sm_pom;
    for (GNU_gama::local::ObservationList::iterator
           i=psm_pom.begin(), e=psm_pom.end(); i!=e; ++i)
      {
        sm_pom.push_back(*i);
      }

    ObservationList SM_S;
    for (GNU_gama::local::ObservationList::iterator
           i=ApproxPoint::SM_S.begin(), e=ApproxPoint::SM_S.end(); i!=e; ++i)
      {
        SM_S.push_back(*i);
      }

    ObservationList SM_U;
    for (GNU_gama::local::ObservationList::iterator
           i=ApproxPoint::SM_U.begin(), e=ApproxPoint::SM_U.end(); i!=e; ++i)
      {
        SM_U.push_back(*i);
      }

    // ----------------------------------------------------------------------

    ObservationList::iterator i, j;
    // distances
    double med;
    std::vector<double> pom_sez;
    i = sm_pom.begin();
    while(i != sm_pom.end())
      {
        if(Distance* d1 = dynamic_cast<Distance*>(*i))
          {
            pom_sez.push_back(d1->value());
            // j = i+1;
            j = i; ++j;
            while(j != sm_pom.end())
              {
                Distance* d2 = dynamic_cast<Distance*>(*j);
                if(d2 && ((d1->from() == d2->from() &&
                           d1->to() == d2->to()) ||
                          (d1->to() == d2->from() &&
                           d1->from() == d2->to())))
                  {
                    pom_sez.push_back(d2->value());
                    j = sm_pom.erase(j);
                  }
                else
                  j++;
              }
            std::sort(pom_sez.begin(),pom_sez.end());
            std::vector<double>::size_type size = pom_sez.size();
            med = (g2d_even(size) ?
                   (pom_sez[size/2-1] + pom_sez[size/2])/2 :
                   pom_sez[(size+1)/2-1]);
            Distance* DD
              = new Distance(CB, (d1->from()==CB ?
                                  d1->to() :
                                  d1->from()), med);
            SM.push_back(DD);
            i = sm_pom.erase(i);
            pom_sez.erase(pom_sez.begin(), pom_sez.end());
          }
        else
          i++;
      }

    // outer bearings
    i = SM_S.begin();
    while(i != SM_S.end())
      {
        Direction* s1 = static_cast<Direction*>(*i);
        pom_sez.push_back(s1->value());
        // j = i+1;
        j = i; ++j;
        while(j != SM_S.end())
          {
            Direction* s2 = static_cast<Direction*>(*j);
            if(s1->from() == s2->from())
              {
                pom_sez.push_back(s2->value());
                j = SM_S.erase(j);
                delete(s2);
              }
            else
              j++;
          }
        std::sort(pom_sez.begin(),pom_sez.end());
        std::vector<double>::size_type size = pom_sez.size();
        med = (g2d_even(size) ?
               (pom_sez[size/2-1] + pom_sez[size/2])/2 :
               pom_sez[(size+1)/2-1]);
        Direction* SS = new Direction(s1->from(),s1->to(),med);
        SM.push_back(SS);
        i = SM_S.erase(i);
        delete(s1);
        pom_sez.erase(pom_sez.begin(), pom_sez.end());
      }

    // inner angles
    // in sm_pom are only inner angels now; dists. have been already removed
    // in SM_U are only angles
    i = sm_pom.begin();
    double u_mer;
    while(i != sm_pom.end())
      {
        Angle* u1 = static_cast<Angle*>(*i);
        j = SM_U.begin();
        pom_sez.push_back(u1->value());
        while(j != SM_U.end())
          {
            Angle* u2 = static_cast<Angle*>(*j);
            if(((u1->to()==u2->to())&&(u1->fs()==u2->fs()))||
               ((u1->to()==u2->fs())&&(u1->fs()==u2->to())))
              {
                u_mer = (u1->to() == u2->to() ?
                         u2->value()
                         : 2*M_PI-u2->value());
                pom_sez.push_back(u_mer);
                j = SM_U.erase(j);
                delete(u2);
              }
            else
              j++;
          }
        std::sort(pom_sez.begin(),pom_sez.end());
        std::vector<double>::size_type size = pom_sez.size();
        med = (g2d_even(size) ?
               (pom_sez[size/2-1] + pom_sez[size/2])/2 :
               pom_sez[(size+1)/2-1]);
        Angle* UU;
        if(med >= M_PI)
          UU = new Angle(u1->from(),u1->fs(),u1->to(),
                         M_PI*2-med);
        else
          UU = new Angle(u1->from(),u1->to(),u1->fs(),med);
        SM.push_back(UU);
        i = sm_pom.erase(i);
        pom_sez.erase(pom_sez.begin(), pom_sez.end());
      }
    i = SM_U.begin();
    // finishing remaining angles
    while(i != SM_U.end())
      {
        Angle* u1 = static_cast<Angle*>(*i);
        // j = i+1;
        j = i; ++j;
        pom_sez.push_back(u1->value());
        while(j != SM_U.end())
          {
            Angle* u2 = static_cast<Angle*>(*j);
            if(((u1->to()==u2->to())&&(u1->fs()==u2->fs()))||
               ((u1->to()==u2->fs())&&(u1->fs()==u2->to())))
              {
                u_mer = (u1->to() == u2->to() ?
                         u2->value() :
                         2*M_PI-u2->value());
                pom_sez.push_back(u_mer);
                j = SM_U.erase(j);
                delete(u2);
              }
            else
              j++;
          }
        std::sort(pom_sez.begin(),pom_sez.end());
        std::vector<double>::size_type size = pom_sez.size();
        med = (g2d_even(size) ?
               (pom_sez[size/2-1] + pom_sez[size/2])/2 :
               pom_sez[(size+1)/2-1]);
        Angle* UU;
        if(med >= M_PI)
          UU = new Angle(u1->from(),u1->fs(),u1->to(),
                         M_PI*2-med);
        else
          UU = new Angle(u1->from(),u1->to(),u1->fs(),med);
        SM.push_back(UU);
        i = SM_U.erase(i);
        delete(u1);
        pom_sez.erase(pom_sez.begin(), pom_sez.end());
      }

    // ----------------------------------------------------------------------
    // now we copy working std::lists back

    psm_pom.clear();
    for (ObservationList::iterator
           i=sm_pom.begin(), e=sm_pom.end(); i!=e; ++i)
      {
        psm_pom.push_back(*i);
      }

    ApproxPoint::SM_S.clear();
    for (ObservationList::iterator
           i=SM_S.begin(), e=SM_S.end(); i!=e; ++i)
      {
        ApproxPoint::SM_S.push_back(*i);
      }

    ApproxPoint::SM_U.clear();
    for (ObservationList::iterator
           i=SM_U.begin(), e=SM_U.end(); i!=e; ++i)
      {
        ApproxPoint::SM_U.push_back(*i);
      }

  }  // void ApproxPoint::ArrangeObservations()


 }}       // namespace GNU_gama::local
