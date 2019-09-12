/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 1999, 2018  Ales Cepek <cepek@fsv.cvut.cz>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef gama_local_Bod_Mer_MatVec_H
#define gama_local_Bod_Mer_MatVec_H

#include <matvec/inderr.h>
#include <gnu_gama/local/float.h>
#include <matvec/svd.h>
#include <matvec/covmat.h>

namespace GNU_gama { namespace local {

    using MatVecException = GNU_gama::Exception::matvec;

    using Vec = GNU_gama::Vec   <double, int, MatVecException>;
    using Mat = GNU_gama::Mat   <double, int, MatVecException>;
    using SVD = GNU_gama::SVD   <double, int, MatVecException>;
    using CovMat = GNU_gama::CovMat<double, int, MatVecException>;

  }}      // GNU_gama::local

#endif
