/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 2014  Ales Cepek <cepek@gnu.org>

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

/** \file xmlerror.cpp
 * \brief XML Schema parser errors
 *
 * \author Ales Cepek
 */

#include <gnu_gama/local/xmlerror.h>

using namespace GNU_gama::local;

XMLerror::XMLerror()
{
  clear();
}


void XMLerror::clear()
{
  _isValid = false;
  _hasLineNumber = false;
  _category.clear();
  _strlist.clear();
}

void XMLerror::setIsValid(bool b)
{
  _isValid = b;
}

bool XMLerror::isValid() const
{
  return _isValid;
}

bool XMLerror::hasLineNumber() const
{
  return _hasLineNumber;
}

void XMLerror::setXmlOutput  (std::string s)
{
  _xmlOutput = s;
  _isValid = true;
}

void XMLerror::setDescription(std::string s)
{
  _strlist.push_back(s);
  _isValid = true;
}

void XMLerror::setCategory(std::string s)
{
  _category = s;
  _isValid = true;
}

void XMLerror::setLineNumber(int n)
{
  _lineNumber = n;
  _hasLineNumber = true;
  _isValid = true;
}

int XMLerror::write_xml(std::string category)
{
  _category = category;

  if (!_xmlOutput.empty())
    {
      if (_xmlOutput == "-")
        {
          write(std::cout);
        }
      else
        {
          std::ofstream file(_xmlOutput.c_str());
          write(file);
        }
    }

  return 0;
}

void XMLerror::write(std::ostream& ostr)
{
  ostr <<
    "<?xml version=\"1.0\"?>\n"
    "<gama-local-adjustment xmlns=\"" << XSD_GAMA_LOCAL_ADJUSTMENT << "\">\n\n"
    "<error category=\"" << _category << "\">\n";

  for (std::vector<std::string>::const_iterator
         i=_strlist.begin(), e=_strlist.end(); i != e; ++i)
    {
      ostr << "<description>" << *i << "</description>\n";
    }

  if (_hasLineNumber)
    {
      ostr << "<lineNumber>" << _lineNumber << "</lineNumber>\n";
    }

  ostr <<
    "</error>\n\n"
    "</gama-local-adjustment>\n";
}
