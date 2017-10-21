/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

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

#include <string>
#include <gnu_gama/g3/g3_parameter.h>

#ifndef GNU_gama_g3_point_h_gnugamag3pointh_gnu_gama_g3point
#define GNU_gama_g3_point_h_gnugamag3pointh_gnu_gama_g3point


namespace GNU_gama {  namespace g3 {

  class Model;

  /** g3 point class.  */

  class Point : public ParXML {
  public:

    typedef std::string   Name;
    typedef Model         Common;

    Name    name;
    Common* common;

    Linear    N;
    Linear    E;
    Linear    U;
    Linear    height;
    Parameter geoid;
    Parameter dB;
    Parameter dL;

    const Parameter& B;
    const Parameter& L;
    const Parameter& H;
    const Parameter& X;
    const Parameter& Y;
    const Parameter& Z;

    Point();
    Point(const Point&);
    Point& operator=(const Point&);

    void set_unused();
    void set_fixed_horizontal_position();
    void set_fixed_height();
    void set_fixed_position();
    void set_free_horizontal_position();
    void set_free_height();
    void set_free_position();
    void set_constr_horizontal_position();
    void set_constr_height();
    void set_constr_position();

    bool active() const { return !unused(); }
    bool unused() const;
    bool fixed_horizontal_position() const;
    bool fixed_height() const;
    bool fixed_position() const;
    bool free_horizontal_position() const;
    bool free_height() const;
    bool free_position() const;
    bool constr_horizontal_position() const;
    bool constr_height() const;
    bool constr_position() const;

    void set_blh(double, double, double);
    void set_xyz(double, double, double);
    void set_height(double);
    void set_geoid(double);

    bool has_position() const { return has_xyz_ || has_blh_; }
    bool has_xyz()      const { return has_xyz_;    }
    bool has_blh()      const { return has_blh_;    }
    bool has_height()   const { return has_height_; }
    bool has_geoid()    const { return has_geoid_;  }

    bool   test_model_height() const;
    double model_height()      const;

    double X_dh(double dh) const { return X() + r13*dh; }
    double Y_dh(double dh) const { return Y() + r23*dh; }
    double Z_dh(double dh) const { return Z() + r33*dh; }

    void write_xml(std::ostream&);

  private:

    double x_transform(double n, double e, double u);
    double y_transform(double n, double e, double u);
    double z_transform(double n, double e, double u);

    void   set_diff_XYZ(double dx, double dy, double dz);
    double diff_N() const;
    double diff_E() const;
    double diff_U() const;

    enum {
      unused_          = 0,
      fixed_h_pos_     = 1,
      fixed_height_    = 2,
      fixed_position_  = fixed_h_pos_ + fixed_height_,
      free_h_pos_      = 4,
      free_height_     = 8,
      free_position_   = free_h_pos_  + free_height_,
      constr_h_pos_    = 16 + free_position_,
      constr_height_   = 32 + free_height_,
      constr_position_ = constr_h_pos_ + constr_height_,
      h_pos_           = fixed_h_pos_  + free_h_pos_,
      hheight_         = fixed_height_ + free_height_,
      position_        = h_pos_ + hheight_
    };

    friend class Model;
    Parameter  B_, L_, H_, X_, Y_, Z_;

    void point_copy(const Point&);
    void transformation_matrix(double b, double l);

    // rotation matrix of transformation from local to global
    // Cartesian coordinates (NEU --> XYZ)

    double   r11, r12, r13,   r21, r22, r23,   r31, r32, r33;
    double   dX, dY, dZ ;
    bool     has_xyz_, has_blh_, has_height_, has_geoid_;

    double   cnn, cne, cnu, cee, ceu, cuu;
    void     set_cov_neu();

    double   cxx, cxy, cxz, cyy, cyz, czz;
    void     set_cov_xyz();
  };


}}

#endif
