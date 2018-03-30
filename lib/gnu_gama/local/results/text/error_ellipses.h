/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999, 2018  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef GaMa_GaMaProg_Prehled_Elipsy_Chyb_h_
#define GaMa_GaMaProg_Prehled_Elipsy_Chyb_h_

#include <gnu_gama/local/network.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/utf8.h>
#include <cmath>

namespace GNU_gama { namespace local {

template <typename OutStream>
void ErrorEllipses(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
   using namespace std;
   using namespace GNU_gama::local;

   const int y_sign = IS->y_sign();

   const Vec& x = IS->solve();
   double elp_k = 0;
   {
     double alfa = (1 - IS->conf_pr());
     if (IS->m_0_apriori())
       {
	 elp_k = sqrt(GNU_gama::Chi_square(float(alfa), 2));
       }
     else
       {
	 int n = IS->degrees_of_freedom();
	 if (n > 0)
	   elp_k = sqrt( n*(pow(alfa, -2.0/n) - 1));
	 else
	   elp_k = 0;
       }
   }
   const int pocnez = IS->sum_unknowns();

   bool sour = false;
   {   // for ...
   for (int i=1; i<=pocnez; i++)
       if (IS->unknown_type(i) == 'X')
       {
         sour = true;
         break;
       }
   }   // for ...

   double mp_max = -1, mp_prum = 0;
   PointID mp_max_cb;
   int pocbod = 0;

   if (sour)
   {
     out.precision(1);

     out
       << T_GaMa_errell_review_of_mean_errors_and_error_ellipses << "\n"
       << underline(T_GaMa_errell_review_of_mean_errors_and_error_ellipses,'*')
       << "\n\n";
     out.width(IS->maxw_id());
     out << T_GaMa_point << ' ';
     out << T_GaMa_errell_header1;
     for (int i=0; i<IS->maxw_id()+1; i++) out << '=';
     if (IS->gons())
       out << T_GaMa_errell_header2;
     else
       out <<
         "== [mm] == [mm] ==== a [mm] b ==== [d] ===== a' [mm] b' ========";
     out << "\n\n";
     {   // for ...
       // 1.3.13 for (int i=1; i<=pocnez; i++)
       // 1.3.13  if (IS->unknown_type(i) == 'X')
       for (PointData::const_iterator
              point=IS->PD.begin(); point!=IS->PD.end(); ++point)
         if ((*point).second.free_xy())
           if ((*point).second.index_x())
             {
               const PointID point_id  = (*point).first;
               out << Utf8::leftPad(point_id.str(), IS->maxw_id()) << ' ';

               const LocalPoint& p = (*point).second;
               double my = IS->unknown_stdev(p.index_y());
               double mx = IS->unknown_stdev(p.index_x());

               double mp = sqrt(my*my+mx*mx);
               if (mp < 1000)
                 out.setf(ios_base::fixed, ios_base::floatfield);
               else
                 out.setf(ios_base::scientific, ios_base::floatfield);
               out.width(7);
               out << mp << ' ';

               mp_prum += mp;
               if (mp > mp_max) {
                 mp_max = mp;
                 mp_max_cb = point_id;
               }
               pocbod++;

               double myx = mp/sqrt(2.0);
               out.width(7);
               if (myx < 1000)
                 out.setf(ios_base::fixed, ios_base::floatfield);
               else
                 out.setf(ios_base::scientific, ios_base::floatfield);
               out << myx << ' ' ;

               double a, b, alfa;
               IS->std_error_ellipse(point_id, a, b, alfa);
               // if (y_sign == -1)
               //   {
               //     // 1.7.10 alfa = 2*M_PI - alfa;
               //   }
               out.width(7);
               if (a < 1000)
                 out.setf(ios_base::fixed, ios_base::floatfield);
               else
                 out.setf(ios_base::scientific, ios_base::floatfield);
               out << a << ' ';
               out.width(7);
               if (b < 1000)
                 out.setf(ios_base::fixed, ios_base::floatfield);
               else
                 out.setf(ios_base::scientific, ios_base::floatfield);
               out << b << ' ';
               out.width(7);
               out.setf(ios_base::fixed, ios_base::floatfield);
               double ea = alfa*R2G;
               if (IS->degrees()) ea *= 360.0/400;
               out << ea << ' ';

               if (mp < 1000 && mp > 1e-3)
                 {           // ********* testing noise (coordinates are OK)
                   double ak = a*elp_k;
                   double bk = b*elp_k;
                   out.width(7);
                   if (ak < 1000)
                     out.setf(ios_base::fixed, ios_base::floatfield);
                   else
                     out.setf(ios_base::scientific, ios_base::floatfield);
                   out << ak << ' ';
                   out.width(7);
                   if (bk < 1000)
                     out.setf(ios_base::fixed, ios_base::floatfield);
                   else
                     out.setf(ios_base::scientific, ios_base::floatfield);
                   out << bk << ' ';

                   double g  = 0;
                   double dx = x( p.index_x() );
                   double dy = y_sign*x( p.index_y() );
                   double p1 = (dx*cos(alfa) + dy*sin(alfa));
                   double p2 = (dy*cos(alfa) - dx*sin(alfa));
                   if (ak > 0 && bk > 0 && bk > ak*1e-4)
                     {           // ***** testing noise (bk is practically 0)
                       p1 /= ak;
                       p2 /= bk;
                       g = sqrt(p1*p1 + p2*p2);
                     }
                   if (g < 1000)
                     out.setf(ios_base::fixed, ios_base::floatfield);
                   else
                     out.setf(ios_base::scientific, ios_base::floatfield);
                   out.width(7);
                   out << g;
                 }

               out << "\n";
               out.flush();
             }
     }   // for ...

     if (pocbod >= 5)
       {
         out.precision(1);
         out << '\n'
             << T_GaMa_adjunk_mean_position_error_maximal << mp_max
             << T_GaMa_adjunk_mean_position_error_on_point
             << mp_max_cb << '\n'
             << T_GaMa_adjunk_mean_position_error_average << mp_prum/pocbod
             << " mm\n";
       }

     out << "\n\n";

   }
}

}}

#endif



