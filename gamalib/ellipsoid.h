/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2002  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: ellipsoid.h,v 1.2 2002/06/17 19:08:53 cepek Exp $
 */

#ifndef GaMaLib__gamalib____ellipsoid_H___________ELLIPSOID_H________
#define GaMaLib__gamalib____ellipsoid_H___________ELLIPSOID_H________

namespace GaMaLib {

  class Ellipsoid {
  public:

    double a() const { return A;  }
    double b() const { return B;  }
    double f() const { return ff; }

    double M(double b) const;
    double N(double b) const;
    double W(double b) const;
    double V(double b) const;
    double F(double b) const;

    void blh2xyz(double, double, double, double&, double&, double&) const;
    void xyz2blh(double, double, double, double&, double&, double&) const;

    void set_ab (double pa, double pb) { set_abff1( pa, pb,  0,  0); }
    void set_af (double pa, double pf) { set_abff1( pa,  0, pf,  0); }
    void set_af1(double pa, double pf) { set_abff1( pa,  0,  0, pf); }

  private:

    double A, B, ff, n, e2, e22;
    double Ime2, Ipe22, AIme2, AB;

    void set_abff1(double, double, double, double);
  };

}   // namespace GaMaLib

#endif








