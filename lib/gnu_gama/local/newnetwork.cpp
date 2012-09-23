/*
  GNU Gama C++ library
  Copyright (C) 2011, 2012  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/local/newnetwork.h>
#include <gnu_gama/local/network_svd.h>
#include <gnu_gama/local/network_gso.h>
#include <gnu_gama/local/network_chol.h>
#include <gnu_gama/local/network_env.h>


namespace GNU_gama { namespace local {

  LocalNetwork* newLocalNetwork(std::string algorithm)
    {
      LocalNetwork* lnet = 0;

      if      (algorithm == "gso"     ) lnet = new LocalNetwork_gso;
      else if (algorithm == "svd"     ) lnet = new LocalNetwork_svd;
      else if (algorithm == "cholesky") lnet = new LocalNetwork_chol;
      else if (algorithm == "envelope") lnet = new LocalNetwork_env;

      if (lnet == 0) lnet = newLocalNetwork("gso");

      return lnet;
    }

}}
