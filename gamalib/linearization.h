/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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
 *  $Id: linearization.h,v 1.1 2001/12/07 12:22:42 cepek Exp $
 */

#ifndef GaMaLib_Linearization_______GaMaLib_____Linearization__
#define GaMaLib_Linearization_______GaMaLib_____Linearization__


namespace GaMaLib {

  class Observation;

  class Linearization {
  public:

    const   long    max_size;       // maximal number of coefficients
    mutable double  rhs;
    mutable double  coeff[6];
    mutable long    index[6];
    mutable long    size;

    Linearization() : max_size(6) {}
    virtual ~Linearization() {}

    virtual void linearization(const Observation*) const = 0;
  };

}

#endif


