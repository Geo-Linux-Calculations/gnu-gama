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
 *  $Id: dataparser_g3adj.cpp,v 1.1 2007/03/31 18:16:22 cepek Exp $
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

}

