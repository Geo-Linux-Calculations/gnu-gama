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
 *  $Id: smatrix_graph.h,v 1.4 2006/06/10 12:19:36 cepek Exp $
 */

#ifndef GNU_gama_matrix_graph_h___GNU_Gama_MatrixGraph
#define GNU_gama_matrix_graph_h___GNU_Gama_MatrixGraph

#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/intlist.h>
#include <algorithm>
#include <set>

namespace GNU_gama {


  template <typename Index=std::size_t>
  class Adjacency
  {
  public:
    
    Adjacency()
    {
    }
    Adjacency(Index nodes)
      : xadj( std::max(nodes+2, Index(3)) ),  nods(nodes)
    {
    }
    Adjacency(Index nodes, Index edges)
      : adjncy(edges), xadj( std::max(nodes+2, Index(3)) ),  nods(nodes)
    {
    }


    IntegerList<Index>  adjncy, xadj;   // 1 based indexes
    Index               nods; 

    typedef const Index* const_iterator;

    Index          nodes ()        const { return nods;                       }
    Index          degree(Index i) const { return xadj(i+1) - xadj(i);        }
    const_iterator begin (Index i) const { return adjncy.begin() + xadj(i);   }
    const_iterator end   (Index i) const { return adjncy.begin() + xadj(i+1); }


  private:

    Adjacency(const Adjacency&);
    void operator=(const Adjacency&);
  };



  template <typename Index=std::size_t>
  class SparseMatrixGraph
  {
  public:
  
    template <typename Float>
    SparseMatrixGraph(const GNU_gama::SparseMatrix<Float, Index>* const sparse)
      : adst(sparse->columns())
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
      
      adst.adjncy.reset(edges.size());
      
      typename std::set<std::pair<Index, Index> >::const_iterator 
        i=edges.begin(), e=edges.end();
      
      adst.xadj(1) = adst.xadj(2) = 0;      // needed by empty graphs
      for (Index count=0, index=1; index<=adst.nods; index++)
        {
          adst.xadj(index) = count;
          while (i!=e && index == i->first) 
            {
              adst.adjncy(count++) = i->second;
              ++i;
            }
          adst.xadj(index+1) = count;
        }
    }
    
    typedef const Index* const_iterator;

    Index          nodes ()        const { return adst.nodes();   }
    const_iterator begin (Index i) const { return adst.begin(i);  }
    const_iterator end   (Index i) const { return adst.end(i);    }
    Index          degree(Index i) const { return adst.degree(i); }
    bool           connected()     const;

  private:

    Adjacency<Index> adst;        // adjacency structure
  };

}


#include <gnu_gama/sparse/smatrix_graph_connected.h>

#endif








