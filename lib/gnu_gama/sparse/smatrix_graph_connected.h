/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2005, 2012  Ales Cepek <cepek@gnu.org>

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

#include <stack>

template <typename Float, typename Index>
bool GNU_gama::SparseMatrixGraph<Float, Index>::connected() const
{
  IntegerList<Index> tag(this->nodes()+1);      // for all nodes i, tag(i)=0
  tag.set_zero();

  std::stack<Index>  stack;

  stack.push(1);                       // start with node 1
  tag(1) = 1;
  Index unreachable = this->nodes() - 1;  // number of unreached nodes

  while (!stack.empty())               // order of O(nodes+edges)
    {
      Index x = stack.top();           // pop a node
      stack.pop();

      for (const_iterator b=this->begin(x), e=this->end(x); b!=e; ++b)
        {
          Index y = *b;                // for all neighbors y of node x
          if (tag(y) == 0)
            {
              tag(y) = 1;              // add y to connected component
              unreachable--;
              stack.push(y);
            }
        }
    }

  return unreachable == 0;
}
