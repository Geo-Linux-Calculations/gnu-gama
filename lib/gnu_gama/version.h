/*
    GNU Gama --- Geodesy and Mapping C++ library
    Copyright (C) 1999, 2003, 2007  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_GAMA_VERSION_H___GNU_Gama_version_h___gnugamaversionh
#define GNU_GAMA_VERSION_H___GNU_Gama_version_h___gnugamaversionh

namespace GNU_gama {

  extern const char* GNU_gama_version;
  extern const char* GNU_gama_compiler;
  extern const char* GNU_gama_year;

  int version(const char* program, const char* copyright_holder);
}

#endif
