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
 *  $Id: envelope.h,v 1.6 2006/08/15 20:06:26 cepek Exp $
 */

#ifndef GNU_Gama_Envelope___gnu_gama_envelope___gnugamaenvelope___envelope_h
#define GNU_Gama_Envelope___gnu_gama_envelope___gnugamaenvelope___envelope_h


#include <gnu_gama/sparse/sbdiagonal.h>
#include <matvec/symmat.h>


namespace GNU_gama {


  template <typename Float=double, typename Index=std::size_t>
  class Envelope
  {
  public:

    Envelope() : dim_(0), diag(0), env(0), xenv(0) 
    {
    }
    Envelope(const Envelope& envelope) : dim_(0), diag(0), env(0), xenv(0)
    {
      copy(envelope);
    }
    Envelope(const BlockDiagonal<Float, Index>& cov) 
    { 
      set(cov); 
    }
    ~Envelope() 
    { 
      clear(); 
    }
    Envelope& operator=(const Envelope& envelope)
    {
      if (this != &envelope)
        {
          clear();
          copy(envelope);
        }

      return *this;
    }

    Index dim() const { return dim_; }

    void cholDec(Float tol=1e-14);
    void lowerSolve   (Index start, Index stop, Float* rhs) const;
    void diagonalSolve(Index start, Index stop, Float* rhs) const;
    void upperSolve   (Index start, Index stop, Float* rhs) const;
    void set(const BlockDiagonal<Float, Index>& cov);
    void write_xml(std::ostream&) const;

    Float  diagonal(Index i) const { return diag[--i]; }
    Float* begin   (Index i) const { return xenv[i];   }
    Float* end     (Index i) const { return xenv[i+1]; }

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

    void copy(const Envelope&);
  };



  template <typename Float, typename Index>
  void Envelope<Float, Index>::cholDec(Float tol)
  {
    for (Index row=2; row<=dim_; row++)
      {
        /*
          submatrix decomposition:
          ------------------------
          
          ( L 0 ) ( D 0 ) (L' u') = ( LDL'     LDu'  )
          ( u 1 ) ( 0 d ) (0  1 )   ( uDL'  uDu' + d )
         
         */

        Float* b = begin(row);
        Float* e = end(row);

        const Index start = row - (e-b);          // position of first row of L in Envelope
        const Index stop  = row - 1;              //             last  row of L

        lowerSolve   (start, stop, begin(row));   // L(Dx) = LDu'
        diagonalSolve(start, stop, begin(row));   // Dx = Du'

        Float* d = diag + (start - 1);            // 1 based indexes
        Float  s = Float();
        while (b != e)
          {
            s += *b * *b++ * *d++;
          }
        *d -= s;                                  // d = diag - uDu'
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::lowerSolve(Index start, Index stop, Float* rhs) const
  {
    Float   s;
    Float*  x;
    const Float*  b;
    const Float*  e;

    rhs++;
    b = xenv[start+1];
    for (Index row=start+1; row<=stop; row++)
      {
        s = Float();
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
  void Envelope<Float, Index>::diagonalSolve(Index start, Index stop, Float* rhs) const
  {
    Float* d = diag + start - 1;     // 1 based indexes
    while (start++ <= stop)
      {
        *rhs++ /= *d++;
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::upperSolve(Index start, Index stop, Float* rhs) const
  {
    Float* col;
    const Float* b;
    const Float* e;

    rhs += dim_ - stop;
    for (Index row=stop; row>=start; row--)
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


  template <typename Float, typename Index>
  SymMat<Float> toSymMat(const Envelope<Float, Index>& env)
  {
    SymMat<Float> smat(env.dim());
    smat.set_zero();

    for (Index i=1; i<=env.dim(); i++)
      {
        smat(i,i) = env.diagonal(i);

        Float* b = env.begin(i);
        Float* e = env.end(i);
        Index  c = i - (e-b);
        while (b != e)
          {
            smat(i, c++) = *b++;
          }
      }


    return smat;
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::copy(const Envelope<Float, Index>& envelope)
  {
    // diag = env = xenv = 0; ... set before calling copy() 

    dim_ = envelope.dim();
    if (dim_ == 0) return;

    diag = new Float[dim_];
    xenv = new Float*[dim_+2];    // 1 based indexes
    Index env_size = envelope.xenv[dim_+1] - envelope.xenv[1];
    if (env_size) env = new Float[env_size];

    Float* t = env;
    Float* d = diag;
    const Float* cd = envelope.diag;
    for (Index i=1; i<=dim_; i++)
      { 
        *d++ = *cd++;

        // pointers to off-diagonal elements
        const Index bw = envelope.xenv[i+1] - envelope.xenv[i];
        xenv[i] = t;
        t += bw;
      }
    xenv[dim_+1] = t;

    Float* e = env;
    const Float* ce = envelope.env;
    for (Index i=1; i<=env_size; i++) *e++ = *ce++;
  }

}  // namespace GNU_gama

#endif
