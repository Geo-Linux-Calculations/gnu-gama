/*
    C++ Matrix/Vector templates (GNU Gama / matvec)
    Copyright (C) 2002, 2007, 2012  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Matrix/Vector template library.

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

#ifndef GNU_gama_gMatVec_Jacobian___h__
#define GNU_gama_gMatVec_Jacobian___h__


#include <matvec/transmat.h>
#include <matvec/mat.h>
#include <matvec/vec.h>


namespace GNU_gama {

  /** \brief Jacobian
   *
   *  Template C++ class computes Jacobian matrix for the given
   *  argument of a vector function. Derivatives are numerically
   *  computed from a Lagrange polynomial of degree 2*n with
   *  equidistant arguments.
   *
   *  For example for degree 4 Lagrange's formula L4(x) goes through
   *  points y1=f(x-2h), y2=f(x-h), y3=f(x), y4=f(x+h) and y5=f(x+2h).
   *  The derivative L'4(x) = 2/24*y1 - 4/6*y2 + 4/6*y4 - 2/24*y5.
   */

  template <typename Float=double, typename Exc=Exception::matvec>
  class Jacobian {

  public:

    typedef Vec<Float, Exc> (*function)(const Vec<Float, Exc>&);
    Mat<Float, Exc> matrix;


    Jacobian(Index out, Index inp, function pf, int d=0);


    void compute(Vec<Float, Exc> x);

    void set_f(function pf);
    void set_h(Vec<Float, Exc> h);
    void set_scale(Float sc=1e-4);
    void set_degree(int n=4);

  private:

    Float d_coef(int index);

    function f;
    Index    odim, idim;
    bool     use_h;
    int      degree;
    Float    scale;
    Vec<Float, Exc>  h;
    Vec<Float, Exc>  coef;

    const Float Abs(const Float x) const { return (x >= Float(0)) ? x : -x ; }
  };


  template <typename Float, typename Exc>
  Jacobian<Float, Exc>::Jacobian(Index out, Index inp, function pf, int d)
    : matrix(out, inp)
  {
    f    = pf;
    odim = out;
    idim = inp;

    Vec<Float, Exc> dh(inp);
    for (Index i=1; i<=inp; i++) dh(i) = 1;
    h     = dh;
    use_h = false;
    set_degree(d);
    set_scale();
  }


  template <typename Float, typename Exc>
  void Jacobian<Float, Exc>::set_f (function pf)
  {
    f = pf;
  }


  template <typename Float, typename Exc>
  void Jacobian<Float, Exc>::set_h (Vec<Float, Exc> dh)
  {
    h     = dh;
    use_h = true;
  }


  template <typename Float, typename Exc>
  void Jacobian<Float, Exc>::set_scale (Float sc)
  {
    scale = sc;
    use_h = false;
  }


  template <typename Float, typename Exc>
  void Jacobian<Float, Exc>::set_degree(int n)
  {
    if (n < 2) n = 4;

    degree = (n/2)*2;
    coef.reset(degree);
    for (int N=degree/2, i=1; i<=N; i++) coef(i) = d_coef(i);
  }


  template <typename Float, typename Exc>
  void Jacobian<Float, Exc>::compute(Vec<Float, Exc> x)
  {
    Float tx, dh, c;

    for (Index j=1; j<=idim; j++)
      {
        tx = x(j);
        dh = tx + ( use_h ? h(j) : scale*(Float(1.0) + this->Abs(x(j))) );
        dh = dh - tx;

        c    = degree/2;
        x(j) = tx - c*dh;     Vec<Float, Exc> d( f(x) );
        x(j) = tx + c*dh;     Vec<Float, Exc> e( f(x) );
        d   -= e;
        d   *= coef(1);

        for (int M=degree/2, n=2; n<=M; n++)
          {
            c--;
            x(j) = tx - c*dh;     Vec<Float, Exc> a( f(x) );
            x(j) = tx + c*dh;     Vec<Float, Exc> b( f(x) );

            a -= b;
            a *= coef(n);
            d += a;
          }

        d   *= Float(1.0)/dh;
        x(j) = tx;

        for (Index i=1; i<=odim; i++) matrix(i,j) = d(i);
      }
  }


  /* Coefficients of derivatives of Lagrange polynomial of the given degree
   *
   * index  :        1          2         3   4         5     6          7
   * ------------------------------------------------------------------------
   * L'2(x) =    -1/2*y1             + 1/2*y3
   * L'4(x) =   +2/24*y1    -4/6*y2       +4/6*y4  -2/24*y5
   * L'6(x) = -12/720*y1 +18/120*y2 -36/48*y3 +36/48*y5 -18/120*y6 +12/720*y7
   * L'8(x) = ......
   *     ......
   * ------------------------------------------------------------------------
   */

  template <typename Float, typename Exc>
  Float Jacobian<Float, Exc>::d_coef(int index)
    {
      int N=degree + 1;
      int M=degree/2 + 1;

      Float p=1, q=1;
      for (int i=1; i<=N; i++)
        {
          if (i != index)
            {
              q *= index - i;
              if (i != M)  p *= M - i;
            }
        }

      return p/q;
    }


}   // namespace GNU_gama

#endif





