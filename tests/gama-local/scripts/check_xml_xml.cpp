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
#include <iomanip>
#include <fstream>
#include <compare_xml_adjustment.h>

using GNU_gama::LocalNetworkAdjustmentResults;

int main(int /*argc*/, char* argv[])
{
  std::cout << "max.diff XML-XML for "
            << argv[1] << "\n";

  LocalNetworkAdjustmentResults* html = new LocalNetworkAdjustmentResults;
  {
    std::ifstream inp_html(argv[2]);
    if (!inp_html) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[2] << "\n";
      return 1;
    }
    html->read_xml(inp_html);
  }

  LocalNetworkAdjustmentResults* xml = new LocalNetworkAdjustmentResults;
  {
    std::ifstream inp_xml(argv[3]);
    if (!inp_xml) {
      std::cout << "   ####  ERROR ON OPENING FILE " << argv[3] << "\n";
      return 1;
    }
    xml->read_xml(inp_xml);
  }

  return compare_xml_adjustment(html, xml, 1e-6);
}
