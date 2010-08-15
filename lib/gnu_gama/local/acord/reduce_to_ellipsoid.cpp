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

#include <gnu_gama/local/acord/reduce_to_ellipsoid.h>
#include <gnu_gama/local/pobs/bearing.h>
#include <cmath>

using namespace std;
using namespace GNU_gama::local;


ReduceToEllipsoid::EllipsoidFunction::EllipsoidFunction(GNU_gama::Ellipsoid EL, double lat):
    el(EL), latitude(lat), centralPoint(LocalPoint(LocalPoint::XYZ(0, 0, 0)) )
{
    r = R();
}

void ReduceToEllipsoid::EllipsoidFunction::setCentralPoint(const LocalPoint& cp)
{
    centralPoint = cp;

    if ( !centralPoint.test_xy() ) centralPoint.set_xy(0, 0);
    if ( !centralPoint.test_z () ) centralPoint.set_z (0);
}

const LocalPoint& ReduceToEllipsoid::EllipsoidFunction::getCentralPoint() const
{
    return centralPoint;
}

double ReduceToEllipsoid::EllipsoidFunction::central_angle12(const LocalPoint& p2) const
{
    double z2 = ( p2.test_z() ? p2.z() : centralPoint.z());

    const double distance12 = distance(centralPoint, p2);

    int iteraction = 5;

    double ca12     = 0;
    double lastca12;

    do
    {
        lastca12 = ca12;

        ca12 = atan2( distance12 - (z2 - centralPoint.z())*tan(ca12),
                      r + centralPoint.z() );
    }
    while ( (fabs(ca12-lastca12) > 7e-8) && --iteraction );

    return ca12;
}

double ReduceToEllipsoid::EllipsoidFunction::central_angle23(const LocalPoint& p2, const LocalPoint& p3) const
{
    double z2 = ( p2.test_z() ? p2.z() : centralPoint.z());
    double z3 = ( p3.test_z() ? p3.z() : centralPoint.z());

    const double ca12 = central_angle12(p2);
    const double ca13 = central_angle12(p3);

    const double q2 = (r + centralPoint.z())*(1/cos(ca12) - 1);
    const double q3 = (r + centralPoint.z())*(1/cos(ca13) - 1);

    const double h2 = centralPoint.z() + q2 + (z2 - centralPoint.z())/cos(ca12);
    const double h3 = centralPoint.z() + q3 + (z3 - centralPoint.z())/cos(ca13);

    double rh2 = r + h2;
    double rh3 = r + h3;

    return acos( (rh2*rh2 + rh3*rh3 - sdistance(p2, p3)*sdistance(p2,p3)) / (2*rh2*rh3) );
}

double ReduceToEllipsoid::EllipsoidFunction::R() const
{
    const double e2 = 1 - el.b()*el.b()/(el.a()*el.a());

    return (el.a() * sqrt(1-e2))/(1-e2*sin(latitude)*sin(latitude));
}

double ReduceToEllipsoid::EllipsoidFunction::distance(const LocalPoint& a, const LocalPoint& b) const
{
    const double dy = b.y() - a.y();
    const double dx = b.x() - a.x();

    return  sqrt(dy*dy + dx*dx);
}

double ReduceToEllipsoid::EllipsoidFunction::sdistance(const LocalPoint& a, const LocalPoint& b) const
{
    const double dy = b.y() - a.y();
    const double dx = b.x() - a.x();
    const double dz = b.z() - a.z();

    return  sqrt(dy*dy + dx*dx + dz*dz);
}



ReduceToEllipsoid::ReduceToEllipsoid(PointData& b, ObservationData& m,
                                     GNU_gama::Ellipsoid el, double lat):
    PD(b), OD(m), EL(el), latitude(lat),  EF(EllipsoidFunction(el, lat))
{

}


bool ReduceToEllipsoid::reduce_z_angle_to_ellipsoid(Z_Angle* obs)
{
    const LocalPoint& p2 = PD[obs->from()];
    const LocalPoint& p3 = PD[obs->to()];

    if ( !p2.active_xy() || !p2.test_xy() || !p3.active_xy() || !p3.test_xy() )
        return false;

    const double bearing21 = bearing(p2, EF.getCentralPoint());
    const double bearing23 = bearing(p2, p3);

    double correction = +EF.central_angle12(p2)*cos(bearing23 - bearing21);
                  //  = -EF.central_angle23(p2,p3)*cos(bearing23 - bearing21);

    reduced[obs] = obs->value();
    obs->set_value( obs->value() + correction);

    return true;
}


bool ReduceToEllipsoid::reduce_direction_to_ellipsoid(Direction* obs)
{
    const PointID p2id = obs->from();
    const PointID p3id = obs->to();

    const LocalPoint& p2   = PD[p2id];
    const LocalPoint& p3   = PD[p3id];

    if ( !p2.active_xy() || !p2.test_xy() || !p3.active_xy() || !p3.test_xy() )
        return false;

    const double bearing21 = bearing(p2, EF.getCentralPoint());
    const double bearing23 = bearing(p2, p3);

    double zenithSum = 0;
    int zenithNum    = 0;

    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    {
        Observation* o = *i;

        if ( !o->active() )
            continue;

        if (/* Z_Angle* z =*/ dynamic_cast<Z_Angle*>(o) )
        {
            PointID from = o->from();
            PointID to   = o->to();

            if ( (p2id == from) && (p3id == to) )
            {
                zenithSum += o->value();
                ++zenithNum;
            }
            else
                if ( (p3id == from) && (p2id == to) )
                {
                    zenithSum += (M_PI - o->value());
                    ++zenithNum;
                }
        }
    }

    double zenith23 = (zenithNum ? (zenithSum / zenithNum) : M_PI/2 );

    double correction = -EF.central_angle12(p2)/tan(zenith23)*sin(bearing23 - bearing21);

    reduced[obs] = obs->value();
    obs->set_value( obs->value() + correction);

    return true;
}

void ReduceToEllipsoid::execute()
{
    if ( !reduced.empty() )
        reduced.clear();

    double sumx = 0;
    double sumy = 0;
    double sumz = 0;

    int numxy = 0;
    int numz  = 0;

    for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
    {
        const LocalPoint& p = (*i).second;
        if (p.active_xy() && p.test_xy())
        {
            sumx += p.x();
            sumy += p.y();
            ++numxy;
        }

        if (p.active_z() && p.test_z())
        {
            sumz += p.z();
            ++numz;
        }
    }

    const double centralX = ( numxy ? sumx / numxy : 0 );
    const double centralY = ( numxy ? sumy / numxy : 0 );
    const double centralZ = ( numz  ? sumz / numz  : 0 );

    EF.setCentralPoint(LocalPoint::XYZ(centralX, centralY, centralZ));

    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    {
        Observation* obs = *i;

        if ( !obs->active() )
            continue;

        if ( Direction* d = dynamic_cast<Direction*>(obs) )
        {
            reduce_direction_to_ellipsoid(d);
        }
        else
            if ( Z_Angle* z = dynamic_cast<Z_Angle*>(obs) )
            {
                reduce_z_angle_to_ellipsoid(z);
            }
    }

}
