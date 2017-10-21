/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_e3_gnu_gama_e3_gnugamae3_h
#define GNU_gama_e3_gnu_gama_e3_gnugamae3_h

#include <iostream>

namespace GNU_gama {

  struct E_3 {

    double e1, e2, e3;

    E_3() {}
    E_3(double a, double b, double c) : e1(a), e2(b), e3(c) {}

    void   operator += (const E_3&);
    void   operator -= (const E_3&);
    void   operator *= (double);

    void   set(double, double, double);
    void   add(double, double, double);
    void   sub(double, double, double);

    void   cross(const E_3&, const E_3&);
    double dot  (const E_3&) const;

  };

  double angle(const E_3&, const E_3&);



  struct R_3 {    // rotation matrix 3x3

    double  r11, r12, r13;
    double  r21, r22, r23;
    double  r31, r32, r33;

    void set_rotation(double b, double l);

    void rotation(const E_3&, E_3&) const;     // dif_NEU --> dif_XYZ
    void inverse (const E_3&, E_3&) const;     // dif_XYZ --> dif_NEU

  };

}

#endif
