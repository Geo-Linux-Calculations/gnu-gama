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
 *  $Id: envelope.h,v 1.2 2006/06/19 19:48:32 cepek Exp $
 */

#ifndef GNU_Gama_Envelope___gnu_gama_envelope___gnugamaenvelope___envelope_h
#define GNU_Gama_Envelope___gnu_gama_envelope___gnugamaenvelope___envelope_h


#include <gnu_gama/sparse/sbdiagonal.h>


namespace GNU_gama {


  template <typename Float=double, typename Index=std::size_t>
  class Envelope
  {
  public:

    Envelope() : dim_(0), diag(0), env(0), xenv(0) 
    {
    }
    ~Envelope() 
    { 
      clear(); 
    }
    Envelope(const BlockDiagonal<Float, Index>& cov) 
    { 
      set(cov); 
    }

    void set(const BlockDiagonal<Float, Index>& cov);
 
    Index dim() const { return dim_; }

  private:

    Index   dim_;
    Float*  diag;
    Float*  env;
    Float** xenv;

    void clear()
    {
      delete[] diag;
      delete[] env;
      delete[] xenv;
    }
  };


  
  template <typename Float, typename Index>
  void Envelope<Float, Index>::set(const BlockDiagonal<Float, Index>& cov)
  {
    clear();
    dim_ = cov.dim();
    if (dim_ == 0) return;

    diag = new Float[dim_];
    xenv = new Float*[dim_+2];    // 1 based indexes
    Index env_size = 0;
    for (Index i=1; i<=cov.blocks(); i++)
      {
        const Index dim  = cov.dim(i);
        const Index band = cov.width(i);

        env_size += (dim + -1 + dim - band)*band/2;
      }
    if (env_size) env = new Float[env_size];

    Float* d = diag;
    Float* e = env;
    for (Index i=1; i<=cov.blocks(); i++)
      {
        const Index dim  = cov.dim(i);
        const Index band = cov.width(i);

        const Float* b = cov.begin(i);
        Index n = (dim + -1 + dim - band)*band/2;

        *d++ = *b++;
        while (n) 
          {
            *e++ = *b++;
            n--;
          }
      }


  }


}  // namespace GNU_gama

#endif
