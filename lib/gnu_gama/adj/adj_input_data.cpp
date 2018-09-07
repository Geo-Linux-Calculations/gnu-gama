/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002, 2018  Ales Cepek <cepek@gnu.org>

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
    along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gnu_gama/adj/adj_input_data.h>

using namespace GNU_gama;
using namespace std;


AdjInputData::AdjInputData()
{
  A     = nullptr;    // SparseMatrix<>  *
  pcov  = nullptr;    // BlockDiagonal<> *
  pminx = nullptr;    // IntegerList<>   *
}



AdjInputData::~AdjInputData()
{
  delete A;
  delete pcov;
  delete pminx;
}



void AdjInputData::swap(AdjInputData *data)
{
  std::swap(A     , data->A    );
  std::swap(pcov  , data->pcov );
  std::swap(prhs  , data->prhs );
  std::swap(pminx , data->pminx);
}



void AdjInputData::write_xml(std::ostream& out) const
{
  out << "\n<adj-input-data>\n";

  if (A && A->check())
    {
      out << "\n  <sparse-mat>\n";
      out << "    "
          << "<rows>" << A->rows() << "</rows> "
          << "<cols>" << A->columns() << "</cols> "
          << "<nonz>" << A->nonzeroes() << "</nonz>\n";

      for (int k=1; k<=A->rows(); k++)
        {
          double* n = A->begin(k);
          double* e = A->end  (k);

          out << "      <row>";
          out << " <nonz>" << (e - n) << "</nonz>";
          for(int* i=A->ibegin(k) ; n!=e; n++, i++)
            {
              out << "\n        "
                  << "<int>" << *i << "</int>"
                  << "<flt>" << *n << "</flt>";
            }
          out << "\n        </row>\n";
        }

      out << "  </sparse-mat>\n";
  }

  // =================================================================

  if (pcov)
    {
      const long blocks = pcov->blocks();

      out << "\n  <block-diagonal>\n"
          << "    <blocks>" << blocks << "</blocks>"
          << " <nonz>" << pcov->nonzeroes() << "</nonz>\n";

      for (long b=1; b<=blocks; b++)
        {
          long dim   = pcov->dim(b);
          long width = pcov->width(b);

          out << "      <block> <dim>"
              << dim    << "</dim> <width>"
              << width  << "</width>\n";

          const double *m = pcov->begin(b);
          const double *e = pcov->end(b);
          while (m != e)
            out << "      <flt>" << *m++ << "</flt>\n";

          out << "      </block>\n";
        }

      out << "  </block-diagonal>\n";
    }

  // =================================================================

  if (prhs.dim())
    {
      out << "\n  <vector>\n"
          << "    <dim>" << prhs.dim() << "</dim>\n";

      for (int i=1; i<=prhs.dim(); i++)
        out << "      <flt>" << prhs(i) << "</flt>\n";

      out << "  </vector>\n";
    }

  // =================================================================

  if (pminx)
    {
      out << "\n  <array>\n"
          << "    <dim>" << pminx->dim() << "</dim>\n";

      const int *indx = pminx->begin();
      for (int i=1; i<=pminx->dim(); i++)
        out << "      <int>" << *indx++ << "</int>\n";

      out << "  </array>\n";
    }

  // =================================================================

  out << "\n</adj-input-data>\n";
}
