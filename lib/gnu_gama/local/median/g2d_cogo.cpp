/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                2001, 2012, 2014, 2018  Ales Cepek  <cepek@gnu.org>

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

 /**************************************************************
  * 2d coordinate geometry                                     *
  **************************************************************/

 /*
  * In Median in all methods "calculation" was added try / catch
  * block. Exceptions g2d_exc are sent by command throw for next
  * processing, for all other exceptions is returned state =
  * no_solution or number_of_solutions_ = -1. This change was motivated
  * by a bug, when in Median computaion of bearing for two identical
  * points failed.
  *
  * AC 2000.04.26 change median-0.7.5 / gnu_gama/local-0.9.57
  * AC 2001.04.20 gnu_gama/local-1.1.61 */

#include <gnu_gama/local/median/g2d_cogo.h>
#include <gnu_gama/local/median/g2d_exception.h>
#include <gnu_gama/local/median/g2d_helper.h>
#include <gnu_gama/local/pobs/bearing.h>

using namespace std;

namespace GNU_gama { namespace local {


    double CoordinateGeometry2D::small_angle_limit_    = 0;
    bool   CoordinateGeometry2D::small_angle_detected_ = false;

    double CoordinateGeometry2D::small_angle_limit()
    {
      return small_angle_limit_;
    }
    bool CoordinateGeometry2D::small_angle_detected()
    {
      return small_angle_detected_;
    }
    void CoordinateGeometry2D::set_small_angle_limit(double sal)
    {
      if (sal <= 0)  sal = 0.15;
      small_angle_limit_ = sal;
      small_angle_detected_ = false;
    }


  // ************** Distance_distance *********************

  void Distance_distance::observation_check(Observation* m1, Observation* m2)
  {
    h1 = dynamic_cast<Distance*>(m1);
    h2 = dynamic_cast<Distance*>(m2);
    if(!(h1 && h2))
      throw g2d_exc("Distance_distance: wrong observation type");
  }

  // ** computation: CHARAMZA, pp.127-130 **

  void Distance_distance::calculation()
  {
    try {

      number_of_solutions_ = 0;             // -1 when computation not done
      if(r1 == -1)
        {
          PointID CB1 = h1->to();
          PointID CB2 = h2->to();
          B1 = (*(SB->find(CB1))).second;
          B2 = (*(SB->find(CB2))).second;
          r1 = h1->value();
          r2 = h2->value();
        };
      double dy = B2.y() - B1.y();
      double dx = B2.x() - B1.x();
      double s12 = sqrt(g2d_sqr(dy)+g2d_sqr(dx));
      if(s12 == 0)                  // given identical points; no solution
        return;
      double s1 = r1 / s12;
      double s2 = r2 / s12;
      double f = ((s1+s2)*(s1-s2)+1)/2;
      double g = (s1+f)*(s1-f);
      if(g < 0)                     // intersection doesn't exist
        return;
      if(sqrt(g) < (small_angle_limit_*s1*s2))    // intersection angle < 10 gon
        {
          small_angle_detected_ = true;
          return;
        }
      point1->set_xy(B1.x()+dx*f-dy*sqrt(g), B1.y()+dy*f+dx*sqrt(g));
      number_of_solutions_ = 1;
      if(g > 0)
        {
          point2->set_xy(B1.x()+dx*f+dy*sqrt(g), B1.y()+dy*f-dx*sqrt(g));
          number_of_solutions_ = 2;
        };
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Distance_distance::calculation


  //----------------------------------------------------------------
  // ************** Direction_direction *********************

  void Direction_direction::observation_check(Observation* m1, Observation* m2)
  {
    h1 = dynamic_cast<Direction*>(m1);
    h2 = dynamic_cast<Direction*>(m2);
    if(!(h1 && h2))
      throw g2d_exc("Direction_direction: wrong observation type");
  }


  // ** computation: CHARAMZA, pp.131-133 **

  void Direction_direction::calculation()
  {
    try {

      number_of_solutions_ = 0;           // -1 when computation not done
      if(fabs(sin(h2->value())) < fabs(sin(h1->value())))
        {
          Direction* h = h1;
          h1 = h2;
          h2 = h;
        };
      const LocalPoint B1 = (*(SB->find(h1->from()))).second;
      const LocalPoint B2 = (*(SB->find(h2->from()))).second;
      double jmen = cos(h1->value())*sin(h2->value())
        -sin(h1->value())*cos(h2->value());
      if(fabs(jmen) < small_angle_limit_)       // unreliable intersection
        {
          small_angle_detected_ = true;
          return;
        }
      double dy = (sin(h1->value())*sin(h2->value())*(B2.x()-B1.x())-
                   cos(h1->value())*sin(h2->value())*(B2.y()-B1.y()))/jmen;
      /*
       * if((signum(dy) != signum(sin(h2->value()))) ||
       *   (signum(B2.x()+(dy*cos(h2->value()))/sin(h2->value())-B1.x()) !=
       *    signum(cos(h1->value()))))
       *  return; //intersection doesn't exist; intersection in the given point
       *
       * AC gnu_gama/local-1.1.61 : problems with MSC
       */
      const int s_1=signum(dy);
      const int s_2=signum(sin(h2->value()));
      const int s_3=signum(B2.x()+(dy*cos(h2->value()))
                           /sin(h2->value())-B1.x());
      const int s_4=signum(cos(h1->value()));
      if ((s_1 != s_2) || (s_3 != s_4)) return;

      point1->set_xy(B2.x()+(dy*cos(h2->value()))/sin(h2->value()), B2.y()+dy);
      number_of_solutions_ = 1;
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Direction_direction::calculation


  // -----------------------------------------------------------------
  // ************** Direction_distance *********************

  void Direction_distance::observation_check(Observation* m1, Observation* m2)
  {
    if((h1 = dynamic_cast<Direction*>(m1)) != 0)
      h2 = dynamic_cast<Distance*>(m2);
    else
      {
        h1 = dynamic_cast<Direction *>(m2);
        h2 = dynamic_cast<Distance*>(m1);
      };
    if(!(h1 && h2))
      throw g2d_exc("Direction_distance: wrong observation type");
  }


  // ** computation: CHARAMZA, pp.115-118 **
  void Direction_distance::calculation()
  {
    try {

      number_of_solutions_ = 0;    // -1 when computation not done
      const LocalPoint B1 = (*(SB->find(h1->from()))).second;
      LocalPoint B2;
      if(r == -1)                 // input Direction*, Distance*
        {
          if(SB->find(h1->to()) == SB->find(h2->to()))
            B2 = (*(SB->find(h2->from()))).second;
          else
            B2 = (*(SB->find(h2->to()))).second;
          r = h2->value();
        }
      else                        // input Direction*, double, LocalPoint
        B2 = B;
      if(r <= 0)                  // radius <= 0
        return;
      double yp = (B1.y()-B2.y())*cos(h1->value())
        -(B1.x()-B2.x())*sin(h1->value());
      if(fabs(yp) > r) // semi-line outside circle; intersection doesn't exist
        return;
      double xp = (B1.x()-B2.x())*cos(h1->value())
        +(B1.y()-B2.y())*sin(h1->value());
      double x1 = sqrt(g2d_sqr(r)-g2d_sqr(yp));
      if(x1 <= xp)     // semi-line outside circle; intersection doesn't exist
        return;
      if(x1 < (small_angle_limit_*r))      // intersection angle < 10 gon
        {
          small_angle_detected_ = true;
          return;
        }
      point1->set_xy(B2.x()+x1*cos(h1->value())-yp*sin(h1->value()),
                     B2.y()+yp*cos(h1->value())+x1*sin(h1->value()));
      number_of_solutions_ = 1;
      if((-x1) <= (xp+1e-6)) // only one solution; 1e-6 ==> roundoff
        return;
      point2->set_xy(B2.x()-x1*cos(h1->value())-yp*sin(h1->value()),
                     B2.y()+yp*cos(h1->value())-x1*sin(h1->value()));
      number_of_solutions_ = 2;
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Direction_distance::calculation()


  //----------------------------------------------------------------
  // ************** Direction_angle *********************

  void Direction_angle::observation_check(Observation* m1, Observation* m2)
  {
    if((h1 = dynamic_cast<Direction*>(m1)) != 0)
      h2 = dynamic_cast<Angle*>(m2);
    else
      {
        h1 = dynamic_cast<Direction*>(m2);
        h2 = dynamic_cast<Angle*>(m1);
      };
    if(!(h1 && h2))
      throw g2d_exc("Direction_angle: wrong observation type");
  }


  // ** computation: CHARAMZA, p.142 **

  void Direction_angle::calculation()
  {
    try {

      number_of_solutions_ = 0;           // -1 when computation not done
      Circle K(h2,SB);
      K.calculation();
      if(K.number_of_solutions() < 1)    // circle parameters not solved
        return;
      Direction_distance SD(h1,K.radius(),K.solution_1(),SB);
      SD.calculation();
      if(SD.number_of_solutions() < 1)   // no intersection exist
        return;
      LocalPoint B1 = (*(SB->find(h2->bs()))).second;
      LocalPoint B2 = (*(SB->find(h2->fs()))).second;
      double uu = bearing(SD.solution_1(),B2) - bearing(SD.solution_1(),B1);
      uu += (uu < 0 ? 2*M_PI : 0);
      // uu should be equal to h2->value(), but ...  uu is either
      // value() or value()+-PI
      if((uu < (h2->value()+0.1)) && (uu > (h2->value()-0.1)))
        {
          point1->set_xy(SD.solution_1().x(), SD.solution_1().y());
          number_of_solutions_ = 1;
        };
      if(SD.number_of_solutions() > 1)
        {
          uu = bearing(SD.solution_2(),B2) - bearing(SD.solution_2(),B1);
          uu += (uu < 0 ? 2*M_PI : 0);
          if((uu < (h2->value()+0.1)) && (uu > (h2->value()-0.1)))
	  { // added { to avoid dangling else 
            if(number_of_solutions_ == 1)
              {
                point2->set_xy(SD.solution_2().x(), SD.solution_2().y());
                number_of_solutions_ = 2;
              }
            else
              {
                point1->set_xy(SD.solution_2().x(), SD.solution_2().y());
                number_of_solutions_ = 1;
              };
	  } //  added } to avoid dangling else
        };
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Direction_angle::calculation()


  //----------------------------------------------------------------
  // ************** Distance_angle *********************

  void Distance_angle::observation_check(Observation* m1, Observation* m2)
  {
    if((h1 = dynamic_cast<Distance*>(m1)) != 0)
      h2 = dynamic_cast<Angle*>(m2);
    else
      {
        h1 = dynamic_cast<Distance*>(m2);
        h2 = dynamic_cast<Angle *>(m1);
      };
    if(!(h1 && h2))
      throw g2d_exc("Distance_angle: wrong observation type");
  }


  // ** computiong: CHARAMZA, p.143 **

  void Distance_angle::calculation()
  {
    try {

      number_of_solutions_ = 0;           // -1 when computation not done
      Circle K(h2,SB);
      K.calculation();
      if(K.number_of_solutions() < 1)    // computation of circle falied
        return;
      PointID CBB = (h1->from() == h2->from() ? h1->to() : h1->from());
      LocalPoint BB = (*(SB->find(CBB))).second;
      double dd1 = h1->value();
      double dd2 = K.radius();
      Distance_distance DD(dd1,dd2,BB,K.solution_1(),SB);
      DD.calculation();
      if(DD.number_of_solutions() < 1)   // intersection doesn't exist
        return;
      LocalPoint B1 = (*(SB->find(h2->bs()))).second;
      LocalPoint B2 = (*(SB->find(h2->fs()))).second;
      double uu = bearing(DD.solution_1(),B2) - bearing(DD.solution_1(),B1);
      uu += (uu < 0 ? 2*M_PI : 0);
      // uu should be equalto h2->value(), but ...  uu is either
      // value() or value()+-PI
      if((uu < (h2->value()+0.1)) && (uu > (h2->value()-0.1)))
        {
          point1->set_xy(DD.solution_1().x(), DD.solution_1().y());
          number_of_solutions_ = 1;
        };
      if(DD.number_of_solutions() > 1)
        {
          uu = bearing(DD.solution_2(),B2) - bearing(DD.solution_2(),B1);
          uu += (uu < 0 ? 2*M_PI : 0);
          if((uu < (h2->value()+0.1)) && (uu > (h2->value()-0.1)))
	  { // added { to avoid dangling else 
            if(number_of_solutions_ == 1)
              {
                point2->set_xy(DD.solution_2().x(), DD.solution_2().y());
                number_of_solutions_ = 2;
              }
            else
              {
                point1->set_xy(DD.solution_2().x(), DD.solution_2().y());
                number_of_solutions_ = 1;
              };
	  } // added } to avoid dangling else 
        };
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Distance_angle::calculation()


  //-----------------------------------------------------------
  // ************** Angle_angle *********************

  void Angle_angle::observation_check(Observation* m1, Observation* m2)
  {
    h1 = dynamic_cast<Angle*>(m1);
    h2 = dynamic_cast<Angle*>(m2);
    if(!(h1 && h2))
      throw g2d_exc("Angle_angle: wrong observation type");
  }


  // ** computation: CHARAMZA, p.143 **

  void Angle_angle::calculation()
  {
    try {

      number_of_solutions_ = 0;           // -1 when computation not done
      Circle K1(h1,SB);
      K1.calculation();
      Circle K2(h2,SB);
      K2.calculation();
      // circle parameters were not solved ?
      if((K1.number_of_solutions() < 1) || (K2.number_of_solutions() < 1))
        return;
      double rr1 = K1.radius();
      double rr2 = K2.radius();
      Distance_distance DD(rr1,rr2,K1.solution_1(),K2.solution_1(),SB);
      DD.calculation();
      if(DD.number_of_solutions() < 1)   // intersection doesn't exist
        return;
      LocalPoint B1 = (*(SB->find(h1->bs()))).second;
      LocalPoint B2 = (*(SB->find(h1->fs()))).second;
      LocalPoint B3 = (*(SB->find(h2->bs()))).second;
      LocalPoint B4 = (*(SB->find(h2->fs()))).second;
      bool Vyhovuje1, Vyhovuje2;
      double uu1, uu2;
      // in the case of common point at both angles is one of
      // intersections this point
      if(!(((B1.x()==DD.solution_1().x()) && (B1.y()==DD.solution_1().y())) ||
           ((B2.x()==DD.solution_1().x()) && (B2.y()==DD.solution_1().y()))))
        {
          uu1 = bearing(DD.solution_1(),B2) - bearing(DD.solution_1(),B1);
          uu2 = bearing(DD.solution_1(),B4) - bearing(DD.solution_1(),B3);
          uu1 += (uu1 < 0 ? 2*M_PI : 0);
          uu2 += (uu2 < 0 ? 2*M_PI : 0);
          // uu should be equal to h2->value(), but ...  uu is either
          // value() or value()+-PI
          Vyhovuje1 = (uu1 < (h1->value()+0.1)) && (uu1 > (h1->value()-0.1));
          Vyhovuje2 = (uu2 < (h2->value()+0.1)) && (uu2 > (h2->value()-0.1));
          if(Vyhovuje1 && Vyhovuje2)
            {
              point1->set_xy(DD.solution_1().x(), DD.solution_1().y());
              number_of_solutions_ = 1;
            };
        };
      if(DD.number_of_solutions() > 1)
        if(!(((B1.x()==DD.solution_2().x()) &&
              (B1.y()==DD.solution_2().y())) ||
             ((B2.x()==DD.solution_2().x()) &&
              (B2.y()==DD.solution_2().y()))))
          {
            uu1 = bearing(DD.solution_2(),B2) - bearing(DD.solution_2(),B1);
            uu2 = bearing(DD.solution_2(),B4) - bearing(DD.solution_2(),B3);
            uu1 += (uu1 < 0 ? 2*M_PI : 0);
            uu2 += (uu2 < 0 ? 2*M_PI : 0);
            Vyhovuje1 = (uu1 < (h1->value()+0.1)) && (uu1 > (h1->value()-0.1));
            Vyhovuje2 = (uu2 < (h2->value()+0.1)) && (uu2 > (h2->value()-0.1));
            if(Vyhovuje1 && Vyhovuje2)
	    { // added { to avoid dangling else 
              if(number_of_solutions_ == 1)
                {
                  point2->set_xy(DD.solution_2().x(), DD.solution_2().y());
                  number_of_solutions_ = 2;
                }
              else
                {
                  point1->set_xy(DD.solution_2().x(), DD.solution_2().y());
                  number_of_solutions_ = 1;
                };
	    } // added } to avoid dangling else 
          };
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Angle_angle::calculation()


  //------------------------------------------------------------
  // ************** Circle *********************

  // ** computation: CHARAMZA, pp.107-109 **

  void Circle::calculation()
  {
    try {

      number_of_solutions_ = 0;       // -1 when computation not done
      double u = h1->value();
      if(fabs(sin(u)) < small_angle_limit_)        // small angle
        {
          small_angle_detected_ = true;
          return;
        }
      B1 = (*(SB->find(h1->bs()))).second;
      B2 = (*(SB->find(h1->fs()))).second;
      double sm, d;
      bearing_distance(B1,B2,sm,d);
      if(d == 0)                     // identical points
        return;
      double rr = d/sin(u)/2;
      R = fabs(rr);
      point1->set_xy(B1.x()-rr*sin(sm - u), B1.y()+rr*cos(sm - u));
      number_of_solutions_ = 1;
      return;

    }
    catch (g2d_exc& exc)
      {
        throw exc;
      }
    catch (...)
      {
        number_of_solutions_ = -1;
        return;
      }

  }  // void Circle::calculation()

}} // namespace GNU_gama::local
