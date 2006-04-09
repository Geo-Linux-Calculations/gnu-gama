/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
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
 *  $Id: hdiff.h,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */

#include <gamalib/local/revision.h>

using namespace GaMaLib;
using namespace std;


bool LocalRevision::h_diff(const H_Diff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_z()) return false;
  if (!(*c).second.test_z()) return false;

  return true;
}
