/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Exception__h_
#define gama_local_Exception__h_

#include <gnu_gama/exception.h>

namespace GNU_gama { namespace local {

    /** A removed class Exception has been replaced by a typedef to
     * GNU_gama::Exception::string.
     *
     * All references to former attribute text were replaced by calls
     * to the virtual class mebmer what().
     */

    typedef GNU_gama::Exception::string Exception;

    // class Exception {
    // public:
    //
    //    const std::string text;
    //
    //    Exception(const std::string& t) : text(t) {}
    //    virtual ~Exception() {}
    //
    // };

}}

#endif

