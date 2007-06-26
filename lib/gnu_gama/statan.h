/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

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

/*
 *  $Id: statan.h,v 1.2 2007/06/26 15:04:09 cepek Exp $
 */

#ifndef GNU_gama___gnu_gama____StatAn_h
#define GNU_gama___gnu_gama____StatAn_h

#include <cmath>

namespace GNU_gama {

float Student(float alfa, int N);
/*
  For the given probability and number of degrees of fredom computes
  critical value of Student's distribution

   alfa - probability for which critical values of Student's
          distribution is computed; value of parameter alfa must be in
          interval (0, 1); function doesn;t check the value of the
          parameter

   N    - degrees of freedom */


double Normal(double alfa);
/*
   For given probability computes critical value of normalized normal
   distribution N(0,1).

   alfa - probability for which critical values of Normal distribution
         is computed; value of parameter alfa must be in interval (0,
         1); function doesn;t check the value of the parameter */


void NormalDistribution(double x, double& D, double& f);
/*
  Function computes in the given point x value D(x) of distributive
  normalized normal distribution and the value of its density
  function.

   x - argument
   D - cumulative distribution
   f - probability density function */


float KSprob(float);
/*
   Kolmogorov-Smirnov probability function
*/

template <typename Float, typename FloatF, typename FloatD, typename FloatP>
void KStest(Float Data[], int n, FloatF (*Func)(FloatF), 
            FloatD& ks, FloatP& prob)
/*
   Kolmogorov-Smirnov test
   Based on the source given in "Numerical Recipes in C", (2nd ed.,
   Cambridge University Press, 1992, ISBN 0 521 43108 5, p. 625)

   Func   user-supplied cumulative probability distribution function
   ks     K-S statistic
   prob   significance level
*/
{
   using namespace std;

   sort(Data, Data+n);

   const float  float_n = n;
   float Fa = 0, Fb, Fi, dl, du, dt;
   float d = 0;
   for (int i=0; i<n;)
   {
      Fi = Func(Data[i]);
      Fb = ++i/float_n;
      dl = fabs(Fa - Fi);
      du = fabs(Fb - Fi);
      dt = dl > du ? dl : du;
      Fa = Fb;
      if (dt > d) d = dt;
   }

   const float sn = sqrt(float_n);
   prob = KSprob((sn + 0.12 + 0.11/sn)*d);
   ks = d;
}

float Chi_square(float probability, int degrees_of_freedom);
/*
  For the given probability and degrees of freedom computes critical
  value of Chi-square distribution.
*/

}      /* namespace GaMaLib */

//---------------------------------------------------------------------------
#endif
