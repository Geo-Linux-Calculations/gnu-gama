/* GNU Gama -- testing Acord2 approximate coordinates
   Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#define A2G_ACORD2
//#define A2G_ACORD
//#define A2G_DEBUG

#ifndef A2G_H
#define A2G_H

#include <string>
#include <gnu_gama/local/network.h>

class A2G {
public:
  A2G();
  ~A2G();

  int read(std::string);
  void set_output_dir(std::string);
  void set_input_dir(std::string);

  void set_ang_right_handed() { ang_right_handed_ = true;  }
  void set_ang_left_handed()  { ang_right_handed_ = false; }

  using CS = GNU_gama::local::LocalCoordinateSystem::CS;
  void set_local_coordinate_system(CS cs) { locos_ = cs; }
  bool right_handed_coordinates() const
      {
        return locos_ < CS::NE;
      }

    bool left_handed_coordinates () const
      {
        return locos_ > CS::WS;
      }

private:
  std::string file_, output_dir_, input_dir_;

  GNU_gama::local::LocalNetwork network_;
  GNU_gama::local::PointData    points_;

  // various adjustment parameters
  double sigma_apr_ = 10.0;
  double conf_pr_   = 0.95;
  double tol_abs_   = 1e3;
  double distance_stdev_  = 5.00;
  double direction_stdev_ = 10.0;
  double angle_stdev_     = 10.0;
  double azimuth_stdev_   = 10.0;

  // input data codes (single characters)
  const char fixed_point_ = '!';   // fixed point with gived xy
  const char free_point_  = '?';   // adjusted point without xy
  const char obs_         = '(';   // <obs> cluster
  const char obs_end_     = ')';   // </obs>
  const char direction_   = '>';   // direction
  const char distance_    = '-';   // distance
  const char dist_dir_    = '=';   // distance + direction
  const char angle_       = '&';   // angle
  const char azimuth_     = '^';   // azimuth
  const char comment_     = '#';   // comment line

  bool ang_right_handed_ = true;
  CS locos_ = CS::EN;  
  
  //double t11, t12, t21, t22;

  struct xy {
    double x, y;
    xy(double X, double Y) : x(X), y(Y) {}
  };

};

#endif
