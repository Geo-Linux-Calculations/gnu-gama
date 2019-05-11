/* GNU Gama -- testing Acord2 approximate coordinates
   Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

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

#include "a2g.h"
#include <gnu_gama/local/lcoords.h>
#include <iostream>
#include <fstream>
#include <string>

using std::cerr;
using std::cout;
using std::string;

int main(int argc, char* argv[])
{
  if (argc < 4)
    {
      cerr << "\n";
      cerr << "Usage: " << argv[0] << "  output_dir  input_dir  files... \n\n";
      return 1;
    }

  cout << "input  dir: " << argv[2] << "\n";
  cout << "output dir: " << argv[1] << "\n";
  cout << "algorithm : ";
#ifdef A2G_ACORD2
  cout << "acord2 ";
#endif
#ifdef A2G_ACORD
  cout << "acord ";
#endif
  cout << "\n\n";
  
  
  int errors = 0;
  for (int i=3; i<argc; i++)
    {
      for (int ang : {1, 0})
        {
          using CS = GNU_gama::local::LocalCoordinateSystem::CS;
          for (CS cs : { CS::EN, CS::NW, CS::SE, CS::WS,
                         CS::NE, CS::SW, CS::ES, CS::WN })
            {
              A2G a2g;
              if (ang == 1) a2g.set_ang_right_handed();
              else          a2g.set_ang_left_handed();
              a2g.set_local_coordinate_system(cs);
              a2g.set_output_dir (argv[1]);
              a2g.set_input_dir  (argv[2]);
              int read = a2g.read(argv[i]);

              if (read)
                {
                  errors += read;
                  cout << "  failed ";
                }
              else
                {
                  cout << "passed   ";
                }
              using LOC = GNU_gama::local::LocalCoordinateSystem;
              cout << " xy " << LOC::locos_tags[int(cs)]
                   << " / angles "
                   << (ang ? "counterclockwise" : "clockwise       ")
                   << "   " << argv[i] << "\n";
            }
        }
    }

  return errors;
}
