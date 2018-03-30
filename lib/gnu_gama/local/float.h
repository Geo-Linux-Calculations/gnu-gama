/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Float_h
#define gama_local_Float_h

#include <cmath>

namespace GNU_gama { namespace local {

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define R2G  200.0/M_PI
#define G2R  M_PI/200.0
#define R2CC 200.0E4/M_PI
#define CC2R M_PI/200.0E4

}}   // namespace GNU_gama::local

#endif








