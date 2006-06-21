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
 *  $Id: envelope.h,v 1.3 2006/06/21 18:55:07 cepek Exp $
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

    Index dim() const { return dim_; }

    void lower_solve(Index start, Float* rhs) const;
    void diagonal_solve(Index start, Float* rhs) const;
    void upper_solve(Index start, Float* rhs) const;
    void set(const BlockDiagonal<Float, Index>& cov);
    void write_xml(std::ostream&) const;

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
  void Envelope<Float, Index>::lower_solve(Index start, Float* rhs) const
  {
    Float   s;
    Float*  x;
    const Float*  b;
    const Float*  e;

    rhs++;
    b = xenv[start+1];
    for (Index row=start+1; row<=dim_; row++)
      {
        s = 0;
        e = xenv[row+1];

        x = rhs - (e-b);
        while (b != e)
          {
            s += *x++ * *b++;
          }
        *rhs++ -= s;

        b = e;
      }
  }

  
  template <typename Float, typename Index>
  void Envelope<Float, Index>::diagonal_solve(Index start, Float* rhs) const
  {
    Float* d = diag + start - 1;     // 1 based indexes
    while (start++ < dim_)
      {
        *rhs++ /= *d++;
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::upper_solve(Index stop, Float* rhs) const
  {
    Float* col;
    const Float* b;
    const Float* e;

    rhs += dim_ - stop;
    for (Index row=dim_; row>=stop; row--)
      {
        b = xenv[row];
        e = xenv[row+1];

        const Float x = *rhs;
        col = rhs - (e-b);
        while (b != e)
          {
            *col++ -= x * *b++;
          }
       
        rhs--;
      }
  }

  
  template <typename Float, typename Index>
  void Envelope<Float, Index>::set(const BlockDiagonal<Float, Index>& cov)
  {
    clear();
    dim_ = cov.dim();
    if (dim_ == 0) return;


    diag = new Float[dim_];
    xenv = new Float*[dim_+2];    // 1 based indexes
    Index env_size = 0;
    for (Index block=1; block<=cov.blocks(); block++)
      {
        const Index dim  = cov.dim  (block);
        const Index band = cov.width(block);

        env_size += (dim + -1 + dim - band)*band/2;
      }
    if (env_size) env = new Float[env_size];


    Float* d = diag;
    Float* e = env;
    for (Index row=1, block=1; block<=cov.blocks(); block++)
      {
        const Index dim  = cov.dim  (block);
        const Index band = cov.width(block);
        const Float* b   = cov.begin(block);

        for (Index r=1; r<=dim; r++)
          {            
            Index c = r > band ? r-band : 1;
            xenv[row] = e;
            while (c++ < r)
              {
                *e++ = *b++;
              }
            xenv[++row] = e;

            *d++ = *b++;
          }
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::write_xml(std::ostream& cout) const
  {
    cout << "<envelope> " << "<dim>" << dim_ << "</dim>\n\n";

    Float* d = diag;
    for (Index i=1; i<=dim_; i++)
      {
        Float* b = xenv[i];
        Float* e = xenv[i+1];
        while (b != e)
          {
            cout << "<env>" << *b++ << "</env> ";
          }

        cout << "<diag>" << *d++ << "</diag>\n";
      }

    cout << "\n</envelope>\n";
  }

}  // namespace GNU_gama

#endif
