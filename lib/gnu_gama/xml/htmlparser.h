/* GNU Gama -- adjustment of geodetic networks
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

/** \file htmlparser.h
 * \brief #GNU_gama::local::HtmlParser class header file
 *
 * \author Ales Cepek
 */


#ifndef HTMLPARSER_H_HtmlParser_h_htmlparser_h
#define HTMLPARSER_H_HtmlParser_h_htmlparser_h

#include <gnu_gama/xml/baseparser.h>
#include <gnu_gama/exception.h>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>

namespace GNU_gama {

class HtmlParser : public GNU_gama::BaseParser<GNU_gama::Exception::parser>
{
 public:

  HtmlParser(GNU_gama::LocalNetworkAdjustmentResults* adj_res);
  ~HtmlParser();
  int startElement(const char *name, const char **atts);
  int characterDataHandler(const char *s, int len);
  int endElement(const char *name);

 private:

  LocalNetworkAdjustmentResults* adjres;
  bool description;

  // tables
  bool coordinates_summary, observations_summary, project_equations, 
    sum_of_squares, standard_deviation, standard_deviation_2, fixed_points,
    adjusted_coordinates, orientation_unknowns, adjusted_observations,
    residuals;
  int   table_row, table_col;
  bool  has_xy;        // fixed points has x,y
  bool  adj_new;       // new adjusted point
  int   adj_ind;       // adjusted coordinates index
  enum {adj_x, adj_y, adj_z} adj_xyz;
  std::string trat;   // startElement attribute
  std::string data;
  std::string obs_point, obs_left, obs_target;   // observations' points id

  void close_table();

  template<typename T, typename U> void set(int row, int col, T& a, U b)
    {
      if (table_row == row && table_col == col) a = b;
    }


  void trim_white_spaces();

  void table_coordinates_summary();
  void table_observations_summary();
  void table_project_equations();
  void table_sum_of_squares();
  void table_standard_deviation();
  void table_standard_deviation_2();
  void table_fixed_points();
  void table_adjusted_coordinates();
  void table_orientation_unknowns();
  void table_adjusted_observations();
  void table_residuals();
};

} // namespace GNU_gama

#endif
