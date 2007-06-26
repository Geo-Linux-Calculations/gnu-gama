/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 1999  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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

/*
 *  $Id: matvec.h,v 1.2 2007/06/26 15:04:04 cepek Exp $
 */

#ifndef GaMaLib_Bod_Mer_MatVec_H
#define GaMaLib_Bod_Mer_MatVec_H

#include <gamalib/exception.h>
#include <gamalib/float.h>
#include <matvec/svd.h>
#include <matvec/covmat.h>

namespace GaMaLib {

  class MatVecException : public GaMaLib::Exception {
  public:
    const int error;
    MatVecException(int e, const char *s) : GaMaLib::Exception(s), error(e) {}
  };
  
  typedef GNU_gama::Index Index;
  
  typedef GNU_gama::Vec   <double, MatVecException>   Vec;
  typedef GNU_gama::Mat   <double, MatVecException>   Mat;
  typedef GNU_gama::SVD   <double, MatVecException>   SVD;
  typedef GNU_gama::CovMat<double, MatVecException>   CovMat;  // covariances
  
}      // GaMaLib

#endif




