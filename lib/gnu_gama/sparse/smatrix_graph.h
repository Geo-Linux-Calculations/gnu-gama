/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2004  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ Library.

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

#ifndef GNU_gama_matrix_graph_h_GNU_Gama_MatrixGraph
#define GNU_gama_matrix_graph_h_GNU_Gama_MatrixGraph

#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/intlist.h>
#include <algorithm>
#include <set>

namespace GNU_gama {

  /** Adjacency structure. */

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

    typedef const Index* const_iterator;

    Index          nodes ()        const { return nods;                       }
    Index          degree(Index i) const { return xadj(i+1) - xadj(i);        }
    const_iterator begin (Index i) const { return adjncy.begin() + xadj(i);   }
    const_iterator end   (Index i) const { return adjncy.begin() + xadj(i+1); }

    void set_nodes(Index n) { nods = n; }

  private:

    Index  nods;

    Adjacency(const Adjacency&);
    void operator=(const Adjacency&);
  };


  /** Sparse matrix graph. */

  template <typename Float=double, typename Index=std::size_t>
  class SparseMatrixGraph : public Adjacency<Index>
  {
  public:

    SparseMatrixGraph(const GNU_gama::SparseMatrix<Float, Index>* const sparse)
      : Adjacency<Index>(sparse->columns())
    {
      std::set<std::pair<Index, Index> >  edges;

      {
        Index *i, *e, *j;
        for (Index k=1; k<=sparse->rows(); k++)
          for(i=sparse->ibegin(k), e=sparse->iend(k); i!=e; i++)
            for (j=i+1; j!=e; j++)
              if (*i != *j)
                {
                  edges.insert(std::pair<Index, Index>(*i, *j));
                  edges.insert(std::pair<Index, Index>(*j, *i));
                }
      }

      this->adjncy.reset(edges.size());

      typename std::set<std::pair<Index, Index> >::const_iterator
        i=edges.begin(), e=edges.end();

      this->xadj(1) = this->xadj(2) = 0;      // needed by empty graphs
      for (Index count=0, index=1; index<=this->nodes(); index++)
        {
          this->xadj(index) = count;
          while (i!=e && index == i->first)
            {
              this->adjncy(count++) = i->second;
              ++i;
            }
          this->xadj(index+1) = count;
        }
    }

    typedef const Index* const_iterator;

    bool           connected()     const;

  };

}


#include <gnu_gama/sparse/smatrix_graph_connected.h>

#endif








