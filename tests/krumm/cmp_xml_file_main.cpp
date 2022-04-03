/* Krumm2gama-local -- conversion from F. Krumm format to XML gama-local
   Copyright (C) 2022 Ales Cepek <cepek@gnu.org>

   This file is part of Krumm2gama-local.

   Krumm2gama-local is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Krumm2gama-local is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Krumm2gama-local. If not, see <https://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include "cmp_xml_file.h"

int help()
{
  std::cerr << R"help(
Compare gama-local xml adjustment results with file containing information
about adjusted coordinates as used in Geodetic Network Adjustment Examples
by Friedhelm Krumm.

Usage: cmp_xml_file  xml_adjustment_results  adjusted_coordinates_file

Adjusted coordinates format:

       [m]   [mm]   [cm]    [m]   [mm]   [cm]    [m]   [mm]   [cm]    [cm]
Point X_adj X_corr stddev  Y_adj Y_corr stddev  Z_adj z_corr stddev  stdd3d

)help";

  return 1;
}

int main(int argc, char* argv[])
{
  if (argc != 3) return help();

  std::ifstream xml(argv[1]);
  std::ifstream adj(argv[2]);
  if (!xml || !adj) return help();

  return cmp_xml_file(xml, adj);
}
