/* GNU Gama -- testing adjustment results from different algorithms
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "check_xyz.h"

using GNU_gama::local::LocalNetwork;

int main(int argc, char* argv[])
{
  if (argc != 5)
    {
      std::cout << "   #### " << argv[0] << " wrong number of arguments\n";
      return 1;
    }

  const int algorithm = alg_gso;
  std::string aconf = argv[1];
  LocalNetwork* aln = getNet(algorithm, argv[2]);
  std::string bconf = argv[3];
  LocalNetwork* bln = getNet(algorithm, argv[4]);

  double maxdiff = xyzMaxDiff(aln,bln);
  bool failed = std::abs(maxdiff) >= 1e-5;

  std::cout << "max.diff"
	    << std::scientific << std::setprecision(3) << std::setw(11)
	    << maxdiff << "[m] "
	    << aconf << " " << bconf;

  if (failed) std::cout << "  !!!";

  std::cout << "\n";
  return failed;
}
