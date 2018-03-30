/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2014, 2018  Ales Cepek <cepek@gnu.org>

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
   along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file unsigned.h
 * \brief Helper functions replacing tests like "k < 0" where k may be
 * unsigned and the condition is always true aand produces compiler warnings
 * (similarly "k >= 0")
 *
 * \author Ales Cepek
 */

#ifndef GNU_gama_matvec_unsigned_h_UNSIGNED_h
#define GNU_gama_matvec_unsigned_h_UNSIGNED_h

template<typename T> inline bool isNegative(T i) { return i < 0; }

template<> inline bool isNegative<unsigned short>(unsigned short) { return false; }
template<> inline bool isNegative<unsigned int  >(unsigned int  ) { return false; }
template<> inline bool isNegative<unsigned long >(unsigned long ) { return false; }

template <typename T> inline bool isNonNegative(T i) { return !isNegative(i); }

#endif
