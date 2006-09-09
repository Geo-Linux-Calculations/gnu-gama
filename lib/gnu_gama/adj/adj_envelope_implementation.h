/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library
    
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
 *  $Id: adj_envelope_implementation.h,v 1.3 2006/09/09 09:46:02 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h
#define GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h

#include <gnu_gama/adj/adj_envelope.h>
#include <gnu_gama/adj/homogenization.h>

namespace 
{
  static GaMaLib::Vec b;
  static GaMaLib::Mat A;
}

namespace GNU_gama {

  template <typename Float, typename Exc> 
  void AdjEnvelope<Float, Exc>::reset(const AdjInputData *data) 
  {
    this->input = data;
    this->stage = 0;

    Homogenization<> hom(data);

    const SparseMatrix<>* mat = hom.mat();
    const Vec<Float>&     rhs = hom.rhs();

    // ######  LADENI  ##########################################
    {
      Index M = mat->rows();
      Index N = mat->columns();
    
      b.reset(M);
      for (Index i=1; i<=M; i++) b(i) = rhs(i);
      
      A.reset(M, N);
      A.set_zero();
      for (Index i=1; i<=M; i++)
        {
          Float *b = mat->begin(i);
          Float *e = mat->end(i);
          Index *n = mat->ibegin(i);
          while(b != e)
            {
              A(i, *n) = *b;
              
              b++;
              n++;
            }
        }
      
       chol->reset(A, b);
    }
  }

}

#endif
