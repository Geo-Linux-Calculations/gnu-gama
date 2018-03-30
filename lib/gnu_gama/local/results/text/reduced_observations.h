/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2002, 2003  Jan Pytel  <pytel@gama.fsv.cvut.cz>
                      2003, 2018  Ales Cepek <cepek@gnu.org>

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

/** \file reduced_observations.h
 * \brief Function for writing reduced observations
 *
 * \author Jan Pytel
 * \author Ales Cepek
 */

#ifndef gama_local_local_results_text_reduced_observations_h
#define gama_local_local_results_text_reduced_observations_h

#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/acord.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/utf8.h>
#include <cctype>
#include <iomanip>

namespace GNU_gama { namespace local {

/** \brief
 *
 * \todo Use visitor pattern for writing S_Distance, Z_Angle and H_Diff.
 */
template <typename OutStream>
void ReducedObservationsText(GNU_gama::local::LocalNetwork* IS,
                             GNU_gama::local::ReducedObservations* reduced,
                             OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;

   if ( !reduced->size() )
       return;

   out << T_GaMa_reduced_Review_of_reduced_observations << "\n"
       << underline(T_GaMa_reduced_Review_of_reduced_observations, '*')
       << "\n\n";

   /* [-Wempty-body]
   if ( reduced->size_nonexist() )
       ; // size_nonexist
   */

   int minval_obs = 12;
   int maxval_obs = minval_obs;

   int minval_dh  = 8;
   int maxval_dh  = minval_dh;

   {
       for (ReducedObservations::ListReducedObs_iter it = reduced->begin();
	                                            it != reduced->end(); it++)
       {
	   double value = it->orig_value();
	   double dh    = it->ptr_obs->to_dh() - it->ptr_obs->from_dh();

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

	   if (dh < 0)
	   {
	       dz = 1;
	       dh = -dh;
	   }

	   if (dh < 1e2) continue;

	   dz += 6; // ... decimal point plus 5 digits

	   do {
	       dz++;
	       dh /= 10;
	   } while (dh >=1);
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
   out << T_GaMa_reduced_header1;

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

   out << "=== [m] ==\n\n";

   out.flush();


   PointID predcs = "";   // provious standpoint ID

   for (ReducedObservations::ListReducedObs_iter it = reduced->begin();
  	                                        it != reduced->end(); it++)
   {
      Observation* pm = it->ptr_obs;
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
        if (/* S_Distance* sd = */ dynamic_cast<S_Distance*>(pm))
          {
            out << T_GaMa_s_distance;
            out.precision(5);
            out.width(maxval_obs);
	    out << it->orig_value() << " ";
            out.width(maxval_obs);
	    if ( it->reduced() )
		out << pm->value();
	    else
		out << str_nonexist.c_str();
            out << " ";
          }

        else if (/* Z_Angle* za = */ dynamic_cast<Z_Angle*>(pm))
          {
	    out << T_GaMa_z_angle;
            out.precision(6);
            out.width(maxval_obs);
	    if (IS->gons())
		out << R2G*(it->orig_value()) << " ";
	    else
		out << GNU_gama::gon2deg(R2G*it->orig_value(), 0, 2) << " ";
            out.width(maxval_obs);
	    if ( it->reduced() )
	    {
		if (IS->gons())
		    out << R2G*pm->value();
		else
		    out << GNU_gama::gon2deg(R2G*pm->value(), 0, 2);
	    }
	    else
		out << str_nonexist.c_str();
	    out << " ";
          }
        else if (/* H_Diff* h = */ dynamic_cast<H_Diff*>(pm))
          {
            out << T_GaMa_levell;
            out.precision(5);
            out.width(maxval_obs);
            out << it->orig_value() << " ";
            out.width(maxval_obs);
	    if ( it->reduced() )
		out << pm->value();
	    else
		out << str_nonexist.c_str();
	    out << " ";
          }
        else
          {
            throw GNU_gama::local::Exception("review/reduced_observations.h - "
                                     "unknown observation type");
          }
      }   // ***************************************************
      out << " ";
      out.precision(4);
      out.width(maxval_dh);
      out << pm->to_dh() - pm->from_dh() << "\n";
      out.flush();

      predcs = cs;  // previous standpoint ID
      }

   out << "\n\n";
   out.flush();
}

}}

#endif




