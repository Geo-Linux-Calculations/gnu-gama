/*
    GNU Gama C++ library
    Copyright (C) 2002 Jan Pytel  <pytel@fsv.cvut.cz>
                  2010 Ales Cepek <cepek@gnu.org>

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
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <gnu_gama/local/acord/reduce_observations.h>
#include <iostream>

using namespace std;
using namespace GNU_gama::local;

const Double EarthRadius = 6378000;  // [m]

class average_value {
public:
    average_value()
    {
	reset();
    }

    void reset()
    {
	sum = number_of_values = 0;
    }

    Double add(const Double& val)
    {
	if (number_of_values++)
	    sum+=val;
	else
	    sum =val;

	return sum / number_of_values;
    }

    Double average() const
    {
	return number_of_values ? sum / number_of_values : 0;
    }

    Double count() const
    {
	return number_of_values;
    }

private:
    Double sum;
    unsigned int number_of_values;
};


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
          dynamic_cast<Z_Angle*   >(obs) ||
          dynamic_cast<Ydiff*     >(obs)  ) list_reduced_obs.push_back(obs);
    }
}

void ReducedObservations::reduce(ReducedObs& r_obs)
{
    Observation* obs = r_obs.ptr_obs;

    if (dynamic_cast<S_Distance*>(obs))
      reduce_sdistance(&r_obs);
    else if (dynamic_cast<Z_Angle*>(obs))
      reduce_zangle(&r_obs);
    else if (dynamic_cast<Ydiff*>(obs))
      reduce_ydiff(&r_obs);
    else
      ; // !? Must I throw exception here ?
}


void ReducedObservations::reduce_sdistance(ReducedObs* r_obs)
{
    S_Distance* obs = dynamic_cast<S_Distance*>(r_obs->ptr_obs);

    if ( !obs ) return;

    const Double orig_value = r_obs->orig_value();

    average_value ZA_from_to_cluster, ZA_to_from_cluster,
                  ZA_from_to, ZA_to_from;

    TypeOfReduction type_of_red = precise_;

    for (ListReducedObs_c_iter ci  = list_reduced_obs.begin();
     	                       ci != list_reduced_obs.end(); ci++)
    {
	Z_Angle* zangle = dynamic_cast<Z_Angle*>(ci->ptr_obs);

	if ( !zangle )
	    continue;

	if ( !zangle->active() )
	    continue;

	const Double value = ci->orig_value();

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
	r_obs->type_of_reduction = nonexist_;
	return;
    }

    const Double dh = obs->to_dh() - obs->from_dh();

    const LocalPoint& from = PD[obs->from()];
    const LocalPoint& to   = PD[obs->to()];

    Double Hm = 0; // 1/2 * (from.H + from_dh - to.H - to_dh)
    Double gravity_angle    = 0; // correction from gravity
    Double refraction_angle = 0;


    if ( from.test_z() && to.test_z() )
	Hm = 0.5 * ( from.z() + obs->from_dh() + to.z() + obs->to_dh() );
    else
    {

	type_of_red = approx_;

	if ( from.test_z() )
	    Hm = from.z();
	else
	    if ( to.test_z() )
		Hm = to.z();
    }


    gravity_angle = orig_value / (EarthRadius + Hm);

    Double observed_za;

    if ( ZA_from_to_cluster.count() )
    {
	observed_za = ZA_from_to_cluster.average();

	if (ZA_to_from_cluster.count() )
	    refraction_angle = M_PI/2 + gravity_angle/2 - 0.5 *
		                     (ZA_from_to_cluster.average() +
				      ZA_to_from_cluster.average() );
    }
    else
	if ( ZA_from_to.count() )
	    observed_za = ZA_from_to.average();
	else
	    if ( ZA_to_from_cluster.average() )
		observed_za = M_PI + gravity_angle - ZA_to_from_cluster.average();
	    else
		observed_za = M_PI + gravity_angle - ZA_to_from.average();


    const Double d2 = (orig_value * orig_value) + dh*dh - 2 * orig_value * dh *
                       std::cos(observed_za + refraction_angle - gravity_angle);

    if ( fabs(d2) <= 0 )
	return;

    const Double d_from = gravity_angle * obs->from_dh();

    obs->set_value( std::sqrt(d2) - d_from );

    r_obs->type_of_reduction = type_of_red;
}


void ReducedObservations::reduce_zangle(ReducedObs* r_obs)
{
    Z_Angle* obs = dynamic_cast<Z_Angle*>(r_obs->ptr_obs);

    if ( !obs ) return;

    const Double orig_value = r_obs->orig_value();

    average_value ZA_to_from_cluster,
	          SD_cluster,
	          SD;

    TypeOfReduction type_of_red = precise_;

    for (ListReducedObs_c_iter ci = list_reduced_obs.begin();
	 ci != list_reduced_obs.end(); ci++)
    {
	{
	    Z_Angle* zangle = dynamic_cast<Z_Angle*>(ci->ptr_obs);

	    if ( zangle )
	    {
		if ( !zangle->active() )
		    continue;

		if ( ( zangle->from()    == obs->to()      ) &&
		     ( zangle->to()      == obs->from()    ) &&
		     ( zangle->from_dh() == obs->to_dh()   ) &&
		     ( zangle->to_dh()   == obs->from_dh() ) &&
		     ( zangle->ptr_cluster() == obs->ptr_cluster() ) )
		    ZA_to_from_cluster.add( ci->orig_value() );

		continue;
	    }
	}

	    S_Distance* sdist = dynamic_cast<S_Distance*>(ci->ptr_obs);

	    if ( !sdist )
		continue;

	    if ( !sdist->active() )
		continue;

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
		SD_cluster.add( ci->orig_value() );
	    else
		SD.add( ci->orig_value() );
	}
    }


    if ( ( SD_cluster.count() + SD.count() ) == 0 )
    {
	r_obs->type_of_reduction = nonexist_;
	return;
    }

    const Double dh = obs->to_dh() - obs->from_dh();

    const LocalPoint& from = PD[obs->from()];
    const LocalPoint& to   = PD[obs->to()];

    Double Hm = 0; // 1/2 * (from.H + from_dh - to.H - to_dh)
    Double gravity_angle    = 0;  // correction from gravity
    Double refraction_angle = 0;


    if ( from.test_z() && to.test_z() )
	Hm = 0.5 * ( from.z() + obs->from_dh() + to.z() + obs->to_dh() );
    else
    {

	type_of_red = approx_;

	if ( from.test_z() )
	    Hm = from.z();
	else
	    if ( to.test_z() )
		Hm = to.z();
    }

    gravity_angle = orig_value / (EarthRadius + Hm);

    if ( ZA_to_from_cluster.count() )
	refraction_angle = M_PI/2 + gravity_angle/2 - 0.5 *
	                   (orig_value + ZA_to_from_cluster.average() );
    Double sdist;

    if ( SD_cluster.count() )
	sdist = SD_cluster.average();
    else
	sdist = SD.average();

    const Double dist_to_vertic_dh = sdist - dh *
	         std::cos ( orig_value + refraction_angle - gravity_angle );

    const Double vertic_dh = dh * std::sin( orig_value + refraction_angle -
					    gravity_angle);

    if ( std::fabs(dist_to_vertic_dh) < 1e-10 )
	return;

    obs->set_value( r_obs->orig_value() + refraction_angle +
		    std::atan2(vertic_dh,dist_to_vertic_dh) );

    r_obs->type_of_reduction = type_of_red;
}


void ReducedObservations::reduce_ydiff(ReducedObs* r_obs)
{
    Ydiff* obs = dynamic_cast<Ydiff*>(r_obs->ptr_obs);

    if ( !obs )
	return;

    obs->set_value( r_obs->orig_value() + obs->from_dh() - obs->to_dh() );

    r_obs->type_of_reduction = precise_;
}


void ReducedObservations::execute()
{

    if ( !number_of_not_reduced_observations() )
	return;

    list_reduced_obs.remove_if( RemoveNonActiveObs() );

    for (ListReducedObs_iter i  = list_reduced_obs.begin();
	                     i != list_reduced_obs.end(); ++i)
	if ( ! (i->type_of_reduction & (precise_ | nonexist_) ) )
	{
	    reduce(*i);
	    if ( i->type_of_reduction & nonexist_ )
		i->ptr_obs->set_passive();
	}

}


void ReducedObservations::print(ostream&)
{
}

