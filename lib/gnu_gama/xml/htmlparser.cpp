/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2012, 2018  Ales Cepek <cepek@gnu.org>

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
  adj_ind = 0;
}


HtmlParser::~HtmlParser()
{
}


// in </table> all tables are 'closed' so we do need to select which
// was really active
void HtmlParser::close_table()
{
  coordinates_summary = observations_summary = project_equations
    = sum_of_squares = standard_deviation = standard_deviation_2
    = fixed_points = adjusted_coordinates = adjusted_heights
    = adjusted_orientations = adjusted_observations = residuals = false;

  table_row = table_col = 0;
  has_xy  = false;
}


int HtmlParser::startElement(const char *name, const char **atts)
{
  std::string tag(name), attribute;

  while (*atts)
    {
      if (atts[0] == std::string("id")) attribute = atts[1];

      atts += 2;
    }

  if      (attribute=="description")           description           = true;
  else if (attribute=="coordinates_summary")   coordinates_summary   = true;
  else if (attribute=="observations_summary")  observations_summary  = true;
  else if (attribute=="project_equations")     project_equations     = true;
  else if (attribute=="sum_of_squares")        sum_of_squares        = true;
  else if (attribute=="standard_deviation")    standard_deviation    = true;
  else if (attribute=="standard_deviation_2")  standard_deviation_2  = true;
  else if (attribute=="fixed_points")          fixed_points          = true;
  else if (attribute=="adjusted_coordinates")  adjusted_coordinates  = true;
  else if (attribute=="adjusted_heights")      adjusted_heights      = true;
  else if (attribute=="adjusted_orientations") adjusted_orientations = true;
  else if (attribute=="adjusted_observations") adjusted_observations = true;
  else if (attribute=="residuals")             residuals             = true;

  if      (tag == "tr")
    {
      trat = attribute;
      table_row++;
      table_col=0;
      adj_new = true;
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
  else if (coordinates_summary)   table_coordinates_summary();
  else if (observations_summary)  table_observations_summary();
  else if (project_equations)     table_project_equations();
  else if (sum_of_squares)        table_sum_of_squares();
  else if (standard_deviation)    table_standard_deviation();
  else if (standard_deviation_2)  table_standard_deviation_2();
  else if (fixed_points)          table_fixed_points();
  else if (adjusted_coordinates)  table_adjusted_coordinates();
  else if (adjusted_heights)      table_adjusted_heights();
  else if (adjusted_orientations) table_adjusted_orientations();
  else if (adjusted_observations) table_adjusted_observations();
  else if (residuals)             table_residuals();

  return 0;
}


void HtmlParser::trim_white_spaces()
{
  std::string::const_iterator b = data.begin();
  std::string::const_iterator e = data.end();
  TrimWhiteSpaces(b, e);
  data = std::string(b,e);
}


void HtmlParser::table_coordinates_summary()
{
  // skip table headers and possible new-line after </tr>
  if (table_col <= 0) return;

  int N;
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

  int N;
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

  int N;
  toIndex(data, N);

  if (table_row == 1)
    adjres->project_equations.connected_network = (trat=="connected_network");

  set(1,2,adjres->project_equations.equations,          N);
  set(1,4,adjres->project_equations.unknowns,           N);
  set(2,2,adjres->project_equations.degrees_of_freedom, N);
  set(2,4,adjres->project_equations.defect,             N);

  if (table_row == 1 && table_col == 4)
    {
      adjres->cov.reset(N,0);
      adjres->original_index.clear();
      adjres->original_index.push_back(-1); // indexing from 1
      adjres->obslist.clear();
    }
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

  double D;
  toDouble(data, D);

  set(1,3,adjres->standard_deviation.ratio, D);
  set(2,3,adjres->standard_deviation.lower, D);
  set(2,5,adjres->standard_deviation.upper, D);

  if (trat == "confidence_scale" && table_col==3)
    adjres->standard_deviation.confidence_scale = D;
}


void HtmlParser::table_fixed_points()
{
  if (table_col < 0) return;

  trim_white_spaces();

  if (table_col == 0)
    {
      if (data == "x") has_xy = true;
      return;
    }

  if (table_col == 1)
    {
      LocalNetworkAdjustmentResults::Point q;
      q.clear();
      q.id = data;
      adjres->fixed_points.push_back(q);
      return;
    }

  if (data.empty()) return;

  LocalNetworkAdjustmentResults::Point& p = adjres->fixed_points.back();

  double D;
  toDouble(data, D);

  if (has_xy)
    {
      switch(table_col)
        {
        case 2: p.x = D; p.hxy = true; break;
        case 3: p.y = D; p.hxy = true; break;
        case 4: p.z = D; p.hz  = true; break;
        };
    }
  else
    {
      switch(table_col)
        {
        case 2: p.z = D; p.hz  = true; break;
        }
    }
}


void HtmlParser::table_adjusted_coordinates()
{
  if (table_col <= 0) return;

  trim_white_spaces();

  // unknown index in column 1 detects a 'continuous row' of the given point
  if (table_col == 1)
    {
      int N;
      toIndex(data, N);
      adjres->original_index.push_back(N);

      adj_new = false;
      return;
    }

  if (adj_new)
    {
      LocalNetworkAdjustmentResults::Point apr, adj;
      apr.clear();
      adj.clear();
      apr.id = adj.id = data;
      adjres->approximate_points.push_back(apr);
      adjres->adjusted_points.push_back(adj);
      return;
    }

  LocalNetworkAdjustmentResults::Point& apr=adjres->approximate_points.back();
  LocalNetworkAdjustmentResults::Point& adj=adjres->adjusted_points.back();

  if (table_col == 2)
    {
      if (data == "x" || data == "X")
        {
          apr.hxy  = adj.hxy = true;
          adj.indx = ++adj_ind;
          adj_xyz  = adj_x;
        }
      else if (data == "y" || data == "Y")
        {
          adj.indy = ++adj_ind;
          adj_xyz  = adj_y;
        }
      else if (data == "z" || data == "Z")
        {
          apr.hz   = adj.hz = true;
          adj.indz = ++adj_ind;
          adj_xyz  = adj_z;
        }
      return;
    }

  if (table_col == 3 && data == "*")
    {
      if      (adj_xyz == adj_x) { adj.cxy = true; }
      else if (adj_xyz == adj_y) { adj.cxy = true; }
      else if (adj_xyz == adj_z) { adj.cz  = true; }
      return;
    }

  double D;
  toDouble(data, D);

  switch(adj_xyz)
    {
    case adj_x:
      if (table_col == 4) apr.x = D;
      if (table_col == 6) adj.x = D;
      break;
    case adj_y:
      if (table_col == 4) apr.y = D;
      if (table_col == 6) adj.y = D;
      break;
    case adj_z:
      if (table_col == 4) apr.z = D;
      if (table_col == 6) adj.z = D;
      break;
    }

  if (table_col == 7)
    {
      int n = adjres->original_index.size()-1;
      adjres->cov(n,n) = D*D;
    }

}


void HtmlParser::table_adjusted_heights()
{
  // table adjusted heights is used only when there aro no other adjusted unknowns (x,y,z)

  // endElement handler sets table_col to -1 for tag <tr>
  // startElement       sets table_col to  0 for tag <tr>
  // startElement increments table_col for tag <td>
  if (table_col <= 0) return;

  trim_white_spaces();

  if (table_col == 1)
    {
      int N;
      toIndex(data, N);
      adjres->original_index.push_back(N);

      if (true || table_row == 3)
        {
          LocalNetworkAdjustmentResults::Point apr, adj;
          apr.clear();
          adj.clear();
          apr.hz = adj.hz = adj.cz = true;
          adj.indz = N;
          adjres->approximate_points.push_back(apr);
          adjres->adjusted_points.push_back(adj);
        }
      return;
    }

  LocalNetworkAdjustmentResults::Point& apr=adjres->approximate_points.back();
  LocalNetworkAdjustmentResults::Point& adj=adjres->adjusted_points.back();

  if (table_col == 2)
    {
      apr.id = adj.id = data;
    }
  else if (table_col == 4)
    {
      double d;
      toDouble(data, d);
      apr.z = d;
    }
  else if (table_col == 6)
    {
      double d;
      toDouble(data, d);
      adj.z = d;
    }
  else if (table_col == 7)
    {
      double d;
      toDouble(data, d);
      int n = adjres->original_index.size()-1;
      adjres->cov(n,n) = d*d;
    }
}


void HtmlParser::table_adjusted_orientations()
{
  if (table_col <= 0) return;

  trim_white_spaces();

  if (table_col == 1)
    {
      int N;
      toIndex(data, N);
      adjres->original_index.push_back(N);

      LocalNetworkAdjustmentResults::Orientation newori;
      newori.index = ++adj_ind;
      adjres->orientations.push_back(newori);
      return;
    }

  LocalNetworkAdjustmentResults::Orientation& ori = adjres->orientations.back();

  if (table_col == 2)
    {
      ori.id  = data;
      return;
    }

  double D;
  toDouble(data, D);

  if      (table_col == 3) ori.approx = D;
  else if (table_col == 5) ori.adj    = D;
  else if (table_col == 6)
    {
      int n = adjres->original_index.size() - 1;
      adjres->cov(n,n) = D*D;
    }
}


void HtmlParser::table_adjusted_observations()
{
  // if (table_col == 1) return; // observation index ignored
  if (table_col <= 1) return;

  trim_white_spaces();

  if (table_col == 2)
    {
      if (!data.empty()) obs_point = data;
      return;
    }
  else if (table_col == 3)
    {
      if (!data.empty())
        {
          obs_left = obs_target;
          obs_target = data;
        }
      return;
    }
  else if (table_col == 4)
    {
      if (data.empty()) return;

      LocalNetworkAdjustmentResults::Observation obs;
      obs.clear();
      obs.from = obs_point;
      if (data == "angle")
        {
          obs.left  = obs_left;
          obs.right = obs_target;
        }
      else
        {
          obs.to = obs_target;
        }

      if      (data == "angle") obs.xml_tag = "angle";
      else if (data == "azim.") obs.xml_tag = "azimuth";
      else if (data == "dir." ) obs.xml_tag = "direction";
      else if (data == "dist.") obs.xml_tag = "distance";
      else if (data == "slope") obs.xml_tag = "slope-distance";
      else if (data == "zenit") obs.xml_tag = "zenith-angle";
      else if (data == "h dif") obs.xml_tag = "height-diff";
      else if (data == "x dif") obs.xml_tag = "dx";
      else if (data == "y dif") obs.xml_tag = "dy";
      else if (data == "z dif") obs.xml_tag = "dz";
      else if (data == "x")     obs.xml_tag = "coordinate-x";
      else if (data == "y")     obs.xml_tag = "coordinate-y";
      else if (data == "z")     obs.xml_tag = "coordinate-z";
      else
        {
          std::string error =
            "UNKNOWN OBSERVATION TYPE \"" + data + "\" in htmlparser.cpp";
          std::cout << error << "\n";
          throw Exception::parser(error, 0, 0);
        }

      adjres->obslist.push_back(obs);
      return;
    }

  LocalNetworkAdjustmentResults::Observation& obs = adjres->obslist.back();

  double D;
  toDouble(data, D);

  if      (table_col == 5) obs.obs   = D;
  else if (table_col == 6) obs.adj   = D;
  else if (table_col == 7) obs.stdev = D;
}


void HtmlParser::table_residuals()
{
  if (table_col <= 0) return;

  if (table_col == 1)
    {
      int N;
      toIndex(data, N);
      adj_ind = N;
      return;
    }

  LocalNetworkAdjustmentResults::Observation& obs = adjres->obslist[adj_ind-1];
  trim_white_spaces();

  if (table_col == 7 || table_col <= 4)
    {
      return;
    }
  else if (table_col == 10)
    {
      obs.err_obs = data;
      return;
    }
  else if (table_col == 11)
    {
      obs.err_adj = data;
      return;
    }

  double D;
  toDouble(data, D);

  if      (table_col == 5) obs.f = D;
  else if (table_col == 8) obs.std_residual = D;

//  std::cerr << adj_ind << "?" << adj_xyz << ":" << trat << ": "
//            << table_row << " " << table_col << " [" << data << "]\n";
}
