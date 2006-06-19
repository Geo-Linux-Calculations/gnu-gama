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
 *  $Id: homogenization.h,v 1.1 2006/06/19 16:27:03 cepek Exp $
 */

// #include <gnu_gama/sparse/smatrix.h>
// #include <gnu_gama/sparse/sbdiagonal.h>
// #include <gnu_gama/sparse/intlist.h>
// #include <gnu_gama/adj/adj_svd.h>
// #include <gnu_gama/adj/adj_gso.h>
// #include <gnu_gama/adj/adj_chol.h>
// 
// #include <iostream>

#ifndef GNU_Gama_Homogenization___gnu_gama_homogenization___homogenization_h
#define GNU_Gama_Homogenization___gnu_gama_homogenization___homogenization_h


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
    Homogenization(const AdjInputData* aid)
    {
      reset(aid);
    }

    void reset(AdjInputData* aid)
    {
      delete sm;
      sm   = 0;
      data = aid;
      ready = false;
      
    }

    const SparseMatrix<Float, Index>* mat() const { run(); return sm; }
    Vec<Float>                        rhs() const { run(); return pr; }
    

  private:

    Homogenization(const Homogenization&);
    void operator=(const Homogenization&);


    const AdjInputData* data;

    typedef SparseMatrix<Float, Index> sparse;
    typedef std::pair<Index, Index>    pair;
    typedef std::set<pair>             set;

    mutable sparse*        sm;
    mutable Vec<Float>     pr;   // right hand side
    mutable bool        ready;

    mutable Vec<Float*>  diag;   // i-th row diagonal element in cholecky dec.
    mutable Vec<Index>   band;   // i-th row number of elements including diag.

    void run() const
    {
      if (ready) return;
      if (!data) throw Exception::matvec(Exception::BadRank, "No input data");

      const BlockDiagonal<Float, Index>& cov = *data->cov();

      BlockDiagonal<Float, Index>* bd = cov.replicate();
      bd->cholDec();
      diag.reset(data->mat()->rows());
      band.reset(data->mat()->rows());

      /* counting total number of nonzeros in scaled sparse matrix */
      /* --------------------------------------------------------- */

      Index total_scaled_nonzeroes = 0;
      {
        Index block_shift = 0;  // block_shift + "block index" == "matrix row"

        for (Index row=1, block=1; block <= data->cov()->blocks(); block++)
          {
            const Index dim   = cov.dim(block);
            const Index width = cov.width(block);
            Float* idiag      = bd->begin(block);

            set   indexes;      // set of index corresponding to current block

            // for all rows corresponding to current block
            for (Index i=1; i<=dim; i++)
              {
                Index iwidth = std::min(width+1, dim-i+1);
                diag(row) = idiag;
                band(row) = iwidth;
                idiag += iwidth;
                row++;

                Index* b=data->mat()->ibegin(block_shift+i); 
                Index* e=data->mat()->iend  (block_shift+i); 
                while (b != e)
                  {
                    Index k = width + 1;
                    while (k)
                      indexes.insert(pair(block_shift + i + --k, *b));

                    ++b;
                  }
              }

            total_scaled_nonzeroes += indexes.size();
            block_shift += dim;
          }

        for (Index i=1; i<=diag.dim(); i++)
          {
            std::cout << "XXX " << *diag(i) << " " << band(i) << std::endl;
          }
      }

      {
        sparse* transp  = data->mat()->transpose();
        
        pr = data->rhs();

        delete transp;
      }
      delete bd;

      ready = true;
    }

  };
  
}  // namespace GNU_gama

#endif
