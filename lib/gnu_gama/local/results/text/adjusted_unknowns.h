/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>
                2014, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GaMa_GaMaProg_Vyrovnane_Nezname_h_
#define GaMa_GaMaProg_Vyrovnane_Nezname_h_

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/cluster.h>

namespace GNU_gama { namespace local {

template <typename OutStream>
void AdjustedUnknowns(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GNU_gama::local;

  const int y_sign = IS->y_sign();

  const Vec& x = IS->solve();
  double kki = IS->conf_int_coef();
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
  if (sour)
    {
      double /*mp,*/ mp_max = -1, mp_prum = 0;
      PointID mp_max_cb, prev_id;
      int pocbod = 0;

      out << T_GaMa_adjunk_Review_of_unknowns_coordidantes << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_coordidantes, '*')
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_point;
      out << T_GaMa_adjunk_header1;
      for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '=';
      out << T_GaMa_adjunk_header2;
      out.setf(ios_base::fixed, ios_base::floatfield);

      for (PointData::const_iterator ii=IS->PD.begin(); ii!=IS->PD.end(); ii++)
        {
          const PointID point_id = (*ii).first;
          const LocalPoint&  b   = (*ii).second;

          if (b.free_xy() && b.index_x())
            {
              out.width(IS->maxw_unk());
              out << " " << " ";
              out.width(IS->maxw_id());
              if (prev_id != point_id)
                out << Utf8::leftPad(point_id.str(), IS->maxw_id());
              else
                out << " ";
              prev_id = point_id;
              double mx = IS->unknown_stdev(b.index_x());
              double my = IS->unknown_stdev(b.index_y());
              // mp = sqrt(my*my+mx*mx);
              out << '\n';

              out.width(IS->maxw_unk());
              out << b.index_x() << " ";
              out.width(IS->maxw_id());
              if (b.constrained_xy())
                out << "X" << " * ";
              else
                out << "x" << "   ";
              out.precision(5);
              out.width(13);
              double adj_x = b.x()+x(b.index_x())/1000;
              out << b.x_0() << " ";
              out.width(9);
              out << (adj_x - b.x_0()) << " ";
              out.width(13);
              out << adj_x << " ";
              out.precision(1);
              out.width(7);
              out << mx << " ";
              out.width(7);
              out << mx*kki;
              out << "\n";

              out.flush();
              out.width(IS->maxw_unk());
              out << b.index_y() << " ";
              out.width(IS->maxw_id());
              if (b.constrained_xy())
                out << "Y" << " * ";
              else
                out << "y" << "   ";
              out.precision(5);
              out.width(13);
              double adj_y = y_sign*(b.y()+x(b.index_y())/1000);
              out << y_sign*b.y_0() << " ";
              out.width(9);
              out << (adj_y - y_sign*b.y_0()) << " ";
              out.width(13);
              out << adj_y << " ";
              out.precision(1);
              out.width(7);
              out << my << " ";
              out.width(7);
              out << my*kki;
              out << "\n";
            }
          if (b.free_z() && b.index_z())
            {
              if (!b.free_xy())
                {
                  out.width(IS->maxw_unk());
                  out << " " << " ";
                  out.width(IS->maxw_id());
                  if (prev_id != point_id)
                    out << Utf8::leftPad(point_id.str(), IS->maxw_id());
                  else
                    out << " ";
                  out << '\n';
                }
              prev_id = point_id;

              out.width(IS->maxw_unk());
              out << b.index_z() << " ";
              out.width(IS->maxw_id());
              if (b.constrained_z())
                out << "Z" << " * ";
              else
                out << "z" << "   ";
              out.precision(5);
              out.width(13);
              double adj_z = b.z()+x(b.index_z())/1000;
              out << b.z_0() << " ";
              out.width(9);
              out << (adj_z - b.z_0()) << " ";
              out.width(13);
              out << adj_z << " ";
              double mz = IS->unknown_stdev(b.index_z());
              out.precision(1);
              out.width(7);
              out << mz << " ";
              out.width(7);
              out << mz*kki;
              out << "\n";
            }

          if ((b.free_xy() && b.index_x()) ||
              (b.free_z()  && b.index_z()) ) out << '\n';
        }

      if (pocbod >= 5)
        {
          out.precision(1);
          out << T_GaMa_adjunk_mean_position_error_maximal << mp_max
              << T_GaMa_adjunk_mean_position_error_on_point
              << mp_max_cb << '\n'
              << T_GaMa_adjunk_mean_position_error_average << mp_prum/pocbod
              << " mm\n\n";
        }

      out << '\n';
    }

  bool orp = false;
  for (int i=1; i<=pocnez; i++)
    if (IS->unknown_type(i) == 'R')
      {
        orp = true;
        break;
      }
  if (orp)
    {
      const double scale  = IS->gons() ? 1.0 : 0.324;

      out << T_GaMa_adjunk_Review_of_unknowns_bearings << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_bearings, '*')
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_standpoint;
      if (IS->degrees()) out << "   ";
      out << T_GaMa_adjunk_header3;
      for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '=';
      if (IS->gons())
        out  << T_GaMa_adjunk_header4;
      else
        out <<
          "====== [d] ========= [d] ======== [d] =========== [ss] ===\n\n";
      out.flush();    // flush() sends read data to output

      {   // for ...
        for (int i=1; i<=pocnez; i++)
          if (IS->unknown_type(i) == 'R')
            {
              out.width(IS->maxw_unk());
              out << i << " " ;
              out.width(IS->maxw_id());
              const PointID cb = IS->unknown_pointid(i);
              out << Utf8::leftPad(cb.str(), IS->maxw_id()) << "  ";
              StandPoint* k = IS->unknown_standpoint(i);
              double z = y_sign*( k->orientation() )*R2G;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              out.setf(ios_base::fixed, ios_base::floatfield);
              out.precision(6);
              out.width(12);
              if (IS->gons())
                out << z << " ";
              else
                out << GNU_gama::gon2deg(z, 0, 2) << " ";
              out.width(10);
              double cor = y_sign*x(i)/10000;
              if (IS->gons())
                out << cor << " ";
              else
                out << GNU_gama::gon2deg(cor, 2, 2) << " ";
              z += cor;
              if (z <  0 ) z += 400;
              if (z > 400) z -= 400;
              out.width(11);
              if (IS->gons())
                out << z << " ";
              else
                out << GNU_gama::gon2deg(z, 0, 2) << " ";
              out.precision(1);
              out.width(8);
              double mz = IS->unknown_stdev(i)*scale;
              out << mz << " ";
              out.width(7);
              out << mz*kki;
              out << '\n';
              out.flush();
            }
      }   // for ...

      out << '\n' << '\n';
    }


  bool vysky = false;
  {
    for (int i=1; i<=pocnez; i++)
      if (IS->unknown_type(i) == 'Z')
        {
          vysky = true;
          break;
       }
  }
  if (vysky && !sour)
    {
      out << T_GaMa_adjunk_Review_of_unknowns_heights << "\n"
          << underline(T_GaMa_adjunk_Review_of_unknowns_heights, '*')
          << "\n\n";
      out.width(IS->maxw_unk());
      out << "i" << " ";
      out.width(IS->maxw_id());
      out << T_GaMa_point;
      out << T_GaMa_adjunk_header5;
      { for (int i=0; i<IS->maxw_unk()+IS->maxw_id()+1; i++) out << '='; }
      out << T_GaMa_adjunk_header6;
      out.setf(ios_base::fixed, ios_base::floatfield);

      for (int i=1; i<=pocnez; i++)
        if (IS->unknown_type(i) == 'Z')
          {
            const PointID cb     = IS->unknown_pointid(i);
            const LocalPoint&  b = IS->PD[cb];

            out.width(IS->maxw_unk());
            out << i << " ";
            out.width(IS->maxw_id());
            out << Utf8::leftPad(cb.str(), IS->maxw_id());
            if (b.constrained_z())
              out << " * ";
            else
              out << "   ";
            out.precision(5);
            out.width(13);
            double adj_z = b.z()+x(i)/1000;
            out << b.z_0() << " ";
            out.width(9);
            out << (adj_z - b.z_0()) << " ";
            out.width(13);
            out << adj_z << " ";
            double mv = IS->unknown_stdev(i);
            out.precision(1);
            out.width(7);
            out << mv << " ";
            out.width(7);
            out << mv*kki;
            out << '\n';
          }

      out << "\n\n";
    }
}

}}

#endif



