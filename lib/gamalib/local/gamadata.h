/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef GaMaLib____ObservationData____and_other_data_objects___h_____
#define GaMaLib____ObservationData____and_other_data_objects___h_____

#include <gnu_gama/obsdata.h>

#include <gamalib/pointid.h>
#include <gamalib/local/lpoint.h>
#include <gamalib/observation.h>
#include <gamalib/cluster.h>
#include <gamalib/angobs.h>
#include <gamalib/local/lcoords.h>

#include <map>
#include <list>
#include <algorithm>

namespace GaMaLib {

  typedef std::list<PointID>    PointIDList;

  class PointData : public std::map <PointID, LocalPoint>,
                    public LocalCoordinateSystem,
                    public AngularObservations
    {
    };


  std::ostream& operator << (std::ostream&,     PointData&);
  std::ostream& operator << (std::ostream& str, ObservationData&);

  inline bool GaMaConsistent(const PointData& lcs)
    {
      return 
        (lcs. left_handed_coordinates() &&  lcs.right_handed_angles) ||
        (lcs.right_handed_coordinates() && !lcs.right_handed_angles) ; 
    }


}   // namepsace GaMaLib



#endif

















