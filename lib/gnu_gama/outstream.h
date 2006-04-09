/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2003  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library.
    
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
 *  $Id: outstream.h,v 1.1 2006/04/09 16:40:25 cepek Exp $
 */

#include <iostream>
#include <string>
#include <gnu_gama/xml_expat.h>
#include <gnu_gama/xml/encoding.h>

#ifndef GNU_gama__outstream__h____output_stream__outstreamh
#define GNU_gama__outstream__h____output_stream__outstreamh

namespace GNU_gama {

  class OutStream {
  public:
    
    enum { utf_8, iso_8859_2, iso_8859_2_flat, cp_1250, cp_1251 }; 
    
    OutStream(std::ostream& str);
    
    OutStream& operator << (const char* c)
      {
        ostr << recode(c);
        return *this;
      }
    OutStream& operator << (const std::string& s)
      {
        ostr << recode(s.c_str());
        return *this;
      }
    
    template<typename T> OutStream& operator << (const T& t)
      {
        ostr << t;
        return *this;
      }
    
    std::ostream& std_stream() { return ostr; }
    
    void setf (std::ios_base::fmtflags t, std::ios_base::fmtflags v) { ostr.setf(t, v); }
    void width     (int t)  { ostr.width(t);     }
    void precision (int t)  { ostr.precision(t); }
    void flush     ()       { ostr.flush();      }
    
    void set_encoding(int e) { encoding = e; }
    
  private:
    
    std::ostream& ostr;
    int           encoding;
    std::string   text;
    
    const char* recode(const char* s);  
  };


  class SaveFlags {
  public:

    SaveFlags(std::ostream& out) : std_stream(out)
    {
      flgs = std_stream.flags();
      prec = std_stream.precision();
    }
    ~SaveFlags()
    {
      std_stream.precision(prec);
      std_stream.flags(flgs);
    }

  private:

    std::ostream&           std_stream;
    std::ios_base::fmtflags flgs;
    int                     prec; 
  };

}

#endif
