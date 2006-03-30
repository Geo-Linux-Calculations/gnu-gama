/*  
    Geodesy and Mapping C++ Library (GNU Gama)
    Copyright (C) 2004  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.
    
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
 *  $Id: smatrix_graph.h,v 1.3 2006/03/30 08:52:55 cepek Exp $
 */

#ifndef GNU_gama_matrix_graph_h___GNU_Gama_MatrixGraph
#define GNU_gama_matrix_graph_h___GNU_Gama_MatrixGraph

#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/intlist.h>
#include <algorithm>
#include <set>

namespace GNU_gama {

  template <typename Float=double, typename Index=std::size_t>
  class SparseMatrixGraph 
  {
  public:
    
    SparseMatrixGraph(const GNU_gama::SparseMatrix<Float, Index>* const m)
      : sparse( m ), 
        xadj  ( std::max(m->columns()+2, Index(3)) ),
        nods  ( m->columns() )
    {
      std::set<std::pair<Index, Index> >  edges;
      
      {
        Index *i, *e, *j;
        for (Index k=1; k<=sparse->rows(); k++)
          for(i=sparse->ibegin(k), e=sparse->iend(k), j; i!=e; i++)
            for (j=i+1; j!=e; j++)
              if (*i != *j)
                {
                  edges.insert(std::pair<Index, Index>(*i, *j));
                  edges.insert(std::pair<Index, Index>(*j, *i));
                }
      }
      
      adjncy.reset(edges.size());
      amem = adjncy.begin();
      
      typename std::set<std::pair<Index, Index> >::const_iterator 
        i=edges.begin(), e=edges.end();
      
      xadj(1) = xadj(2) = 0;      // needed by empty graphs
      for (Index count=0, index=1; index<=nods; index++)
        {
          xadj(index) = count;
          while (i!=e && index == i->first) 
            {
              adjncy(count++) = i->second;
              ++i;
            }
          xadj(index+1) = count;
        }
    }
    
    typedef const Index* const_iterator;

    Index           nodes()        const  { return nods;             } 
    const_iterator  begin(Index i) const  { return amem + xadj(i);   }
    const_iterator  end  (Index i) const  { return amem + xadj(i+1); }
    bool            connected()    const;

  private:
    
    const SparseMatrix<Float, Index>* const sparse;

    IntegerList<Index>  adjncy, xadj;   // 1 based indexes
    const Index         nods; 
    const Index*        amem;
  };

}


#include <gnu_gama/sparse/smatrix_graph_connected.h>

#endif








