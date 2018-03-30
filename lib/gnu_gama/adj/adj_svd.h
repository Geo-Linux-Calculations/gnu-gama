/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999, 2006, 2018  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_Gama_gnu_gama_gnu_gama_GaMa_OLS_svd_h
#define GNU_Gama_gnu_gama_gnu_gama_GaMa_OLS_svd_h

#include <gnu_gama/adj/adj_basefull.h>
#include <matvec/svd.h>
#include <cmath>

namespace GNU_gama {

  template <typename Float, typename Index, typename Exc>
  class AdjSVD : public AdjBaseFull<Float, Index, Exc> {

    SVD<Float, Index, Exc> svd;

  public:
    AdjSVD() {}
    AdjSVD(const Mat<Float, Index, Exc>& A,
	   const Vec<Float, Index, Exc>& b)
      : AdjBaseFull<Float, Index, Exc>(A, b) {}

    void reset(const Mat<Float, Index, Exc>& A,
               const Vec<Float, Index, Exc>& b)
    {
      AdjBaseFull<Float, Index, Exc>::reset(A, b);
      svd.reset(A);
    }

    Index defect() { return svd.nullity(); }
    bool  lindep(Index i) { return svd.lindep(i); }

    Float q_xx(Index i, Index j)
    {
      if(!this->is_solved) solve();
      return svd.q_xx(i, j);
    }
    Float q_bb(Index i, Index j)
    {
      if (!this->is_solved) solve();
      return svd.q_bb(i, j);
    }
    Float q_bx(Index i, Index j)
    {
      if (!this->is_solved) solve();
      return svd.q_bx(i, j);
    }

    void min_x()   {  svd.min_x(); }
    void min_x(Index n, Index x[]) { svd.min_x(n, x); }

    Float cond();
    void solve();

  };

  // ...................................................................

  template <typename Float, typename Index, typename Exc>
  void AdjSVD<Float, Index, Exc>::solve()
  {
    using namespace GNU_gama;
    using namespace std;

    if (this->is_solved) return;

    svd.reset(*this->pA);
    svd.solve(*this->pb, this->x);

    this->r  = *this->pA * this->x;
    this->r -= *this->pb;

    this->is_solved = true;
  }

  template <typename Float, typename Index, typename Exc>
  Float AdjSVD<Float, Index, Exc>::cond()
  {
    const Vec<Float, Index, Exc>& W = svd.SVD_W();

    Float  f, sv_min=W(1), sv_max=W(1);

    for (Index i=2; i<=W.dim(); i++)
      if (!svd.lindep(i))
        {
          f = W(i); if (f < 0) f = -f;

          if (f < sv_min) sv_min = f;
          if (f > sv_max) sv_max = f;
        }

    return sv_max/sv_min;
  }

}
#endif
