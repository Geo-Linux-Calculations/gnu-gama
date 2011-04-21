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

#ifndef GaMa_GaMaProg_Vybocujici_Absolutni_Cleny_h_
#define GaMa_GaMaProg_Vybocujici_Absolutni_Cleny_h_

#include <iomanip>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/pobs/format.h>
#include <gnu_gama/statan.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/utf8.h>

namespace GNU_gama { namespace local {

template <typename OutStream>
void OutlyingAbsoluteTerms(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GNU_gama::local;

  if (!IS->huge_abs_terms()) return;

  out << T_GaMa_abstrm_Review_of_outlying_abs_terms << "\n"
      << underline(T_GaMa_abstrm_Review_of_outlying_abs_terms, '*') << "\n\n";

  out.width(IS->maxw_obs());
  out << "i" << " ";
  out.width(IS->maxw_id());
  out << T_GaMa_standpoint << " ";
  out.width(IS->maxw_id());
  out << T_GaMa_target << T_GaMa_abstrm_header1;
  {  // for ...
    for (int i=0; i < (IS->maxw_obs() + 2*(IS->maxw_id()) + 13); i++)
      out << "=";
  }  // for ...
  out << T_GaMa_abstrm_header2;
  out.flush();

  PointID predcs = "";   // previous standpoint ID
  for (int i=1; i<=IS->sum_observations(); i++)
    {
      Observation* pm = IS->ptr_obs(i);
      if (IS->test_abs_term(i))
        {
          out.width(IS->maxw_obs());
          out << i << " ";

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

          {   // ************************************************
            if (Distance* d = dynamic_cast<Distance*>(pm))
              {
                out << T_GaMa_distance;
                out.precision(5);
                out.width(12);
                Double m = d->value();
                out << m << " ";
              }
            else if (Direction* s = dynamic_cast<Direction*>(pm))
              {
                out << T_GaMa_direction;
                out.precision(6);
                out.width(12);
                Double m = R2G*(s->value());
		if (IS->gons())
		    out << m << " ";
		else
		    out << GNU_gama::gon2deg(m, 0, 2) << " ";
              }
            else if (Angle* u = dynamic_cast<Angle*>(pm))
              {
                out << '\n';
                out.width(IS->maxw_obs() + 2 + 2*(IS->maxw_id()));
                out << Utf8::leftPad(u->fs().str(), IS->maxw_id());
                out << T_GaMa_angle;
                out.precision(6);
                out.width(12);
                Double m = R2G*(u->value());
		if (IS->gons())
		    out << m << " ";
		else
		    out << GNU_gama::gon2deg(m, 0, 2) << " ";
              }
            else if (Z_Angle* z = dynamic_cast<Z_Angle*>(pm))
              {
                out << T_GaMa_z_angle;
                out.precision(6);
                out.width(12);
                Double m = R2G*(z->value());
		if (IS->gons())
		    out << m << " ";
		else
		    out << GNU_gama::gon2deg(m, 0, 2) << " ";
              }
            else if (S_Distance* p = dynamic_cast<S_Distance*>(pm))
              {
                out << T_GaMa_s_distance;
                out.precision(5);
                out.width(12);
                Double m = p->value();
                out << m << " ";
              }
            else if (H_Diff* h = dynamic_cast<H_Diff*>(pm))
              {
                out << T_GaMa_levell;
                out.precision(5);
                out.width(12);
                Double m = h->value();
                out << m << " ";
              }
            else if (Xdiff* dx = dynamic_cast<Xdiff*>(pm))
              {
                out << T_GaMa_xdiff;
                out.precision(5);
                out.width(12);
                Double m = dx->value();
                out << m << " ";
              }
            else if (Ydiff* dy = dynamic_cast<Ydiff*>(pm))
              {
                out << T_GaMa_ydiff;
                out.precision(5);
                out.width(12);
                Double m = dy->value();
                out << m << " ";
              }
            else if (Zdiff* dz = dynamic_cast<Zdiff*>(pm))
              {
                out << T_GaMa_zdiff;
                out.precision(5);
                out.width(12);
                Double m = dz->value();
                out << m << " ";
              }
            else if (X* x = dynamic_cast<X*>(pm))
              {
                out << T_GaMa_x;
                out.precision(5);
                out.width(12);
                Double m = x->value();
                out << m << " ";
              }
            else if (Y* y = dynamic_cast<Y*>(pm))
              {
                out << T_GaMa_y;
                out.precision(5);
                out.width(12);
                Double m = y->value();
                out << m << " ";
              }
            else if (Z* z = dynamic_cast<Z*>(pm))
              {
                out << T_GaMa_z;
                out.precision(5);
                out.width(12);
                Double m = z->value();
                out << m << " ";
              }
            else
              {
                throw GNU_gama::local::Exception(
                   "GaMa internal error - unknown observation\n");
              }
          }   // ************************************************

          out << setiosflags(ios_base::scientific) << setprecision(5);
          out << setw(13) << IS->rhs(i);       // 1.1.56 << pm->rhs();
          out << '\n';
          out.flush();
        }
    }

  out << "\n\n";
}

}}

#endif

