/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004  Jan Pytel  <pytel@gama.fsv.cvut.cz>

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

#ifndef gama_local_acord_ReduceToElipsoid_header_h
#define gama_local_acord_ReduceToElipsoid_header_h

#include <gnu_gama/ellipsoid.h>

#include <gnu_gama/local/gamadata.h>
#include <algorithm>
#include <map>

namespace GNU_gama { namespace local {

    class ReduceToEllipsoid {

        class EllipsoidFunction {
        public:
            EllipsoidFunction(GNU_gama::Ellipsoid EL, double lat);

            void setCentralPoint(const LocalPoint& cp);
            const LocalPoint& getCentralPoint() const;
            double central_angle12(const LocalPoint& p2) const;
            double central_angle23(const LocalPoint& p2, const LocalPoint& p3) const;
        private:
            double R() const;
            double distance(const LocalPoint& a, const LocalPoint& b) const;
            double sdistance(const LocalPoint& a, const LocalPoint& b) const;

            const GNU_gama::Ellipsoid el;
            const double latitude;
            LocalPoint centralPoint;

            double r;
        };

        bool reduce_z_angle_to_ellipsoid  (Z_Angle* obs);
        bool reduce_direction_to_ellipsoid(Direction* obs);

    public:
        typedef std::map<Observation*, double> ObsMap;

        ReduceToEllipsoid(PointData& b, ObservationData& m,
                          GNU_gama::Ellipsoid el, double lat);

        void execute();

        const ObsMap& getMap() const
        {
            return reduced;
        }

    private:

        PointData&          PD;
        ObservationData&    OD;
        // GNU_gama::Ellipsoid EL;        ... unused
        // double              latitude;  ... unused
        EllipsoidFunction   EF;
        ObsMap              reduced;
    };

}} // namespace GNU_gama::local

#endif
