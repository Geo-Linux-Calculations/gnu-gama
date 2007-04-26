/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

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
 *  $Id: g3_adjres.h,v 1.2 2007/04/26 11:11:42 cepek Exp $
 */

#ifndef GNU_gama__g3_adjres_h_gnugamag3adjresh___gnu_gama_g3adjres
#define GNU_gama__g3_adjres_h_gnugamag3adjresh___gnu_gama_g3adjres

#include <ostream>
#include <string>

namespace GNU_gama {  namespace g3 {

  class AdjustmentResults
  {
  public:
    
    // <adjustment-statistics>

    std::string algorithm;
    std::string ell_cap;       // <ellipsoid> <caption>
    std::string ell_id;        //             <id> 
    std::string ell_a;         //             <a>
    std::string ell_b;         //             <b>
    std::string parameters ;
    std::string equations;
    std::string defect;
    std::string redundancy;
    std::string sum_of_squares;
    std::string apriori_var;
    std::string aposteriori_var;
    std::string variance_factor;
    std::string design_m_graph;

    void write_xml(std::ostream&) const;
  };
  
}}

#endif
