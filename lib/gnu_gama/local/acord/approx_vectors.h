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

#ifndef gama_local_acord_ApproximateVectors_header_h
#define gama_local_acord_ApproximateVectors_header_h

#include <gnu_gama/local/gamadata.h>
#include <algorithm>
#include <vector>

namespace GNU_gama { namespace local {


    class ApproximateVectors
	{
	private:

	    struct ObservedVData {
		std::vector<const Xdiff*>  XD;
		std::vector<const Ydiff*>  YD;
		std::vector<const Zdiff*>  ZD;
	    };

	    typedef std::vector<const Xdiff*>::const_iterator X_const_iterator;
	    typedef std::vector<const Ydiff*>::const_iterator Y_const_iterator;
	    typedef std::vector<const Zdiff*>::const_iterator Z_const_iterator;

	    bool obs_list_empty() const
		{
		    return OVD.XD.empty() && OVD.YD.empty() && OVD.ZD.empty();
		}

	    Y_const_iterator find_ydiff(const PointID& from_id, const PointID& to_id )
		{
		    for (Y_const_iterator ci = OVD.YD.begin(); ci != OVD.YD.end(); ++ci)
			if ( ( (*ci)->from() == from_id ) && ( (*ci)->to() == to_id) )
			    return ci;
		    return OVD.YD.end();
		}

	    static bool known_xy(const LocalPoint& lp)
		{
		    return lp.active_xy() && lp.test_xy();
		}

	    static bool unknown_xy(const LocalPoint& lp)
		{
		    return lp.active_xy() && !lp.test_xy();
		}

	    static bool known_z(const LocalPoint& lp)
		{
		    return lp.active_z() && lp.test_z();
		}

	    static bool unknown_z(const LocalPoint& lp)
		{
		    return lp.active_z() && !lp.test_z();
		}

	    bool                missing_coords;
	    PointData&          PD;
	    ObservationData&    OD;

	    ObservedVData      OVD;

	public:

	    ApproximateVectors(PointData& b, ObservationData& m);
	    void execute();
	    void print(std::ostream&);

	};

 }}   // namespace GNU_gama::local

#endif

