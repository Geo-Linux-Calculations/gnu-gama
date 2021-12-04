/*
  GNU Gama C++ library
  Copyright (C) 2002 Jan Pytel  <pytel@fsv.cvut.cz>
                2010, 2018, 2021 Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

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

#include <gnu_gama/local/acord/reduce_observations.h>
#include <iostream>

using namespace std;
using namespace GNU_gama::local;

namespace  {

  // https://en.wikipedia.org/wiki/Earth

  const double mean_radius       { 6371000 };  // meters
  const double equatorial_radius { 6378137 };
  const double polar_radius      { 6356752 };

  const double EarthRadius = mean_radius;


  class Average {
  public:
    Average() {
      reset();
    }

    void reset() {
      sum_ = number_of_values_ = 0;
    }

    double add(const double& val) {
      if (number_of_values_++)
        sum_+=val;
      else
        sum_ =val;

      return sum_ / number_of_values_;
    }

    double average() const {
      return number_of_values_ ? sum_ / number_of_values_ : 0;
    }

    double count() const {
      return number_of_values_;
    }

  private:
    double sum_;
    unsigned int number_of_values_;
  };

}


ReducedObservations::ReducedObservations(PointData& b, ObservationData& m):
    PD(b), OD(m)
{
  for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    {
      Observation* obs = *i;

      if ( !obs->active() )  continue;

      list_obs.push_back(obs);

      if ( (obs->from_dh() == 0) && (obs->to_dh() == 0 ) ) continue;

      if (dynamic_cast<S_Distance*>(obs) ||
          dynamic_cast<Z_Angle*   >(obs) ) reduced_obs.push_back(obs);
    }
}

void ReducedObservations::reduce(ReducedObs& r_obs)
{
    Observation* obs = r_obs.ptr_obs;

    if (dynamic_cast<S_Distance*>(obs))
      reduce_sdistance(&r_obs);
    else if (dynamic_cast<Z_Angle*>(obs))
      reduce_zangle(&r_obs);
    else
      {
        /* should we throw exception here? */
      }
}


void ReducedObservations::reduce_sdistance(ReducedObs* r_obs)
{
    S_Distance* obs = dynamic_cast<S_Distance*>(r_obs->ptr_obs);
    if ( !obs ) return;

    const double orig_value = r_obs->orig_value();

    Average   ZA_from_to_cluster, ZA_to_from_cluster, ZA_from_to, ZA_to_from;
    Reduction reduction_dh_type = exact;

    for (auto ci  = reduced_obs.begin(); ci != reduced_obs.end(); ci++)
    {
        Z_Angle* zangle = dynamic_cast<Z_Angle*>(ci->ptr_obs);

        if ( !zangle )
            continue;

        if ( !zangle->active() )
            continue;

        const double value = ci->orig_value();

        if ( ( zangle->from() == obs->from()       ) &&
             ( zangle->to() == obs->to()           ) &&
             ( zangle->from_dh() == obs->from_dh() ) &&
             ( zangle->to_dh() == obs->to_dh()     )  )
        {
            if (zangle->ptr_cluster() == obs->ptr_cluster() )
                ZA_from_to_cluster.add( value );
            else
                ZA_from_to.add( value );
        }
        else
            if ( ( zangle->from() == obs->to()       ) &&
                 ( zangle->to() == obs->from()       ) &&
                 ( zangle->from_dh() == obs->to_dh() ) &&
                 ( zangle->to_dh() == obs->from_dh() )  )
            {
                if ( zangle->ptr_cluster() == obs->ptr_cluster() )
                    ZA_to_from_cluster.add( value );
                else
                    ZA_to_from.add( value );
            }

    }

    if ( ( ZA_from_to_cluster.count() + ZA_to_from_cluster.count() +
           ZA_from_to.count() + ZA_to_from.count() ) == 0 )
    {
        obs->set_value( r_obs->orig_value() );
        double r = 0;
        obs->set_reduction_dh( r );
        r_obs->reduction = not_available;
        return;
    }

    const double dh = obs->to_dh() - obs->from_dh();

    const LocalPoint& from = PD[obs->from()];
    const LocalPoint& to   = PD[obs->to()];

    double Hm = 0; // 1/2 * (from.H + from_dh + to.H + to_dh)
    double central_angle    = 0; // Earth curvature correction central angle
    double refraction_angle = 0;


    if ( from.test_z() && to.test_z() )
      {
        Hm = 0.5 * ( from.z() + obs->from_dh() + to.z() + obs->to_dh() );
      }
    else
      {
        reduction_dh_type = approximate;

        if ( from.test_z() )
          Hm = from.z();
        else if ( to.test_z() )
          Hm = to.z();
      }


    central_angle = orig_value / (EarthRadius + Hm);

    double observed_za;

    if ( ZA_from_to_cluster.count() )
    {
        observed_za = ZA_from_to_cluster.average();

        if (ZA_to_from_cluster.count() )
            refraction_angle = M_PI/2 + central_angle/2 - 0.5 *
                                     (ZA_from_to_cluster.average() +
                                      ZA_to_from_cluster.average() );
    }
    else
        if ( ZA_from_to.count() )
            observed_za = ZA_from_to.average();
        else
            if ( ZA_to_from_cluster.average() )
                observed_za = M_PI + central_angle - ZA_to_from_cluster.average();
            else
                observed_za = M_PI + central_angle - ZA_to_from.average();

    double d2 = (orig_value * orig_value) + dh*dh - 2 * orig_value * dh *
                 std::cos(observed_za + refraction_angle - central_angle);
    if ( fabs(d2) < 0 ) d2 = 0;

    const double d_from = central_angle * obs->from_dh();

    obs->set_value( orig_value );
    obs->set_reduction_dh( std::sqrt(d2) - d_from - orig_value);

    r_obs->reduction = reduction_dh_type;
}


void ReducedObservations::reduce_zangle(ReducedObs* r_obs)
{
  Z_Angle* obs = dynamic_cast<Z_Angle*>(r_obs->ptr_obs);
  if ( !obs ) return;

  const double orig_value = r_obs->orig_value();

  Average ZA_opposite_direction_same_cluster,
          SD_same_cluster, SD_other_clusters;

  Reduction type_of_red = exact;

  for (auto ci = reduced_obs.begin(); ci != reduced_obs.end(); ci++)
    {
      if (ci->ptr_obs->passive()) continue;

      if (Z_Angle* zangle = dynamic_cast<Z_Angle*>(ci->ptr_obs))
        {
          // Trying to find reciprocal observations in the same cluster.
          // Concurrent observations made in opposite directions should
          // decrease measurement uncertainty caused by curvature and
          // refraction.
          if ( ( zangle->from()    == obs->to()      ) &&
               ( zangle->to()      == obs->from()    ) &&
               ( zangle->from_dh() == obs->to_dh()   ) &&
               ( zangle->to_dh()   == obs->from_dh() ) &&
               ( zangle->ptr_cluster() == obs->ptr_cluster() ) )
            ZA_opposite_direction_same_cluster.add( ci->orig_value() );
        }
      else if (S_Distance* sdist = dynamic_cast<S_Distance*>(ci->ptr_obs))
        {
          // Looking for corresponing slope distances from the same
          // or any other clusters
          if ( ( ( sdist->from()    == obs->from()    ) &&
                 ( sdist->to()      == obs->to()      ) &&
                 ( sdist->from_dh() == obs->from_dh() ) &&
                 ( sdist->to_dh()   == obs->to_dh()   ) ) ||
               ( ( sdist->from()    == obs->to()      ) &&
                 ( sdist->to()      == obs->from()    ) &&
                 ( sdist->from_dh() == obs->to_dh()   ) &&
                 ( sdist->to_dh()   == obs->from_dh() ) ) )
            {
              if ( sdist->ptr_cluster() == obs->ptr_cluster() )
                SD_same_cluster.add( ci->orig_value() );
              else
                SD_other_clusters.add( ci->orig_value() );
            }
        }

    }

  const double dh = obs->to_dh() - obs->from_dh();

  const LocalPoint& from = PD[obs->from()];
  const LocalPoint& to   = PD[obs->to()];
  double sdist {};  // space distance
  double hdist {};  // horizontal distance

  if ( SD_same_cluster.count() > 0)
    sdist = SD_same_cluster.average();
  else if (SD_other_clusters.count() > 0)
    sdist = SD_other_clusters.average();
  else if (from.test_xyz() && to.test_xyz())
    {
      // to calculate zenith angle reduction we need some substitue for
      // missing distance between the two points
      double dx = from.x() - to.x();
      double dy = from.y() - to.y();
      double dz = from.z() - to.z();
      sdist = std::sqrt(dx*dx + dy*dy + dz*dz);
      r_obs->reduction = approximate;
    }
  else if (from.test_xy() && to.test_xy())
    {
      double dx = from.x() - to.x();
      double dy = from.y() - to.y();
      hdist = std::sqrt(dx*dx + dy*dy);
      sdist = hdist/std::cos(obs->value() - M_PI/2);
      r_obs->reduction = approximate;
    }
  else
    {
      r_obs->reduction = not_available;
      return;
    }

  double H_middle            = 0;  // 1/2 * (from.H + from_dh + to.H + to_dh)
  double central_angle       = 0;  // Earth curvature correction central angle
  double vertical_refraction = 0;


  if ( from.test_z() && to.test_z() )
    {
      H_middle = 0.5 * ( from.z() + obs->from_dh() + to.z() + obs->to_dh() );
    }
  else
    {
      type_of_red = approximate;

      if ( from.test_z() )
        H_middle = from.z() + obs->from_dh();
      else if ( to.test_z() )
        H_middle = to.z() + obs->to_dh();
    }

  central_angle = sdist / (EarthRadius + H_middle);

  if ( ZA_opposite_direction_same_cluster.count() )
    {
      vertical_refraction = M_PI/2 + central_angle/2 - 0.5 *
          (orig_value + ZA_opposite_direction_same_cluster.average() );
    }

  const double dist_to_vertic_dh = sdist - dh *
      std::cos ( orig_value + vertical_refraction - central_angle );

  const double vertic_dh = dh * std::sin( orig_value + vertical_refraction -
                                          central_angle);

  // if ( std::fabs(dist_to_vertic_dh) < 1e-10 ) return;

  obs->set_value( orig_value );
  double r = vertical_refraction + std::atan2(vertic_dh,dist_to_vertic_dh);
  obs->set_reduction_dh( r );
  r_obs->reduction = type_of_red;
}


void ReducedObservations::execute()
{
  if ( !unreduced_observations() ) return;

  reduced_obs.remove_if( [](const ReducedObs& red_obs) {
      return red_obs.ptr_obs->passive();} );

  for (auto i  = reduced_obs.begin(); i != reduced_obs.end(); ++i)
    {
      if ((i->reduction != exact) && (i->reduction != not_available))
        {
          reduce(*i);
          if ( i->reduction == not_available )
            i->ptr_obs->set_passive();
        }
    }
}


void ReducedObservations::print(ostream&)
{
}
