/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2007  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library
    
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
 *  $Id: dataparser_g3adj.cpp,v 1.2 2007/04/26 11:11:42 cepek Exp $
 */



#include <gnu_gama/g3/g3_adjres.h>
#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/gon2deg.h>
#include <gnu_gama/radian.h>
#include <cstring>

using namespace std;
using namespace GNU_gama;

namespace GNU_gama {

  struct DataParser_g3adj {

    g3::AdjustmentResults* adj;

    DataParser_g3adj() : adj(0)
    {
    }
    ~DataParser_g3adj()
    {  
      delete adj;
    }

  };

}


void DataParser::close_g3adj()
{
  delete g3adj;
}

void DataParser::init_g3adj()
{
  g3adj = new DataParser_g3adj;


  // .....  <g3-adjustment-results>  .................................

  init(s_gama_data, t_g3_adj_results,
       s_g3a_adj_results, 0, 0,
       &DataParser::g3a_adj_results, 0, &DataParser::g3a_adj_results);

  // .....  <g3-adjustment-results>  <adjustment-statistics>  ........

  init(s_g3a_adj_results, t_adj_statistics,
       s_g3a_statistics, 0, 0,
       0, 0, 0);

  init(s_g3a_statistics, t_algorithm,
       s_g3a_algorithm, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_algorithm);

  init(s_g3a_statistics, t_ellipsoid,
       s_g3a_ellipsoid, 0, 0,
       0, 0, 0);

  init(s_g3a_ellipsoid, t_caption,
       s_g3a_ellipsoid_cap, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_ell_caption);

  init(s_g3a_ellipsoid, t_id,
       s_g3a_ellipsoid_id, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_ell_id);

  init(s_g3a_ellipsoid, t_a,
       s_g3a_ellipsoid_a, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_ell_a);

  init(s_g3a_ellipsoid, t_b,
       s_g3a_ellipsoid_b, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_ell_b);

  init(s_g3a_statistics, t_parameters,
       s_g3a_parameters, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_parameters);

  init(s_g3a_statistics, t_equations,
       s_g3a_equations, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_equations);

  init(s_g3a_statistics, t_defect,
       s_g3a_defect, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_defect);

  init(s_g3a_statistics, t_redundancy,
       s_g3a_redundancy, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_redundancy);

  init(s_g3a_statistics, t_sum_of_squares,
       s_g3a_sum_of_squares, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_sum_of_squares);

  init(s_g3a_statistics, t_apriori_var,
       s_g3a_apriori_var, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_apriori_var);

  init(s_g3a_statistics, t_aposteriori_var,
       s_g3a_aposteriori_var, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_aposteriori_var);

  init(s_g3a_statistics, t_variance_factor,
       s_g3a_variance_factor, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_variance_factor);

  init(s_g3a_statistics, t_design_m_graph,
       s_g3a_design_m_graph, 0, 0,
       0, &DataParser::add_text, &DataParser::g3a_design_m_graph);
}

int DataParser::g3a_adj_results(const char *name, const char **atts)
{
  no_attributes( name, atts );
  state = next[state][tag(name)];

  delete g3adj->adj;  
  g3adj->adj = new g3::AdjustmentResults;
  
  return 0;
}

int DataParser::g3a_adj_results(const char *name)
{
  objects.push_back( new DataObject::g3_adj_results(g3adj->adj) );
  g3adj->adj = 0;

  return  end_tag(name);
}


void DataParser::g3a_text_string(std::string& str)
{
  std::string::const_iterator b=text_buffer.begin();
  std::string::const_iterator e=text_buffer.end();
  TrimWhiteSpaces(b, e);
  str = std::string(b, e);
  text_buffer.clear();
}


void DataParser::g3a_text_float(std::string& str)
{
  std::string::const_iterator b=text_buffer.begin();
  std::string::const_iterator e=text_buffer.end();
  TrimWhiteSpaces(b, e);
  str = std::string(b, e);
  if (!IsFloat(b, e)) error("### bad format of float");
  text_buffer.clear();
}


void DataParser::g3a_text_integer(std::string& str)
{
  std::string::const_iterator b=text_buffer.begin();
  std::string::const_iterator e=text_buffer.end();
  TrimWhiteSpaces(b, e);
  str = std::string(b, e);
  if (!IsInteger(b, e)) error("### bad format of integer");
  text_buffer.clear();
}


int DataParser::g3a_algorithm(const char *name)
{
  g3a_text_string(g3adj->adj->algorithm);

  return  end_tag(name);
}


int DataParser::g3a_ell_caption(const char *name)
{
  g3a_text_string(g3adj->adj->ell_cap);

  return  end_tag(name);
}


int DataParser::g3a_ell_id(const char *name)
{
  g3a_text_string(g3adj->adj->ell_id);
  return  end_tag(name);
}


int DataParser::g3a_ell_a(const char *name)
{
  g3a_text_float(g3adj->adj->ell_a);
  return  end_tag(name);
}


int DataParser::g3a_ell_b(const char *name)
{
  g3a_text_float(g3adj->adj->ell_b);
  return  end_tag(name);
}


int DataParser::g3a_parameters(const char *name)
{
  g3a_text_integer(g3adj->adj->parameters);
  return  end_tag(name);
}


int DataParser::g3a_equations(const char *name)
{
  g3a_text_integer(g3adj->adj->equations);
  return  end_tag(name);
}


int DataParser::g3a_defect(const char *name)
{
  g3a_text_integer(g3adj->adj->defect);
  return  end_tag(name);
}


int DataParser::g3a_redundancy(const char *name)
{
  g3a_text_integer(g3adj->adj->redundancy);
  return  end_tag(name);
}


int DataParser::g3a_sum_of_squares(const char *name)
{
  g3a_text_float(g3adj->adj->sum_of_squares);
  return  end_tag(name);
}


int DataParser::g3a_apriori_var(const char *name)
{
  g3a_text_float(g3adj->adj->apriori_var);
  return  end_tag(name);
}


int DataParser::g3a_aposteriori_var(const char *name)
{
  g3a_text_float(g3adj->adj->aposteriori_var);
  return  end_tag(name);
}


int DataParser::g3a_variance_factor(const char *name)
{
  g3a_text_string(g3adj->adj->variance_factor);
  return  end_tag(name);
}


int DataParser::g3a_design_m_graph(const char *name)
{
  g3a_text_string(g3adj->adj->design_m_graph);
  return  end_tag(name);
}

