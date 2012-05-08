/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003, 2005  Ales Cepek <cepek@gnu.org>

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

#include <matvec/covmat.h>
#include <gnu_gama/visitor.h>
#include <gnu_gama/model.h>
#include <gnu_gama/g3/g3_point.h>


#ifndef GNU_gama__g3_observation_h_gnugamag3obs_baseh___gnu_gama_g3obs
#define GNU_gama__g3_observation_h_gnugamag3obs_baseh___gnu_gama_g3obs


namespace GNU_gama {  namespace g3 {


  /** g3 observation base class. */

  class Observation :
    public GNU_gama::Observation<Cluster<Observation>, GNU_gama::CovMat<> >
  {
  public:

  };


  /** g3 helper class */

  class FromTo {
  public:

    FromTo() : from_dh(0), to_dh(0) {}

    Point::Name  from;
    Point::Name  to;

    double from_dh, to_dh;
  };


  /** g3 helper class. */

  class Value {
    double value;
  public:

    Value() {}
    Value(double d) : value(d) {}

    double obs() const   { return value; }
    void   set(double d) { value = d;    }
  };


  /** g3 distance class. */

  class Distance : public Accept<Distance, Observation>,
                   public FromTo, public Value {
  public:

    Distance() {}
    Distance(double d) : Value(d) {}

    int dimension() const { return 1; }
  };


  /** g3 zenith angle class. */

  class ZenithAngle : public Accept<ZenithAngle, Observation>,
                      public FromTo, public Value {
  public:

    ZenithAngle() {}
    ZenithAngle(double d) : Value(d) {}

    int dimension() const { return 1; }
  };


  /** g3 azimuth class. */

  class Azimuth : public Accept<Azimuth, Observation>,
                  public FromTo, public Value {
  public:

    Azimuth() {}
    Azimuth(double d) : Value(d) {}

    int dimension() const { return 1; }
  };


  /** g3 vector class. */

  class Vector : public Accept<Vector, Observation>, public FromTo {
  public:

    Vector()
    {
    }
    Vector(double x, double y, double z)
    {
      dx_ = x; dy_ = y; dz_ = z;
    }

    int  dimension() const { return 3; }
    void set_dxyz(double x, double y, double z)
    {
      dx_ = x; dy_ = y; dz_ = z;
    }

    double dx() const { return dx_; }
    double dy() const { return dy_; }
    double dz() const { return dz_; }


  private:
    double dx_, dy_, dz_;
  };


  /** g3 observed coordinates class. */

  class XYZ : public Accept<XYZ, Observation> {
  public:

    Point::Name id;

    XYZ()
    {
    }
    XYZ(double xp, double yp, double zp)
    {
      x_ = xp; y_ = yp; z_ = zp;
    }

    int  dimension() const { return 3; }
    void set_xyz(double xp, double yp, double zp)
    {
      x_ = xp; y_ = yp; z_ = zp;
    }

    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }


  private:
    double x_, y_, z_;
  };


  /** g3 height difference class. */

  class HeightDiff : public Accept<HeightDiff, Observation>,
                     public FromTo, public Value {
  public:

    HeightDiff() {}
    HeightDiff(double d) : Value(d) {}

    int dimension() const { return 1; }
  };


  /** g3 observed height class. */

  class Height : public Accept<Height, Observation>, public Value {
  public:

    Point::Name id;

    Height() {}
    Height(double d) : Value(d) {}

    int dimension() const { return 1; }
  };


  /** g3 horizontal angle class. */

  class Angle : public Accept<Angle, Observation>, public Value {
  public:

    Point::Name from;
    Point::Name left;
    Point::Name right;

    double from_dh, left_dh, right_dh;

    Angle() {}
    Angle(double d) : Value(d) {}

    int dimension() const { return 1; }
  };

}}

#endif
