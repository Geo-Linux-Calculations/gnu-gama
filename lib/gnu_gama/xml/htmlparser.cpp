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
 * \brief #GNU_gama::local::HtmlParser implementation
 *
 * \author Ales Cepek
 */

#include <iostream>
#include <gnu_gama/xml/htmlparser.h>

using namespace GNU_gama;

HtmlParser::HtmlParser(GNU_gama::LocalNetworkAdjustmentResults* adj_res)
  : adjres(adj_res)
{
  adjres->init();

  description = false;
  close_table();
}

HtmlParser::~HtmlParser()
{
}

// in </table> all tables are 'closed' so we do need te select which
// on was really active
void HtmlParser::close_table()
{
  coordinates_summary = observations_summary = project_equations 
    = sum_of_squares = standard_deviation = standard_deviation_2 = false;
  table_row = table_col = 0;
}

int HtmlParser::startElement(const char *name, const char **atts)
{
  std::string tag(name), attribute;

  while (*atts)
    {
      if (atts[0] == std::string("id")) attribute = atts[1];

      atts += 2;
    }

  if      (attribute=="description")          description          = true;
  else if (attribute=="coordinates_summary")  coordinates_summary  = true;
  else if (attribute=="observations_summary") observations_summary = true;
  else if (attribute=="project_equations")    project_equations    = true;
  else if (attribute=="sum_of_squares")       sum_of_squares       = true;
  else if (attribute=="standard_deviation")   standard_deviation   = true;
  else if (attribute=="standard_deviation_2") standard_deviation_2 = true;
    
  if      (tag == "tr")
    {
      trat = attribute;
      table_row++;
      table_col=0;
    }
  else if (tag == "td") table_col++;

  return 0;
}

int HtmlParser::endElement(const char *name)
{
  std::string tag(name);

  if (description && tag == "p")
    {
      description = false;
    }
  else if (tag == "table")
    {
      close_table();
    }
  else if (tag == "tr")
    {
      table_col = -1;
    }

  return 0;
}
  
int HtmlParser::characterDataHandler(const char *s, int len)
{
  data = std::string(s, len);

  if (description) adjres->description += std::string(s, len);
  else if (coordinates_summary)  table_coordinates_summary();
  else if (observations_summary) table_observations_summary();
  else if (project_equations)    table_project_equations();
  else if (sum_of_squares)       table_sum_of_squares();
  else if (standard_deviation)   table_standard_deviation();
  else if (standard_deviation_2) table_standard_deviation_2();

  return 0;
}

void HtmlParser::table_coordinates_summary()
{
  // skip table heades and possible new-line after </tr>
  if (table_col <= 0) return;

  std::size_t N;
  toIndex(data, N);

  set(2,2,adjres->coordinates_summary.adjusted.xyz,    N);
  set(2,3,adjres->coordinates_summary.adjusted.xy,     N);
  set(2,4,adjres->coordinates_summary.adjusted.z,      N);
  set(3,2,adjres->coordinates_summary.constrained.xyz, N);
  set(3,3,adjres->coordinates_summary.constrained.xy,  N);
  set(3,4,adjres->coordinates_summary.constrained.z,   N);
  set(4,2,adjres->coordinates_summary.fixed.xyz,       N);
  set(4,3,adjres->coordinates_summary.fixed.xy,        N);
  set(4,4,adjres->coordinates_summary.fixed.z,         N);
}

void HtmlParser::table_observations_summary()
{
  if (table_col <= 0 || table_col != 2) return;

  std::size_t N;
  toIndex(data, N);
  
  if (trat == "count_dist" ) adjres->observations_summary.distances  = N;
  if (trat == "count_dir"  ) adjres->observations_summary.directions = N;
  if (trat == "count_ang"  ) adjres->observations_summary.angles     = N;
  if (trat == "count_coord") adjres->observations_summary.xyz_coords = N;
  if (trat == "count_level") adjres->observations_summary.h_diffs    = N;
  if (trat == "count_zang" ) adjres->observations_summary.z_angles   = N;
  if (trat == "count_sdist") adjres->observations_summary.s_dists    = N;
  if (trat == "count_vect" ) adjres->observations_summary.vectors    = N;
}

void HtmlParser::table_project_equations()
{
  if (table_col <= 0) return;

  std::size_t N;
  toIndex(data, N);
  
  if (table_row == 1)
    adjres->project_equations.connected_network = (trat=="connected_network");

  set(1,2,adjres->project_equations.equations,          N);
  set(1,4,adjres->project_equations.unknowns,           N);
  set(2,2,adjres->project_equations.degrees_of_freedom, N);
  set(2,4,adjres->project_equations.defect,             N);
}

void HtmlParser::table_sum_of_squares()
{
  if (table_col <= 0) return;

  double D;
  toDouble(data, D);

  set(1,2,adjres->standard_deviation.apriori,       D);
  set(2,2,adjres->standard_deviation.aposteriori,   D);
  set(2,4,adjres->project_equations.sum_of_squares, D);
}

void HtmlParser::table_standard_deviation()
{
  if (table_col <= 0) return;

  if (table_row == 2)
    adjres->standard_deviation.using_aposteriori = (trat=="a_posteriori");

  double D;
  toDouble(data, D);

  set(3,2,adjres->standard_deviation.probability,   D/100);
}

void HtmlParser::table_standard_deviation_2()
{
  if (table_col <= 0) return;

  if (table_row == 2)
    adjres->standard_deviation.passed = (trat=="test_m0_passed");

  //std::cerr << ":" << trat << ": "
  //          << table_row << " " << table_col << " |" << data << "|\n";
  double D;
  toDouble(data, D);
  
  set(1,3,adjres->standard_deviation.ratio, D);
  set(2,3,adjres->standard_deviation.lower, D);
  set(2,5,adjres->standard_deviation.upper, D);

  if (trat == "confidence_scale" && table_col==3)
    adjres->standard_deviation.confidence_scale = D;
}


