/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Jan Pytel  <pytel@gama.fsv.cvut.cz>

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

#include <gnu_gama/local/acord/approx_vectors.h>
#include <iostream>

using namespace std;
using namespace GNU_gama::local;

ApproximateVectors::ApproximateVectors(PointData& b, ObservationData& m)
    : PD(b), OD(m)
{
    missing_coords = false;

    for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
    {
	const LocalPoint& p = (*i).second;
	if ( ApproximateVectors::unknown_xy(p) ||
	     ApproximateVectors::unknown_z(p) )
	    {
		missing_coords = true;
		break;
	    }
    }

    if (missing_coords)
      for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
        {
          Observation* obs = *i;

          LocalPoint& from = PD[obs->from()];
          LocalPoint& to   = PD[obs->to  ()];

          if ( ApproximateVectors::unknown_xy(from) ||
               ApproximateVectors::unknown_xy(to) )
            {
              if (const Xdiff* vx = dynamic_cast<const Xdiff*>(obs))
                OVD.XD.push_back(vx);
              else
                if (const Ydiff* vy = dynamic_cast<const Ydiff*>(obs))
                  OVD.YD.push_back(vy);
            }
          if ( ApproximateVectors::unknown_z(from) ||
               ApproximateVectors::unknown_z(to) )
            if (const Zdiff* vz = dynamic_cast<const Zdiff*>(obs))
              OVD.ZD.push_back(vz);

        }
}

void ApproximateVectors::execute()
{
    if (!missing_coords || obs_list_empty() ) return;

    bool updated;

    do {

	updated = false;

	for (Z_const_iterator ci = OVD.ZD.begin(); ci != OVD.ZD.end(); ++ci)
	{
	    const PointID& from_id = (*ci)->from();
	    const PointID& to_id   = (*ci)->to();

	    LocalPoint& from = PD[from_id];
	    LocalPoint& to   = PD[to_id];

	    if  ( ApproximateVectors::unknown_z(from) &&
		   ApproximateVectors::known_z(to) )
	    {
		from.set_z( to.z() - (*ci)->value() );
		updated = true;
	    }
	    else
		if (ApproximateVectors::known_z(from) &&
		    ApproximateVectors::unknown_z(to) )
	    {
		to.set_z( from.z() + (*ci)->value() );
		updated = true;
	    }
	} // for

	for (X_const_iterator ci = OVD.XD.begin(); ci != OVD.XD.end(); ++ci)
	{
	    const PointID& from_id = (*ci)->from();
	    const PointID& to_id   = (*ci)->to();

	    LocalPoint& from = PD[from_id];
	    LocalPoint& to   = PD[to_id];

	    if ( ApproximateVectors::unknown_xy(from) &&
		   ApproximateVectors::known_xy(to) )
	    {
		Y_const_iterator cii = find_ydiff(from_id, to_id);
		if ( cii == OVD.YD.end() )
		    continue;

		from.set_xy( to.x() - (*ci)->value(), to.y() - (*cii)->value() );
		updated = true;
	    }
	    else
		if ( ApproximateVectors::known_xy(from) &&
		     ApproximateVectors::unknown_xy(to) )
		{
		    Y_const_iterator cii = find_ydiff(from_id, to_id);
		    if ( cii == OVD.YD.end() )
			continue;

		    to.set_xy( from.x() + (*ci)->value(), from.y() + (*cii)->value() );
		    updated = true;
		}

	} // for
    }
    while ( updated );
}

void ApproximateVectors::print(ostream&)
{
}

