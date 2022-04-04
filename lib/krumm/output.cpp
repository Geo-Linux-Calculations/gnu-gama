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

#include <krumm/output.h>
#include <krumm/common.h>
#include <gnu_gama/xml/str2xml.h>

using namespace GNU_gama::local;

Output::Output(std::ostream& out, Common& c) : out_(out), common(c)
{
  output();
}


void Output::output()
{
  xml_head();
  description();
  parameters();
  points_observations();
  points();
  directions();
  distances();
  angles();
  spatial_distances();
  vertical_angles();
  zenith_angles();
  vectors();
  levelled_hdiffs();
  datum_dyn();
  azimuths();
  out_xml_tail();
}

void Output::xml_head()
{
  out_ <<
R"info(<?xml version="1.0" ?>
<gama-local xmlns="http://www.gnu.org/software/gama/gama-local">
<network axes-xy="en" angles="left-handed">
)info";

  if (common.examples) out_ <<
R"info(
<!-- This example is based on published material Geodetic Network Adjustment
     Examples by Friedhelm Krumm, Geodätisches Institut Universität Stuttgart,
     Rev. 3.5, January 20, 2020
-->
)info";
}

void Output::description()
{
  using GNU_gama::str2xml;

  if (common.project.empty() && common.source.empty()) return;

  out_ << "\n<description>\n";

  bool newline {false};
  if (!common.project.empty())
    {
      out_ << str2xml(common.project);
      newline = true;
    }

  if (!common.source.empty())
    {
      if (newline) out_ << "\n";   // separate project from source
      out_ << str2xml(common.source);
    }

  out_ << "</description>\n";


}

void Output::parameters()
{
  out_
      << "\n<!-- sigma-apr/sigma0 gon to cc -->\n"
      << "<parameters\n"
         "   sigma-apr = \""
      << /*m2mm*/(common.sigma0)
      << "\"\n"
         "   conf-pr   = \" 0.95 \"\n"
         "   tol-abs   = \" 1000 \"\n"
         "   sigma-act = \"aposteriori\"\n"
         "   algorithm = \"gso\"\n"
         "   cov-band  = \"-1\"\n"
         "/>\n\n";
}

void Output::points_observations()
{
  out_ << "<points-observations>\n";
}

void Output::spatial_distances()
{
  if (common.spatial_dist_list.empty()) return;

  out_ << "\n<obs>\n"
       << common.spatial_dist_list
       << "</obs>\n";
}

void Output::vertical_angles()
{
  if (common.vertical_angles_list.empty()) return;

  out_ << "\n<!-- vertical angles replaced by zenith angles -->\n"
       << "<obs>\n"
       << common.vertical_angles_list
       << "</obs>\n";
}

void Output::zenith_angles()
{
  if (common.zenith_angles_list.empty()) return;

  out_ << "\n<obs>\n"
       << common.zenith_angles_list
       << "</obs>\n";
}

void Output::vectors()
{
  if (common.vectors.empty()) return;

  for (const std::string& vec : common.vectors)
    {
      out_ << "\n<vectors>\n";
      out_ << vec;
      out_ << "</vectors>\n";
    }
}

void Output::points()
{
  // implicit points settings
  std::set<char> impl;
  if      (common.dimension == 1) impl = {'z'};
  else if (common.dimension == 2) impl = {'x', 'y'};
  else if (common.dimension == 3) impl = {'x', 'y', 'z'} ;

  for (auto& p: common.point_map) p.second.adj = impl;

  // datum settings
  std::string datum {"fix"};
  for (std::string t : common.datum_tokens)
    {
      if (t == "fix" || t == "free") {
          datum = t;
          continue;
        }

      if (common.dimension == 1) {
          auto& pt = common.point_map[t];
          if (datum == "fix")
            {
              pt.adj.erase ('z'); // implicit value
              pt.fix.insert('z');
            }
          else if (datum == "free")
            {
              pt.adj.erase ('z');
              pt.adj.insert('Z');
            }
          else if (datum == "dyn")
            {

              // NEEXISTUJE common.datum_dyn_cov_list += point(t, pt);
              //common.point_map.erase(t);
            }
        }
      else
        {
          char c = std::tolower(t.front());
          t.erase(0,1);
          auto& pt = common.point_map[t];

          if (datum == "fix")
            {
              pt.adj.erase (c);
              pt.fix.insert(c);
            }
          else if (datum == "free")
            {
              pt.fix.erase(c);
              pt.adj.erase(c);
              c = std::toupper(c);
              pt.adj.insert(c);
            }
        }
    }

  // points XML output
  out_ << "\n";

  for (auto& p : common.point_map)
    {
      auto id = p.first;
      auto pt = p.second;
      if (!pt.datum_dyn) out_ << point(id, pt);
    }
}

void Output::directions()
{
  for (const auto& dir : common.direction_map)
    {
      std::string from = dir.first;
      const Common::DirectionSet& dirs = dir.second;

      out_ << "\n<obs from=\"" << from << "\"";
      if (!dirs.orientation.empty())
        {
          //out_ << " orientation=\"" << dirs.orientation << "\"";
        }
      out_ << ">\n";

      for (auto f = dirs.directions.cbegin(); f!=dirs.directions.cend(); f++)
        {
          out_ << *f;
        }

      out_ << "</obs>\n";
    }
}

void Output::distances()
{
  if (common.distances_list.empty()) return;

  out_ << "\n<obs>\n";
  out_ << common.distances_list;
  out_ << "</obs>\n";
}

void Output::angles()
{
  if (common.angles_list.empty()) return;

  out_ << "\n<obs>\n";
  out_ << common.angles_list;
  out_ << "</obs>\n";
}

void Output::levelled_hdiffs()
{
  if (common.levell_hdiffs_list.empty()) return;

  out_ << "\n<height-differences>\n"
       << common.levell_hdiffs_list
       << "</height-differences>\n";
}

void Output::datum_dyn()
{
  if (common.datum_dyn_cov.empty()) return;

  out_ << "\n<coordinates>\n";
  for (auto& p : common.point_map)
    {
      auto id = p.first;
      auto pt = p.second;
      if (pt.datum_dyn) out_ << point(id, pt);
    }

  int rows = common.datum_dyn_cov.rows();
  int cols = common.datum_dyn_cov.cols();
  int band = cols - 1;
  out_ << "\n<cov-mat dim='" << rows << "' band='" << band << "'>\n";

  for (int r=1; r<=rows; r++)
    if (band != 0)
      {
        for (int c=r; c<=cols; c++) out_ << common.datum_dyn_cov[r][c] << " ";
        out_ << "\n";
      }
    else
      {
        out_ << common.datum_dyn_cov[r][1] << "e4\n";
      }

  out_ << "</cov-mat>\n"
          "</coordinates>\n";
}

void Output::azimuths()
{
  if (common.azimuths_list.empty()) return;

  out_ << "\n<obs>\n";
  out_ << common.azimuths_list;
  out_ << "</obs>\n";
}


void Output::out_xml_tail()
{
  out_
      << "\n</points-observations>\n\n"

         "</network>\n"
         "</gama-local>\n";
}
