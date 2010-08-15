/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef GaMa_GaMaProg_Priblizne_Souradnice_h_
#define GaMa_GaMaProg_Priblizne_Souradnice_h_

#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/local/acord.h>
#include <cctype>
#include <iomanip>

namespace GNU_gama { namespace local {

template <typename OutStream>
void ApproximateCoordinates(GNU_gama::local::Acord* acord, OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;

   if (!acord->missing_coordinates          &&
       acord->given_xyz == acord->total_xyz &&
       acord->given_xy  == acord->total_xy  &&
       acord->given_z   == acord->total_z   ) return;

   out << T_GaMa_approx_Review_of_approximate_coordinates << "\n"
       << underline(T_GaMa_approx_Review_of_approximate_coordinates, '*')
       << "\n\n";

   const int aw = 10;
   out << T_GaMa_approx_header1
       << setw(aw) << "xyz" << setw(aw) << "xy" << setw(aw) << "z" << "\n\n";
   out << T_GaMa_approx_given_coordinates
       << setw(aw) << acord->given_xyz
       << setw(aw) << acord->given_xy
       << setw(aw) << acord->given_z
       << "\n";
   out << T_GaMa_approx_computed_coordinates
       << setw(aw) << acord->computed_xyz
       << setw(aw) << acord->computed_xy
       << setw(aw) << acord->computed_z
       << "\n";
   out << T_GaMa_approx_separator << "\n"
       << T_GaMa_approx_total
       << setw(aw) << acord->total_xyz
       << setw(aw) << acord->total_xy
       << setw(aw) << acord->total_z
       << "\n\n";
   out << T_GaMa_approx_observations
       << setw(aw) << acord->observations << "\n\n";

   if (acord->missing_coordinates)
     {
       PointData& PD = acord->PD;
       out << T_GaMa_missing_coordinates << "\n"
           << underline(T_GaMa_missing_coordinates, '-') << "\n";
       for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
         {
           const LocalPoint& p = (*i).second;
           bool cp = p.active_xy() && !p.test_xy();
           bool hp = p.active_z()  && !p.test_z();
           if (cp && hp)
             out << "xyz " << (*i).first << "\n";
           else if (cp)
             out << " xy " << (*i).first << "\n";
           else if (hp)
             out << "  z " << (*i).first << "\n";
         }
       out << "\n";
     }

   out << "\n";
   out.flush();
}

}}

#endif




