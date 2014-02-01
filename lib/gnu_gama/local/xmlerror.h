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

/** \file xmlerror.h
 * \brief XMLerror class for storing parser messages about nonvalid data
 *
 * \author Ales Cepek
 */

#ifndef XMLerror_h_XMLERROR_H_xmlerror_h_xmlerror_h_local_xmlerror_h_
#define XMLerror_h_XMLERROR_H_xmlerror_h_xmlerror_h_local_xmlerror_h_

#include <gnu_gama/xsd.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace GNU_gama { namespace local {

class XMLerror {
public:

  XMLerror();

  void clear();
  void setIsValid(bool b);
  bool isValid() const;
  bool hasLineNumber() const;

  void setXmlOutput  (std::string s);
  void setCategory   (std::string s);
  void setDescription(std::string s);
  void setLineNumber (int n);

  int  getLineNumber() const { return _lineNumber; }
  const std::string& getCategory() const { return _category; }
  const std::vector<std::string>& getDescription() const { return _strlist; }

  int  write_xml(std::string category);

private:
  bool                     _isValid;
  bool                     _hasLineNumber;
  std::string              _xmlOutput;
  std::string              _category;
  std::vector<std::string> _strlist;
  int                      _lineNumber;

  void write(std::ostream& ostr);
};

}}

#endif
