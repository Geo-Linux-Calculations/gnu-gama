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

#include <algorithm>
#include <krumm/string_matrix.h>

using namespace GNU_gama::local;

StringMatrix::StringMatrix()
{
}

int StringMatrix::rows() const
{
  int maxr = 0;
  for (auto row=this->cbegin(); row!=this->cend(); row++)
    maxr = std::max(maxr, row->first);
  return  maxr;
}

int StringMatrix::cols() const
{
  int maxc = 0;
  for (auto row=this->cbegin(); row!=this->cend(); row++)
    for (auto col=row->second.cbegin(); col!=row->second.cend(); col++)
    maxc = std::max(maxc, col->first);
  return  maxc;
}
