/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2018  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/xml/dataobject.h>

using namespace std;
using namespace GNU_gama::DataObject;

/** \brief Gama XML data header */

string Base::xml_begin()
{
  return
    "<?xml version=\"1.0\" ?>\n"
    "<gnu-gama-data "
    "xmlns=\"http://www.gnu.org/software/gama/gnu-gama-data\">\n\n";
}


/** \brief Gama XML data 'footer' */

string Base::xml_end()
{
  return "\n</gnu-gama-data>\n";
}
