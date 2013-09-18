/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>
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

/** \file local_revision.h
 * \brief #GNU_gama::local::LocalRevision class header file
 *
 * \author Ales Cepek
 * \author Vaclav Petras (acyclic visitor pattern)
 */

#ifndef gama_local__LocalRevision______GNU_gama_local___Local___Revision_____
#define gama_local__LocalRevision______GNU_gama_local___Local___Revision_____

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/observation.h>

namespace GNU_gama { namespace local {

  /** Local observation visitor which performs observation revision.
   *
   * If revision
   */
  class LocalRevision : public AllObservationsVisitor
    {

    public:

      LocalRevision(const PointData& pd) : PD(pd) {}

      void  visit(Direction *element)  { if (!direction(element))  element->set_passive(); }
      void  visit(Distance *element)   { if (!distance(element))   element->set_passive(); }
      void  visit(Angle *element)      { if (!angle(element))      element->set_passive(); }
      void  visit(H_Diff *element)     { if (!h_diff(element))     element->set_passive(); }
      void  visit(S_Distance *element) { if (!s_distance(element)) element->set_passive(); }
      void  visit(Z_Angle *element)    { if (!z_angle(element))    element->set_passive(); }
      void  visit(X *element)          { if (!x(element))          element->set_passive(); }
      void  visit(Y *element)          { if (!y(element))          element->set_passive(); }
      void  visit(Z *element)          { if (!z(element))          element->set_passive(); }
      void  visit(Xdiff *element)      { if (!xdiff(element))      element->set_passive(); }
      void  visit(Ydiff *element)      { if (!ydiff(element))      element->set_passive(); }
      void  visit(Zdiff *element)      { if (!zdiff(element))      element->set_passive(); }
      void  visit(Azimuth *element)    { if (!azimuth(element))    element->set_passive(); }

    private:

      const PointData& PD;

      bool direction  (const Direction  *obs) const;
      bool distance   (const Distance   *obs) const;
      bool angle      (const Angle      *obs) const;
      bool h_diff     (const H_Diff     *obs) const;
      bool s_distance (const S_Distance *obs) const;
      bool z_angle    (const Z_Angle    *obs) const;
      bool x          (const X          *obs) const;
      bool y          (const Y          *obs) const;
      bool z          (const Z          *obs) const;
      bool xdiff      (const Xdiff      *obs) const;
      bool ydiff      (const Ydiff      *obs) const;
      bool zdiff      (const Zdiff      *obs) const;
      bool azimuth    (const Azimuth    *obs) const;
    };

}}

#endif





