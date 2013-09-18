/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001, 2011  Ales Cepek <cepek@fsv.cvut.cz>
                  2011  Vaclav Petras <wenzeslaus@gmail.com>
                  2013  Ales Cepek <cepek@gnu.org>

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

/** \file local_linearization.h
 * \brief #GNU_gama::local::LocalLinearization class header file
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef gama_local__LocalLinearization__GNU_gama_local_Local_Linearization
#define gama_local__LocalLinearization__GNU_gama_local_Local_Linearization

#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/gamadata.h>

namespace GNU_gama { namespace local {

  /** Local linearization class implemented as a visitor. */
  class LocalLinearization : public AllObservationsVisitor
    {

    public:

      LocalLinearization(PointData& pd, double m) : max_size(6), PD(pd), maxn(0), m0(m) {}

      int  unknowns() const { return maxn; }

      void  visit(Direction *element)  { direction(element); }
      void  visit(Distance *element)   { distance(element); }
      void  visit(Angle *element)      { angle(element); }
      void  visit(H_Diff *element)     { h_diff(element); }
      void  visit(S_Distance *element) { s_distance(element); }
      void  visit(Z_Angle *element)    { z_angle(element); }
      void  visit(X *element)          { x(element); }
      void  visit(Y *element)          { y(element); }
      void  visit(Z *element)          { z(element); }
      void  visit(Xdiff *element)      { xdiff(element); }
      void  visit(Ydiff *element)      { ydiff(element); }
      void  visit(Zdiff *element)      { zdiff(element); }
      void  visit(Azimuth *element)    { azimuth(element); }

      const   long    max_size; ///< maximal number of coefficients
      mutable double  rhs;
      mutable double  coeff[6];
      mutable long    index[6];
      mutable long    size;

    private:

      PointData&           PD;
      mutable int          maxn;
      double               m0;

      void direction  (const Direction  *obs) const;
      void distance   (const Distance   *obs) const;
      void angle      (const Angle      *obs) const;
      void h_diff     (const H_Diff     *obs) const;
      void s_distance (const S_Distance *obs) const;
      void z_angle    (const Z_Angle    *obs) const;
      void x          (const X          *obs) const;
      void y          (const Y          *obs) const;
      void z          (const Z          *obs) const;
      void xdiff      (const Xdiff      *obs) const;
      void ydiff      (const Ydiff      *obs) const;
      void zdiff      (const Zdiff      *obs) const;
      void azimuth    (const Azimuth    *obs) const;
    };

}}

#endif
