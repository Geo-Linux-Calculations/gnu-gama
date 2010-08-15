/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.

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

#include <gnu_gama/rand.h>

#ifndef _MSC_VER
using namespace std;
#endif

namespace GNU_gama {

long IRAND55::ia[56];
long IRAND55::jrand = 0;

static const long C1000000000 = 1000000000;

void IRAND55::IN55(long seed)
{
   ia[55] = seed;
   long j = seed;
   long k = 1;

   for (long ii, i=1; i<=54; i++)
      {
         ii = 21*i % 55;
         ia[ii] = k;
         k = j - k;
         if (k < 0) k += C1000000000;
         j = ia[ii];
      }

   IRN55();
   IRN55();
   IRN55();
   jrand = 1;
}

void IRAND55::IRN55()
{
   long i, j;
   for (i=1; i<=24; i++)
      {
         j = ia[i] - ia[i+31];
         if (j < 0) j += C1000000000;
         ia[i] = j;
      }
   for (i=25; i<=55; i++)
      {
         j = ia[i] - ia[i-24];
         if (j < 0) j += C1000000000;
         ia[i] = j;
      }
}

float Rand_U()
{
   if (IRAND55::jrand == 0) IRAND55::IN55();

   if (IRAND55::jrand > 55)
      {
         IRAND55::IRN55();
         IRAND55::jrand = 1;
      }

   return float(IRAND55::ia[IRAND55::jrand++])*1.0e-9;
}

/*
   Ratio method for normal deviates

   Algorithm R at The Art of Computer Programming by DEK,
   Addison-Wesley Publishing Company, 2nd ed., 1981, vol. 2,
   ISBN 0-201-03822-6, pp. 125-127.
*/

float Rand_N()
{
   static bool start = true;
   static float C1, C2, C3;
   if (start)
      {
         C1 = sqrt(8.0/exp(1.0));
         C2 = 4*exp(0.25);
         C3 = 4*exp(-1.35);
         start = false;
      }

   float U, V, X, X2;

   for (;;)
      {
         do
            U = Rand_U();
         while (!U);
         V = Rand_U();
         X = C1*( V - 0.5)/U;
         X2 = X*X;
         if (X2 <= 5.0 - C2*U)  return X;
         if (X2 >= C3/U + 1.4)  continue;
         if (X2 <= -4.0*log(U)) return X;
      }
}

// ---------------------------------------------------------------

void Comb::reset(int pn, int pk)
{
   delete[] cmb;
   if (pk > pn || pn < 1 || pk < 1)
      {
         n_ = k_ = k__ = 0;
         cmb = 0;
         return;
      }
   n_ = pn;
   k_ = k__ = pk;
   cmb = new int[pk];
   c = cmb - 1;
   begin();
}


void Comb::begin()
{
   k_ = k__;
   for (int i=1; i<=k__; i++) c[i] = i;
}

void Comb::next()
{
   if (k_ == 0) return;

   if (c[k_] < n_)
      {
         c[k_]++;
         return;
      }

   for (int i=k_; i>1; i--)
      if (c[i-1] < n_-k_+i-1)
         {
            c[i-1]++;
            for (int j=i; j<=k_; j++)
               c[j] = c[i-1] + j - i + 1;
            return;
         }

   k_ = 0;
}

}      /* namespace GNU_gama */







