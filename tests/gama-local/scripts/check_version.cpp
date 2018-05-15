/*
  GNU Gama -- testing version numbers defined in configure.ac and version.cpp
  Copyright (C) 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library
  
  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <config.h>
#include <gnu_gama/version.h>

int main()
{
  std::string ver = GNU_gama::GNU_gama_version();  // lib/gnu_gama/version.cpp

  if (ver != std::string(VERSION))  /* configure.ac */
    {
      std::cout
        << "\nPackage version " << VERSION << ", defined in configure.ac, "
        << "\nis different from version "
        << ver << ", defined in lib/gnu_gama/version.cpp\n\n";

      return 1;
    }

  return 0;
}
