/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *  $Id: rand.h,v 1.1 2001/12/07 12:22:42 cepek Exp $
 */

#ifndef GaMaLib_RanGen_h
#define GaMaLib_RanGen_h

#include <cmath>

namespace GaMaLib {

/*
   IRAND55 random number generator
   
   Based on the code form The Art of Computer Programming by D.E.K., 
   Addison-Wesley Publishing Company, 2nd ed., 1981, vol. 2,
   ISBN 0-201-03822-6, pp. 171-172.
*/

class IRAND55 {
protected:
   static long ia[56];
   static long jrand;
   
public:   
   static void IN55(long seed=987654321);
   static void IRN55();

   friend float Rand_U();    // Uniform random numbers <0, 1>
   friend float Rand_N();    // Normal distribution N(0, 1)
};


// K-combinations of N positive integers <1, N>

class Comb
{
   int n_, k_, k__;
   int* cmb;
   int* c;
   
public:
   const int& n;
   const int& k;
   
   Comb(int pn=0, int pk=0) : cmb(0), n(n_), k(k_) { reset(pn, pk); }
   ~Comb() { delete[] cmb; }
   void reset(int pn=0, int pk=0);
   void begin();
   void next();
   int operator[](int i) const { return c[i]; }   // i=1,2,3...
   void operator++(int) { next(); }
   void operator++()    { next(); }
};

};      /* namespace GaMaLib */

#endif
//----------------------------------------------------------------------








