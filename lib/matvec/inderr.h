/*  
    C++ Matrix/Vector templates (GNU Gama / matvec 0.9.25)
    Copyright (C) 1999  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Matrix/Vector template library.
    
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
 *  $Id: inderr.h,v 1.1 2006/04/09 16:12:01 cepek Exp $
 *  http://www.gnu.org/software/gama/
 */

#ifndef GNU_gama_gMatVec__IndexErr__h_
#define GNU_gama_gMatVec__IndexErr__h_

#include <cstddef>

namespace GNU_gama {

  typedef size_t Index;

  namespace Exception {

    enum
      { 
        BadRank,  
        BadIndex, 
        Singular,
        BadRegularization,
        NoConvergence,  
        ZeroDivision,  
        NonPositiveDefinite,
        NotImplemented,
        StreamError 
      };
    
    class base {
    public:
      virtual ~base() 
      {
      }
    };

    class matvec : public base
    {
    public:
      const int    error;
      const char*  description;
      
      matvec(int e, const char* t) : error(e), description(t) 
      {
      }
    };
  }
}

#endif




