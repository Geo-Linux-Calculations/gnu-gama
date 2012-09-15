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

std::string version, netconfig, input;
const char* netfile;

std::vector<std::string>   algname;
std::vector<LocalNetwork*> algorithm;

double condnum;


int main(int argc, char* argv[])
{
  if (argc != 4) return 1;

  version   = std::string(argv[1]);
  netconfig = std::string(argv[2]);
  netfile   = argv[3];

  std::ifstream inp(argv[3]);
  if (!inp)
    {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[3] << "\n";
      return 1;
    }

  double maxdiff;
  bool failed = false;
  algname.push_back(" svd ");   algorithm.push_back(getNet(alg_svd,  argv[3]));
  algname.push_back(" gso ");   algorithm.push_back(getNet(alg_gso,  argv[3]));
  algname.push_back(" chol");   algorithm.push_back(getNet(alg_chol, argv[3]));
  algname.push_back(" env ");   algorithm.push_back(getNet(alg_env,  argv[3]));

  condnum = algorithm[0]->cond();

  for (int i=0; i<algname.size(); i++)
    for (int j=i+1; j<algname.size(); j++)
      {
        std::cout << "cond.n "
                  << std::scientific << std::setprecision(2)
                  << condnum;

        maxdiff = xyzMaxDiff(algorithm[i],algorithm[j]);
        bool ok = std::abs(maxdiff) < 1e-5;

        std::cout << "  max.diff"
                  << std::scientific << std::setprecision(3) << std::setw(11)
                  << maxdiff << " [m] "
                  << algname[i] << algname[j] << "  " << netconfig;

        if (ok)
          {
            std::cout << "\n";
          }
        else
          {
            failed = true;
            std::cout << "  !!!\n";
          }
      }

  if (failed) return 1;
}
