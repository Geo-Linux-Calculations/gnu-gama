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
#include <vector>
#include <unordered_set>
#include "k2gkf.h"
#include "input.h"
#include "output.h"

using namespace std;

std::string help_notice = R"help_notice(
Conversion from data format used in Geodetic Network Adjustment Examples
by Friedhelm Krumm to XML input format of gama-local (GNU Gama)
https://www.gis.uni-stuttgart.de/lehre/campus-docs/adjustment_examples.pdf

     krumm2gkf  input_file  [ output_file  ]
     krumm2gkf < std_input  [ > std_output ]

Options:
-h, --help      this text
-v, --version   print program version
-e, --examples  add the following comment to the generated XML file

     This example is based on published material Geodetic Network Adjustment
     Examples by Friedhelm Krumm, Geodätisches Institut Universität Stuttgart,
     Rev. 3.5, January 20, 2020

)help_notice";

int main(int argc, char *argv[])
{
  const std::unordered_set<std::string>
      e_arg {"-e", "--examples"},
      h_arg {"-h", "--h", "-help", "--help"},
      v_arg {"-v", "--version"};

  std::vector<std::string> files;
  bool help = false;
  bool examples = false;

  for (int i = 1; i < argc; i++)
    {
      if (h_arg.find(argv[i]) != h_arg.end())
        {
          help = true;
          break;
        }
      else if (v_arg .find(argv[i]) != v_arg .end())
        {
          std::cout << K2gkf::version();
          return 0;
        }
      else if (e_arg.find(argv[i]) != e_arg.end())
        {
          examples = true;
          continue;
        }

      files.push_back(argv[i]);
    }

  if (help || files.size() > 2)
    {
      std::cout << help_notice;
      return 1;
    }

  std::ifstream ifstr;
  if (files.size() > 0)
    {
      ifstr.open(files[0]);
      // error handling ...
    }
  std::istream istr(files.size() > 0 ? ifstr.rdbuf() : std::cin.rdbuf());

  std::ofstream ofstr;
  if (files.size() > 1)
    {
      ofstr.open(files[1]);
      // error handling ...
    }
  std::ostream ostr(files.size() > 1 ? ofstr.rdbuf() : std::cout.rdbuf());

  K2gkf k2gkf(istr, ostr);
  k2gkf.set_examples(examples);
  k2gkf.run();

  return k2gkf.error();
}
