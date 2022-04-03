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

#include "k2gkf.h"
#include "input.h"
#include "output.h"

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype> // toupper()

K2gkf::K2gkf(std::istream &inp, std::ostream &out)
  : inp_(inp), out_(out), examples_(false)
{
}

void K2gkf::run()
{
  Common common(this);
  if (examples_) common.examples = true;

  Input  input (inp_, common);
  Output output(out_, common);
}


std::string K2gkf::version()
{
  return "1.00";
}
