/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000, 2014, 2016  Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA  02110-1301  USA
*/

#ifndef gama_local_class_LocalPoint_h
#define gama_local_class_LocalPoint_h

namespace GNU_gama { namespace local {


class LocalPoint {
public:

 LocalPoint()
   : x_(0), y_(0), z_(0),
    bxy_(false), bz_(false),
    ix_(0), iy_(0), iz_(0),
    x0_(0), y0_(0), z0_(0),
    pst_(unused_)
    {
    }
 LocalPoint(double px, double py, double pz)
    : x_(px), y_(py), z_(pz),
    bxy_(true), bz_(true),
    ix_(0), iy_(0), iz_(0),
    x0_(0), y0_(0), z0_(0),
    pst_(unused_)
    {
    }
 LocalPoint(double px, double py)
    : x_(px), y_(py), z_(0),
    bxy_(true), bz_(false),
    ix_(0), iy_(0), iz_(0),
    x0_(0), y0_(0), z0_(0),
    pst_(unused_)
    {
    }
  LocalPoint(double pz)
    : x_(0), y_(0), z_(pz),
    bxy_(false), bz_(true),
    ix_(0), iy_(0), iz_(0),
    x0_(0), y0_(0), z0_(0),
    pst_(unused_)
    {
    }


  double y() const { return y_; }        // check test_xy()
  double x() const { return x_; }
  double z() const { return z_; }        // check test_z()

  void set_xy  (double x, double y) { bxy_ = true; x_ = x; y_ = y; }
  void set_z   (double z)           { bz_  = true; z_ = z; }
  void unset_xy() { bxy_ = false; }
  void unset_z () { bz_  = false; }
  bool test_xy () const { return bxy_; }
  bool test_z  () const { return bz_;  }

  int& index_y() { return iy_; }         // indexes in project equations
  int& index_x() { return ix_; }
  int& index_z() { return iz_; }
  int index_y() const { return iy_; }
  int index_x() const { return ix_; }
  int index_z() const { return iz_; }

  void set_fixed_xy()       { pst_ &= ~active_xy_; pst_ |= xy_fixed_;     }
  void set_free_xy()        { pst_ &= ~active_xy_; pst_ |= xy_adjusted_;  }
  void set_constrained_xy() { pst_ &= ~active_xy_;
                              pst_ |= (xy_adjusted_ | xy_constrained_);   }
  void set_fixed_z()        { pst_ &= ~active_z_;  pst_ |= z_fixed_;      }
  void set_free_z()         { pst_ &= ~active_z_;  pst_ |= z_adjusted_;   }
  void set_constrained_z()  { pst_ &= ~active_z_;
                              pst_ |= (z_adjusted_  | z_constrained_);    }
  void set_unused_xy()      { pst_ &= ~active_xy_;                        }
  void set_unused_z()       { pst_ &= ~active_z_;                         }
  void set_unused()         { pst_ = unused_;                             }

  bool active_xy()      const { return pst_ & active_xy_;      }
  bool fixed_xy()       const { return pst_ & xy_fixed_;       }
  bool free_xy()        const { return pst_ & xy_adjusted_;    }
  bool constrained_xy() const { return pst_ & xy_constrained_; }
  bool fixed_z()        const { return pst_ & z_fixed_;        }
  bool free_z()         const { return pst_ & z_adjusted_;     }
  bool constrained_z()  const { return pst_ & z_constrained_;  }
  bool active_z()       const { return pst_ & active_z_;       }
  bool active()         const { return pst_ & active_;         }


  // initial values of coordinates used in the first adjustment iteration

  double x_0() const { return x0_; }
  double y_0() const { return y0_; }
  double z_0() const { return z0_; }

  void   set_xyz_0() { x0_ = x_; y0_ = y_; z0_ = z_;  }

private:

  double  x_, y_, z_;       // coordinates
  bool    bxy_, bz_;        // coordinates are/are_not defined
  int     ix_, iy_, iz_;    // indexes of unknowns in project equations
  double  x0_, y0_, z0_;    // initial values of cordinates in adjustment
  int     pst_;             // point status in adjustment

  enum
  {
    unused_          =  0,
    xy_fixed_        =  1,
    xy_adjusted_     =  2,
    xy_constrained_  =  4,
    z_fixed_         =  8,
    z_adjusted_      = 16,
    z_constrained_   = 32,
    active_xy_   = xy_fixed_  | xy_adjusted_ | xy_constrained_,
    active_z_    = z_fixed_   | z_adjusted_  | z_constrained_,
    active_      = active_xy_ | active_z_
  };

};

}}   // namespace GNU_gama::local;

#endif
