/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 1999  Jiri Vesely <vesely@gama.fsv.cvut.cz>
                  2001  Ales Cepek  <cepek@fsv.cvut.cz>

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

/******************************************************************
 * local exception for GNU_gama::local (Median)                           *
 ******************************************************************/

#ifndef gama_local_g2d_exception_h__GNU_gama_local_Median_Vyjimky_H
#define gama_local_g2d_exception_h__GNU_gama_local_Median_Vyjimky_H

#include <gnu_gama/local/exception.h>
#include <gnu_gama/local/language.h>

namespace GNU_gama { namespace local
{

  class g2d_exc : public GNU_gama::local::Exception
    {
    public:
      g2d_exc(const std::string& description) :
        GNU_gama::local::Exception(T_IE_internal_error+std::string(" ")+description)
        {
        }

    g2d_exc* clone() const { return new g2d_exc(*this); }
    void     raise() const { throw *this; }

    };

}}  // GNU_gama::local

#endif
