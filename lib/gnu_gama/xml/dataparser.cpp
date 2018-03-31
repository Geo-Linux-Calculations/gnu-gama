/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2002, 2005, 2018  Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/radian.h>
#include <cstring>

using namespace std;
using namespace GNU_gama;


const char* const DataParser::xml_start =
    "<?xml version=\"1.0\" ?>\n"
    "<!DOCTYPE gnu-gama-data SYSTEM \"gnu-gama-data.dtd\">\n\n"
    "<gnu-gama-data>\n";

const char* const DataParser::xml_end =
    "</gnu-gama-data>\n";


DataParser::~DataParser()
{
  close_adj();
  close_g3();
  close_g3adj();

  delete adj_sparse_mat;
  delete adj_block_diagonal;
  delete adj_array;
}


DataParser::DataParser(std::list<DataObject::Base*>& obs) : objects(obs)
{
  adj   = 0;
  g3    = 0;
  g3adj = 0;

  adj_sparse_mat = 0;
  adj_block_diagonal = 0;
  adj_array = 0;

  point = 0;

  // initial parser state and implicit handlers

  state = s_start;

  for (int s=s_error; s<=s_stop; s++)
    {
      for (int t=0; t<=t_unused; t++)
        {
          next[s][t] = s_error;
          stag[s][t] = &DataParser::parser_error;
        }
      after[s] = s_error;
      data [s] = &DataParser::white_spaces;
      etag [s] = &DataParser::end_tag;
    }


  // .....  <gnu-gama-data>  .........................................

  init(s_start, t_gama_data,
       s_gama_data, 0, s_stop,
       &DataParser::gama_data, 0, 0);


  // .....  <g3-model>  ..............................................

  init_g3();

  // .....  <g3-adjustment-reults>  ..................................

  init_g3adj();

  // .....  <text>  ..................................................

  init(s_gama_data, t_text,
       s_text, 0, 0,
       0, &DataParser::add_text, &DataParser::text);

  // .....  <adj-input-data>  ........................................

  init_adj();

  // .................................................................
}

// #######################################################
// #                                                     #
// # states:      s         n         z=n        a=s     #
// #              |         |         |          |       #
// # tag t:        <...t...>           </...t...>        #
// #                                                     #
// # functions:    [ Stag  ][  Data   ][  Etag  ]        #
// #                                                     #
// #######################################################

void DataParser::init(int s,   int t,           // current state, tag
                      int n,   int z,   int a,  // states: next , end, after
                      Stag s_, Data d_, Etag e_,
                      int z2)
{
  if (z == 0)  z = n;
  if (a == 0)  a = s;

  next [s][t] = n;
  after[z]    = a;

  if (s_) stag[s][t] = s_;
  else    stag[s][t] = &DataParser::start_tag;

  if (d_) data[n] = d_;

  if (e_) etag[z] = e_;

  if (z2)         // alternative end-state
    {
      after[z2] = a;
      etag [z2] = e_;
    }

}

DataParser::data_tag DataParser::tag(const char* c)
{
  switch (*c)
    {
    case 'a':
      if (!strcmp(c, "a"                         )) return t_a;
      if (!strcmp(c, "algorithm"                 )) return t_algorithm;
      if (!strcmp(c, "angle"                     )) return t_angle;
      if (!strcmp(c, "adj-input-data"            )) return t_adj_input_data;
      if (!strcmp(c, "adjusted"                  )) return t_adjusted;
      if (!strcmp(c, "adjusted-observations"     )) return t_adj_observations;
      if (!strcmp(c, "adjustment-results"        )) return t_adj_results;
      if (!strcmp(c, "adjustment-statistics"     )) return t_adj_statistics;
      if (!strcmp(c, "angular-units-degrees"     )) return t_ang_degrees;
      if (!strcmp(c, "angular-units-gons"        )) return t_ang_gons;
      if (!strcmp(c, "apriori-standard-deviation")) return t_apriori_sd;
      if (!strcmp(c, "apriori-variance"          )) return t_apriori_var;
      if (!strcmp(c, "aposteriori-variance"      )) return t_aposteriori_var;
      if (!strcmp(c, "array"                     )) return t_array;
      if (!strcmp(c, "azimuth"                   )) return t_azimuth;
      break;
    case 'b':
      if (!strcmp(c, "b"                         )) return t_b;
      if (!strcmp(c, "b-adjusted"                )) return t_b_adjusted;
      if (!strcmp(c, "b-correction"              )) return t_b_correction;
      if (!strcmp(c, "b-given"                   )) return t_b_given;
      if (!strcmp(c, "band"                      )) return t_band;
      if (!strcmp(c, "block-diagonal"            )) return t_block_diagonal;
      if (!strcmp(c, "blocks"                    )) return t_blocks;
      if (!strcmp(c, "block"                     )) return t_block;
      break;
    case 'c' :
      if (!strcmp(c, "caption"                   )) return t_caption;
      if (!strcmp(c, "cee"                       )) return t_cee;
      if (!strcmp(c, "ceu"                       )) return t_ceu;
      if (!strcmp(c, "cnn"                       )) return t_cnn;
      if (!strcmp(c, "cne"                       )) return t_cne;
      if (!strcmp(c, "cnu"                       )) return t_cnu;
      if (!strcmp(c, "confidence-level"          )) return t_conf_level;
      if (!strcmp(c, "cols"                      )) return t_cols;
      if (!strcmp(c, "constants"                 )) return t_constants;
      if (!strcmp(c, "constr"                    )) return t_constr;
      // (!strcmp(c, "constr-height"             )) return t_constr_height;
      // (!strcmp(c, "constr-position"           )) return t_constr_position;
      if (!strcmp(c, "cov-mat"                   )) return t_covmat;
      if (!strcmp(c, "cuu"                       )) return t_cuu;
      if (!strcmp(c, "cxx"                       )) return t_cxx;
      if (!strcmp(c, "cxy"                       )) return t_cxy;
      if (!strcmp(c, "cxz"                       )) return t_cxz;
      if (!strcmp(c, "cyy"                       )) return t_cyy;
      if (!strcmp(c, "cyz"                       )) return t_cyz;
      if (!strcmp(c, "czz"                       )) return t_czz;
      break;
    case 'd' :
      if (!strcmp(c, "db"                        )) return t_db;
      if (!strcmp(c, "de"                        )) return t_de;
      if (!strcmp(c, "defect"                    )) return t_defect;
      if (!strcmp(c, "design-matrix-graph"       )) return t_design_m_graph;
      if (!strcmp(c, "dim"                       )) return t_dim;
      if (!strcmp(c, "distance"                  )) return t_dist;
      if (!strcmp(c, "dl"                        )) return t_dl;
      if (!strcmp(c, "dn"                        )) return t_dn;
      if (!strcmp(c, "du"                        )) return t_du;
      if (!strcmp(c, "dx"                        )) return t_dx;
      if (!strcmp(c, "dx-observed"               )) return t_dx_observed;
      if (!strcmp(c, "dx-residual"               )) return t_dx_residual;
      if (!strcmp(c, "dx-adjusted"               )) return t_dx_adjusted;
      if (!strcmp(c, "dx-stdev-obs"              )) return t_dx_stdev_obs;
      if (!strcmp(c, "dx-stdev-adj"              )) return t_dx_stdev_adj;
      if (!strcmp(c, "dy"                        )) return t_dy;
      if (!strcmp(c, "dy-observed"               )) return t_dy_observed;
      if (!strcmp(c, "dy-residual"               )) return t_dy_residual;
      if (!strcmp(c, "dy-adjusted"               )) return t_dy_adjusted;
      if (!strcmp(c, "dy-stdev-obs"              )) return t_dy_stdev_obs;
      if (!strcmp(c, "dy-stdev-adj"              )) return t_dy_stdev_adj;
      if (!strcmp(c, "dz"                        )) return t_dz;
      if (!strcmp(c, "dz-observed"               )) return t_dz_observed;
      if (!strcmp(c, "dz-residual"               )) return t_dz_residual;
      if (!strcmp(c, "dz-adjusted"               )) return t_dz_adjusted;
      if (!strcmp(c, "dz-stdev-obs"              )) return t_dz_stdev_obs;
      if (!strcmp(c, "dz-stdev-adj"              )) return t_dz_stdev_adj;
      break;
    case 'e':
      if (!strcmp(c, "e"                         )) return t_e;
      if (!strcmp(c, "e-fixed"                   )) return t_e_fixed;
      if (!strcmp(c, "e-free"                    )) return t_e_free;
      if (!strcmp(c, "e-constr"                  )) return t_e_constr;
      if (!strcmp(c, "e-unused"                  )) return t_e_unused;
      if (!strcmp(c, "ellipsoid"                 )) return t_ellipsoid;
      if (!strcmp(c, "equations"                 )) return t_equations;
      break;
    case 'f' :
      if (!strcmp(c, "fixed"                     )) return t_fixed;
      if (!strcmp(c, "flt"                       )) return t_flt;
      if (!strcmp(c, "from"                      )) return t_from;
      if (!strcmp(c, "from-dh"                   )) return t_from_dh;
      if (!strcmp(c, "free"                      )) return t_free;
      break;
    case 'g' :
      if (!strcmp(c, "g3-adjustment-results"     )) return t_g3_adj_results;
      if (!strcmp(c, "g3-model"                  )) return t_g3_model;
      if (!strcmp(c, "geoid"                     )) return t_geoid;
      if (!strcmp(c, "gnu-gama-data"             )) return t_gama_data;
      break;
    case 'h':
      if (!strcmp(c, "h"                         )) return t_h;
      if (!strcmp(c, "h-adjusted"                )) return t_h_adjusted;
      if (!strcmp(c, "h-correction"              )) return t_h_correction;
      if (!strcmp(c, "h-given"                   )) return t_h_given;
      if (!strcmp(c, "hdiff"                     )) return t_hdiff;
      if (!strcmp(c, "height"                    )) return t_height;
      if (!strcmp(c, "hobs"                      )) return t_hobs;
      break;
    case 'i':
      if (!strcmp(c, "id"                        )) return t_id;
      if (!strcmp(c, "ind"                       )) return t_ind;
      if (!strcmp(c, "int"                       )) return t_int;
      if (!strcmp(c, "inv-f"                     )) return t_inv_f;
      break;
    case 'l':
      if (!strcmp(c, "l"                         )) return t_l;
      if (!strcmp(c, "l-adjusted"                )) return t_l_adjusted;
      if (!strcmp(c, "l-correction"              )) return t_l_correction;
      if (!strcmp(c, "l-given"                   )) return t_l_given;
      if (!strcmp(c, "left"                      )) return t_left;
      if (!strcmp(c, "left-dh"                   )) return t_left_dh;
      break;
    case 'n':
      if (!strcmp(c, "n"                         )) return t_n;
      if (!strcmp(c, "n-fixed"                   )) return t_n_fixed;
      if (!strcmp(c, "n-free"                    )) return t_n_free;
      if (!strcmp(c, "n-constr"                  )) return t_n_constr;
      if (!strcmp(c, "n-unused"                  )) return t_n_unused;
      if (!strcmp(c, "nonz"                      )) return t_nonz;
      break;
    case 'o':
      if (!strcmp(c, "obs"                       )) return t_obs;
      if (!strcmp(c, "observed"                  )) return t_observed;
      break;
    case 'p':
      if (!strcmp(c, "parameters"                )) return t_parameters;
      if (!strcmp(c, "point"                     )) return t_point;
      break;
    case 'r':
      if (!strcmp(c, "reason"                    )) return t_reason;
      if (!strcmp(c, "redundancy"                )) return t_redundancy;
      if (!strcmp(c, "rejected"                  )) return t_rejected;
      if (!strcmp(c, "rejected-observations"     )) return t_rejected_obs;
      if (!strcmp(c, "residual"                  )) return t_residual;
      if (!strcmp(c, "right"                     )) return t_right;
      if (!strcmp(c, "right-dh"                  )) return t_right_dh;
      if (!strcmp(c, "row"                       )) return t_row;
      if (!strcmp(c, "rows"                      )) return t_rows;
      break;
    case 's':
      if (!strcmp(c, "stdev"                     )) return t_stdev;
      if (!strcmp(c, "stdev-obs"                 )) return t_stdev_obs;
      if (!strcmp(c, "stdev-adj"                 )) return t_stdev_adj;
      if (!strcmp(c, "sparse-mat"                )) return t_sparse_mat;
      if (!strcmp(c, "sum-of-squares"            )) return t_sum_of_squares;
      break;
    case 't' :
      if (!strcmp(c, "text"                      )) return t_text;
      if (!strcmp(c, "to"                        )) return t_to;
      if (!strcmp(c, "to-dh"                     )) return t_to_dh;
      if (!strcmp(c, "tol-abs"                   )) return t_tol_abs;
      break;
    case 'u' :
      if (!strcmp(c, "u"                         )) return t_u;
      if (!strcmp(c, "u-fixed"                   )) return t_u_fixed;
      if (!strcmp(c, "u-free"                    )) return t_u_free;
      if (!strcmp(c, "u-constr"                  )) return t_u_constr;
      if (!strcmp(c, "u-unused"                  )) return t_u_unused;
      if (!strcmp(c, "unused"                    )) return t_unused;
      break;
    case 'v' :
      if (!strcmp(c, "val"                       )) return t_val;
      if (!strcmp(c, "variance"                  )) return t_variance;
      if (!strcmp(c, "variance-factor-used"      )) return t_variance_factor;
      if (!strcmp(c, "vector"                    )) return t_vector;
      break;
    case 'w' :
      if (!strcmp(c, "width"                     )) return t_width;
      break;
    case 'x' :
      if (!strcmp(c, "x"                         )) return t_x;
      if (!strcmp(c, "x-adjusted"                )) return t_x_adjusted;
      if (!strcmp(c, "x-correction"              )) return t_x_correction;
      if (!strcmp(c, "x-given"                   )) return t_x_given;
      if (!strcmp(c, "x-observed"                )) return t_x_observed;
      if (!strcmp(c, "x-residual"                )) return t_x_residual;
      if (!strcmp(c, "x-adjusted"                )) return t_x_adjusted;
      if (!strcmp(c, "x-stdev-obs"               )) return t_x_stdev_obs;
      if (!strcmp(c, "x-stdev-adj"               )) return t_x_stdev_adj;
      if (!strcmp(c, "xyz"                       )) return t_xyz;
      break;
    case 'y' :
      if (!strcmp(c, "y"                         )) return t_y;
      if (!strcmp(c, "y-adjusted"                )) return t_y_adjusted;
      if (!strcmp(c, "y-correction"              )) return t_y_correction;
      if (!strcmp(c, "y-given"                   )) return t_y_given;
      if (!strcmp(c, "y-observed"                )) return t_y_observed;
      if (!strcmp(c, "y-residual"                )) return t_y_residual;
      if (!strcmp(c, "y-adjusted"                )) return t_y_adjusted;
      if (!strcmp(c, "y-stdev-obs"               )) return t_y_stdev_obs;
      if (!strcmp(c, "y-stdev-adj"               )) return t_y_stdev_adj;
      break;
    case 'z' :
      if (!strcmp(c, "z"                         )) return t_z;
      if (!strcmp(c, "z-adjusted"                )) return t_z_adjusted;
      if (!strcmp(c, "z-correction"              )) return t_z_correction;
      if (!strcmp(c, "z-given"                   )) return t_z_given;
      if (!strcmp(c, "z-observed"                )) return t_z_observed;
      if (!strcmp(c, "z-residual"                )) return t_z_residual;
      if (!strcmp(c, "z-adjusted"                )) return t_z_adjusted;
      if (!strcmp(c, "z-stdev-obs"               )) return t_z_stdev_obs;
      if (!strcmp(c, "z-stdev-adj"               )) return t_z_stdev_adj;
      if (!strcmp(c, "zenith"                    )) return t_zenith;
      break;
    default:
      break;
    }

  error(string("### unknown tag <") + string(c) + ">");

  return t_unknown;
}

// *****************************************************************

int DataParser::parser_error(const char *name, const char **atts)
{
  return error(string("### tag <") + string(name)
               + string("> cannot be used in this context"));
}


int DataParser::start_tag(const char *name, const char **atts)
{
  no_attributes(name, atts);
  return (state = next[state][tag(name)]);
}

int DataParser::end_tag(const char *name)
{
  return (state = after[state]);
}

int DataParser::no_attributes(const char *name, const char **atts)
{
  if (*atts)
    {
      return error(string("### tag <") + string(name)
                 + string("> cannot have any attributes"));
    }
  return 0;
}

int DataParser::white_spaces(const char* s, int len)
{
  while (len--)
    {
      if (!isspace(s[len])) return error("### illegal text");
    }

  return 0;
}

int DataParser::add_text(const char* s, int len)
{
  text_buffer += ' ';
  text_buffer += string(s, len);
  return 0;
}

bool DataParser::pure_data(std::istream& istr)
{
  if (istr.eof()) return true;

  char j;
  if (istr >> j)
    return false;  // trailing junk in data
  else
    return true;
}

// ......  <gnu-gama-data>  ................................................

int DataParser::gama_data(const char *name, const char **atts)
{
  no_attributes  (name, atts);   // will have attribute 'version' later?
  state = next[state][tag(name)];
  return 0;
}

// ......  <text>  .........................................................

int DataParser::text(const char* name)
{
  objects.push_back( new DataObject::Text(text_buffer) );
  text_buffer.erase();
  return end_tag(name);
}
