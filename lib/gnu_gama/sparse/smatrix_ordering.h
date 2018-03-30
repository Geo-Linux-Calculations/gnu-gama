/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_sparse_matrix_ordering_h_GNU_Gama_SparseMatrixOrdering
#define GNU_gama_sparse_matrix_ordering_h_GNU_Gama_SparseMatrixOrdering

#include <gnu_gama/sparse/intlist.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/smatrix_graph.h>

#include <vector>

namespace GNU_gama {

  /** \brief Rooted level structure */

  template <typename Index=int>
  class RootedLevelStructure
  {
  public:

    Adjacency<Index> adst;

    void root(Index r, const Adjacency<Index>* graph)
    {
      const Index nodes = graph->nodes();

      if (nodes == 0)
        {
          adst.adjncy.reset();
          adst.xadj.reset(Index(3));
          adst.xadj(1) = adst.xadj(2) = 0;
          return;
        }

      if (mask.dim() != nodes+1)  mask.reset(nodes+1);

      for (Index i=1; i<=nodes; i++) mask(i) = i;
      const Index mnodes = nodes;     // masked nodes

      if (adst.xadj.dim() != nodes+2) adst.xadj.reset(nodes+2);
      if (adst.adjncy.dim() != nodes) adst.adjncy.reset(nodes);

      Index index=0, level=1;

      mask(r) = 0;
      adst.adjncy(index) = r;
      adst.xadj(level) = index;
      adst.xadj(level+1) = index+1;
      index++;

      while (index != mnodes)
        {
          // all masked neighbours of nodes in the current level

          Index width = 0;
          Index ii=adst.xadj(level);
          Index ee=adst.xadj(level+1);
          while (ii != ee)
            {
              const Index nox = adst.adjncy(ii);
              typename Adjacency<Index>::const_iterator
                i=graph->begin(nox), e=graph->end(nox);
              while(i != e)
                {
                  const Index node = *i;
                  if (mask(node))
                    {
                      mask(node) = 0;
                      adst.adjncy(index + width++) = node;
                    }
                  i++;
                }
              ii++;
            }

          if (width == 0) break;

          level++;
          adst.xadj(level) = index;
          index += width;
          adst.xadj(level+1) = index;
        }

      adst.set_nodes(level);
    }

  private:

    IntegerList<Index> mask;
  };


  /** \brief Pseudo-peripheral node */

  template <typename Index=int>
  class PseudoPeripheralNode
  {
  public:

    PseudoPeripheralNode() : starting_node(1) {}

    Index operator()(const Adjacency<Index>* graph)
    {
      if (graph->nodes() == 0)  return 0;

      RootedLevelStructure<Index> rls;
      Index rlevel, t, r = starting_node;
      rls.root(r, graph);
      xlevel = rls.adst.nodes();
      do
        {
          rlevel = xlevel;
          typename Adjacency<Index>::const_iterator b = rls.adst.begin(rlevel);
          typename Adjacency<Index>::const_iterator e = rls.adst.end  (rlevel);
          r = *b;
          ++b;
          while(b != e)
            {
              t = *b;
              if (graph->degree(t) < graph->degree(r))  r = t;
              ++b;
            }
          rls.root(r, graph);
          xlevel = rls.adst.nodes();
        }
      while(xlevel > rlevel);

      return r;
    }

    void  set_starting_node(Index p) { starting_node = p; }
    Index levels() const { return xlevel; }

  private:

    Index starting_node;
    Index xlevel;
  };


  /** \brief Sprase matrix ordring */

  template <typename Index=int>
  class SparseMatrixOrdering
  {
  public:

    IntegerList<Index> perm;
    IntegerList<Index> invp;

    SparseMatrixOrdering()
    {
    }
    virtual ~SparseMatrixOrdering()
    {
    }
    SparseMatrixOrdering(const Adjacency<Index>* graph)
    {
      reset(graph);
    }
    void reset(const Adjacency<Index>* graph)
    {
      nods = graph->nodes();

      if (perm.dim() != nods+1)   // 1 based indexes ... N+1
        {
          perm.reset(nods+1);
          invp.reset(nods+1);
        }

      algorithm(graph);
      inverse_permutaion();
    }
    Index nodes() const { return nods; }

  protected:

    virtual void algorithm(const Adjacency<Index>* g) = 0;

  private:

    void inverse_permutaion()
    {
      const Index N = this->nodes();
      for (Index i=1; i<=N; i++)
        {
          invp(perm(i)) = i;
        }
    }

    Index nods;
  };

  /** \brief Reverse Cuthill-McKee ordering */

  template <typename Index=int>
  class ReverseCuthillMcKee : public SparseMatrixOrdering<Index>
  {
  public:

    ReverseCuthillMcKee()
    {
    }
    ReverseCuthillMcKee(const Adjacency<Index>* graph)
    {
      this->reset(graph);
    }

  private:

    void algorithm(const Adjacency<Index>* graph)
    {
      const Index N = graph->nodes();
      for (Index i=1; i<=N; i++)
        {
          this->invp(i) = 1;   // in this function, invp is used as a mask
        }

      Index count = 0;         // ordered nodes
      while (count < N)
        {
          PseudoPeripheralNode<Index> ppn;
          for (Index i=1; i<=N; i++)
            if (this->invp(i))
              {
                ppn.set_starting_node(i);
                break;
              }
          const Index r = ppn(graph);
          this->perm(++count) = r;
          this->invp(r) = 0;

          for (Index i=1; i<=count; i++)
            {
              // add all unnumbered neighbours, sorted in increasing order
              // of degree

              typedef std::pair<Index, Index>  Pair;
              typedef std::vector<Pair>        Vector;
              Vector  tmp;

              const Index x = this->perm(i);
              typename Adjacency<Index>::const_iterator b=graph->begin(x);
              typename Adjacency<Index>::const_iterator e=graph->end  (x);
              while (b != e)
                {
                  const Index n = *b;
                  if (this->invp(n))
                    {
                      tmp.push_back(Pair(graph->degree(n), n));
                      this->invp(n) = 0;
                    }
                  b++;
                }

              std::sort(tmp.begin(), tmp.end());

              for (typename Vector::const_iterator
                     i=tmp.begin(), e=tmp.end(); i!=e; ++i)
                {
                  this->perm(++count) = (*i).second;
                }
            }
        }

      // reverse ordering
      for (Index j=N, i=1; i<j; i++, j--)
        {
          std::swap(this->perm(i), this->perm(j));
        }
    }
  };

}

#endif
