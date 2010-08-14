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

#ifndef GNU_Gama_Homogenization___gnu_gama_homogenization___homogenization_h
#define GNU_Gama_Homogenization___gnu_gama_homogenization___homogenization_h


#include <gnu_gama/adj/adj.h>
#include <gnu_gama/adj/envelope.h>
#include <matvec/covmat.h>
#include <set>


namespace GNU_gama {


  template <typename Float=double, typename Index=std::size_t>
  class Homogenization
  {
  public:

    Homogenization() : data(0), sm(0), ready(false)
    {
    }
    Homogenization(const AdjInputData* aid) : sm(0)
    {
      reset(aid);
    }
    ~Homogenization()
    {
      delete sm;
    }

    void reset(const AdjInputData* aid=0)
    {
      delete sm;
      pr.reset();
      sm    = 0;
      data  = aid;
      ready = false;
    }

    const SparseMatrix<Float, Index>* mat() { run(); return sm; }
    const Vec<Float>&                 rhs() { run(); return pr; }


  private:

    Homogenization(const Homogenization&);
    void operator=(const Homogenization&);

    const AdjInputData* data;

    typedef SparseMatrix<Float, Index> Sparse;
    typedef std::set<Index>            Indices;

    Sparse*        sm;
    Vec<Float>     pr;   // right hand side
    bool        ready;


    void run()
    {
      if (ready) return;
      if (!data) throw Exception::matvec(Exception::BadRank, "Homogenization : No input data");

      const BlockDiagonal<Float, Index>& cov = *data->cov();

      BlockDiagonal<Float, Index>* blockdiagonal = cov.replicate();
      blockdiagonal->cholDec();
      const BlockDiagonal<Float, Index>* bd = blockdiagonal;

      UpperBlockDiagonal<Float, Index> upper(bd);
      const Sparse* mata           = data->mat();
      Index total_scaled_nonzeroes = 0;


      /* homogenised right-hand side */

      pr = data->rhs();
      for (Index n, row=1; row<=pr.dim(); row++)   // forward substitution
        {
          const Float* b = upper.begin(row);
          const Float* e = upper.end  (row);
          const Float  x = pr(row) / *b++;
          pr(row) = x;
          n = row + 1;
          while(b != e)
            {
              pr(n++) -= *b++ * x;
            }
        }


      /* counting total number of nonzeros in scaled sparse matrix */

      std::vector<Index> block_cols(bd->blocks()+1);   // 1 based indexing

      for (Index row=1, block_index=1; block_index<=bd->blocks(); block_index++)
        {
          const Index  block_dim   = bd->dim  (block_index);
          const Index  block_width = bd->width(block_index);

          if (block_width == 0)    // uncorrelated observations
            {
              Index nonz = 0;
              for (Index i=1; i<=block_dim; i++, row++)
                {
                  nonz += mata->end(row) - mata->begin(row);
                }
              total_scaled_nonzeroes += nonz;
            }
          else                     // correlated observations
            {
              Indices indices;
              for (Index i=1; i<=block_dim; i++, row++)
                {
                  const Index* n = mata->ibegin(row);
                  const Index* e = mata->iend  (row);
                  while (n != e)
                    {
                      indices.insert(*n++);
                    }
                }
              block_cols[block_index] = indices.size();
              total_scaled_nonzeroes += block_dim*indices.size();
            }
        }


      sm = new Sparse(total_scaled_nonzeroes, mata->rows(), mata->columns());


      /* assembling scaled sparse matrix */

      std::vector<Index> perm(mata->columns()+1);  // block index permutation

      for (Index row=1, block_index=1; block_index<=bd->blocks(); block_index++)
        {
          const Float* block_b     = bd->begin(block_index);
          const Index  block_dim   = bd->dim  (block_index);
          const Index  block_width = bd->width(block_index);

          if (block_width == 0)    // uncorrelated observations
            for (Index i=1; i<=block_dim; i++, row++)
              {
                sm->new_row();
                const Float  d = *block_b++;
                const Index* n = mata->ibegin(row);
                const Float* b = mata->begin (row);
                const Float* e = mata->end   (row);
                while (b != e)
                  {
                    sm->add_element(*b++/d, *n++);
                  }
              }
          else                     // correlated observations
            {
              const Index bcols = block_cols[block_index];
              std::vector<Index> invp(bcols+1);     // block inverse permutaion
              Index invp_count = 0;

              Mat<Float> T(block_dim, bcols);       // matrix of block columns
              T.set_zero();

              /* copy block sparse columns to T */

              for (Index i=1; i<=block_dim; i++, row++)
                {
                  const Float* b = mata->begin (row);
                  const Float* e = mata->end   (row);
                  const Index* n = mata->ibegin(row);
                  while (b != e)
                    {
                      const Index c = *n++;
                      if (perm[c] == 0)
                        {
                          perm[c] = ++invp_count;
                          invp[invp_count] = c;
                        }

                      T(i, perm[c]) = *b++;
                    }
                }

              /* forward substitution for T */

              for (Index c=1; c<=bcols; c++)
                for (Index n, r=row-block_dim, i=1; i<=block_dim; i++, r++)
                  {
                    const Float* b = upper.begin(r);
                    const Float* e = upper.end  (r);
                    const Float  x = T(i,c) / *b++;
                    T(i,c) = x;
                    n = i + 1;
                    while (b != e)
                      {
                        T(n++,c) -= *b++ * x;
                      }
                  }

              /* move transformed T to ouput sparse matrix  */

              for (Index i=1; i<=block_dim; i++)
                {
                  sm->new_row();
                  for (Index j=1; j<=bcols; j++)
                    if (const Float element = T(i,j))
                      {
                        sm->add_element(element, invp[j]);
                      }
                }

              for (Index i=1; i<=bcols; i++)      // clear permutation vector
                {
                  perm[invp[i]] = 0;
                }
            }
        }

      delete blockdiagonal;
      ready = true;
    }

  };

}  // namespace GNU_gama

#endif
