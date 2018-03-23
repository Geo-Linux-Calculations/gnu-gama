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

#ifndef gama_local_Bod_Mer_MatVec_H
#define gama_local_Bod_Mer_MatVec_H

#include <matvec/inderr.h>
#include <gnu_gama/local/float.h>
#include <matvec/svd.h>
#include <matvec/covmat.h>

namespace GNU_gama { namespace local {

    /** A removed class \a MatVecException has been replaced by a typedef to
	\a GNU_gama::Exception::matvec.
     */

    typedef GNU_gama::Exception::matvec MatVecException;

  // class MatVecException : public GNU_gama::local::Exception {
  // public:
  //   const int error;
  //   MatVecException(int e, const char *s) : GNU_gama::local::Exception(s), error(e) {}
  // };

  typedef GNU_gama::Index Index;

  typedef GNU_gama::Vec   <double, MatVecException>   Vec;
  typedef GNU_gama::Mat   <double, MatVecException>   Mat;
  typedef GNU_gama::SVD   <double, MatVecException>   SVD;
  typedef GNU_gama::CovMat<double, MatVecException>   CovMat;  // covariances

}}      // GNU_gama::local

#endif




