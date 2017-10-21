/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Linearization_GNU_gama_local_Linearization_
#define gama_local_Linearization_GNU_gama_local_Linearization_


namespace GNU_gama { namespace local {

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

}}

#endif


