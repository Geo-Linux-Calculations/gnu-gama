/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: observation.cpp,v 1.5 2004/03/29 12:06:51 cepek Exp $
 */

#include <gamalib/observation.h>
#include <gamalib/cluster.h>
#include <typeinfo>

namespace GaMaLib 
{
  // the static member Observation::gons was introduced in 1.7.09 to
  // enable selection between grades and degrees in virtual functions
  // Observation::write() with minimal changes in existing old code of
  // GaMaLib

  bool Observation::gons = true;
}

using namespace GaMaLib;
using namespace std;

bool  Observation::check_std_dev() const
{
  return cluster && cluster->covariance_matrix.dim();
}


Double Observation::stdDev() const 
{
   return cluster->stdDev(cluster_index); 
}  

Double Direction::orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->orientation();
}

void Direction::set_orientation(Double p)
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->set_orientation(p);
}

bool Direction::test_orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->test_orientation();
}

void Direction::delete_orientation()
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->delete_orientation();
}

void Direction::index_orientation(int n)
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  sp->index_orientation(n);
}

int Direction::index_orientation() const
{
  StandPoint* sp = static_cast<StandPoint*>(cluster);
  return sp->index_orientation();
}


