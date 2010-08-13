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

#ifndef GaMaLib_Bod_Mer_BMFCE_H
#define GaMaLib_Bod_Mer_BMFCE_H

#include <cmath>
#include <gamalib/float.h>
#include <gamalib/local/gamadata.h>
#include <gamalib/language.h>

namespace GaMaLib {

inline Double bearing(Double ya, Double xa, Double yb, Double xb)
{
   using namespace std;
   const Double dy = yb - ya;
   const Double dx = xb - xa;
   if (dy == 0 && dx == 0)
      throw 
        GaMaLib::Exception(T_POBS_computation_of_bearing_for_identical_points);
   const Double s  = atan2( dy , dx );
   return s >= 0 ? s : s + 2*M_PI;
}


inline Double bearing(const LocalPoint& a, const LocalPoint& b)
{
   return bearing(a.y(), a.x(), b.y(), b.x());
}

inline void bearing_distance(Double ya, Double xa, Double yb, Double xb,
                             Double& br, Double& d)
{
   using namespace std;
   const Double dy = yb - ya;
   const Double dx = xb - xa;
   if (dy == 0 && dx == 0)
      throw 
        GaMaLib::Exception(T_POBS_computation_of_bearing_for_identical_points);
   const Double s  = atan2( dy , dx );
   br = s >= 0 ? s : s + 2*M_PI;
   d  = sqrt(dy*dy + dx*dx);
}

inline void bearing_distance(const LocalPoint& a, const LocalPoint& b, 
                             Double& br, Double& d)
{
   bearing_distance(a.y(), a.x(), b.y(), b.x(), br, d);
}

}   // namespace GaMaLib

#endif













