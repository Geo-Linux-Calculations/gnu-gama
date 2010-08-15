/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999, 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_Gama__gnu_gama__gnu_gama_GaMa_OLS_svd_h
#define GNU_Gama__gnu_gama__gnu_gama_GaMa_OLS_svd_h

#include <gnu_gama/adj/adj_basefull.h>
#include <matvec/svd.h>
#include <cmath>

namespace GNU_gama {

template <typename Float, typename Exc>
class AdjSVD : public AdjBaseFull<Float, Exc> {

  SVD<Float, Exc> svd;

public:
  AdjSVD() {}
  AdjSVD(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b)
    : AdjBaseFull<Float, Exc>(A, b) {}

  void reset(const Mat<Float, Exc>& A, const Vec<Float, Exc>& b)
    {
      AdjBaseFull<Float, Exc>::reset(A, b);
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

template <typename Float, typename Exc>
void AdjSVD<Float, Exc>::solve()
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

template <typename Float, typename Exc>
Float AdjSVD<Float, Exc>::cond()
{
  const Vec<Float, Exc>& W = svd.SVD_W();

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









