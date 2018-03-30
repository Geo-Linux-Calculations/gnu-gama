/*
  GNU Gama C++ library
  Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                2001, 2012, 2018  Ales Cepek  <cepek@gnu.org>

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
 * 2d coordinate geometry                                    *
 * --------------------------------------------------------- *
 * - class CoordinateGeometry2D                              *
 * - class Distance_distance : public CoordinateGeometry2D   *
 * - class Direction_direction : public CoordinateGeometry2D *
 * - class Direction_distance : public CoordinateGeometry2D  *
 * - class Direction_angle : public CoordinateGeometry2D     *
 * - class Distance_angle : public CoordinateGeometry2D      *
 * - class Angle_angle : public CoordinateGeometry2D         *
 * - class Circle : public CoordinateGeometry2D              *
 *************************************************************/

#ifndef gama_local_g2d_cogo_h_GNU_gama_local_Median_G_ulohy_H
#define gama_local_g2d_cogo_h_GNU_gama_local_Median_G_ulohy_H

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/median/g2d_exception.h>

namespace GNU_gama { namespace local {

  class CoordinateGeometry2D
    {

    protected:
      int number_of_solutions_;
      LocalPoint*  point1;
      LocalPoint*  point2;
      PointData*   SB;
      virtual void observation_check(Observation*, Observation*) = 0;

      static double small_angle_limit_;
      static bool   small_angle_detected_;

    private:

      friend class  ApproximateCoordinates;
      static double small_angle_limit();
      static bool   small_angle_detected();
      static void   set_small_angle_limit(double sal=0);

    public:
      CoordinateGeometry2D(PointData* sb) : number_of_solutions_(-1), SB(sb)
        {
          point1 = new LocalPoint;
          point2 = new LocalPoint;

	  // Implicit value 0.15 for detecting small angles is ~ 10 gons.
	  // Static variable small_angle_limit_ is by default set to 0,
	  // CoordinateGeometry2D constructor must guarantee that the value
	  // is initialized properly.
	  if (small_angle_limit_ <= 0) set_small_angle_limit();
        }
      virtual ~CoordinateGeometry2D()
        {
          delete point1;
          delete point2;
        }
      virtual void calculation() = 0;
      int number_of_solutions() const
        {
          return number_of_solutions_;
        }
      LocalPoint solution_1() const
        {
          if(number_of_solutions_ == -1)
            throw g2d_exc("CoordinateGeometry2D: calculation not done");
          if(number_of_solutions_ < 1)
            throw g2d_exc("CoordinateGeometry2D: no solution");
          return *point1;
        }
      LocalPoint solution_2() const
        {
          if(number_of_solutions_ == -1)
            throw g2d_exc("CoordinateGeometry2D: calculation not done");
          if(number_of_solutions_ < 2)
            throw g2d_exc("CoordinateGeometry2D: two solutions");
          return *point2;
        }
    };


  //---------------------------------------------------------------

  class Distance_distance : public CoordinateGeometry2D
    {

    private:
      Distance*   h1;
      Distance*   h2;
      PointID     CB;     // for easier search in computed distances
      double      r1, r2;
      LocalPoint  B1;
      LocalPoint  B2;
      void observation_check(Observation*, Observation*);

    public:
      Distance_distance() : CoordinateGeometry2D(0), h1(0), h2(0), r1(-1)
        {
        }
      Distance_distance(Observation* m1, Observation* m2,
                        PointData* sb, PointID cb)
        : CoordinateGeometry2D(sb), CB(cb), r1(-1)
        {
          observation_check(m1, m2);
        }
      Distance_distance(double& m1, double& m2,
                        LocalPoint b1, LocalPoint b2, PointData* sb)
        : CoordinateGeometry2D(sb), r1(m1), r2(m2), B1(b1), B2(b2)
        {
        }
      ~Distance_distance()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2,
                           PointData* sb, PointID cb)
        {
          point1 = point2 = 0;
          SB = sb;
          CB = cb;
          r1 = -1;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------

  class Direction_direction : public CoordinateGeometry2D
    {

    private:
      Direction* h1;
      Direction* h2;
      void observation_check(Observation*, Observation*);

    public:
      Direction_direction() : CoordinateGeometry2D(0), h1(0), h2(0)
        {
        }
      Direction_direction(Observation* m1, Observation* m2, PointData* sb)
        : CoordinateGeometry2D(sb)
        {
          observation_check(m1, m2);
        }
      ~Direction_direction()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2, PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------

  class Direction_distance : public CoordinateGeometry2D
    {

    private:

      Direction*  h1;
      Distance*   h2;
      LocalPoint  B;
      double r;
      void observation_check(Observation*, Observation*);

    public:

      Direction_distance() : CoordinateGeometry2D(0), h1(0), h2(0), r(-1)
        {
        }
      Direction_distance(Observation* m1, Observation* m2, PointData* sb)
        : CoordinateGeometry2D(sb), r(-1)
        {
          observation_check(m1, m2);
        }
      Direction_distance(Direction* m1, double m2, LocalPoint b, PointData* sb)
        : CoordinateGeometry2D(sb), h1(m1), B(b), r(m2)
        {
        }
      ~Direction_distance()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2, PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          r = -1;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------

  class Direction_angle : public CoordinateGeometry2D
    {

    private:
      Direction* h1;
      Angle* h2;
      void observation_check(Observation*, Observation*);

    public:
      Direction_angle() : CoordinateGeometry2D(0), h1(0), h2(0)
        {
        }
      Direction_angle(Observation* m1, Observation* m2, PointData* sb)
        : CoordinateGeometry2D(sb)
        {
          observation_check(m1, m2);
        }
      ~Direction_angle()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2, PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------

  class Distance_angle : public CoordinateGeometry2D
    {

    private:
      Distance* h1;
      Angle* h2;
      void observation_check(Observation*, Observation*);

    public:
      Distance_angle() : CoordinateGeometry2D(0), h1(0), h2(0)
        {
        }
      Distance_angle(Observation* m1, Observation* m2, PointData* sb)
        : CoordinateGeometry2D(sb)
        {
          observation_check(m1, m2);
        }
      ~Distance_angle()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2, PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------

  class Angle_angle : public CoordinateGeometry2D
    {

    private:
      Angle* h1;
      Angle* h2;
      void observation_check(Observation*, Observation*);

    public:
      Angle_angle() : CoordinateGeometry2D(0), h1(0), h2(0)
        {
        }
      Angle_angle(Observation* m1, Observation* m2, PointData* sb)
        : CoordinateGeometry2D(sb)
        {
          observation_check(m1, m2);
        }
      ~Angle_angle()
        {
        }
      void calculation();
      void new_calculation(Observation* m1, Observation* m2, PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          observation_check(m1, m2);
          calculation();
        }
    };


  //---------------------------------------------------------------
  // ** circle parameters from two points and perimeter angle **

  class Circle : public CoordinateGeometry2D
    {

    private:
      Angle*      h1;
      LocalPoint  B1, B2;
      double      R;
      void observation_check(Observation*, Observation*) {}

    public:
      Circle() : CoordinateGeometry2D(0), h1(0)
        {
        }
      Circle(Angle* u,PointData* sb) : CoordinateGeometry2D(sb), h1(u)
        {
        }
      ~Circle()
        {
        }
      void calculation();
      void new_calculation(Angle* u,PointData* sb)
        {
          point1 = point2 = 0;
          SB = sb;
          h1 = u;
          calculation();
        }
      double radius() const
        {
          if(number_of_solutions_ == -1)
            throw g2d_exc("Circle: computation not done");
          if(number_of_solutions_ < 1)
            throw g2d_exc("Circle: two solutions");
          return R;
        }
    };

  }} // namespace GNU_gama::local

#endif
