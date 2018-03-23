/*
  GNU Gama C++ library
  Copyright (C) 1999, 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef gama_local_Bod_Mer_MatVec_H
#define gama_local_Bod_Mer_MatVec_H

#include <gnu_gama/index.h>
#include <gnu_gama/local/float.h>
#include <matvec/inderr.h>
#include <matvec/svd.h>
#include <matvec/covmat.h>

namespace GNU_gama { namespace local {

  using GNU_gama::Index;

  typedef GNU_gama::Exception::matvec MatVecException;

  typedef GNU_gama::Vec   <double, MatVecException>   Vec;
  typedef GNU_gama::Mat   <double, MatVecException>   Mat;
  typedef GNU_gama::SVD   <double, MatVecException>   SVD;
  typedef GNU_gama::CovMat<double, MatVecException>   CovMat;  // covariances

}}      // GNU_gama::local

#endif
