/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef GNU_gama__g3_adjres_h_gnugamag3adjresh___gnu_gama_g3adjres
#define GNU_gama__g3_adjres_h_gnugamag3adjresh___gnu_gama_g3adjres

#include <ostream>
#include <string>
#include <list>

namespace GNU_gama {  namespace g3 {

  /** g3 adjustment results class. */

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


    // <adjustment-results>

    struct Point
    {
      std::string id;
      std::string height;
      std::string n;        // fixed, free, constr, 'empty string'
      std::string n_dn;     // adjustment correction
      std::string n_ind;    // adjustment index
      std::string e;
      std::string e_de;
      std::string e_ind;
      std::string u;
      std::string u_du;
      std::string u_ind;
      std::string cnn, cne, cnu, cee, ceu, cuu;
      std::string x_given;
      std::string x_correction;
      std::string x_adjusted;
      std::string y_given;
      std::string y_correction;
      std::string y_adjusted;
      std::string z_given;
      std::string z_correction;
      std::string z_adjusted;
      std::string cxx, cxy, cxz, cyy, cyz, czz;
      std::string b_given;
      std::string b_correction;
      std::string b_adjusted;
      std::string l_given;
      std::string l_correction;
      std::string l_adjusted;
      std::string h_given;
      std::string h_correction;
      std::string h_adjusted;

      void clear() { *this = Point(); }

    } point;

    std::list<Point> points;


    // <adjusted-observations>

    struct Observation
    {
      std::string  type;      // observation type (vector, distance, ...)
      std::string   ind;      // index (first index if dim > 1)

      std::string   id1,  id2,  id3;     // identification
      std::string  obs1, obs2, obs3;     // observed value
      std::string  res1, res2, res3;     // residual
      std::string  adj1, adj2, adj3;     // adjusted

      // standard devations of observed / adjusted value(s)

      std::string stdev_obs1, stdev_obs2, stdev_obs3;
      std::string stdev_adj1, stdev_adj2, stdev_adj3;

      std::string c11, c12, c13, c22, c23, c33;   // covariances (dim > 1)

      void clear() { *this = Observation(); }

    } observation;

    std::list<Observation> observations;
    std::list<Observation> rejected_observations;

    void write_xml(std::ostream&) const;
  };

}}

#endif
