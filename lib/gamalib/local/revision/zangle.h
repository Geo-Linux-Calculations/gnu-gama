/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Jan Pytel <pytel@gama.fsv.cvut.cz>

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

#include <gamalib/local/revision.h>

using namespace GaMaLib;
using namespace std;


bool LocalRevision::z_angle(const Z_Angle* obs) const
{
  if (!obs->active()) return false; 
  
  PointData::const_iterator s = PD.find(obs->from());  // station point
  if (s == PD.end()) return false;
  // if (!(*s).second.active_xy()) return false; 
  if (!(*s).second.test_xy())   return false;
  if (!(*s).second.active_z())  return false;
  if (!(*s).second.test_z())    return false;

  PointData::const_iterator t = PD.find(obs->to());    // target  point
  if (t == PD.end()) return false;
  // if (!(*t).second.active_xy()) return false;
  if (!(*t).second.test_xy())   return false;
  if (!(*t).second.active_z())  return false;
  if (!(*t).second.test_z())    return false;

  return true;
}
