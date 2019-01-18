/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>,
                      Jan Pytel <jan.pytel@gmail.com>,
                2010, 2013, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/local/local_revision.h>
#include <gnu_gama/local/local_linearization.h>
#include <gnu_gama/local/bearing.h>

using namespace GNU_gama::local;
using namespace std;


void LocalLinearization::direction(const Direction* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   double s, d;
   bearing_distance(sbod, cbod, s, d);
   // const double p = m0 / obs->stdDev();
   const double K = 10*R2G/d;
   const double ps = K*sin(s);
   const double pc = K*cos(s);

   const StandPoint*  csp = static_cast<const StandPoint*>(obs->ptr_cluster());
   StandPoint* sp = const_cast<StandPoint*>(csp);

   // double w = p*p;                                       // weight
   double a = (obs->value() + sp->orientation() - s)*R2CC;  // rhs

   while (a > 200e4)
      a -= 400e4;
   while (a < -200e4)
      a += 400e4;
   rhs = a;          // absolute termm is in cc

   size = 0;
   if (!sp->index_orientation()) sp->index_orientation(++maxn);
   index[ size ] = sp->index_orientation();
   coeff[ size ] = -1;
   size++;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -pc;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = ps;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = pc;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = -ps;
      size++;
   }
}


void LocalLinearization::distance(const Distance* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   double s, d;
   bearing_distance(PD[obs->from()], PD[obs->to()], s, d);
   // double p = M_0 / stdDev();
   double ps = sin(s);
   double pc = cos(s);

   // double w = p*p;                  // weight
   rhs = (obs->value() - d)*1e3;       // abs. term in millimetres

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -ps;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = -pc;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = ps;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = pc;
      size++;
   }
}


void LocalLinearization::h_diff(const H_Diff* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   double h = cbod.z() - sbod.z();
   // double p = M_0 / stdDev();

   // double w = p*p;                  // weight
   rhs = (obs->value() - h)*1e3;       // abs. term in millimetres

   size = 0;
   if (sbod.free_z())
   {
      if (!sbod.index_z())  sbod.index_z() = ++maxn;
      index[ size ] = sbod.index_z();
      coeff[ size ] = -1;
      size++;
   }
   if (cbod.free_z())
   {
      if (!cbod.index_z())  cbod.index_z() = ++maxn;
      index[ size ] = cbod.index_z();
      coeff[ size ] = 1;
      size++;
   }
}


void LocalLinearization::s_distance(const S_Distance* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   // double s, sd;
   // bearing_sdistance(PD[obs->from()], PD[obs->to()], s, sd);
   // double p = M_0 / stdDev();
   double dx = cbod.x() - sbod.x();
   double dy = cbod.y() - sbod.y();
   double dz = cbod.z() - sbod.z();
   double sd = sqrt(dx*dx + dy*dy + dz*dz);
   if (sd == 0)
     throw GNU_gama::local::Exception(T_POBS_zero_or_negative_slope_distance);

   double px = dx / sd;
   double py = dy / sd;
   double pz = dz / sd;

   // double w = p*p;                // weight
   rhs = (obs->value() - sd)*1e3;    // abs. term in millimetres

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -py;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = -px;
      size++;
   }
   if (sbod.free_z())
   {
      if (!sbod.index_z()) sbod.index_z() = ++maxn;
      index[ size ] = sbod.index_z();
      coeff[ size ] = -pz;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = py;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = px;
      size++;
   }
   if (cbod.free_z())
   {
      if (!cbod.index_z()) cbod.index_z() = ++maxn;
      index[ size ] = cbod.index_z();
      coeff[ size ] = pz;
      size++;
   }
}


void LocalLinearization::x(const X* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // double p = M_0 / stdDev();

   // double w = p*p;                          // weight
   rhs = (obs->value() - point.x())*1e3;       // abs. term in millimetres

   size = 0;
   if (point.free_xy())
   {
      if (!point.index_x()) point.index_x() = ++maxn;
      index[ size ] = point.index_x();
      coeff[ size ] = 1;
      size++;
   }
}


void LocalLinearization::y(const Y* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // double p = M_0 / stdDev();

   // double w = p*p;                          // weight
   rhs = (obs->value() - point.y())*1e3;       // abs. term in millimetres

   size = 0;
   if (point.free_xy())
   {
      if (!point.index_y()) point.index_y() = ++maxn;
      index[ size ] = point.index_y();
      coeff[ size ] = 1;
      size++;
   }
}


void LocalLinearization::z(const Z* obs) const
{
   LocalPoint& point = PD[obs->from()];
   // double p = M_0 / stdDev();

   // double w = p*p;                          // weight
   rhs = (obs->value() - point.z())*1e3;       // abs. term in millimetres

   size = 0;
   if (point.free_z())
   {
      if (!point.index_z()) point.index_z() = ++maxn;
      index[ size ] = point.index_z();
      coeff[ size ] = 1;
      size++;
   }
}


void LocalLinearization::xdiff(const Xdiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];               // stand point
  LocalPoint& tpoint = PD[obs-> to() ];               // target
  double df = tpoint.x() - spoint.x();
  // double p  = M_0 / stdDev();

  // double w = p*p;                             // weight
  rhs = (obs->value() - df)*1e3;                 // abs. term in millimetres

  size = 0;
  if (spoint.free_xy())
    {
      if (!spoint.index_x()) spoint.index_x() = ++maxn;
      index[ size ] =  spoint.index_x();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_xy())
    {
      if (!tpoint.index_x()) tpoint.index_x() = ++maxn;
      index[ size ] =  tpoint.index_x();
      coeff[ size ] = +1;
      size++;
    }
}


void LocalLinearization::ydiff(const Ydiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];
  LocalPoint& tpoint = PD[obs-> to() ];
  double df = tpoint.y() - spoint.y();
  // double p = M_0 / stdDev();

  // double w = p*p;
  rhs = (obs->value() - df)*1e3;

  size = 0;
  if (spoint.free_xy())
    {
      if (!spoint.index_y()) spoint.index_y() = ++maxn;
      index[ size ] =  spoint.index_y();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_xy())
    {
      if (!tpoint.index_y()) tpoint.index_y() = ++maxn;
      index[ size ] =  tpoint.index_y();
      coeff[ size ] = +1;
      size++;
    }
}


void LocalLinearization::zdiff(const Zdiff* obs) const
{
  LocalPoint& spoint = PD[obs->from()];
  LocalPoint& tpoint = PD[obs-> to() ];
  double df = tpoint.z() - spoint.z();
  // double p = M_0 / stdDev();

  // double w = p*p;
  rhs = (obs->value() - df)*1e3;

  size = 0;
  if (spoint.free_z())
    {
      if (!spoint.index_z()) spoint.index_z() = ++maxn;
      index[ size ] =  spoint.index_z();
      coeff[ size ] = -1;
      size++;
    }
  if (tpoint.free_z())
    {
      if (!tpoint.index_z()) tpoint.index_z() = ++maxn;
      index[ size ] =  tpoint.index_z();
      coeff[ size ] = +1;
      size++;
    }
}


void LocalLinearization::z_angle(const Z_Angle* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   // double s, d, sd;
   // bearing_distance(PD[obs->from()], PD[obs->to()], s, d);
   // bearing_sdistance(PD[obs->from()], PD[obs->to()], s, sd);

   double dx = cbod.x() - sbod.x();
   double dy = cbod.y() - sbod.y();
   double dz = cbod.z() - sbod.z();

   double d2 = dx*dx + dy*dy;
   double d  = sqrt(d2);
   double sd = sqrt(d2 + dz*dz);
   if (d == 0 || sd == 0)
     throw GNU_gama::local::Exception(T_POBS_zero_or_negative_zenith_angle);

   double k  = 10*R2G/(d*sd*sd);

   // double p = M_0 / stdDev();
   double px =  k * dz * dx;
   double py =  k * dz * dy;
   double pz = -k *  d *  d;
   // double w = p*p;                  // weight

   double za = acos(dz/sd);

   if (obs->value() > M_PI) za = 2*M_PI - za;
   double a  = (obs->value() - za);

   rhs = a * R2CC; // abs. term in cc
   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -py;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = -px;
      size++;
   }
   if (sbod.free_z())
   {
      if (!sbod.index_z()) sbod.index_z() = ++maxn;
      index[ size ] = sbod.index_z();
      coeff[ size ] = -pz;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = py;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = px;
      size++;
   }
   if (cbod.free_z())
   {
      if (!cbod.index_z()) cbod.index_z() = ++maxn;
      index[ size ] = cbod.index_z();
      coeff[ size ] = pz;
      size++;
   }
}


void LocalLinearization::angle(const Angle* obs) const
{
   LocalPoint& sbod  = PD[obs->from()];
   LocalPoint& cbod1 = PD[obs->bs()];
   LocalPoint& cbod2 = PD[obs->fs()];
   double s1, d1, s2, d2;
   bearing_distance(PD[obs->from()], PD[obs->bs()], s1, d1);
   bearing_distance(PD[obs->from()], PD[obs->fs()], s2, d2);
   // double p = m0 / obs->stdDev();
   const double K1 = 10*R2G/d1;
   const double K2 = 10*R2G/d2;
   const double ps1 = K1*sin(s1);
   const double pc1 = K1*cos(s1);
   const double ps2 = K2*sin(s2);
   const double pc2 = K2*cos(s2);

   // double w = p*p;                          // weight
   double ds = s2 - s1;
   if (ds < 0) ds += 2*M_PI;
   double a = (obs->value() - ds)*R2CC;        // rhs
   // "big" positive/negative angle transformed to "lesser" positive/negative
   while (a > 200e4)
      a -= 400e4;
   while (a < -200e4)
      a += 400e4;
   rhs = a;          // rhs in cc

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] =  sbod.index_y();
      coeff[ size ] = -pc2 + pc1;
      size++;
      index[ size ] =  sbod.index_x();
      coeff[ size ] =  ps2 - ps1;
      size++;
   }
   if (cbod1.free_xy())
   {
      if (!cbod1.index_x()) cbod1.index_x() = ++maxn;
      if (!cbod1.index_y()) cbod1.index_y() = ++maxn;
      index[ size ] =  cbod1.index_y();
      coeff[ size ] = -pc1;
      size++;
      index[ size ] =  cbod1.index_x();
      coeff[ size ] =  ps1;
      size++;
   }
   if (cbod2.free_xy())
   {
      if (!cbod2.index_x()) cbod2.index_x() = ++maxn;
      if (!cbod2.index_y()) cbod2.index_y() = ++maxn;
      index[ size ] =  cbod2.index_y();
      coeff[ size ] =  pc2;
      size++;
      index[ size ] =  cbod2.index_x();
      coeff[ size ] = -ps2;
      size++;
   }
}


void LocalLinearization::azimuth(const Azimuth* obs) const
{
   LocalPoint& sbod = PD[obs->from()];
   LocalPoint& cbod = PD[obs->to()];
   double s, d;
   bearing_distance(sbod, cbod, s, d);
   const double K = 10*R2G/d;
   const double ps = K*sin(s);
   const double pc = K*cos(s);

   // const StandPoint*  csp = static_cast<const StandPoint*>(obs->ptr_cluster()); ... unused
   // StandPoint* sp = const_cast<StandPoint*>(csp); ... unused

   double a = (obs->value() + PD.xNorthAngle() - s)*R2CC;  // rhs

   while (a > 200e4)
      a -= 400e4;
   while (a < -200e4)
      a += 400e4;
   rhs = a;          // absolute termm is in cc

   size = 0;
   if (sbod.free_xy())
   {
      if (!sbod.index_x()) sbod.index_x() = ++maxn;
      if (!sbod.index_y()) sbod.index_y() = ++maxn;
      index[ size ] = sbod.index_y();
      coeff[ size ] = -pc;
      size++;
      index[ size ] = sbod.index_x();
      coeff[ size ] = ps;
      size++;
   }
   if (cbod.free_xy())
   {
      if (!cbod.index_x()) cbod.index_x() = ++maxn;
      if (!cbod.index_y()) cbod.index_y() = ++maxn;
      index[ size ] = cbod.index_y();
      coeff[ size ] = pc;
      size++;
      index[ size ] = cbod.index_x();
      coeff[ size ] = -ps;
      size++;
   }
}


bool LocalRevision::direction(const Direction* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::distance(const Distance* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::x(const X* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  return true;
}


bool LocalRevision::y(const Y* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  return true;
}


bool LocalRevision::z(const Z* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  return true;
}


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


bool LocalRevision::s_distance(const S_Distance* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;
  if (!(*s).second.active_z()) return false;
  if (!(*s).second.test_z()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;
  if (!(*c).second.active_z()) return false;
  if (!(*c).second.test_z()) return false;

  return true;
}


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


bool LocalRevision::angle(const Angle* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->bs());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  PointData::const_iterator d = PD.find(obs->fs());
  if (d == PD.end()) return false;
  if (!(*d).second.active_xy()) return false;
  if (!(*d).second.test_xy()) return false;

  return true;
}


bool LocalRevision::xdiff(const Xdiff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::ydiff(const Ydiff* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}


bool LocalRevision::zdiff(const Zdiff* obs) const
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


bool LocalRevision::azimuth(const Azimuth* obs) const
{
  if (!obs->active()) return false;

  PointData::const_iterator s = PD.find(obs->from());
  if (s == PD.end()) return false;
  if (!(*s).second.active_xy()) return false;
  if (!(*s).second.test_xy()) return false;

  PointData::const_iterator c = PD.find(obs->to());
  if (c == PD.end()) return false;
  if (!(*c).second.active_xy()) return false;
  if (!(*c).second.test_xy()) return false;

  return true;
}
