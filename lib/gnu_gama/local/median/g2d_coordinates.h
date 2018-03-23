/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                  2001, 2012  Ales Cepek  <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*************************************************************
 * computation of approximate coordinates:                   *
 * - single point                                            *
 * - list of points                                          *
 * - all points with unknown coordinates                     *
 *************************************************************/

#ifndef gama_local_g2d_coordinates_h_GNU_gama_local_Median_Pribl_s_H
#define gama_local_g2d_coordinates_h_GNU_gama_local_Median_Pribl_s_H

#include <algorithm>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/median/g2d_helper.h>
#include <gnu_gama/local/median/g2d_cogo.h>


namespace GNU_gama { namespace local {

  class ApproximateCoordinates
    {
    private:

      // point list used to return results
      PointData&       SB;
      ObservationData& OD;
      ObservationList  SM;

      // Solution_state_tag -> see g2d_helper.h
      Solution_state_tag state;

      // list of selected points (with unknown coordinates)
      PointIDList selected;

      // list of solved points
      PointData solved_pd;
      int depth;

      // number of points with known coordinates
      int known_coordinates_;

      bool absent(PointID cb)
        {
          return
            (std::find(selected.begin(),selected.end(),cb) == selected.end());
        }

      bool local_observations(ObservationList::iterator sm, PointIDList sb)
        {
          bool pom = false;
          pom = (std::find(sb.begin(), sb.end(), (*sm)->from()) != sb.end()) &&
            (std::find(sb.begin(), sb.end(), (*sm)->to()) != sb.end());
          if(Angle* u = dynamic_cast<Angle*>(*sm))
            {
              pom = pom && (std::find(sb.begin(), sb.end(), u->fs())
                            != sb.end());
            }
          return pom;
        }

      void reset();

      bool observation_hasID(ObservationList::iterator m,
                             PointIDList::iterator cb)
        {
          bool pom = (((*m)->from() == (*cb)) || ((*m)->to() == (*cb)));

          if (Angle* u = dynamic_cast<Angle*>(*m))
            {
              pom = pom || (u->fs() == (*cb));
            };
          return pom;
        }

      // true - at least two points exist with known coordinates
      bool solvable_data(PointData& b);

      // true - at least 2 observations with point ID exist
      bool necessary_observations(PointID ID);

      // in lists SM and SB finds points without coordinates and
      // stores their IDs in "selected" list
      void find_missing_coordinates();

      // move point with ID from list From to list To
      void move_point(PointData& From, PointData& To, PointID& ID);

      // solution by simple intersection; true - at least one point solved
      bool solve_intersection(PointData& body, PointIDList& co);

      // computation of points that cannot be solved by a simple
      // intersection, eg polygonal traverse inserted between two
      // known points; true - coordinates of at least one point solved
      bool solve_insertion();

      // combines both previous methods
      void computational_loop();

      void copy_horizontal(const ObservationData& from, ObservationList& to);

      ApproximateCoordinates(PointData& b, ObservationData& m, int vn)
        : SB(b), OD(m), depth(vn)
        {
          copy_horizontal(OD, SM);
          reset();
        }

      void reset(PointData& b, ObservationList& m)
        {
          SB = b;
          SM = m;
          depth = 0;
          reset();
        }

    public:

      ApproximateCoordinates(PointData& b, ObservationData& m)
        : SB(b), OD(m),  depth(0)
        {
          set_small_angle_limit();

          copy_horizontal(OD, SM);
          reset();
        }

      bool small_angle_detected() const
      {
         return CoordinateGeometry2D::small_angle_detected_;
      }

      // one point (even if already solved); true - succeeded to get
      // coordinates
      bool calculation(PointID cb);

      // poinst in PointIDList (even those already solved); true -
      // succeded to get all coordinates
      bool calculation(PointIDList cb);

      // all points without coordinates; true - succeeded to get all
      // coordinates
      bool calculation()
        {
          // looking for all points not placed in the point list (ie
          // found only in observation list) and points without
          // coordinates
          state = calculation_done;

          // gnu_gama/local-0.9.51 (AC) ... added test on empty lists
          // gnu_gama/local-1.1.51 (AC) ... the test moved where it belongs ;-)
          if (SB.empty() || SM.empty()) return true;

          find_missing_coordinates();
          if(!solvable_data(SB))
            return false;
          computational_loop();
          return all_is_solved();
        }

      bool all_is_solved() const
        {
          if(state <= calculation_not_done)
            throw g2d_exc("ApproximateCoordinates::"
                          "all_is_solved - nothing to do");
          return selected.empty();
        }

      PointIDList unsolved() const
        {
          if(state <= calculation_not_done)
            throw g2d_exc("ApproximateCoordinates::"
                          "unsolved - calculation not done");
          return selected;
        }

      PointData solved() const
        {
          if(state <= calculation_not_done)
            throw g2d_exc("ApproximateCoordinates::"
                          "solved - calculation not done");
          return solved_pd;
        }

      int Total_points () const
        {
          return SB.size();
        }

      int Total_observations () const
        {
          return SM.size();
        }

      // number of points with known coordinates (see reset)
      int Known_coordinates() const
        {
          return known_coordinates_;
        }

      double small_angle_limit() const
      {
        return CoordinateGeometry2D::small_angle_limit();
      }
      void set_small_angle_limit(double sal=0)
      {
        CoordinateGeometry2D::set_small_angle_limit(sal);
      }

    };


 }} // namespace GNU_gama::local


#endif
