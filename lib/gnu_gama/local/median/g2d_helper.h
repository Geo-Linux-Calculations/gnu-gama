/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                  2001  Ales Cepek  <cepek@fsv.cvut.cz>

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

/*********************************************************************
 * helper functions and classes:                                     *
 * ----------------------------------------------------------------- *
 * - typedef std::vector<LocalPoint> Helper_list;                    *
 * - inline int signum(const Double& d1)                             *
 * - inline Double g2d_sqr(const Double& d)                          *
 * - enum Solution_state_tag                                         *
 * - enum Observation_types                                          *
 * - inline Observation_types ObservationType(Observation* m)        *
 * - inline Double g2d_distance(const LocalPoint& b1, const Bod& b2) *
 * - inline bool g2d_even(std::vector<Double>::size_type& x)         *
 * - class Select_solution_g2d                                       *
 * - class Statistics_g2d                                            *
 * - class Transformation_g2d                                        *
 * - class SimilarityTr2D : public Transformation_g2d                *
 *********************************************************************/

#ifndef gama_local_g2d_helper_h_GNU_gama_local_Median_G_fce_H
#define gama_local_g2d_helper_h_GNU_gama_local_Median_G_fce_H

#include <algorithm>
#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/median/g2d_exception.h>

namespace GNU_gama { namespace local {

  // --------------------------------------------------------------

  typedef std::vector<LocalPoint> Helper_list;

  // --------------------------------------------------------------
  // inline int signum(const Double& d1) wass added into
  // gnu_gama/local-1.1.61 due to problems with MSC (AC)

  inline int signum(Double d1)
    {
      if(d1 < 0) return -1;
      if(d1 > 0) return  1;
      return 0;
    }

  // --------------------------------------------------------------

  inline Double g2d_sqr(const Double& d)
    {
      return (d*d);
    }

  // --------------------------------------------------------------

  enum Solution_state_tag
  {
    missing_init = -2,
    calculation_not_done = -1,
    no_solution = 0,
    unique_solution = 1,
    ambiguous_solution = 2,
    calculation_done = 3
  };

  // --------------------------------------------------------------

  enum Observation_types
  {
    is_Distance = 1,
    is_Direction = 10,
    is_Angle = 100
  };

  // --------------------------------------------------------------

  inline Observation_types ObservationType(const Observation* m)
    {
      if(/* const Distance  *d =*/ dynamic_cast<const Distance* >(m) )
	return is_Distance;

      if(/* const Direction *s =*/ dynamic_cast<const Direction*>(m) )
        return is_Direction;

      return is_Angle;
    }

  // --------------------------------------------------------------
  inline Double g2d_distance(const LocalPoint& b1, const LocalPoint& b2)
    {
      using namespace std;
      return sqrt(g2d_sqr(b1.x()-b2.x())+g2d_sqr(b1.y()-b2.y()));
    }

  // --------------------------------------------------------------
  inline bool g2d_even(std::vector<Double>::size_type& x)
    {
      // using namespace std;
      // return (fmod(x,2) == 0);
      return x%2 == 0;
    }


  // --------------------------------------------------------------
  // in the case of ambiguous (equivocal) solution, the one is chosen
  // to be in accordance with the others

  class Select_solution_g2d
    {
    private:

      Solution_state_tag state_;
      LocalPoint   B1, B2;
      PointData* SB;
      ObservationList* SM;

    public:
      Select_solution_g2d(PointData* sb, ObservationList* sm)
        : state_(calculation_not_done), SB(sb), SM(sm)
        {
        }
      Select_solution_g2d(LocalPoint& b1, LocalPoint& b2, PointData* sb,
                          ObservationList* sm) :
        state_(calculation_not_done), B1(b1), B2(b2), SB(sb), SM(sm)
        {
        }
      void calculation();
      void calculation(LocalPoint b1, LocalPoint b2)
        {
          state_ = calculation_not_done;
          B1 = b1; B2 = b2;
          calculation();
        }
      LocalPoint Solution()
        {
          if(state_ == calculation_not_done)
            throw g2d_exc("Select_solution_g2d: calculation not done");
          if(state_ == no_solution)
            throw g2d_exc("Select_solution_g2d: ambiguous solution");
          return B1;
        }
      int state() const
        {
          return (state_ > no_solution ? unique_solution : state_);
        }

    };  // class Select_solution_g2d


  // --------------------------------------------------------------
  // Statistics_g2d - calculation of medina from coordinate list

  class Statistics_g2d
    {
    private:

      Helper_list* PS;
      LocalPoint median;
      Solution_state_tag state_;

    public:
      Statistics_g2d() : state_(missing_init)
        {
        }
      Statistics_g2d(Helper_list* ps)
        : PS(ps), state_(calculation_not_done)
        {
        }
      void calculation();
      void calculation(Helper_list* ps)
        {
          state_ = calculation_not_done;
          PS = ps;
          calculation();
        }
      Solution_state_tag state() const
        {
          return state_;
        }
      LocalPoint Median()       // resulting coordinate
        {
          if(state_ < no_solution)
            throw g2d_exc("Statistics_g2d: calculation not done");

          return median;
        }

    };	// class Statistics_g2d


  // --------------------------------------------------------------
  // base transfromation class

  class Transformation_g2d
    {
    protected:

      PointData& SB;              // point list in target syste
      PointData& local;           // point list in local system
      PointIDList& computed;      // list of computed points
      PointData transf_points_;   // points transformed into target system
      virtual void reset() = 0;

      Solution_state_tag state_;

    public:
      Transformation_g2d(PointData& sb, PointData& locl,
                         PointIDList& comp)
        : SB(sb), local(locl), computed(comp), state_(calculation_not_done)
        {
        }
      virtual ~Transformation_g2d()
        {
          transf_points_.erase(transf_points_.begin(), transf_points_.end());
        }
      void reset(PointData& sb, PointData& locl, PointIDList& comp)
        {
          SB = sb;
          local = locl;
          computed = comp;
          state_ = calculation_not_done;
          transf_points_.erase(transf_points_.begin(), transf_points_.end());
          reset();
        }
      virtual void calculation() = 0;
      Solution_state_tag state() const
        {
          return state_;
        }
      PointData transf_points() const
        {
          if(state_ == calculation_not_done)
            throw g2d_exc("Transformation_g2d: calculation not done");
          if(state_ == no_solution)
            throw
              g2d_exc("Transformation_g2d: not enough of identical points");

          return transf_points_;
        }

    };  // class Transformation_g2d

  // --------------------------------------------------------------
  // Transformation in plane
  // Transformation from minimal number of point - selects the best tuple
  // - similarity transformation
  /*
   * transformation key for similarity transformation:
   *
   *  | x |   |tx | |a1  -a2| |x'|
   *  |   | = |   |+|       |*|  |
   *  | y |   |ty | |a2   a1| |y'|
   *
   *  transf_key[0] = a2
   *  transf_key[1] = a1
   *  transf_key[2] = ty
   *  transf_key[3] = tx
   *
   */

  class SimilarityTr2D : public Transformation_g2d
    {
    private:

      std::vector<Double> transf_key_;
      void reset();
      bool Given_point(const PointID& cb)
        {
          return (std::find(computed.begin(), computed.end(), cb) ==
                  computed.end());
        }
      void Identical_points(PointData::iterator& b1,
                            PointData::iterator& b2);
      void transformation_key(PointData::iterator& b1,
                              PointData::iterator& b2);

    public:
      SimilarityTr2D(PointData& sb, PointData& locl, PointIDList& comp)
        : Transformation_g2d(sb, locl, comp)
        {
          reset();
        }
      void calculation();
      std::vector<Double> transf_key() const
        {
          if(state_ == calculation_not_done)
            throw g2d_exc("SimilarityTr2D: calculation not done");
          if(state_ == no_solution)
            throw g2d_exc("SimilarityTr2D: not enough of identical poinst");
          return transf_key_;
        }

    };  // class SimilarityTr2D

 }} // namespace GNU_gama::local

#endif
