/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GaMaLib_LocalNetwork_envelope_h
#define GaMaLib_LocalNetwork_envelope_h

#include <gamalib/local/network.h>
#include <gnu_gama/adj/adj_envelope.h>

namespace GaMaLib 
{
  class LocalNetwork_env : public LocalNetwork
    {
      typedef GNU_gama::AdjEnvelope<Double, Index, GaMaLib::MatVecException> OLS_env;
      OLS_env* ols_env;

    public:

      LocalNetwork_env()
      { 
        ols_env = new OLS_env;
        set_algorithm(ols_env);
      }
      ~LocalNetwork_env() 
      {
        delete ols_env;
      }

      bool   lindep(Index i) { return ols_env->lindep(i); }
      Double cond()          { return ols_env->cond();    } 

      const char* const algorithm() const { return "envelope"; }
    };
}

#endif









