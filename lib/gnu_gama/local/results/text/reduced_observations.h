/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2021  Ales Cepek <cepek@gnu.org>

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
 * \author Ales Cepek
 */

#ifndef gama_local_local_results_text_reduced_observations_h
#define gama_local_local_results_text_reduced_observations_h

#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/utf8.h>
#include <cctype>
#include <iomanip>

namespace GNU_gama { namespace local {

template <typename OutStream>
void ReducedObservations(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;

   bool has_reduced_observations {false};

   const int obscount = IS->observations_count();
   for (int i=1; i<=obscount; i++)
     if (IS->ptr_obs(i)->reduction())
       {
         has_reduced_observations = true;
         break;
       }

   if (!has_reduced_observations) return;

   out << T_GaMa_reduced_Review_of_reduced_observations << "\n"
       << underline(T_GaMa_reduced_Review_of_reduced_observations, '*')
       << "\n\n";

   const int distPrecision = 5;
   const int angularPrecision = 6;
   const int coordPrecision = 5;
   const int maxw_id = 12;

   std::string header {};
   {
     std::ostringstream out;
     out.width(IS->maxw_obs());
     out << "i" << " ";
     out.width(IS->maxw_id());
     out << T_GaMa_standpoint << " ";
     out.width(IS->maxw_id());
     out << T_GaMa_target << " ";
     out << T_GaMa_padding_obs;
     out.width(maxw_id);
     out << T_GaMa_adjobs_observed;
     out.width(maxw_id);
     out << T_GaMa_reduced_reduced << "  ";

     header = out.str();
   }

   out << header << "\n"
       << underline(header, '=') << "\n\n";

   using std::setw;
   PointID previous {};
   for (int i=1; i<=obscount; i++)
     {
        Observation* pm = IS->ptr_obs(i);
        if (pm->reduction() == 0) continue;

        out.width(IS->maxw_obs());
        out << i << " ";
        PointID cs = pm->from();
        out.width(IS->maxw_id());
        if (cs != previous)
           out << Utf8::leftPad(cs.str(), IS->maxw_id());
        else
           out << " ";
        // out << " ";
        PointID cc = pm->to();
        out << Utf8::leftPad(cc.str(), IS->maxw_id()) << " ";
        out.setf(ios_base::fixed, ios_base::floatfield);

        if (Z_Angle* zang = dynamic_cast<Z_Angle*>(pm))
          {
            out << T_GaMa_z_angle;
          }
        else if (S_Distance* sdis = dynamic_cast<S_Distance*>(pm))
          {
            out << T_GaMa_s_distance;
          }
        else
          {
            out << T_GaMa_padding_obs;
          }

        double observed = pm->value() - pm->reduction();
        double reduced  = pm->value();
        if (pm->angular())
          {
            observed *= 200/M_PI;
            reduced  *= 200/M_PI;

            if (IS->gons())
                out << " " << setw(maxw_id) << std::setprecision(6) << observed
                    << " " << setw(maxw_id) << std::setprecision(6) << reduced;
            else
                out << " " << setw(12) << GNU_gama::gon2deg(observed, 0, 2)
                    << " " << setw(12) << GNU_gama::gon2deg(reduced,  0, 2);
          }
        else
          {
            out << " " << std::setw(12) << std::setprecision(5) << observed
                << " " << std::setw(12) << std::setprecision(5) << reduced;
          }

        out << "\n";
     }

   out << "\n\n";
   out.flush();
}

}}
#endif
