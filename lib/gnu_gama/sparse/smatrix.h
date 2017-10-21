/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001, 2003  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_gama_local_Sparse_General_Matrix_General_Matrix_
#define GNU_gama_gama_local_Sparse_General_Matrix_General_Matrix_

#include <cstddef>
#include <cstring>


namespace GNU_gama {

template <typename Float=double, typename Index=std::size_t>

  /** Sparse general matrix is a set of unordered sparse rows. */

  class SparseMatrix {

    // sparse general matrix is a set of unordered sparse rows

    Index   rows_, cols_;

    Float*  nonz;   // non-zero elements
    Index*  cind;   // column indexes of nonzero elements
    Index*  rptr;   // indexes to the beginning of the i-th row

    Index*  rptr1;
    Index   rcnt_, rnxt_, ncnt_;

    SparseMatrix  (const SparseMatrix&);
    void operator=(const SparseMatrix&);

    SparseMatrix  (const SparseMatrix* sm)
    {
      nonz = new Float[sm->ncnt_];
      cind = new Index[sm->ncnt_];
      rptr = new Index[sm->cols_ + 4];

      rptr1  = rptr + 1;
      rows_  = sm->cols_;
      cols_  = sm->rows_;
      rcnt_  = sm->cols_;
      rnxt_  = rcnt_ + 1;
      ncnt_  = sm->ncnt_;
    }

    public:

    SparseMatrix()
    {
      nonz = 0;
      cind = rptr = 0;
    }

    SparseMatrix(Index floats, Index rows, Index cols)
    {
      nonz = new Float[floats];
      cind = new Index[floats];
      rptr = new Index[rows+2];

      rptr1  = rptr + 1;
      rows_  = rows;
      cols_  = cols;
      rcnt_  = 0;
      rnxt_  = 1;
      ncnt_  = 0;
    }

    ~SparseMatrix()
    {
      delete[]  nonz;
      delete[]  cind;
      delete[]  rptr;
    }

    Index rows()      const { return rows_; }
    Index columns()   const { return cols_; }
    Index nonzeroes() const { return ncnt_; }

    Float* begin (Index i) const { return nonz + rptr [i]; }
    Float* end   (Index i) const { return nonz + rptr1[i]; }
    Index* ibegin(Index i) const { return cind + rptr [i]; }
    Index* iend  (Index i) const { return cind + rptr1[i]; }

    Index  size  (Index i) const { return rptr1[i]-rptr[i]; }

    void reset(Index floats, Index rows, Index cols)
    {
      delete[]  nonz;
      delete[]  cind;
      delete[]  rptr;

      nonz = new Float[floats];
      cind = new Index[floats];
      rptr = new Index[rows+2];

      rptr1  = rptr + 1;
      rows_  = rows;
      cols_  = cols;
      rcnt_  = 0;
      rnxt_  = 1;
      ncnt_  = 0;
    }


    SparseMatrix* replicate() const
    {
      return replicate(ncnt_, rows_, cols_);
    }

    SparseMatrix* replicate(Index new_n, Index new_r, Index new_c) const
    {
      SparseMatrix* r = new SparseMatrix(new_n, new_r, new_c);

      r->rows_ = new_r;
      r->cols_ = new_c;
      r->rcnt_ = rcnt_;
      r->rnxt_ = rnxt_;
      r->ncnt_ = ncnt_;
      using namespace std;
      memcpy(r->rptr, rptr, (rcnt_+2)*sizeof(Index) );
      memcpy(r->nonz, nonz,  ncnt_   *sizeof(Float) );
      memcpy(r->cind, cind,  ncnt_   *sizeof(Index) );

      return r;
    }

    SparseMatrix* transpose() const
    {
      SparseMatrix* t = new SparseMatrix(this);

      const Index  trows_ = t->rows_;
      Index*       tcind  = t->cind;
      Index*       trptr  = t->rptr;
      Float*       tnonz  = t->nonz;

      Index  i, j, k, r, irb, ire;

      /* count non-zeroes in all columns and form new transposed row
       * pointer lists in trptr[k], k=3, 4, ... (ie. shifted by 2) */

      for (i=0; i<trows_+2; i++)  trptr[i] = 0;
      for (i=0; i<ncnt_;    i++)  trptr[cind[i]+2]++;
      for (i=3; i<trows_+2; i++)  trptr[i] += trptr[i-1];

      /* now we go over each matrix row and copy non-zero elements
       * with its corresponding column index (ie. current row) to
       * their destination location; incrementing gradually elements
       * in the "shifted pointer lists" automatically creates row
       * pointer lists for the transposed matrix */

      ire = rptr[1];             // end iterator for the r-th row
      for (r=1; r<=rows_; r++)
      {
        irb = ire;
        ire = rptr[r+1];
        while (irb != ire)
          {
            k = cind[irb]+1;
            j = trptr[k];
            tcind[j] = r;
            tnonz[j] = nonz[irb];
            trptr[k]++;
            irb++;
          }
      }

      return t;
    }

    /* functions for sequeantial fill-in of non-zero elements by rows */

    void new_row()
    {
      rptr[++rcnt_] = ncnt_;
      rptr[++rnxt_] = ncnt_;
    }

    void add_element(Float e, Index k)
    {
      nonz[ncnt_  ] = e;
      cind[ncnt_++] = k;
      rptr[rnxt_]++;
    }

    bool check() const
    {
      bool ok_ = true;

      for (Index k=1; k<=rows(); k++)
      {
          if (begin(k) > end(k)) ok_ = false;
      }

      return ok_;
    }

  };

}   // namespace GNU_gama

#endif

#ifdef GNU_gama_sparse_demo

#include <iostream>

using namespace std;
using namespace GNU_gama;


void write(ostream& cout, SparseMatrix<>* sgm)
{
  cout << endl;
  for (unsigned long k=1; k<=sgm->rows(); k++)
    {
      cout << k << " : ";
      double* n = sgm->begin(k);
      double* e = sgm->end  (k);
      for(std::size_t* i=sgm->ibegin(k) ; n!=e; n++, i++)
        {
          cout << *n << " [" << *i << "]  ";
        }
      cout << endl;
    }
}

int main()
{
  cout << "\n---  Sparse General Matrix demo  ---------------------------\n";

  SparseMatrix<>* inp = new SparseMatrix<>(500, 6, 5);

  for (int n=1; n<=6; n++)
    {
      inp->new_row();
      switch (n)
        {
        case 1:
          inp->add_element(12.3, 3);
          break;
        case 2:
          break;
        case 3:
          inp->add_element(4.2, 1);
          inp->add_element(9.4, 5);
          inp->add_element(0.2, 2);
          break;
        case 4:
          inp->add_element(7.3, 4);
          break;
        case 5:
          inp->add_element(2.3, 5);
          inp->add_element(3.7, 3);
          break;
        case 6:
          inp->add_element(2.4, 3);
          inp->add_element(6.7, 2);
          inp->add_element(2.9, 1);
          break;
        };
    }

  SparseMatrix<>* sgm = inp->replicate();
  delete inp;

  write(cout, sgm);

  inp  = sgm->transpose();
  delete sgm;
  sgm  = inp->transpose();
  delete inp;

  write(cout, sgm);

  delete sgm;
}


#endif








