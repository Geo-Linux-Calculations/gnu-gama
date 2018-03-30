/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2004, 2006, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_AdjBase_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_AdjBase_h

#include <matvec/matvec.h>

namespace GNU_gama {

  /** \brief Base adjustment class. */

  template <typename Float, typename Index, typename Exc=Exception::matvec>
  class AdjBase {

  public:

    virtual ~AdjBase() {}

    /* unknown parameters and adjusted residuals */

    virtual const Vec<Float,Index,Exception::matvec>& unknowns()   = 0;
    virtual const Vec<Float,Index,Exception::matvec>& residuals()  = 0;

    virtual Float sum_of_squares()     = 0;
    virtual Index defect()             = 0;

    virtual Float q_xx(Index, Index)   = 0;   // weight coefficient (xi,xj)
    virtual Float q_bb(Index, Index)   = 0;   //                    (bi,bj)
    virtual Float q_bx(Index, Index)   = 0;   //                    (bi,xj)

    virtual bool lindep(Index)         = 0;   // linearly dependent column
    virtual void min_x()               = 0;   // all x used in regularization
    virtual void min_x(Index, Index[]) = 0;   // subset of x for regularization

    virtual Float cond() { return Float(); }  // 0 if not available

    // weight coefficients for the particular solution (if defined)

    virtual Float q0_xx(Index i, Index j)  { return q_xx(i,j); }

  };

}

#endif

