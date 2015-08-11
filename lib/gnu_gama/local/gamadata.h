/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local____ObservationData____and_other_data_objects___h_____
#define gama_local____ObservationData____and_other_data_objects___h_____

#include <gnu_gama/obsdata.h>

#include <gnu_gama/local/pointid.h>
#include <gnu_gama/local/lpoint.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/cluster.h>
#include <gnu_gama/local/angobs.h>
#include <gnu_gama/local/lcoords.h>

#include <map>
#include <list>
#include <algorithm>

namespace GNU_gama { namespace local {

  typedef std::list<PointID>    PointIDList;

  class PointData : public std::map <PointID, LocalPoint>,
                    public LocalCoordinateSystem,
                    public AngularObservations
    {
    public:
      double xNorthAngle() const;
    };


  std::ostream& operator << (std::ostream&,     PointData&);
  std::ostream& operator << (std::ostream& str, ObservationData&);

  inline bool GaMaConsistent(const PointData& lcs)
    {
      return
        (lcs. left_handed_coordinates() && lcs. left_handed_angles()) ||
        (lcs.right_handed_coordinates() && lcs.right_handed_angles());
    }

}}   // namepsace GNU_gama::local



#endif

















