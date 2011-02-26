/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003, 2011  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama__exception__exception_class_hierarchy_____exception_h
#define GNU_gama__exception__exception_class_hierarchy_____exception_h

#include <matvec/inderr.h>
#include <string>

namespace GNU_gama { namespace Exception {

   /** \section GNU gama exceptions
   *
   * \a Exception::base and \a Exception::matvec are defined in
   * <lib/matvec/inderr>
   *
   * Classes \a GNU_gama::local::Exception,
   * GNU_gama::local::ParserException and
   * GNU_gama::local::MatVecException are only typedefs to \a
   * Exception::string, \a Exception::parser and
   * GNU_gama::Exception::matvec respectively.
   *
   * Class \a g2d_exc is limited to Median namespace usage.
  */

  class string : public base {
  public:

    const std::string  str;

    string(const std::string& s) : str(s) {}
    ~string() throw() {}

    string* clone() const { return new string(*this); }
    void    raise() const { throw *this; }

    const char* what() const throw() { return str.c_str(); }
  };


  class adjustment : public string {
  public:

    adjustment(const char* s) : string(s) {}

    adjustment* clone() const { return new adjustment(*this); }
    void        raise() const { throw *this; }
  };


  class parser : public string {
  public:

    const int line, error_code;

    parser(const std::string& s, int r, int c)
      : string(s), line(r), error_code(c)
      {
      }

    parser* clone() const { return new parser(*this); }
    void    raise() const { throw *this; }
  };


}}

#endif
