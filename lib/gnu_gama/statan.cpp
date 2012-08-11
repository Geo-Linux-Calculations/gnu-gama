/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 1999, 2012  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <gnu_gama/statan.h>
#include <gnu_gama/radian.h>
#include <cfloat>

using namespace std;

namespace GNU_gama {

float Student(float palfa, int N)
{
   float alfa = palfa;
   if(alfa > 0.5) alfa=1.0-alfa;
   alfa *= 2;

   if(palfa == 0.5) return 0;

   if (N <= 1)
   {
      float a = M_PI/2*alfa;
      float stu_ = cos(a)/sin(a);
      if (palfa > 0.5) stu_ = -stu_;
      return stu_;
   }

   if (N <= 2)
   {
      float stu_ = sqrt(2.0/(alfa*(2.0-alfa))-2.0);
      if (palfa > 0.5) stu_ = -stu_;
      return stu_;
    }

   float r = N;
   float a = 1.0/(r-0.5);
   float b = 48.0/(a*a);
   float c = ((20700.0*a/b-98.0)*a-16.0)*a+96.36;
   float d = ((94.5/(b+c)-3.0)/b+1.0)*sqrt(M_PI/2*a)*r;
   float x = d*alfa, xx = 2.0/r;
   float y = pow(x,xx);

   if (y > a+0.05)
   {
      x = -Normal(0.5*alfa);
      y = x*x;
      if (N < 5) c += 0.3*(r-4.5)*(x+0.6);
      c = (((0.05*d*x-5.0)*x-7.0)*x-2.0)*x+b+c;
      y = (((((0.4*y+6.3)*y+36.0)*y+94.5)/c-y-3.0)/b+1.0)*x;
      y = a*y*y;
      if (y <= 0.002)
         y = 0.5*y*y+y;
      else
         y = exp(y)-1.0;
      float stu_ = sqrt(r*y);
      if (palfa > 0.5) stu_ = -stu_;
      return stu_;
   }

   y = ((1.0/(((r+6.0)/(r*y)-0.089*d-0.822)*(r+2.0)*3.0)+0.5/(r+4.0))
        *y-1.0)*(r+1.0)/(r+2.0)+1.0/y;
   float stu_ = sqrt(r*y);
   if (palfa > 0.5) stu_ = -stu_;
   return stu_;

}

double Normal(double alfa)
{
   double a = alfa;
   if (a > 0.5)
      a = 1 - a;

   double z = sqrt(-2*log(a));
   z -= ((7.47395*z+494.877)*z+1637.72)/(((z+117.9407)*z+908.401)*z+659.935);

   double g, f;
   NormalDistribution(z, f, g);
   f = 1 - f;
   f = (f-a)/g;
   double norm_  = (((((0.75*z*z+0.875)*f+z)*z+0.5)*f/3+0.5*z)*f+1)*f+z;
   if (alfa > 0.5) norm_ = -norm_;
   return norm_;
}

void NormalDistribution(double x, double& D, double& f)
{
   const double maxd = 1e30;
   const double mind = 1e-30;
   double s;

   f=0.3989422804014327;
   if (x == 0)
   {
      D = 0.5;
      return;
   }

   bool typv = (x <= 0);
   double b = x; if (b < 0) b = -b;

   double x2 = x*x;
   f *= exp(-0.5*x2);
   double r = f/b;

   if (r <= 0)
   {
      if (typv)
         D = 0;
      else
         D = 1;
      return;
   }

   if( typv)
      r = 2.32;
   else
      r = 3.5;

   if (b - r <= 0)
   {
      double y = f*b;
      D = y;
      s = y;
      r = 3;
      for (;;)
      {
         y *= x2/r;
         D += y;
         if (D-s <= 0) break;
         s = D;
         r += 2;
      }
      if (typv)
         D = 0.5-D;
      else
         D += 0.5;

      return;
   }

   double a1 = 2;
   double a2 = 0;
   double t  = x2+3;
   double p1 = f;
   double q1 = b;
   double p2 = (t-1)*f;
   double q2 = t*b;
   r  = p1/q1;
   D  = p2/q2;
   if (!typv)
   {
      r = 1 - r;
      D = 1 - D;
   }

   do
   {
      t  += 4;
      a1 -= 8;
      a2 += a1;
      s  = a2*p1+t*p2;
      p1 = p2;
      p2 = s;
      s  = a2*q1+t*q2;
      q1 = q2;
      q2 = s;
      if (q2 > maxd)
      {
         q1 *= mind;
         q2 *= mind;
         p1 *= mind;
         p2 *= mind;
      }
      s = r;
      r = D;
      D = p2/q2;
      if (!typv)
         D = 1 - D;
   } while (std::abs(r - D) > DBL_EPSILON);

   if (s - D) return;
   if (typv) D = 0; else D = 1;
}

float KSprob(float lambda)
{
   const float eps = 1.0e-8;
   const float C = -2*lambda*lambda;
   float term;
   float j = 1;
   float sum = 0;

   do {
      term = exp(C*j*j);  j++;  sum += term;
      term = exp(C*j*j);  j++;  sum -= term;
   } while (term > eps*sum && j < 100);

   return j<100 ? 2*sum : 1;
}

float Chi_square(float p, int n)
{
   if (n < 2)
   {
      float a = Normal(0.5*p);
      return a*a;
   }
   else if (n == 2)
      return -2.0*log(p);

   float f=n;
   float f1=1.0/f;
   float t=Normal(p);
   float f2=sqrt(f1)*t;

   float z;
   if(n < (2+int(4*fabs(t))))
      z=(((((((0.1565326e-2*f2 + 0.1060438e-2)*f2 - 0.6950356e-2)*f2 -
      0.1323293e-1)*f2 + 0.2277679e-1)*f2 - 0.8986007e-2)*f2 - 0.1513904e-1)
      *f1+((((((0.253001e-2 - 0.1450117e-2*f2)*f2 + 0.5169654e-2)*f2 -
      0.1153761e-1)*f2 + 0.1128186e-1)*f2 + 0.2607083e-1)*f2 - 0.2237368))*
      f1+(((((0.9780499e-4*f2 - 0.8426812e-3)*f2 + 0.312558e-2)*f2 -
      0.8553069e-2)*f2 + 0.1348028e-3)*f2 + 0.4713941)*f2 + 1.0000886;
   else
      z=(((0.1264616e-1 - 0.1425296e-1*f2)*f1+(((0.1400483e-1 -
      0.588609e-2*f2)*f2 - 0.1091214e-1)*f2 - 0.2304527e-1))*f1 + (((((
      0.3135411e-2 - 0.2728484e-3*f2)*f2 - 0.9699681e-2)*f2 + 0.1316872e-1)*
      f2+0.2618914e-1)*f2-0.2222222))*f1+(((((0.5406674e-4*f2
      +0.3483789e-4)*f2-0.7274761e-3)*f2+0.3292181e-2)*f2-0.8729713e-2)
      *f2*f2+0.4714045)*f2+1.0;

   return f*z*z*z;
}

}   // namespace GNU_gama::local
