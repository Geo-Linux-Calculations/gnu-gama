/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_LocalNetwork_svd_h
#define gama_local_LocalNetwork_svd_h

#include <gnu_gama/local/network.h>
#include <gnu_gama/adj/adj_svd.h>

namespace GNU_gama { namespace local
{
  class LocalNetwork_svd : public LocalNetwork
    {
      typedef GNU_gama::AdjSVD<Double, GNU_gama::local::MatVecException> OLS_svd;
      OLS_svd* ols_svd;

    public:

      LocalNetwork_svd()
      {
        ols_svd = new OLS_svd;
        set_algorithm(ols_svd);
      }
      ~LocalNetwork_svd()
      {
        delete ols_svd;
      }

      bool   lindep(Index i) { return ols_svd->lindep(i); }
      Double cond()          { return ols_svd->cond();    }

      const char* const algorithm() const { return "svd"; }
    };
}}

#endif









