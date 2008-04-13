/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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

/*
 *  $Id: fixed_points.h,v 1.3 2008/04/13 10:02:31 cepek Exp $
 */

#ifndef GaMa_GaMaProg_Pevne_Body_h_
#define GaMa_GaMaProg_Pevne_Body_h_

#include <iomanip>
#include <gamalib/local/network.h>
#include <gamalib/local/pobs/format.h>

namespace GaMaLib {

template <typename OutStream>
void FixedPoints(GaMaLib::LocalNetwork* IS, OutStream& out)
{
  using namespace std;
  using namespace GaMaLib;
  
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
            out.width(IS->maxw_id());
            out << ((*i).first).c_str();
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

}

#endif




