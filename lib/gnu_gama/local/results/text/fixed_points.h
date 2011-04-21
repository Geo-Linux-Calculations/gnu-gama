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

#ifndef GaMa_GaMaProg_Pevne_Body_h_
#define GaMa_GaMaProg_Pevne_Body_h_

#include <iomanip>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/pobs/format.h>
#include <gnu_gama/utf8.h>

namespace GNU_gama { namespace local {

template <typename OutStream>
void FixedPoints(GNU_gama::local::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GNU_gama::local;

  const int y_sign = GaMaConsistent(IS->PD) ? +1 : -1;

  int pocpevb=0, pocpevv=0;
  {   // for ...
    for (PointData::iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
      {
        if ((*i).second.fixed_xy())  pocpevb++;
        if ((*i).second.fixed_z()) pocpevv++;
      }
  }   // for ...
  if (pocpevb == 0 && pocpevv == 0) return;

  out << T_GaMa_Review_of_fixed_points << "\n"
      << underline(T_GaMa_Review_of_fixed_points, '*') << "\n\n";


  out.width(IS->maxw_id());
  out << T_GaMa_point;
  int table=0;
  if (pocpevb)
    {
      out.width(13);
      out << "x   ";
      out.width(13+2);
      out << "y   ";
      table += 2*13 + 2;
    }
  if (pocpevv)
    {
      if (pocpevb)
        {
          out << "  ";
          table += 2;
        }
      out.width(13);
      out << "z   ";
      table += 13;
    }
  out << '\n';
  {
    for (int i=0; i<IS->maxw_id()+table; i++) out << '=';
  }
  out << "\n\n";

  {   // for ...
    for (PointData::iterator i=IS->PD.begin(); i!=IS->PD.end(); ++i)
      {
        if ((*i).second.fixed_xy() || (*i).second.fixed_z())
          {
            // out.width(IS->maxw_id());
            // out << ((*i).first).c_str();
	    out << Utf8::leftPad(i->first.str(), IS->maxw_id());
          }
        if ((*i).second.fixed_xy())
          {
            out.precision(3);
            out.width(13);
            out << (*i).second.x();
            out << "  ";
            out.width(13);
            out << (*i).second.y()*y_sign;
          }
        if ((*i).second.fixed_z())
          {
            if (pocpevb && !(*i).second.fixed_xy())
              {
                out.width(2*13+2);
                out << " ";
              }
            out.precision(3);
            if (pocpevb)  out << "  ";
            out.width(13);
            out << (*i).second.z();
          }
        if ((*i).second.fixed_xy() || (*i).second.fixed_z())
          {
            out << "\n";
            out.flush();
          }
      }
  }   // for ...

  out << "\n\n";
  out.flush();
}

}}

#endif




