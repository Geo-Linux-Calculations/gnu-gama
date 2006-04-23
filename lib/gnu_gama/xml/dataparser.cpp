/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2002, 2005  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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
 *  $Id: dataparser.cpp,v 1.2 2006/04/23 15:36:21 cepek Exp $
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

  delete adj_sparse_mat;
  delete adj_block_diagonal;
  delete adj_array;
}


DataParser::DataParser(List<DataObject::Base*>& obs) : objects(obs)
{
  adj = 0;
  g3  = 0;

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
      if (!strcmp(c, "angle"                     )) return t_angle;
      if (!strcmp(c, "adj-input-data"            )) return t_adj_input_data;
      if (!strcmp(c, "angular-units-degrees"     )) return t_ang_degrees;
      if (!strcmp(c, "angular-units-gons"        )) return t_ang_gons;
      if (!strcmp(c, "apriori-standard-deviation")) return t_apriori_sd;
      if (!strcmp(c, "array"                     )) return t_array;
      if (!strcmp(c, "azimuth"                   )) return t_azimuth;
      break;                                     
    case 'b':                                    
      if (!strcmp(c, "b"                         )) return t_b;
      if (!strcmp(c, "band"                      )) return t_band;
      if (!strcmp(c, "block-diagonal"            )) return t_block_diagonal;
      if (!strcmp(c, "blocks"                    )) return t_blocks;
      if (!strcmp(c, "block"                     )) return t_block;
      break;                                     
    case 'c' :                                   
      if (!strcmp(c, "confidence-level"          )) return t_conf_level;
      if (!strcmp(c, "cols"                      )) return t_cols;
      if (!strcmp(c, "constants"                 )) return t_constants;
      if (!strcmp(c, "constr"                    )) return t_constr;
      if (!strcmp(c, "cov-mat"                   )) return t_covmat;
      break;                                     
    case 'd' :                                   
      if (!strcmp(c, "db"                        )) return t_db;
      if (!strcmp(c, "dim"                       )) return t_dim;
      if (!strcmp(c, "distance"                  )) return t_dist;
      if (!strcmp(c, "dl"                        )) return t_dl;
      if (!strcmp(c, "dx"                        )) return t_dx;
      if (!strcmp(c, "dy"                        )) return t_dy;
      if (!strcmp(c, "dz"                        )) return t_dz;
      break;                                     
    case 'e':                                    
      if (!strcmp(c, "e"                         )) return t_e;
      if (!strcmp(c, "ellipsoid"                 )) return t_ellipsoid;
      break;                                     
    case 'f' :                                   
      if (!strcmp(c, "fixed"                     )) return t_fixed;
      if (!strcmp(c, "flt"                       )) return t_flt;
      if (!strcmp(c, "from"                      )) return t_from;
      if (!strcmp(c, "from-dh"                   )) return t_from_dh;
      if (!strcmp(c, "free"                      )) return t_free;
      break;                                     
    case 'g' :                                   
      if (!strcmp(c, "g3-model"                  )) return t_g3_model;
      if (!strcmp(c, "geoid"                     )) return t_geoid;
      if (!strcmp(c, "gnu-gama-data"             )) return t_gama_data;
      break;                                     
    case 'h':                                    
      if (!strcmp(c, "h"                         )) return t_h;
      if (!strcmp(c, "hdiff"                     )) return t_hdiff;
      if (!strcmp(c, "height"                    )) return t_height;
      break;                                     
    case 'i':                                    
      if (!strcmp(c, "id"                        )) return t_id;
      if (!strcmp(c, "int"                       )) return t_int;
      if (!strcmp(c, "inv-f"                     )) return t_inv_f;
      break;                                     
    case 'l':                                    
      if (!strcmp(c, "l"                         )) return t_l;
      if (!strcmp(c, "left"                      )) return t_left;
      if (!strcmp(c, "left-dh"                   )) return t_left_dh;
      break;                                     
    case 'n':                                    
      if (!strcmp(c, "n"                         )) return t_n;
      if (!strcmp(c, "nonz"                      )) return t_nonz;
      break;                                     
    case 'o':                                    
      if (!strcmp(c, "obs"                       )) return t_obs;
      break;                                     
    case 'p':                                    
      if (!strcmp(c, "point"                     )) return t_point;
      break;                                     
    case 'r':                                    
      if (!strcmp(c, "right"                     )) return t_right;
      if (!strcmp(c, "right-dh"                  )) return t_right_dh;
      if (!strcmp(c, "row"                       )) return t_row;
      if (!strcmp(c, "rows"                      )) return t_rows;
      break;                                     
    case 's':                                    
      if (!strcmp(c, "stdev"                     )) return t_stdev;
      if (!strcmp(c, "sparse-mat"                )) return t_sparse_mat;
      break;                                     
    case 't' :                                   
      if (!strcmp(c, "text"                      )) return t_text;
      if (!strcmp(c, "to"                        )) return t_to;
      if (!strcmp(c, "to-dh"                     )) return t_to_dh;
      break;                                     
    case 'u' :                                   
      if (!strcmp(c, "u"                         )) return t_u;
      if (!strcmp(c, "unused"                    )) return t_unused;
      break;                                     
    case 'v' :                                   
      if (!strcmp(c, "val"                       )) return t_val;
      if (!strcmp(c, "variance"                  )) return t_variance;
      if (!strcmp(c, "vector"                    )) return t_vector;
      break;                                     
    case 'w' :                                   
      if (!strcmp(c, "width"                     )) return t_width;
      break;                                     
    case 'x' :                                   
      if (!strcmp(c, "x"                         )) return t_x;
      if (!strcmp(c, "xyz"                       )) return t_xyz;
      break;                                     
    case 'y' :                                   
      if (!strcmp(c, "y"                         )) return t_y;
      break;                                     
    case 'z' :                                   
      if (!strcmp(c, "z"                         )) return t_z;
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
