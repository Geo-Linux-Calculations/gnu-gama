/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2003, 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_Symmetric_Block_Diagonal_Symmetric_Block_Diagonal
#define GNU_gama_Symmetric_Block_Diagonal_Symmetric_Block_Diagonal

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <cmath>

namespace GNU_gama {

  /** Symmetric block diagonal matrix. */

  template <typename Float=double, typename Index=int>
  class BlockDiagonal {    // symmetric block diagonal matrix

    Index   blocks_;       // number of diagonal blocks
    Float*  nonz_;         // all nonzero elements
    Index   ncnt_;         // number of all nonzeroes
    Index   size_;
    Index*  dim_;
    Index*  width_;
    Float** begin_;

    BlockDiagonal (const BlockDiagonal&);
    void operator=(const BlockDiagonal&);

    void clear()
    {
      delete[] dim_;
      delete[] width_;
      delete[] begin_;
      delete[] nonz_;
    }

    void init(Index blcks, Index floats)
    {
      dim_    = new Index [blcks+1];  // 1 based indexes
      width_  = new Index [blcks+1];
      begin_  = new Float*[blcks+2];
      nonz_   = new Float [floats];

      ncnt_ = size_ = blocks_ = 0;    // matrix is initially empty
      begin_[1] = nonz_;
    }

  public:

    BlockDiagonal()
    {
      blocks_ = ncnt_ = size_ = 0;
      dim_ = width_ = 0;
      nonz_ = 0;
      begin_ = 0;
    }

    BlockDiagonal(Index blcks, Index floats)
    {
      init(blcks, floats);
    }

    ~BlockDiagonal()
    {
      clear();
    }

    void reset(Index blcks, Index floats)
    {
      clear();
      init (blcks, floats);
    }

    Index  blocks()        const { return blocks_;   }
    Index  dim()           const { return size_;     }
    Index  nonzeroes()     const { return ncnt_;     }
    Index  dim   (Index i) const { return dim_  [i]; }
    Index  width (Index i) const { return width_[i]; }

    const Float* begin (Index i) const { return begin_[i];   }
    const Float* end   (Index i) const { return begin_[i+1]; }
    Float*       begin (Index i)       { return begin_[i];   }
    Float*       end   (Index i)       { return begin_[i+1]; }

    BlockDiagonal* replicate() const
    {
      return replicate(blocks_, ncnt_);
    }

    BlockDiagonal* replicate(Index new_blocks, Index new_floats) const
    {
      BlockDiagonal* r = new BlockDiagonal(new_blocks, new_floats);

      for (Index i=1; i<=blocks_; i++)
      {
        r->add_block(dim(i), width(i), begin(i));
      }

      return r;
    }

    void add_block(Index bdim, Index bwidth, const Float* mem)
    {
      Index N = bdim*(bwidth+1) - (bwidth)*(bwidth+1)/2;

      blocks_++;
      size_ += bdim;
      ncnt_ += N;
      Float* b = begin_[blocks_];
      begin_[blocks_+1] = b + N;
      memcpy(b, mem, N*sizeof(Float));

      dim_  [blocks_] = bdim;
      width_[blocks_] = bwidth;
    }


    int cholDec(Float tol = 1e-14)
    {
      Float* B;
      Float* p;
      Index  N, W, row, k, l, n;
      Float  q, pivot;

      for (Index block=1; block<=blocks_; block++)
      {
        B = begin(block);
        N = dim  (block);
        W = width(block);

        for (row=1; row<=N; row++)
          {
            if ((pivot = *B) < tol)
              return block;                  // not positive-definite

            k = std::min(W, N-row);          // number of of-diagonal elements
            p = B+k;                         // next row address -1
            for (n=1; n<=k; n++)
              {
                q = B[n]/pivot;
                for (l=n; l<=k; l++) p[l] -= q*B[l];
                p += std::min(W, N-row-n);
              }
            *B++ = pivot = std::sqrt(pivot); // scaling pivot row
            for (; k; k--) *B++ /= pivot;
          }

      }   // block ...

      return 0;
    }

  };



  template <typename Float=double, typename Index=int>
  class UpperBlockDiagonal {     // upper triangular block diagonal matrix
  public:

    UpperBlockDiagonal(const BlockDiagonal<Float, Index> *bd);
    ~UpperBlockDiagonal()   { delete[] row;               }

    Index dim()       const { return blockd->dim();       }
    Index nonzeroes() const { return blockd->nonzeroes(); }

    const Float* begin(Index i) const { return row[ i ];  }
    const Float* end  (Index i) const { return row[i+1];  }

  private:

    const BlockDiagonal<Float, Index> *blockd;
    const Float **row;

    UpperBlockDiagonal(const UpperBlockDiagonal&);
    UpperBlockDiagonal& operator=(UpperBlockDiagonal&);
  };


  template <typename Float, typename Index>
  UpperBlockDiagonal<Float, Index>::UpperBlockDiagonal(const BlockDiagonal<Float, Index> *bd)
    : blockd(bd), row(0)
  {
    const Index N = bd->dim();
    if (N == 0) return;

    row = new const Float*[N+2];   // 1 based indexes plus extra pointer for last row
    row[0] = 0;

    for (Index r=0, b=1; b<=bd->blocks(); b++)
      {
        const Index  dim   = bd->dim  (b);
        const Index  width = bd->width(b);
        const Float* mem   = bd->begin(b);

        for (Index row_width, i=1; i<=dim; i++)
          {
            row_width = width + 1;
            if (i+row_width > dim) row_width = dim - i + 1;

            row[++r] = mem;
            mem     += row_width;
            row[r+1] = mem;
          }
      }
  }



}   // namespace GNU_gama

#endif



#ifdef GNU_gama_sparse_demo

#include <iostream>

using namespace std;
using namespace GNU_gama;


void write(ostream& cout, BlockDiagonal<>* bd)
{
  // cout.precision(7);
  cout << endl;
  for (unsigned long i=1; i<=bd->blocks(); i++)
    {
      cout << i << " : [" << bd->dim(i) << " | " << bd->width(i) << "]\n";
      double* b = bd->begin(i);
      double* e = bd->end(i);
      cout << "    ";
      while (b != e) cout << *b++ << ' ';
      cout << endl;
    }
}

int main()
{
  cout << "\n---  Symmetric Block Diagonal Matrix demo  -----------------\n";

  double b1[] = {1.1, 1.2, 1.3};
  double b2[] = {44.2, 5.2, 66.2, 7.2, 88.2};
  double b3[] = {19, 1, 2, 3, 18, 1, 2, 17, 1, 16};
  double b4[] = {81, 81, 81, 81, 145, 145, 145, 64, 194, 194, 113, 49,
                 230, 149, 85, 36, 174, 110, 61, 25, 126, 77, 41, 16, 86,
                 50, 25, 54, 29, 30};

  BlockDiagonal<>* m1 = new BlockDiagonal<>(20, 5000);
  m1->add_block(3, 0, b1);
  m1->add_block(3, 1, b2);
  m1->add_block(4, 3, b3);
  m1->add_block(9, 3, b4);
  write(cout, m1);

  BlockDiagonal<>* m2 = m1->replicate();
  m2->cholDec();
  write(cout, m2);

  cout << "\n---  Upper Triangular Block Diagonal Matrix  ---------------\n\n";

  UpperBlockDiagonal<> upper(m1);

  cout << "dimension = " << upper.dim() << endl
       << "nonzeroes = " << upper.nonzeroes() << "\n\n";

  for (unsigned i=1; i<=upper.dim(); i++)
    {
      cout << i << " : ";
      const double* b = upper.begin(i);
      const double* e = upper.end(i);
      while (b!=e)
        {
          cout << *b++ << " ";
        }
      cout << endl;
    }

  delete m1;
  delete m2;
}


#endif
