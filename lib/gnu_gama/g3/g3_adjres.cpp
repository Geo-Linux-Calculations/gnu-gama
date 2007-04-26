/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2007  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 *  $Id: g3_adjres.cpp,v 1.2 2007/04/26 11:11:42 cepek Exp $
 */

#include <gnu_gama/g3/g3_adjres.h>

using namespace std;
using namespace GNU_gama::g3;

namespace 
{
  void xml(std::ostream& out, const std::string data, const std::string& tag)
  {
    if (!data.empty())
      out << "   <" << tag << ">" << data << "</" << tag << ">\n";
  }
}

void AdjustmentResults::write_xml(std::ostream& out) const
{
  out << "<g3-adjustment-results>\n\n";

  out << "<adjustment-statistics>\n";
  
  xml(out, algorithm, "algorithm"); 
  if (!(ell_cap.empty() && ell_id.empty() && ell_a.empty() && ell_b.empty()))
    {
      out << "   <ellipsoid>\n";
      xml(out, ell_cap, "caption");
      xml(out, ell_id,  "id");
      xml(out, ell_a,   "a");
      xml(out, ell_b,   "b");
      out << "   </ellipsoid>\n";      
    }
  xml(out, parameters,      "parameters"); 
  xml(out, equations,       "equations");
  xml(out, defect,          "defect");
  xml(out, redundancy,      "redundancy");
  xml(out, sum_of_squares,  "sum-of-squares");
  xml(out, apriori_var,     "apriori-varance");
  xml(out, aposteriori_var, "aposteriori-variance");
  xml(out, variance_factor, "variance-factor-used");
  xml(out, design_m_graph,  "design-matrix-graph");

  out << "</adjustment-statistics>\n\n";

  out << "</g3-adjustment-results>\n";
}
