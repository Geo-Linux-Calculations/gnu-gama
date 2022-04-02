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

#include "common.h"
#include <sstream>
#include "k2gkf.h"

void Common::set_error()
{
  k2gkf->set_error();
}

std::string point(std::string id, const Common::Point& pt)
{
  std::string s = "<point id='" + id + "'";

  if (!pt.x.empty())
    s += " x='" + pt.x + "'";
  if (!pt.y.empty())
    s += " y='" + pt.y + "'";
  if (!pt.z.empty())
    s += " z='" + pt.z + "'";

  if (!pt.fix.empty())
    {
      s += " fix='";
      for (auto c : pt.fix) s += c;
      s += + "'";
    }

  if (!pt.adj.empty())
    {
      s += " adj='";
      for (auto c : pt.adj) s += c;
      s += "'";
    }

  s += " />\n";

  return s;
}

std::vector<std::string> get_tokens(std::string line)
{
  std::istringstream istr(line);
  std::vector<std::string> toks;
  std::string token;
  while (istr >> token) toks.push_back(token);

  return toks;
}

std::string gon2cc(std::string g)
{
  return std::to_string(std::stod(g) * 1e4);
}

std::string mgon2cc(std::string g)
{
  return std::to_string(std::stod(g) * 1e3);
}

std::string m2mm(std::string g)
{
  return std::to_string(std::stod(g) * 1e3);
}

std::string cm2mm(std::string g)
{
  return std::to_string(std::stod(g) * 1e1);
}
