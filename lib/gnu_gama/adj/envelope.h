/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library

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

#ifndef GNU_Gama_Envelope_gnu_gama_envelope_gnugamaenvelope_envelope_h
#define GNU_Gama_Envelope_gnu_gama_envelope_gnugamaenvelope_envelope_h


#include <limits>
#include <gnu_gama/sparse/smatrix_graph.h>
#include <gnu_gama/sparse/smatrix_ordering.h>
#include <gnu_gama/sparse/sbdiagonal.h>
#include <matvec/symmat.h>


namespace GNU_gama {


  template <typename Float=double, typename Index=std::size_t>
  class Envelope
  {
  public:

    Envelope() : dim_(0), defect_(0), diag(0), env(0), xenv(0)
    {
    }
    Envelope(const Envelope& envelope) : diag(0), env(0), xenv(0)
    {
      copy(envelope);
    }
    Envelope(const SparseMatrix         <Float, Index>* sm,
             const SparseMatrixGraph    <Float, Index>* graph,
             const SparseMatrixOrdering <Index>*        ordering)
      : diag(0), env(0), xenv(0)
    {
      set(sm, graph, ordering);
    }
    Envelope(const BlockDiagonal<Float, Index>& cov) : diag(0), env(0), xenv(0)
    {
      set(cov);
    }
    Envelope(const Float* bdiag, const Float* ediag,
             const Float* benv,  const Float* eenv,
             const Index* bbend, const Index* eband) : diag(0), env(0), xenv(0)
    {
      set(bdiag, ediag, benv, eenv, bbend, eband);
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

    Index dim()    const { return dim_;    }
    Index defect() const { return defect_; }

    void cholDec(Float tol=Float());
    void solve (Float* rhs, Index dimension) const;
    void lowerSolve   (Index start, Index stop, Float* rhs) const;
    void diagonalSolve(Index start, Index stop, Float* rhs) const;
    void upperSolve   (Index start, Index stop, Float* rhs) const;
    void set(const SparseMatrix         <Float, Index>* sm,
             const SparseMatrixGraph    <Float, Index>* graph,
             const SparseMatrixOrdering <Index>*        ordering);
    void set(const BlockDiagonal<Float, Index>& cov);
    void set(const Float* b_diad, const Float* e_diag,
             const Float* b_env,  const Float* e_envxo,
             const Index* b_bend, const Index* e_band);
    void inverse  (const Envelope& choldec);
    void write_xml(std::ostream&) const;

    Float& diagonal(Index i)       { return diag[--i]; }
    Float  diagonal(Index i) const { return diag[--i]; }
    Float* begin   (Index i) const { return xenv[i];   }
    Float* end     (Index i) const { return xenv[i+1]; }

    Float* element(Index i, Index j)
    {
      Float* b;
      Float* e;
      Index  n;
      if (i > j)
        {
          b = xenv[i];
          e = xenv[i+1];
          n = i - j;
          if (n > Index(e-b)) return 0;

          return e - n;
        }
      else if (i < j)
        {
          b = xenv[j];
          e = xenv[j+1];
          n = j - i;
          if (n > Index(e-b)) return 0;

          return e - n;
        }

      return diag + --i;
    }
    const Float* element(Index i, Index j) const
    {
      const Float* b;
      const Float* e;
      Index  n;
      if (i > j)
        {
          b = xenv[i];
          e = xenv[i+1];
          n = i - j;
          if (n > Index(e-b)) return 0;

          return e - n;
        }
      else if (i < j)
        {
          b = xenv[j];
          e = xenv[j+1];
          n = j - i;
          if (n > Index(e-b)) return 0;

          return e - n;
        }

      return diag + --i;
    }

  private:

    Index   dim_;
    Index   defect_;
    Float*  diag;   // diagonal elements
    Float*  env;    // of-diagonal elements
    Float** xenv;

    void clear()
    {
      defect_ = 0;

      delete[] diag;   diag = 0;
      delete[] env;    env  = 0;
      delete[] xenv;   xenv = 0;
    }

    void copy(const Envelope&);
  };



  template <typename Float, typename Index>
  void Envelope<Float, Index>::cholDec(Float tol)
  {
    if (tol <= Float())
      {
        tol = std::sqrt( std::numeric_limits<Float>::epsilon() );
      }
    defect_ = 0;

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

        const Index start = row - (e-b);          // first row of L in Envelope
        const Index stop  = row - 1;              //       last  row of L

        lowerSolve   (start, stop, begin(row));   // L(Dx) = LDu'
        diagonalSolve(start, stop, begin(row));   // Dx = Du'

        Float* d = diag + (start - 1);            // 1 based indexes
        Float  s = Float();
        while (b != e)
          {
            s += *b * *b * *d++;
            b++;
          }
        *d -= s;                                  // d = diag - uDu'

        if (std::abs(*d) < tol)
          {
            *d = Float();                         // linearly dependend unknown
            defect_++;
          }
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::solve(Float* rhs, Index dimension) const
  {
    lowerSolve   (1, dimension, rhs);
    diagonalSolve(1, dimension, rhs);
    upperSolve   (1, dimension, rhs);
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::lowerSolve(Index start, Index stop, Float* rhs) const
  {
    Float   s;
    const Float*  x;
    const Float*  b;
    const Float*  e;
    const Float*  rhs0 = rhs;

    rhs++;
    for (Index row=start+1; row<=stop; row++)
      {
         b = xenv[row];
         e = xenv[row+1];
         x = rhs;
         s = Float();
         while (b != e && x != rhs0)  s += *--x * *--e;
         *rhs++ -= s;
      }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::diagonalSolve(Index start, Index stop, Float* rhs) const
  {
    const Float* d = diag + start - 1;     // 1 based indexes
    while (start++ <= stop)
      if (*d)
        {
          *rhs++ /= *d++;
        }
      else
        {
          *rhs++ = Float();
          d++;
        }
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::upperSolve(Index start, Index stop, Float* rhs) const
  {
    Float* col;
    const Float* b;
    const Float* e;

    rhs += stop - 1;
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
    const Index env_size = envelope.xenv[dim_+1] - envelope.xenv[1];
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


  template <typename Float, typename Index>
  void Envelope<Float, Index>::set(const SparseMatrix<Float, Index>* sm,
                                   const SparseMatrixGraph<Float, Index>* graph,
                                   const SparseMatrixOrdering<Index>* ordering)
  {
    clear();
    dim_ = sm->columns();
    if (dim_ == 0) return;

    diag = new Float[dim_];
    xenv = new Float*[dim_+2];    // 1 based indexes

    Index* min_neighbour = new Index[dim_+1];
    for (Index i=1; i<=dim_; i++) min_neighbour[i] = i;
    for (Index node=1; node<=graph->nodes(); node++)
      {
        // original number of the node
        const Index i = ordering->perm(node);

        // scan all neighbours
        typedef typename SparseMatrixGraph<Float, Index>::const_iterator const_iterator;
        const_iterator b = graph->begin(i);
        const_iterator e = graph->end(i);
        while (b != e)
          {
            const Index c = ordering->invp(*b++);
            if (min_neighbour[node] > c)
              min_neighbour[node] =  c;
          }
      }

    Index  env_size = 0;
    for (Index i=1; i<=dim_; i++)
      {
        env_size +=  i - min_neighbour[i];
      }
    if (env_size) env = new Float[env_size];
    Float* e = env;
    for (Index i=1; i<=dim_; i++)
      {
        xenv[i] = e;
        e += i - min_neighbour[i];
        xenv[i+1] = e;
      }
    delete[] min_neighbour;


    for (Index i=0; i<dim_; i++) diag[i] = 0;
    for (Index i=0; i<env_size; i++) env[i] = 0;

    Float* a = new Float[dim_];  // nonzeroe row elements
    Index* c = new Index[dim_];  //              indexes

    for (Index r=1; r<=sm->rows(); r++)
      {
        Index count = 0;
        Float* b = sm->begin(r);
        Float* e = sm->end(r);
        Index* n = sm->ibegin(r);
        while (b != e)
          {
            a[count] = *b++;
            c[count] = ordering->invp(*n++);
            count++;
          }

        for (Index i=0; i<count; i++)
          {
            const Index ia = c[i];
            const Float fa = a[i];
            diag[ia-1] += fa*fa;

            for (Index j=i+1; j<count; j++)
              {
                const Index ib = c[j];
                const Float fb = a[j];

                const Index row = std::max(ia, ib);
                const Index col = std::min(ia, ib);

                Float* element = end(row) - (row - col);
                *element += fa*fb;
              }
          }
      }

    delete[] a;
    delete[] c;
  }

  template <typename Float, typename Index>
  void Envelope<Float, Index>::set(const Float* b_diag, const Float* e_diag,
                                   const Float* b_env,  const Float* e_env,
                                   const Index* b_bend, const Index* /*e_bend*/)
  {
    clear();
    dim_ = e_diag - b_diag;
    if (dim_ == 0) return;

    diag = new Float[dim_];
    xenv = new Float*[dim_+2];    // 1 based indexes

    const Index env_size = e_env - b_env;
    if (env_size) env = new Float[env_size];

    Float* t = env;
    Float* d = diag;
    for (Index i=1; i<=dim_; i++)
      {
        *d++ = *b_diag++;

        // poiners to off-diagonal elements
        xenv[i] = t;
        t += *b_bend++;
      }
    xenv[dim_+1] = t;

    Float* e = env;
    while (b_env != e_env) *e++ = *b_env++;
  }


  template <typename Float, typename Index>
  void Envelope<Float, Index>::inverse(const Envelope& chol)
  {
    if (this == &chol)
      {
        Envelope tmp(chol);
        inverse(tmp);
        return;
      }

    clear();
    dim_ = chol.dim();
    if (dim_ == 0) return;

    diag = new Float[dim_];
    xenv = new Float*[dim_+2];
    const Index env_size = chol.xenv[dim_+1] - chol.xenv[1];
    if (env_size) env = new Float[env_size];

    Float* t = env;
    for (Index i=1; i<=dim_; i++)
      {
        const Index bw = chol.xenv[i+1] - chol.xenv[i];
        xenv[i] = t;
        t += bw;
      }
    xenv[dim_+1] = t;

    // Z = inv(D)*inv(L) + (I - L')*Z

    Float        d, s;
    const Float*    u;   // element of upper triangular matrix L'
    Float      *    z;   //            inverse matrix
    for (Index step=dim_; step>=1; step--)
      {
        d = chol.diagonal(step);
        if (d == 0)
          {
            diagonal(step) = Float();
            Float* b = begin(step);
            Float* e = end(step);
            while (b != e) *b++ = Float();
            continue;
          }

        d = Float(1)/d;
        for (Index n=1, k=step+1; k<=dim_; k++, n++)
          {
            // d -= U(k, step)*Z(step, k);
            u = chol.element(k, step);
            if (u == 0) continue;
            z = element(step, k);
            d -= *u * *z;
          }
        diagonal(step) = d;

        Float* b = begin(step);
        Float* e = end(step);
        for (Index i=step-1; i>=1 && b != e ; i--)
          {
            s = Float();
            for (Index k=i+1; k<=dim_; k++)
              {
                // s -= U(i, k)*Z(k, step);
                u = chol.element(i,k);
                if (u == 0) continue;
                z = element(k, step);
                s -= *u * *z;
              }
            *--e = s;
          }
      }
  }

}  // namespace GNU_gama

#endif
