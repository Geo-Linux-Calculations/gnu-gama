/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006, 2012, 2014  Ales Cepek <cepek@gnu.org>

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

#include <iostream>
#include <cstring>
#include <string>
#include <stack>
#include <sstream>
#include <gnu_gama/xml/localnetwork_adjustment_results.h>
#include <gnu_gama/intfloat.h>
#include <gnu_gama/xml/htmlparser.h>
#include <gnu_gama/xsd.h>

using namespace std;
using namespace GNU_gama;

void LocalNetworkAdjustmentResults::init()
{
  gons = true;

  description.clear();

  network_general_parameters.gama_local_version.  clear();
  network_general_parameters.gama_local_algorithm.clear();
  network_general_parameters.gama_local_compiler. clear();
  network_general_parameters.axes_xy.  clear();
  network_general_parameters.angles.   clear();
  network_general_parameters.epoch.    clear();
  network_general_parameters.latitude. clear();
  network_general_parameters.ellipsoid.clear();

  coordinates_summary.adjusted.xyz = 0;
  coordinates_summary.adjusted.xy  = 0;
  coordinates_summary.adjusted.z   = 0;
  coordinates_summary.constrained.xyz = 0;
  coordinates_summary.constrained.xy  = 0;
  coordinates_summary.constrained.z   = 0;
  coordinates_summary.fixed.xyz = 0;
  coordinates_summary.fixed.xy  = 0;
  coordinates_summary.fixed.z   = 0;

  observations_summary.distances = 0;
  observations_summary.directions = 0;
  observations_summary.angles = 0;
  observations_summary.xyz_coords = 0;
  observations_summary.h_diffs = 0;
  observations_summary.z_angles = 0;
  observations_summary.s_dists = 0;
  observations_summary.vectors = 0;
  observations_summary.azimuths = 0;

  project_equations.equations = 0;
  project_equations.unknowns = 0;
  project_equations.degrees_of_freedom = 0;
  project_equations.defect = 0;
  project_equations.sum_of_squares = 0;
  project_equations.connected_network = true;

  standard_deviation.apriori = 0;
  standard_deviation.aposteriori = 0;
  standard_deviation.using_aposteriori = true;
  standard_deviation.probability = 0;
  standard_deviation.ratio = 0;
  standard_deviation.lower = 0;
  standard_deviation.upper = 0;
  standard_deviation.passed = false;
  standard_deviation.confidence_scale = 0;

  fixed_points      .clear();
  approximate_points.clear();
  adjusted_points   .clear();
}

double LocalNetworkAdjustmentResults::Observation::residual() const
  throw()
{
  double r = (adj - obs);
  if (xml_tag == "direction" || xml_tag == "angle" ||
      xml_tag == "zenith-angle")
    {
      if (r >= 400 || std::abs(r - 400) < std::abs(r)) r -= 400;
      r *= 10;
    }

  return r*1000;
}


void LocalNetworkAdjustmentResults::read_xml(std::istream& xml)
  throw(GNU_gama::Exception::parser)
{
  string text;

  Parser p(this);
  while (getline(xml, text))
    {
      p.xml_parse(text.c_str(), text.length(), 0);
      p.xml_parse("\n", 1, 0);
    }
  p.xml_parse("", 0, 1);
}


void LocalNetworkAdjustmentResults::read_html(std::istream& html)
  throw(GNU_gama::Exception::parser)
{
  GNU_gama::HtmlParser parser(this);
  {
    std::string line;
    while (std::getline(html, line, '\0'))
      {
        std::string text;
        for (int i=0; i<line.length(); i++)
          {
            // replace some special hardcoded characters used in
            // gama-local HTML output for better formatting
            if (line[i] == '&')
              {
                if (line.substr(i, 6) == "&nbsp;")
                  {
                    text += ' ';
                    i += 5;
                    continue;
                  }
                if (line.substr(i, 7) == "&minus;")
                  {
                    text += '-';
                    i += 6;
                    continue;
                  }
              }
            text += line[i];
          }
        parser.xml_parse(text.c_str(), text.length(), 0);
     }
    parser.xml_parse("", 0, 1);
  }
}

void LocalNetworkAdjustmentResults::Parser::check_and_clear_data()
{
  for (std::string::const_iterator
         i=data.begin(), e=data.end(); i!=e; ++i)
    {
      if (!std::isspace(*i)) error("Bad Data");
    }
  data.clear();
}


void LocalNetworkAdjustmentResults::Parser::init()
{
  state = s_start;

  for (int s=0; s<=s_stop; s++)
    for (int t=0; t<=t_unknown; t++)
      tagfun[s][t] = &Parser::unknown;

  tagfun[s_start                              ][t_gama_local_adjustment          ] = &Parser::gama_local_adjustment;
  tagfun[s_gama_local_adjustment              ][t_error_xml                      ] = &Parser::error_xml;
  tagfun[s_error_xml                          ][t_description                    ] = &Parser::error_xml_description;
  tagfun[s_error_xml                          ][t_line_number                    ] = &Parser::error_xml_line_number;
  tagfun[s_gama_local_adjustment              ][t_description                    ] = &Parser::description;
  tagfun[s_description_end                    ][t_network_general_parameters     ] = &Parser::network_general_parameters;
  tagfun[s_network_general_parameters_end     ][t_network_processing_summary     ] = &Parser::network_processing_summary;
  tagfun[s_network_processing_summary         ][t_coordinates_summary            ] = &Parser::coordinates_summary;
  tagfun[s_coordinates_summary                ][t_coordinates_summary_adjusted   ] = &Parser::coordinates_summary_adjusted;
  tagfun[s_coordinates_summary_adjusted_end   ][t_coordinates_summary_constrained] = &Parser::coordinates_summary_constrained;
  tagfun[s_coordinates_summary_constrained_end][t_coordinates_summary_fixed      ] = &Parser::coordinates_summary_fixed;
  tagfun[s_coordinates_summary_adjusted       ][t_count_xyz                      ] = &Parser::count_xyz;
  tagfun[s_coordinates_summary_constrained    ][t_count_xyz                      ] = &Parser::count_xyz;
  tagfun[s_coordinates_summary_fixed          ][t_count_xyz                      ] = &Parser::count_xyz;
  tagfun[s_count_xyz_end                      ][t_count_xy                       ] = &Parser::count_xy;
  tagfun[s_count_xy_end                       ][t_count_z                        ] = &Parser::count_z;
  tagfun[s_coordinates_summary_end            ][t_observations_summary           ] = &Parser::observations_summary;
  tagfun[s_observations_summary               ][t_distances                      ] = &Parser::distances;
  tagfun[s_distances_end                      ][t_directions                     ] = &Parser::directions;
  tagfun[s_directions_end                     ][t_angles                         ] = &Parser::angles;
  tagfun[s_angles_end                         ][t_xyz_coords                     ] = &Parser::xyz_coords;
  tagfun[s_xyz_coords_end                     ][t_h_diffs                        ] = &Parser::h_diffs;
  tagfun[s_h_diffs_end                        ][t_z_angles                       ] = &Parser::z_angles;
  tagfun[s_z_angles_end                       ][t_s_dists                        ] = &Parser::s_dists;
  tagfun[s_s_dists_end                        ][t_vectors                        ] = &Parser::vectors;
  tagfun[s_vectors_end                        ][t_azimuths                       ] = &Parser::azimuths;
  tagfun[s_observations_summary_end           ][t_project_equations              ] = &Parser::project_equations;
  tagfun[s_project_equations                  ][t_equations                      ] = &Parser::equations;
  tagfun[s_equations_end                      ][t_unknowns                       ] = &Parser::unknowns;
  tagfun[s_unknowns_end                       ][t_degrees_of_freedom             ] = &Parser::degrees_of_freedom;
  tagfun[s_degrees_of_freedom_end             ][t_defect                         ] = &Parser::defect;
  tagfun[s_defect_end                         ][t_sum_of_squares                 ] = &Parser::sum_of_squares;
  tagfun[s_sum_of_squares_end                 ][t_connected_network              ] = &Parser::connected_network;
  tagfun[s_sum_of_squares_end                 ][t_disconnected_network           ] = &Parser::disconnected_network;
  tagfun[s_project_equations_end              ][t_standard_deviation             ] = &Parser::standard_deviation;
  tagfun[s_standard_deviation                 ][t_apriori                        ] = &Parser::apriori;
  tagfun[s_apriori_end                        ][t_aposteriori                    ] = &Parser::aposteriori;
  tagfun[s_aposteriori_end                    ][t_used                           ] = &Parser::used;
  tagfun[s_used_end                           ][t_probability                    ] = &Parser::probability;
  tagfun[s_probability_end                    ][t_ratio                          ] = &Parser::ratio;
  tagfun[s_ratio_end                          ][t_lower                          ] = &Parser::lower;
  tagfun[s_lower_end                          ][t_upper                          ] = &Parser::upper;
  tagfun[s_upper_end                          ][t_passed                         ] = &Parser::passed;
  tagfun[s_upper_end                          ][t_failed                         ] = &Parser::failed;
  tagfun[s_passed_end                         ][t_confidence_scale               ] = &Parser::confidence_scale;
  tagfun[s_network_processing_summary_end     ][t_coordinates                    ] = &Parser::coordinates;
  tagfun[s_coordinates                        ][t_fixed                          ] = &Parser::fixed;
  tagfun[s_fixed_end                          ][t_approximate                    ] = &Parser::approximate;
  tagfun[s_approximate_end                    ][t_adjusted                       ] = &Parser::adjusted;
  tagfun[s_fixed                              ][t_point                          ] = &Parser::point;
  tagfun[s_approximate                        ][t_point                          ] = &Parser::point;
  tagfun[s_adjusted                           ][t_point                          ] = &Parser::point;
  tagfun[s_point_end                          ][t_point                          ] = &Parser::point;
  tagfun[s_point                              ][t_id                             ] = &Parser::id;
  tagfun[s_id_end                             ][t_x                              ] = &Parser::x;
  tagfun[s_id_end                             ][t_z                              ] = &Parser::z;
  tagfun[s_x_end                              ][t_y                              ] = &Parser::y;
  tagfun[s_y_end                              ][t_z                              ] = &Parser::z;
  tagfun[s_adjusted_end                       ][t_orientation_shifts             ] = &Parser::orientation_shifts;
  tagfun[s_orientation_shifts                 ][t_orientation                    ] = &Parser::orientation;
  tagfun[s_orientation_end                    ][t_orientation                    ] = &Parser::orientation;
  tagfun[s_orientation                        ][t_id                             ] = &Parser::id;
  tagfun[s_id_end                             ][t_approx                         ] = &Parser::ors_approx;
  tagfun[s_ors_approx_end                     ][t_adj                            ] = &Parser::ors_adj;
  tagfun[s_orientation_shifts_end             ][t_cov_mat                        ] = &Parser::cov_mat;
  tagfun[s_cov_mat                            ][t_dim                            ] = &Parser::dim;
  tagfun[s_cov_mat_end                        ][t_original_index                 ] = &Parser::original_index;
  tagfun[s_dim_end                            ][t_band                           ] = &Parser::band;
  tagfun[s_flt_end                            ][t_flt                            ] = &Parser::flt;
  tagfun[s_original_index                     ][t_ind                            ] = &Parser::ind;
  tagfun[s_ind_end                            ][t_ind                            ] = &Parser::ind;
  tagfun[s_coordinates_end                    ][t_observations                   ] = &Parser::observations;
  tagfun[s_observation                        ][t_from                           ] = &Parser::from;
  tagfun[s_from_end                           ][t_to                             ] = &Parser::to;
  tagfun[s_from_end                           ][t_left                           ] = &Parser::left;
  tagfun[s_left_end                           ][t_right                          ] = &Parser::right;
  tagfun[s_to_end                             ][t_obs                            ] = &Parser::obs;
  tagfun[s_observation                        ][t_id                             ] = &Parser::obs_id;
  tagfun[s_obs_id_end                         ][t_obs                            ] = &Parser::obs;
  tagfun[s_obs_end                            ][t_adj                            ] = &Parser::obs_adj;
  tagfun[s_obs_adj_end                        ][t_stdev                          ] = &Parser::stdev;
  tagfun[s_stdev_end                          ][t_qrr                            ] = &Parser::obs_qrr;
  tagfun[s_obs_qrr_end                        ][t_f                              ] = &Parser::obs_f;
  tagfun[s_obs_f_end                          ][t_std_residual                   ] = &Parser::std_residual;
  tagfun[s_std_residual_end                   ][t_err_obs                        ] = &Parser::err_obs;
  tagfun[s_err_obs_end                        ][t_err_adj                        ] = &Parser::err_adj;
  tagfun[s_observations                       ][t_distance                       ] = &Parser::observation;
  tagfun[s_observations                       ][t_direction                      ] = &Parser::observation;
  tagfun[s_observations                       ][t_angle                          ] = &Parser::observation;
  tagfun[s_observations                       ][t_slope_distance                 ] = &Parser::observation;
  tagfun[s_observations                       ][t_zenith_angle                   ] = &Parser::observation;
  tagfun[s_observations                       ][t_azimuth                        ] = &Parser::observation;
  tagfun[s_observations                       ][t_dx                             ] = &Parser::observation;
  tagfun[s_observations                       ][t_dy                             ] = &Parser::observation;
  tagfun[s_observations                       ][t_dz                             ] = &Parser::observation;
  tagfun[s_observations                       ][t_height_diff                    ] = &Parser::observation;
  tagfun[s_observations                       ][t_coordinate_x                   ] = &Parser::observation;
  tagfun[s_observations                       ][t_coordinate_y                   ] = &Parser::observation;
  tagfun[s_observations                       ][t_coordinate_z                   ] = &Parser::observation;
}


void LocalNetworkAdjustmentResults::Parser::unknown(bool)
{
  error("illegal context or unknown tag <" + name + ">");
}


int LocalNetworkAdjustmentResults::Parser::tag(const char* c)
{
  name = c;

  switch (*c)
    {
    case 'a':
      if (!strcmp(c, "adj"                       )) return t_adj;
      if (!strcmp(c, "adjusted"                  )) return t_adjusted;
      if (!strcmp(c, "angle"                     )) return t_angle;
      if (!strcmp(c, "angles"                    )) return t_angles;
      if (!strcmp(c, "apriori"                   )) return t_apriori;
      if (!strcmp(c, "aposteriori"               )) return t_aposteriori;
      if (!strcmp(c, "approx"                    )) return t_approx;
      if (!strcmp(c, "approximate"               )) return t_approximate;
      if (!strcmp(c, "azimuth"                   )) return t_azimuth;
      if (!strcmp(c, "azimuths"                  )) return t_azimuths;
      break;
    case 'b':
      if (!strcmp(c, "band"                      )) return t_band;
      break;
    case 'c':
      if (!strcmp(c, "confidence-scale"          )) return t_confidence_scale;
      if (!strcmp(c, "connected-network"         )) return t_connected_network;
      if (!strcmp(c, "coordinate-x"              )) return t_coordinate_x;
      if (!strcmp(c, "coordinate-y"              )) return t_coordinate_y;
      if (!strcmp(c, "coordinate-z"              )) return t_coordinate_z;
      if (!strcmp(c, "coordinates"               )) return t_coordinates;
      if (!strcmp(c, "coordinates-summary"       )) return t_coordinates_summary;
      if (!strcmp(c, "coordinates-summary-adjusted")) return t_coordinates_summary_adjusted;
      if (!strcmp(c, "coordinates-summary-constrained")) return t_coordinates_summary_constrained;
      if (!strcmp(c, "coordinates-summary-fixed" )) return t_coordinates_summary_fixed;
      if (!strcmp(c, "count-xyz"                 )) return t_count_xyz;
      if (!strcmp(c, "count-xy"                  )) return t_count_xy;
      if (!strcmp(c, "count-z"                   )) return t_count_z;
      if (!strcmp(c, "cov-mat"                   )) return t_cov_mat;
      break;
    case 'd':
      if (!strcmp(c, "defect"                    )) return t_defect;
      if (!strcmp(c, "degrees-of-freedom"        )) return t_degrees_of_freedom;
      if (!strcmp(c, "description"               )) return t_description;
      if (!strcmp(c, "dim"                       )) return t_dim;
      if (!strcmp(c, "direction"                 )) return t_direction;
      if (!strcmp(c, "directions"                )) return t_directions;
      if (!strcmp(c, "distance"                  )) return t_distance;
      if (!strcmp(c, "distances"                 )) return t_distances;
      if (!strcmp(c, "disconnected-network"      )) return t_disconnected_network;
      if (!strcmp(c, "dx"                        )) return t_dx;
      if (!strcmp(c, "dy"                        )) return t_dy;
      if (!strcmp(c, "dz"                        )) return t_dz;
      break;
    case 'e':
      if (!strcmp(c, "equations"                 )) return t_equations;
      if (!strcmp(c, "err-adj"                   )) return t_err_adj;
      if (!strcmp(c, "err-obs"                   )) return t_err_obs;
      if (!strcmp(c, "error"                     )) return t_error_xml;
      break;
    case 'f':
      if (!strcmp(c, "f"                         )) return t_f;
      if (!strcmp(c, "failed"                    )) return t_failed;
      if (!strcmp(c, "fixed"                     )) return t_fixed;
      if (!strcmp(c, "flt"                       )) return t_flt;
      if (!strcmp(c, "from"                      )) return t_from;
      break;
    case 'g':
      if (!strcmp(c, "gama-local-adjustment"     )) return t_gama_local_adjustment;
      break;
    case 'h':
      if (!strcmp(c, "h-diffs"                   )) return t_h_diffs;
      if (!strcmp(c, "height-diff"               )) return t_height_diff;
      break;
    case 'i':
      if (!strcmp(c, "id"                        )) return t_id;
      if (!strcmp(c, "ind"                       )) return t_ind;
      break;
    case 'l':
      if (!strcmp(c, "left"                      )) return t_left;
      if (!strcmp(c, "lower"                     )) return t_lower;
      if (!strcmp(c, "lineNumber"                )) return t_line_number;
      break;
    case 'n':
      if (!strcmp(c, "network-general-parameters")) return t_network_general_parameters;
      if (!strcmp(c, "network-processing-summary")) return t_network_processing_summary;
      break;
    case 'o':
      if (!strcmp(c, "obs"                       )) return t_obs;
      if (!strcmp(c, "observations"              )) return t_observations;
      if (!strcmp(c, "observations-summary"      )) return t_observations_summary;
      if (!strcmp(c, "orientation-shifts"        )) return t_orientation_shifts;
      if (!strcmp(c, "orientation"               )) return t_orientation;
      if (!strcmp(c, "original-index"            )) return t_original_index;
      break;
    case 'p':
      if (!strcmp(c, "passed"                    )) return t_passed;
      if (!strcmp(c, "point"                     )) return t_point;
      if (!strcmp(c, "probability"               )) return t_probability;
      if (!strcmp(c, "project-equations"         )) return t_project_equations;
      break;
    case 'q':
      if (!strcmp(c, "qrr"                       )) return t_qrr;
      break;
    case 'r':
      if (!strcmp(c, "ratio"                     )) return t_ratio;
      if (!strcmp(c, "right"                     )) return t_right;
      break;
    case 's':
      if (!strcmp(c, "s-dists"                   )) return t_s_dists;
      if (!strcmp(c, "slope-distance"                   )) return t_slope_distance;
      if (!strcmp(c, "standard-deviation"        )) return t_standard_deviation;
      if (!strcmp(c, "stdev"                     )) return t_stdev;
      if (!strcmp(c, "std-residual"              )) return t_std_residual;
      if (!strcmp(c, "sum-of-squares"            )) return t_sum_of_squares;
      break;
    case 't':
      if (!strcmp(c, "to"                        )) return t_to;
      break;
    case 'u':
      if (!strcmp(c, "unknowns"                  )) return t_unknowns;
      if (!strcmp(c, "upper"                     )) return t_upper;
      if (!strcmp(c, "used"                      )) return t_used;
      break;
    case 'v':
      if (!strcmp(c, "vectors"                   )) return t_vectors;
      break;
    case 'x':
      if (!strcmp(c, "x"                         )) return t_x;
      if (!strcmp(c, "xyz-coords"                )) return t_xyz_coords;
      break;
    case 'y':
      if (!strcmp(c, "y"                         )) return t_y;
    case 'z':
      if (!strcmp(c, "z"                         )) return t_z;
      if (!strcmp(c, "z-angles"                  )) return t_z_angles;
      if (!strcmp(c, "zenith-angle"              )) return t_zenith_angle;
      break;

      /* constrained coordinates */

    case 'X':
      if (!strcmp(c, "X"                         )) { point_con_x = true; return t_x; }
      break;
    case 'Y':
      if (!strcmp(c, "Y"                         )) { point_con_y = true; return t_y; }
      break;
    case 'Z':
      if (!strcmp(c, "Z"                         )) { point_con_z = true; return t_z; }
      break;
    }

  return error("unknown tag");
}


int LocalNetworkAdjustmentResults::Parser::get_int()
{
  string::const_iterator b=data.begin();
  string::const_iterator e=data.end();
  if (!IsInteger(b,e)) error("integer syntax error");
  istringstream istr(data);
  int n;
  istr >> n;

  return n;
}


double LocalNetworkAdjustmentResults::Parser::get_float()
{
  string::const_iterator b=data.begin();
  string::const_iterator e=data.end();
  if (!IsFloat(b,e)) error("float syntax error");
  istringstream istr(data);
  double n;
  istr >> n;

  return n;
}


string LocalNetworkAdjustmentResults::Parser::get_string()
{
  // string::const_iterator b=data.begin();
  // string::const_iterator e=data.end();
  // if (!IsFloat(b,e)) error("float syntax error");
  istringstream istr(data);
  string n;
  istr >> n;

  return n;
}


void LocalNetworkAdjustmentResults::Parser::gama_local_adjustment(bool start)
{
  if (start)
    {
      stack.push(&Parser::gama_local_adjustment);
      set_state(s_gama_local_adjustment);

      while (*attributes)
        {
          string atr = *attributes++;
          string val = *attributes++;

          if (atr == "xmlns")
            {
              if (val != XSD_GAMA_LOCAL_ADJUSTMENT)
                {
                  error("bad namespace xmlns=\"" + val + "\"");
                  return;
                }
            }
          else
            {
              error("unknown attribute");
              return;
            }
        }
    }
  else
    {
      set_state(s_stop);
    }
}


void LocalNetworkAdjustmentResults::Parser::error_xml(bool start)
{
  if (start)
    {
      stack.push(&Parser::error_xml);
      set_state(s_error_xml);

      adj->xmlerror.clear();
      while (*attributes)
        {
          string atr = *attributes++;
          string val = *attributes++;

          if (atr == "category")
            adj->xmlerror.setCategory(val);
          else
            {
              error("bad attribute in <error>");
              return;
            }
        }
      if (adj->xmlerror.getCategory().empty())
        {
          error("Attribute 'category' is not defined in <error>");
          return;
        }
    }
  else
    {
      set_state(s_error_xml_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::error_xml_description(bool start)
{
  if (start)
    {
      stack.push(&Parser::error_xml_description);
    }
  else
    {
      adj->xmlerror.setDescription(data);
      set_state(s_error_xml);
    }
}


void LocalNetworkAdjustmentResults::Parser::error_xml_line_number(bool start)
{
  if (start)
    {
      stack.push(&Parser::error_xml_line_number);
    }
  else
    {
      adj->xmlerror.setLineNumber(get_int());
      set_state(s_error_xml);
    }
}


void LocalNetworkAdjustmentResults::Parser::description(bool start)
{
  if (start)
    {
      stack.push(&Parser::description);
      set_state(s_description);
    }
  else
    {
      adj->description = data;
      set_state(s_description_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::network_general_parameters(bool start)
{
  if (start)
    {
      stack.push(&Parser::network_general_parameters);
      while (*attributes)
        {
          string atr = *attributes++;
          string val = *attributes++;

          if (atr == "gama-local-version")
            adj->network_general_parameters.gama_local_version = val;
          else if (atr == "gama-local-algorithm")
            adj->network_general_parameters.gama_local_algorithm = val;
          else if (atr == "gama-local-compiler")
            adj->network_general_parameters.gama_local_compiler = val;
          else if (atr == "axes-xy")
            adj->network_general_parameters.axes_xy = val;
          else if (atr == "angles")
            adj->network_general_parameters.angles = val;
          else if (atr == "epoch")
            adj->network_general_parameters.epoch = val;
          else if (atr == "latitude")
            adj->network_general_parameters.latitude = val;
          else if (atr == "ellipsoid")
            adj->network_general_parameters.ellipsoid = val;
          else
            {
              error("unknown attribute");
              return;
            }
        }
    }
  else
    {
      set_state(s_network_general_parameters_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::network_processing_summary(bool start)
{
  if (start)
    {
      stack.push(&Parser::network_processing_summary);
      set_state(s_network_processing_summary);
    }
  else
    {
      set_state(s_network_processing_summary_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::coordinates_summary(bool start)
{
  if (start)
    {
      stack.push(&Parser::coordinates_summary);
      set_state(s_coordinates_summary);
    }
  else
    {
      set_state(s_coordinates_summary_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::coordinates_summary_adjusted(bool start)
{
  if (start)
    {
      stack.push(&Parser::coordinates_summary_adjusted);
      set_state(s_coordinates_summary_adjusted);
      coordinates_summary_stage = 1;
    }
  else
    {
      set_state(s_coordinates_summary_adjusted_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::coordinates_summary_constrained(bool start)
{
  if (start)
    {
      stack.push(&Parser::coordinates_summary_constrained);
      set_state(s_coordinates_summary_constrained);
      coordinates_summary_stage = 2;
    }
  else
    {
      set_state(s_coordinates_summary_constrained_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::coordinates_summary_fixed(bool start)
{
  if (start)
    {
      stack.push(&Parser::coordinates_summary_fixed);
      set_state(s_coordinates_summary_fixed);
      coordinates_summary_stage = 3;
    }
  else
    {
      set_state(s_coordinates_summary_fixed_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::count_xyz(bool start)
{
  if (start)
    {
      stack.push(&Parser::count_xyz);
      set_state(s_count_xyz);
    }
  else
    {
      switch (coordinates_summary_stage)
        {
        case 1: adj->coordinates_summary.adjusted.xyz    = get_int();  break;
        case 2: adj->coordinates_summary.constrained.xyz = get_int();  break;
        case 3: adj->coordinates_summary.fixed.xyz       = get_int();  break;
        default:
          error("internal error");
        }

      set_state(s_count_xyz_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::count_xy(bool start)
{
  if (start)
    {
      stack.push(&Parser::count_xy);
      set_state(s_count_xy);
    }
  else
    {
      switch (coordinates_summary_stage)
        {
        case 1: adj->coordinates_summary.adjusted.xy    = get_int();  break;
        case 2: adj->coordinates_summary.constrained.xy = get_int();  break;
        case 3: adj->coordinates_summary.fixed.xy       = get_int();  break;
        default:
          error("internal error");
        }

      set_state(s_count_xy_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::count_z(bool start)
{
  if (start)
    {
      stack.push(&Parser::count_z);
      set_state(s_count_z);
    }
  else
    {
      switch (coordinates_summary_stage)
        {
        case 1: adj->coordinates_summary.adjusted.z    = get_int();  break;
        case 2: adj->coordinates_summary.constrained.z = get_int();  break;
        case 3: adj->coordinates_summary.fixed.z       = get_int();  break;
        default:
          error("internal error");
        }

      set_state(s_count_z_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::observations_summary(bool start)
{
  if (start)
    {
      stack.push(&Parser::observations_summary);
      set_state(s_observations_summary);
    }
  else
    {
      set_state(s_observations_summary_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::distances(bool start)
{
  if (start)
    {
      stack.push(&Parser::distances);
      set_state(s_distances);
    }
  else
    {
      adj->observations_summary.distances = get_int();
      set_state(s_distances_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::directions(bool start)
{
  if (start)
    {
      stack.push(&Parser::directions);
      set_state(s_directions);
    }
  else
    {
      adj->observations_summary.directions = get_int();
      set_state(s_directions_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::angles(bool start)
{
  if (start)
    {
      stack.push(&Parser::angles);
      set_state(s_angles);
    }
  else
    {
      adj->observations_summary.angles = get_int();
      set_state(s_angles_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::xyz_coords(bool start)
{
  if (start)
    {
      stack.push(&Parser::xyz_coords);
      set_state(s_xyz_coords);
    }
  else
    {
      adj->observations_summary.xyz_coords = get_int();
      set_state(s_xyz_coords_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::h_diffs(bool start)
{
  if (start)
    {
      stack.push(&Parser::h_diffs);
      set_state(s_h_diffs);
    }
  else
    {
      adj->observations_summary.h_diffs = get_int();
      set_state(s_h_diffs_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::z_angles(bool start)
{
  if (start)
    {
      stack.push(&Parser::z_angles);
      set_state(s_z_angles);
    }
  else
    {
      adj->observations_summary.z_angles = get_int();
      set_state(s_z_angles_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::s_dists(bool start)
{
  if (start)
    {
      stack.push(&Parser::s_dists);
      set_state(s_s_dists);
    }
  else
    {
      adj->observations_summary.s_dists = get_int();
      set_state(s_s_dists_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::vectors(bool start)
{
  if (start)
    {
      stack.push(&Parser::vectors);
      set_state(s_vectors);
    }
  else
    {
      adj->observations_summary.vectors = get_int();
      set_state(s_vectors_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::azimuths(bool start)
{
  if (start)
    {
      stack.push(&Parser::azimuths);
      set_state(s_azimuths);
    }
  else
    {
      adj->observations_summary.azimuths = get_int();
      set_state(s_azimuths_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::project_equations(bool start)
{
  if (start)
    {
      stack.push(&Parser::project_equations);
      set_state(s_project_equations);
    }
  else
    {
      set_state(s_project_equations_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::equations(bool start)
{
  if (start)
    {
      stack.push(&Parser::equations);
      set_state(s_equations);
    }
  else
    {
      adj->project_equations.equations = get_int();
      set_state(s_equations_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::unknowns(bool start)
{
  if (start)
    {
      stack.push(&Parser::unknowns);
      set_state(s_unknowns);
    }
  else
    {
      adj->project_equations.unknowns = get_int();
      set_state(s_unknowns_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::degrees_of_freedom(bool start)
{
  if (start)
    {
      stack.push(&Parser::degrees_of_freedom);
      set_state(s_degrees_of_freedom);
    }
  else
    {
      adj->project_equations.degrees_of_freedom = get_int();
      set_state(s_degrees_of_freedom_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::defect(bool start)
{
  if (start)
    {
      stack.push(&Parser::defect);
      set_state(s_defect);
    }
  else
    {
      adj->project_equations.defect = get_int();
      set_state(s_defect_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::sum_of_squares(bool start)
{
  if (start)
    {
      stack.push(&Parser::sum_of_squares);
      set_state(s_sum_of_squares);
    }
  else
    {
      adj->project_equations.sum_of_squares = get_float();
      set_state(s_sum_of_squares_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::connected_network(bool start)
{
  if (start)
    {
      stack.push(&Parser::connected_network);
      set_state(s_connected_network);
    }
  else
    {
      check_and_clear_data();
      adj->project_equations.connected_network = true;
      set_state(s_connected_network_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::disconnected_network(bool start)
{
  if (start)
    {
      stack.push(&Parser::disconnected_network);
      set_state(s_disconnected_network);
    }
  else
    {
      check_and_clear_data();
      adj->project_equations.connected_network = false;
      set_state(s_disconnected_network_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::standard_deviation(bool start)
{
  if (start)
    {
      stack.push(&Parser::standard_deviation);
      set_state(s_standard_deviation);
    }
  else
    {
      set_state(s_standard_deviation_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::apriori(bool start)
{
  if (start)
    {
      stack.push(&Parser::apriori);
      set_state(s_apriori);
    }
  else
    {
      adj->standard_deviation.apriori = get_float();
      set_state(s_apriori_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::aposteriori(bool start)
{
  if (start)
    {
      stack.push(&Parser::aposteriori);
      set_state(s_aposteriori);
    }
  else
    {
      adj->standard_deviation.aposteriori = get_float();
      set_state(s_aposteriori_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::used(bool start)
{
  if (start)
    {
      stack.push(&Parser::used);
      set_state(s_used);
    }
  else
    {
      string s = get_string();
      if (s != "apriori" && s != "aposteriori")
        error("bad value, can be apriori/aposteriori");
      adj->standard_deviation.using_aposteriori = (s == "aposteriori");
      set_state(s_used_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::probability(bool start)
{
  if (start)
    {
      stack.push(&Parser::probability);
      set_state(s_probability);
    }
  else
    {
      adj->standard_deviation.probability = get_float();
      set_state(s_probability_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::ratio(bool start)
{
  if (start)
    {
      stack.push(&Parser::ratio);
      set_state(s_ratio);
    }
  else
    {
      adj->standard_deviation.ratio = get_float();
      set_state(s_ratio_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::lower(bool start)
{
  if (start)
    {
      stack.push(&Parser::lower);
      set_state(s_lower);
    }
  else
    {
      adj->standard_deviation.lower = get_float();
      set_state(s_lower_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::upper(bool start)
{
  if (start)
    {
      stack.push(&Parser::upper);
      set_state(s_upper);
    }
  else
    {
      adj->standard_deviation.upper = get_float();
      set_state(s_upper_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::passed(bool start)
{
  if (start)
    {
      stack.push(&Parser::passed);
      set_state(s_passed);
    }
  else
    {
      check_and_clear_data();
      adj->standard_deviation.passed = true;
      set_state(s_passed_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::failed(bool start)
{
  if (start)
    {
      stack.push(&Parser::failed);
      set_state(s_failed);
    }
  else
    {
      check_and_clear_data();
      adj->standard_deviation.passed = false;
      set_state(s_passed_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::confidence_scale(bool start)
{
  if (start)
    {
      stack.push(&Parser::confidence_scale);
      set_state(s_confidence_scale);
    }
  else
    {
      adj->standard_deviation.confidence_scale = get_float();
      set_state(s_confidence_scale_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::coordinates(bool start)
{
  if (start)
    {
      stack.push(&Parser::coordinates);
      set_state(s_coordinates);
    }
  else
    {
      set_state(s_coordinates_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::fixed(bool start)
{
  if (start)
    {
      pointlist = &adj->fixed_points;
      tmp_point_adjusted = false;

      stack.push(&Parser::fixed);
      set_state(s_fixed);
    }
  else
    {
      set_state(s_fixed_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::approximate(bool start)
{
  if (start)
    {
      pointlist = &adj->approximate_points;
      tmp_point_adjusted = false;

      stack.push(&Parser::approximate);
      set_state(s_approximate);
    }
  else
    {
      set_state(s_approximate_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::adjusted(bool start)
{
  if (start)
    {
      pointlist = &adj->adjusted_points;
      tmp_point_adjusted = true;
      tmp_adj_index = 0;

      stack.push(&Parser::adjusted);
      set_state(s_adjusted);
    }
  else
    {
      set_state(s_adjusted_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::point(bool start)
{
  if (start)
    {
      tmp_point.clear();
      point_has_x = point_has_y = point_has_z = false;
      point_con_x = point_con_y = point_con_z = false;  // constrained x,y,z

      stack.push(&Parser::point);
      set_state(s_point);
    }
  else
    {
      // if (!(state == s_y_end || state == s_z_end))
      //   error("point must have both x and y");
      if (point_has_x != point_has_y) error("point must have both x and y");
      if (point_con_x != point_con_y) error("point must have both X and Y");

      tmp_point.id  = tmp_id;
      tmp_point.hxy = point_has_x && point_has_y;
      tmp_point.hz  = point_has_z;
      tmp_point.cxy = point_con_x && point_con_y;
      tmp_point.cz  = point_con_z;

      if (tmp_point_adjusted)
        {
          if (tmp_point.hxy)
            {
              tmp_point.indx = ++tmp_adj_index;
              tmp_point.indy = ++tmp_adj_index;
            }
          if (tmp_point.hz)
            {
              tmp_point.indz = ++tmp_adj_index;
            }
        }

      pointlist->push_back(tmp_point);

      set_state(s_point_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::id(bool start)
{
  if (start)
    {
      stack.push(&Parser::id);
      set_state(s_id);
    }
  else
    {
      tmp_id = get_string();
      set_state(s_id_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::obs_id(bool start)
{
  if (start)
    {
      stack.push(&Parser::obs_id);
      set_state(s_obs_id);
    }
  else
    {
      tmp_obs.from = get_string();
      set_state(s_obs_id_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::x(bool start)
{
  if (start)
    {
      stack.push(&Parser::x);
      set_state(s_x);
    }
  else
    {
      tmp_point.x = get_float();
      point_has_x = true;
      set_state(s_x_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::y(bool start)
{
  if (start)
    {
      stack.push(&Parser::y);
      set_state(s_y);
    }
  else
    {
      tmp_point.y = get_float();
      point_has_y = true;
      set_state(s_y_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::z(bool start)
{
  if (start)
    {
      stack.push(&Parser::z);
      set_state(s_z);
    }
  else
    {
      tmp_point.z = get_float();
      point_has_z = true;
      set_state(s_z_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::orientation_shifts(bool start)
{
  if (start)
    {
      stack.push(&Parser::orientation_shifts);
      set_state(s_orientation_shifts);
    }
  else
    {
       set_state(s_orientation_shifts_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::orientation(bool start)
{
  if (start)
    {
      stack.push(&Parser::orientation);
      set_state(s_orientation);
    }
  else
    {
      if (state != s_ors_adj_end) error("missing tag <approx> or <adj>");

      tmp_orientation.id = tmp_id;
      tmp_orientation.index = ++tmp_adj_index;
      adj->orientations.push_back(tmp_orientation);

      set_state(s_orientation_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::ors_approx(bool start)
{
  if (start)
    {
      stack.push(&Parser::ors_approx);
      set_state(s_ors_approx);
    }
  else
    {
      tmp_orientation.approx = get_float();
      set_state(s_ors_approx_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::ors_adj(bool start)
{
  if (start)
    {
      stack.push(&Parser::ors_adj);
      set_state(s_ors_adj);
    }
  else
    {
      tmp_orientation.adj = get_float();
      set_state(s_ors_adj_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::cov_mat(bool start)
{
  if (start)
    {
      stack.push(&Parser::cov_mat);
      set_state(s_cov_mat);
    }
  else
    {
      if (tmp_i != tmp_e) error("bad number of elements in covariance matrix");
      set_state(s_cov_mat_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::original_index(bool start)
{
  if (start)
    {
      adj->original_index.clear();
      adj->original_index.push_back(-1);  // indexing from 1

      stack.push(&Parser::original_index);
      set_state(s_original_index);
    }
  else
    {
      set_state(s_original_index_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::dim(bool start)
{
  if (start)
    {
      stack.push(&Parser::dim);
      set_state(s_dim);
    }
  else
    {
      tmp_dim = get_int();
      set_state(s_dim_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::band(bool start)
{
  if (start)
    {
      stack.push(&Parser::band);
      set_state(s_band);
    }
  else
    {
      tmp_band = get_int();
      adj->cov.reset(tmp_dim, tmp_band);

      tmp_i = adj->cov.begin();
      tmp_e = adj->cov.end();

      set_state(s_flt_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::flt(bool start)
{
  if (start)
    {
      stack.push(&Parser::flt);
      set_state(s_flt);
    }
  else
    {
      if (tmp_i != tmp_e)  *tmp_i++ = get_float();
      set_state(s_flt_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::ind(bool start)
{
  if (start)
    {
      stack.push(&Parser::ind);
      set_state(s_ind);
    }
  else
    {
      adj->original_index.push_back( get_int() );
      set_state(s_ind_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::observations(bool start)
{
  if (start)
    {
      adj->obslist.clear();
      stack.push(&Parser::observations);
      set_state(s_observations);
    }
  else
    {
      set_state(s_observations_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::observation(bool start)
{
  if (start)
    {
      tmp_obs.clear();
      tmp_obs.xml_tag = tmp_tag;

      stack.push(&Parser::observation);
      set_state(s_observation);
    }
  else
    {
      if (state != s_obs_f_end        &&
          state != s_std_residual_end &&
          state != s_err_adj_end        ) error("observation attribute(s) missing");
      adj->obslist.push_back(tmp_obs);
      set_state(s_observations);
    }
}


void LocalNetworkAdjustmentResults::Parser::from(bool start)
{
  if (start)
    {
      stack.push(&Parser::from);
      set_state(s_from);
    }
  else
    {
      tmp_obs.from = get_string();
      set_state(s_from_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::to(bool start)
{
  if (start)
    {
      stack.push(&Parser::to);
      set_state(s_to);
    }
  else
    {
      tmp_obs.to = get_string();
      set_state(s_to_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::left(bool start)
{
  if (start)
    {
      stack.push(&Parser::left);
      set_state(s_left);
    }
  else
    {
      tmp_obs.left = get_string();
      set_state(s_left_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::right(bool start)
{
  if (start)
    {
      stack.push(&Parser::right);
      set_state(s_right);
    }
  else
    {
      tmp_obs.right = get_string();
      set_state(s_to_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::obs(bool start)
{
  if (start)
    {
      stack.push(&Parser::obs);
      set_state(s_obs);
    }
  else
    {
      tmp_obs.obs = get_float();
      set_state(s_obs_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::obs_adj(bool start)
{
  if (start)
    {
      stack.push(&Parser::obs_adj);
      set_state(s_obs_adj);
    }
  else
    {
      tmp_obs.adj = get_float();
      set_state(s_obs_adj_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::stdev(bool start)
{
  if (start)
    {
      stack.push(&Parser::stdev);
      set_state(s_stdev);
    }
  else
    {
      tmp_obs.stdev = get_float();
      set_state(s_stdev_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::obs_qrr(bool start)
{
  if (start)
    {
      stack.push(&Parser::obs_qrr);
      set_state(s_obs_qrr);
    }
  else
    {
      tmp_obs.qrr = get_float();
      set_state(s_obs_qrr_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::obs_f(bool start)
{
  if (start)
    {
      stack.push(&Parser::obs_f);
      set_state(s_obs_f);
    }
  else
    {
      tmp_obs.f = get_float();
      set_state(s_obs_f_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::std_residual(bool start)
{
  if (start)
    {
      stack.push(&Parser::std_residual);
      set_state(s_std_residual);
    }
  else
    {
      tmp_obs.std_residual = get_float();
      set_state(s_std_residual_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::err_obs(bool start)
{
  if (start)
    {
      stack.push(&Parser::err_obs);
      set_state(s_err_obs);
    }
  else
    {
      tmp_obs.err_obs = get_string();
      set_state(s_err_obs_end);
    }
}


void LocalNetworkAdjustmentResults::Parser::err_adj(bool start)
{
  if (start)
    {
      stack.push(&Parser::err_adj);
      set_state(s_err_adj);
    }
  else
    {
      tmp_obs.err_adj = get_string();
      set_state(s_err_adj_end);
    }
}
