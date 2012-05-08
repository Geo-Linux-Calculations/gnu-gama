/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004  Jan Pytel  <pytel@gama.fsv.cvut.cz>

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

/** \file reduced_observations_to_ellipsoid.h
 * \brief Function for writing reduced observations to elipsoid
 *
 * \author Jan Pytel
 */

#ifndef gama_local_local_results_text_reduced_observations_to_ellipsoid_h
#define gama_local_local_results_text_reduced_observations_to_ellipsoid_h

#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/local/acord/reduce_to_ellipsoid.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/acord.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/utf8.h>
#include <cctype>
#include <iomanip>

namespace GNU_gama { namespace local {

/** \brief
 *
 * \todo Consider using visitor pattern for writing S_Distance, Z_Angle and H_Diff.
 */
template <typename OutStream>
void ReducedObservationsToEllipsoidText(GNU_gama::local::LocalNetwork* IS,
                           const ReduceToEllipsoid::ObsMap& reduced, OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;

   if ( !reduced.size() )
       return;

   out << T_GaMa_reduced_Review_of_reduced_observations_to_ellipsoid << "\n"
       << underline(T_GaMa_reduced_Review_of_reduced_observations_to_ellipsoid, '*')
       << "\n\n";

   int minval_obs = 12;
   int maxval_obs = minval_obs;

   int minval_dh  = 8;
   int maxval_dh  = minval_dh;

   {
       for (ObservationData::iterator i=IS->OD.begin(), e=IS->OD.end(); i!=e; ++i)
       {
           Observation* o = *i;

           ReduceToEllipsoid::ObsMap::const_iterator ci = reduced.find(o);

           if ( ci == reduced.end() )
               continue;

	   Double orig_value = ci->second;
           Double value      = o->value();
	   Double diff       = R2G*(value - orig_value);

	   int oz = 0;
	   int dz = 0;

	   if (value < 0)
           {
               oz = 1;
               value = -value;
           }

	   if (value < 1e5) continue;

	   oz += 6;   // ... decimal point plus 5 digits

	   do {
	       oz++;
	       value /= 10;
	   } while (value >= 1);

	   if (oz > maxval_obs) maxval_obs = oz;

	   if (diff < 0)
	   {
	       dz = 1;
	       diff = -diff;
	   }

	   if (diff < 1e2) continue;

	   dz += 6; // ... decimal point plus 5 digits

	   do {
	       dz++;
	       diff /= 10;
	   } while (diff >=1);
       }
   }

   // out.width(IS->maxw_obs());
   //   out << "i" << " ";

   out.width(IS->maxw_id());
   out << T_GaMa_standpoint << " ";
   out.width(IS->maxw_id());
   out << T_GaMa_target << "       ";
   out.width(maxval_obs);
   out << T_GaMa_reduced_observed << " ";
   out.width(maxval_obs);
   out << T_GaMa_reduced_reduced << " ";
   out.width(maxval_dh);
   out << T_GaMa_reduced_to_ellipsoid_header1;

   {
     int kk = 12 + 2*IS->maxw_id() + maxval_obs - minval_obs;
     for (int i=0; i < kk; i++) out << "=";
   }

   out << T_GaMa_reduced_value;

   {
     int kk = maxval_obs - minval_obs + 1;
     for (int i=0; i < kk; i++) out << "=";
   }

   if (IS->gons())
       out << "==== [m|g] ";
   else
       out << "==== [m|d] ";

   {
       int kk = maxval_dh - minval_dh;
       for (int i=0; i < kk; i++) out << "=";
   }

   if (IS->gons())
       out << "=== [g] ==\n\n";
   else
       out << "=== [d] ==\n\n";

   out.flush();


   PointID predcs = "";   // provious standpoint ID

   for (ObservationData::iterator i=IS->OD.begin(), e=IS->OD.end(); i!=e; ++i)
   {
       ReduceToEllipsoid::ObsMap::const_iterator ci = reduced.find(*i);

       if ( ci == reduced.end() )
           continue;

       Double orig_value = ci->second;

       Observation* pm = *i;

       PointID cs = pm->from();
       out.width(IS->maxw_id());
       if (cs != predcs)
           out << Utf8::leftPad(cs.str(), IS->maxw_id());
       else
           out << " ";
       out << " ";
       PointID cc = pm->to();
       out << Utf8::leftPad(cc.str(), IS->maxw_id());
       out.setf(ios_base::fixed, ios_base::floatfield);

       string str_nonexist("");
       for (int i=0; i < maxval_obs - 2; i++)
           str_nonexist+="-";

       {   // ***************************************************

          if (// Z_Angle* za =
              dynamic_cast<Z_Angle*>(pm))
          {
              out << T_GaMa_z_angle;
              out.precision(6);
              out.width(maxval_obs);
              if (IS->gons())
                  out << R2G*orig_value << " ";
              else
                  out << GNU_gama::gon2deg(R2G*orig_value, 0, 2) << " ";
              out.width(maxval_obs);
              if (IS->gons())
                  out << R2G*pm->value();
              else
                  out << GNU_gama::gon2deg(R2G*pm->value(), 0, 2);
              out << " ";
          }
          else if (// Direction* d =
                   dynamic_cast<Direction*>(pm))
          {
              out << T_GaMa_direction;
              out.precision(6);
              out.width(maxval_obs);
              if (IS->gons())
                  out << R2G*orig_value << " ";
              else
                  out << GNU_gama::gon2deg(R2G*orig_value, 0, 2) << " ";
              out.width(maxval_obs);
              if (IS->gons())
                  out << R2G*pm->value();
              else
                  out << GNU_gama::gon2deg(R2G*pm->value(), 0, 2);
              out << " ";
          }
       }   // ***************************************************
       out << " ";
       out.precision(4);
       out.width(maxval_dh);
       if (IS->gons())
           out << R2G*(orig_value - pm->value())*10000;
       else
           out << R2G*0.9*(orig_value - pm->value())*3600;
           //out << GNU_gama::gon2deg(R2G*(orig_value - pm->value()), 0, 2);
       out << "\n";
       out.flush();

       predcs = cs;  // previous standpoint ID
   }

   out << "\n\n";
   out.flush();
}

}}

#endif

