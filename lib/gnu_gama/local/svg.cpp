/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This program is free software; you can redistribute it and/or modify
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


#include <gnu_gama/local/svg.h>
#include <sstream>

using namespace GNU_gama::local;

std::string GamaLocalSVG::string() const
{
  std::ostringstream stream;
  write(stream);
  return stream.str();
}

void GamaLocalSVG::write(std::ostream& str) const
{
  str << "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n";
}
