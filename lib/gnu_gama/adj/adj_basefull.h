/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2006, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h
#define GNU_Gama_gnu_gama_gnugama_GaMa_AdjBaseFull_h

#include <gnu_gama/adj/adj_base.h>

namespace GNU_gama {

  /** \brief Base adjustment class for full matrix solutions. */

  template <typename Float, typename Index, typename Exc>
  class AdjBaseFull : public AdjBase<Float, Index, Exception::matvec>
  {
  public:

    AdjBaseFull() : pA(0), pb(0), is_solved(false)
      {
      }

    AdjBaseFull(const Mat<Float, Index, Exc>& A,
                const Vec<Float, Index, Exc>& b)
      : pA(&A), pb(&b), is_solved(false)
      {
      }

    virtual ~AdjBaseFull()
    {
    }

    virtual void reset(const Mat<Float, Index, Exc>& A,
                       const Vec<Float, Index, Exc>& b)
    {
      pA = &A;
      pb = &b;
      is_solved = false;
    }

    const Vec<Float, Index, Exc>& unknowns()
    {
      if (!is_solved) solve();
      return x;
    }

    const Vec<Float, Index, Exc>& residuals()
    {
      if (!is_solved) solve();
      return r;
    }

    Float sum_of_squares()
    {
      const Vec<Float, Index, Exc>& res = residuals();
      return res.dot(res);
    }

    // solve() must compute vectors x, r  and set is_solved=true
    virtual void solve() = 0;


  protected:

    const Mat<Float, Index, Exc>* pA;
    const Vec<Float, Index, Exc>* pb;

    Vec<Float, Index, Exc> x;
    Vec<Float, Index, Exc> r;
    bool is_solved;

  };


}
#endif
