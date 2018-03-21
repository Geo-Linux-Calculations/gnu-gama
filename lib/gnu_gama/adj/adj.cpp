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
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <gnu_gama/adj/adj.h>
#include <gnu_gama/xml/dataparser.h>
#include <vector>
#include <cstddef>
#include <algorithm>

using namespace GNU_gama;
using namespace std;

AdjInputData::AdjInputData()
{
  A     = 0;    // SparseMatrix<>  *
  pcov  = 0;    // BlockDiagonal<> *
  pminx = 0;    // IntegerList<>   *
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

      for (std::size_t k=1; k<=A->rows(); k++)
        {
          double* n = A->begin(k);
          double* e = A->end  (k);

          out << "      <row>";
          out << " <nonz>" << (e - n) << "</nonz>";
          for(std::size_t* i=A->ibegin(k) ; n!=e; n++, i++)
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
      const size_t blocks = pcov->blocks();

      out << "\n  <block-diagonal>\n"
          << "    <blocks>" << blocks << "</blocks>"
          << " <nonz>" << pcov->nonzeroes() << "</nonz>\n";

      for (long b=1; b<=blocks; b++)
        {
          size_t dim   = pcov->dim(b);
          size_t width = pcov->width(b);

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

      for (std::size_t i=1; i<=prhs.dim(); i++)
        out << "      <flt>" << prhs(i) << "</flt>\n";

      out << "  </vector>\n";
    }

  // =================================================================

  if (pminx)
    {
      out << "\n  <array>\n"
          << "    <dim>" << pminx->dim() << "</dim>\n";

      const std::size_t *indx = pminx->begin();
      for (std::size_t i=1; i<=pminx->dim(); i++)
        out << "      <int>" << *indx++ << "</int>\n";

      out << "  </array>\n";
    }

  // =================================================================

  out << "\n</adj-input-data>\n";
}



void AdjInputData::read_xml(std::istream& inp)
{
  string                       line;
  std::list<DataObject::Base*> objects;
  DataParser                   dp(objects);

  while (getline(inp, line))
    {
      line += '\n';
      dp.xml_parse(line.c_str(), int(line.length()), 0);
    }
  dp.xml_parse("", 0, 1);

  for (std::list<DataObject::Base*>::iterator i=objects.begin();
       i!=objects.end(); ++i)
    {
      if (DataObject::AdjInput *adj = dynamic_cast<DataObject::AdjInput*>(*i))
        {
          // take over the data from DataObject
          swap(adj->data);
        }

      delete *i;
    }
}



void AdjInputData::read_gama_local_old_format(std::istream& inp)
{
  // see void LocalNetwork::project_equations(std::ostream& out)

  using namespace std;
  vector<long>   ind;
  vector<double> flt;

  long cols, rows;
  inp >> cols >> rows;                    // dimensions

  delete pminx;
  pminx = 0;        // no regularization is defined for singular systems
  prhs.reset(rows);

  Vec<> c(rows);

  IntegerList<> tmplist(rows);
  IntegerList<>::iterator m = tmplist.begin();

  long floats=0;
  for (long nonz, n, k, i=1; i<=rows; i++)
    {
      inp >> nonz;                        // number of nonzero elements
      *m++ = nonz;
      floats += nonz;
      for (k=1; k<=nonz; k++)
        {
          inp >> n;                       // indexes of nonzero elements
          ind.push_back(n);
        }

      double d;
      inp >> d;                           // i-th weight
      c(i) = 1/d;
      inp >> d;                           // i-th right-hand site element
      prhs(i) = d;
      for (k=1; k<=nonz; k++)
        {
          inp >> d;                       // nonzeroes elements
          flt.push_back(d);
        }
    }

  delete A;
  A = new SparseMatrix<>(floats, rows, cols);

  m = tmplist.begin();
  for (long k=0, r=1; r<=rows; r++)
    {
      A->new_row();
      size_t nonz = *m++;
      for (size_t i=1; i<=nonz; i++, k++)  A->add_element(flt[k], ind[k]);
    }

  delete pcov;
  pcov = new BlockDiagonal<>(1, rows);
  pcov->add_block(rows, 0, c.begin());
}


// -----------------------------------------------------------------

Adj::~Adj()
{
  delete least_squares;
  delete minx;
}



void Adj::init(const AdjInputData* inp)
{
  delete data;
  data = inp;
  least_squares = 0;
  solved = false;
  n_obs_ = n_par_ = 0;

  if (data)
    {
      n_obs_ = data->A->rows();
      n_par_ = data->A->columns();
    }
}



void Adj::init_least_squares()
{
  delete least_squares;

  switch (algorithm_)
    {
    case envelope:
      least_squares = new AdjEnvelope<double, Index, Exception::matvec>;
      break;
    case svd:
      least_squares = new AdjSVD<double, Exception::matvec>;
      break;
    case gso:
      least_squares = new AdjGSO<double, Exception::matvec>;
      break;
    case cholesky:
      least_squares = new AdjCholDec<double, Exception::matvec>;
      break;
    default:
      throw Exception::adjustment("### unknown algorithm");
    }

  if (const IntegerList<>* p = data->minx())
    {
      delete minx;
      minx_dim = 0;

      if (Index N = p->dim())
        {
          minx_dim = N;
          Index* q = minx = new Index[N];
          for (IntegerList<>::const_iterator
                 i=p->begin(), e=p->end(); i!=e; i++)
            {
              *q++ = *i;
            }
        }

      least_squares->min_x(minx_dim, minx);
    }

  if (AdjBaseSparse* sprs = dynamic_cast<AdjBaseSparse*>(least_squares))
    {
      sprs->reset(data);

      x_   = least_squares->unknowns();
      r_   = least_squares->residuals();
      rtr_ = least_squares->sum_of_squares();
    }
  else if (AdjBaseFull* full = dynamic_cast<AdjBaseFull*>(least_squares))
    {
      A_dot.reset(data->A->rows(), data->A->columns());
      A_dot.set_zero();
      b_dot.reset(data->A->rows());

      for (std::size_t k=1; k<=data->A->rows(); k++)
        {
          double* n = data->A->begin(k);
          double* e = data->A->end  (k);
          for(size_t* i=data->A->ibegin(k) ; n!=e; n++, i++)  A_dot(k,*i) = *n;
        }

      for (std::size_t i, j, dim, width, r=0, b=1;
           b<=data->pcov->blocks(); b++, r += dim)
        {
          dim   = data->pcov->dim(b);
          width = data->pcov->width(b);
          CovMat<> C(dim, width);

          const double *p = data->pcov->begin(b), *e = data->pcov->end(b);
          CovMat<>::iterator c = C.begin();
          while (p != e) *c++ = *p++;
          choldec(C);

          Vec<> t(dim);
          for (i=1; i<=dim; i++) t(i) = data->prhs(r+i);
          forwardSubstitution(C, t);
          for (i=1; i<=dim; i++) b_dot(r+i) = t(i);

          for (j=1; j<=data->A->columns(); j++)
            {
              for (i=1; i<=dim; i++) t(i) = A_dot(r+i,j);
              forwardSubstitution(C, t);
              for (i=1; i<=dim; i++) A_dot(r+i,j) = t(i);
            }
        }

      full->reset(A_dot, b_dot);

      const Vec<>& v = least_squares->residuals();
      rtr_ = trans(v)*v;

      x_   = least_squares->unknowns();

      const Vec<>& rhs = data->rhs();
      r_.reset(data->A->rows());
      for (Index i=1; i<=r_.dim(); i++)
        {
          double* b = data->A->begin(i);
          double* e = data->A->end(i);
          Index * n = data->A->ibegin(i);
          double  s = 0;
          while (b != e)
            s += *b++ * x_(*n++);

          r_(i) = s - rhs(i);
        }
    }
  else
    {
      throw Exception::adjustment("### unknown algorithm");
    }

  solved = true;
}



void Adj::set_algorithm(Adj::algorithm alg)
{
  switch (alg)
    {
    case envelope:
    case svd:
    case gso:
    case cholesky:
      solved = false;
      algorithm_ = alg;
      break;

    default:
      throw Exception::adjustment("### unknown algorithm");
    }
}



const Vec<>& Adj::x()
{
  if (!solved) init_least_squares();

  return x_;
}



const Vec<>& Adj::r()
{
  if (!solved) init_least_squares();

  return r_;
}



double Adj::q_bb(Index i, Index j)
{
  double* ib;
  double* ie;
  Index * in;

  double* jb = data->A->begin(j);
  double* je = data->A->end(j);
  Index * jn = data->A->ibegin(j);

  double t, sum = 0;
  while (jb != je)
    {
      ib = data->A->begin(i);
      ie = data->A->end(i);
      in = data->A->ibegin(i);
      t  = 0;
      while (ib != ie)  t += *ib++ * least_squares->q0_xx(*in++, *jn);

      sum += *jb * t;
      jb++;
      jn++;
    }

  return sum;
}



/* ######################################################################
 * functions cholesky() and forwardSubstitution are identical in Adj and
 * in LocalNetwork and shall be moved to a single class
 * ###################################################################### */

void Adj::choldec(CovMat<>& chol)
{
  chol.cholDec();

  using namespace std;
  const std::size_t N = chol.rows();
  const std::size_t b = chol.bandWidth();

  for (std::size_t m, j, i=1; i<=N; i++)
    {
      double d = sqrt(chol(i,i));
      chol(i,i) = d;

      m = i+b;  if(N < m) m = N;    // m = min(N, i+b);

      for (j=i+1; j<=m; j++) chol(i,j) *= d;
    }
}

void Adj::forwardSubstitution(const CovMat<>& chol, Vec<>& v)
{
  using namespace std;
  const std::size_t N = chol.rows();
  const std::size_t b = chol.bandWidth();

  for (std::size_t m, i=1; i<=N; i++)
    {
      if (i > b+1) m = i - b;
      else         m = 1;
      for (std::size_t j=m; j<i; j++) v(i) -= chol(i,j)*v(j);

      v(i) /= chol(i,i);
    }
}

