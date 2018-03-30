/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                2001, 2018  Ales Cepek  <cepek@gnu.org>

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
 * helper functions and classes                              *
 *************************************************************/

/* try /catch block was added in all methods "calculation()" in
 * Median.  Exceptions Median::g2d_exc are sent by throw command for
 * next processing, for all other exceptions is returned state =
 * no_solution or number_of_solutions_ = -1. This change was motivated
 * by a bug, when in Median failed computation of bearing for two
 * identical points.
 *
 * AC 2000.04.26 change median-0.7.5 / gnu_gama/local-0.9.57 */


#include <gnu_gama/local/median/g2d_helper.h>
#include <gnu_gama/local/pobs/bearing.h>

using namespace std;

namespace GNU_gama { namespace local
{

// ** Tolerance for selection of unique solution, see Charamza pp. 152-154 **
// ** distance  : 1 m
// ** direction : 1/distance [rad]
// ** angle     : 1/average distance [rad]

void Select_solution_g2d::calculation()
{
  /* ***********************************************************************

     Change  median-0.7.4  / gnu_gama/local-0.9.35  AC            1999.12.30
     ------------------------------------------

     I think, that calculation() didn't do exactly, what describes
     Charamza on p. 153 Geolib/PC, ie. that there was missing the test
     |delta_i| < tol_i, i=1,2.

     And as I was digging through sources I have added tests on zero
     distances. Thus in principle we calculate reciprocal value of tol_i.

   ************************************************************************ */

  try {

    state_ = no_solution;
    double delta1, delta2, tol1, tol2;
    LocalPoint PB1, PB2;
    for(ObservationList::const_iterator i = SM->begin(); i != SM->end(); i++)
      {
        switch (ObservationType(*i))
          {
          case is_Distance :
            {
              tol1 = tol2 = 1;
              PB1 = (*(SB->find((*i)->to()))).second;
              delta1 = fabs((*i)->value() - g2d_distance(B1,PB1));
              delta2 = fabs((*i)->value() - g2d_distance(B2,PB1));
            }
            break;
          case is_Direction :
            {
              PB1 = (*(SB->find((*i)->from()))).second;
              tol1 = g2d_distance(B1,PB1);      // was  1/g2d_distance(...)
              tol2 = g2d_distance(B2,PB1);      // ...
              delta1 = fabs((*i)->value() - bearing(PB1,B1));
              delta2 = fabs((*i)->value() - bearing(PB1,B2));
            }
            break;
          case is_Angle :
            {
              const Angle* u = dynamic_cast<const Angle*>(*i);
              PB1 = (*(SB->find(u->to()))).second;
              PB2 = (*(SB->find(u->fs()))).second;
              double uu1 = bearing(B1,PB2)-bearing(B1,PB1);
              double uu2 = bearing(B2,PB2)-bearing(B2,PB1);
              uu1 += (uu1 < 0 ? 2*M_PI : 0);
              uu2 += (uu2 < 0 ? 2*M_PI : 0);
              tol1 = (g2d_distance(B1,PB1)+g2d_distance(B1,PB2))/2;
              tol2 = (g2d_distance(B2,PB1)+g2d_distance(B2,PB2))/2;
              delta1 = fabs(u->value()-uu1);
              delta2 = fabs(u->value()-uu2);
            }
            break;
          }

        // in geodesy observed distances are usually longer then 10 cm
        // ... thus I consider this to be zero
        if (tol1 < 0.1 || tol2 < 0.1) continue;

        // ********* this test was missing *********
        if (delta1 < tol1 && delta2 < tol2) continue;
        // *****************************************

        delta1 *= tol1;
        delta2 *= tol2;

        if(delta1 > 10*delta2)
          {
            B1 = B2;		        // second solution selected
            state_ = unique_solution;
            break;
          }
        if(delta1 < 0.1*delta2)
          {
            state_ = unique_solution;    // first solution selected
            break;
          }
      }
    return;

  }
  catch (g2d_exc& exc)
    {
      throw exc;
    }
  catch (...)
    {
      state_ = no_solution;
      return;
    }

}  // void Select_solution_g2d::calculation()


// ----------------------------------------------------------

void Statistics_g2d::calculation()
{
  try {
    state_ = unique_solution;
    if(PS->empty())
      throw g2d_exc("Statistics_g2d: empty list");
    Helper_list::size_type n = PS->size();
    if(n == 1)
      {
        median = *(PS->begin());
        return;
      }
    std::vector<double> Y, X;
    for(Helper_list::const_iterator i = PS->begin(); i != PS->end(); i++)
      {
        Y.push_back(i->y());
        X.push_back(i->x());
      }
    std::sort(Y.begin(), Y.end());
    std::sort(X.begin(), X.end());
    std::vector<double>::size_type size = Y.size();
    double x, y;
    x = (g2d_even(size) ? (X[size/2-1] + X[size/2])/2 : X[(size+1)/2-1]);
    y = (g2d_even(size) ? (Y[size/2-1] + Y[size/2])/2 : Y[(size+1)/2-1]);
    median.set_xy(x, y);
    return;

  }
  catch (g2d_exc& exc)
    {
      throw exc;
    }
  catch (...)
    {
      state_ = no_solution;
      return;
    }

}	// void Statistics_g2d::calculation()


// ----------------------------------------------------------

void SimilarityTr2D::reset()
{
  transf_key_.erase(transf_key_.begin(), transf_key_.end());
  // clear  test_xy() = false from local
  // the checj for needed number of identical points
  PointData pom_sb;
  int pocet_identickych = 0;
  for(PointData::iterator j = local.begin(); j != local.end(); j++)
    if(Given_point((*j).first) && (*j).second.test_xy())
      pocet_identickych++;
  if(pocet_identickych < 2)
    state_ = no_solution;
  // nothing to transform
  if(computed.empty())
    state_ = no_solution;
}  //  void SimilarityTr2D::reset()

// the best pair - transformed points are close to circle around the
// set of identical points

void SimilarityTr2D::Identical_points(PointData::iterator& b1,
                                      PointData::iterator& b2)
{
  LocalPoint stred;
  double pomocna_d1, delka_max;
  double rozdil_min = 1e5;
  PointData::iterator pom1, pom2;
  for(PointData::iterator i = local.begin(); i != local.end(); i++)
    if(Given_point((*i).first) && (*i).second.test_xy())
      for(PointData::iterator j = local.begin(); j != local.end(); j++)
        if(Given_point((*j).first) && (i != j) && (*j).second.test_xy())
        {
          stred.set_xy(((*i).second.x()+(*j).second.x())/2,
                       ((*i).second.y()+(*j).second.y())/2);
          delka_max = 0;
          for(PointIDList::iterator cb = computed.begin();
	      cb != computed.end(); cb++)
          {
            pomocna_d1 = g2d_distance(stred, local[*cb]);
            if(pomocna_d1 > delka_max)
              delka_max = pomocna_d1;
          }
          pomocna_d1 = fabs(g2d_distance(stred, (*i).second) - delka_max);
          if(pomocna_d1 < rozdil_min)
          {
            rozdil_min = pomocna_d1;
            pom1 = i;
            pom2 = j;
          }
        }
  b1 = pom1;
  b2 = pom2;
}


void SimilarityTr2D::transformation_key(PointData::iterator& b1,
                                        PointData::iterator& b2)
{
  LocalPoint odkud1, odkud2, kam1, kam2;
  odkud1 = (*b1).second;
  odkud2 = (*b2).second;
  PointData::iterator pom;
  pom = SB.find((*b1).first);
  if(pom == SB.end())
    throw g2d_exc("SimilarityTr2D: identical point doesn't exist"
		  " in target coordinate system - "+(*b1).first.str());
  kam1 = (*pom).second;
  pom = SB.find((*b2).first);
  if(pom == SB.end())
    throw g2d_exc("SimilarityTr2D: identical point doesn't exist"
		  " in target coordinate system - "+(*b2).first.str());
  kam2 = (*pom).second;
  double dy1, dy2, dx1, dx2;
  dy1 = odkud2.y() - odkud1.y();
  dx1 = odkud2.x() - odkud1.x();
  dy2 = kam2.y() - kam1.y();
  dx2 = kam2.x() - kam1.x();
  transf_key_.push_back((dy2*dx1-dx2*dy1)/(g2d_sqr(dx1)+g2d_sqr(dy1)));
  transf_key_.push_back((dy1*dy2+dx1*dx2)/(g2d_sqr(dx1)+g2d_sqr(dy1)));
  transf_key_.push_back(kam1.y()-transf_key_[1]*odkud1.y()-
                        transf_key_[0]*odkud1.x());
  transf_key_.push_back(kam1.x()-transf_key_[1]*odkud1.x()+
                        transf_key_[0]*odkud1.y());
}

void SimilarityTr2D::calculation()
{

  try {
    // not enough identical points
    if(state_ == no_solution)
      return;
    PointData::iterator identicky1, identicky2;
    Identical_points(identicky1, identicky2);
    transformation_key(identicky1, identicky2);
    LocalPoint pom;
    PointData::iterator pom_i;
    for(PointIDList::iterator cb=computed.begin(); cb!=computed.end(); cb++)
      {
        pom_i = local.find((*cb));
        if(pom_i == SB.end())
          throw g2d_exc("SimilarityTr2D: point from the list of computed"
                         " points is missing in local coordinate system");
        pom = (*pom_i).second;
        if(pom.test_xy())
          {
            transf_points_[(*cb)] =
              LocalPoint(
                        transf_key_[3] + transf_key_[1]*pom.x() -
                                         transf_key_[0]*pom.y(),
                        transf_key_[2] + transf_key_[1]*pom.y() +
                                         transf_key_[0]*pom.x()
                        );

          }
      }
    state_ = calculation_done;
    return;

  }
  catch (g2d_exc& exc)
    {
      throw exc;
    }
  catch (...)
    {
      state_ = no_solution;
      return;
    }

}  // void SimilarityTr2D::calculation()

}} // namespace GNU_gama::local
