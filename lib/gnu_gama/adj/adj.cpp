/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

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

#include <gnu_gama/adj/adj.h>
#include <gnu_gama/adj/adj_input_data.h>
#include <gnu_gama/xml/dataparser.h>
#include <gnu_gama/size_to.h>
#include <vector>
#include <cstddef>
#include <algorithm>

using namespace GNU_gama;
using namespace std;



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
      least_squares = new AdjEnvelope<double, int, Exception::matvec>;
      break;
    case svd:
      least_squares = new AdjSVD<double, int, Exception::matvec>;
      break;
    case gso:
      least_squares = new AdjGSO<double, int, Exception::matvec>;
      break;
    case cholesky:
      least_squares = new AdjCholDec<double, int, Exception::matvec>;
      break;
    default:
      throw Exception::adjustment("### unknown algorithm");
    }

  if (const IntegerList<>* p = data->minx())
    {
      delete minx;
      minx_dim = 0;

      if (int   N = p->dim())
        {
          minx_dim = N;
          int  * q = minx = new int  [N];
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

      for (int k=1; k<=data->A->rows(); k++)
        {
          double* n = data->A->begin(k);
          double* e = data->A->end  (k);
          for(int* i=data->A->ibegin(k) ; n!=e; n++, i++)  A_dot(k,*i) = *n;
        }

      for (int i, j, dim, width, r=0, b=1;
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
      for (int   i=1; i<=r_.dim(); i++)
        {
          double* b = data->A->begin(i);
          double* e = data->A->end(i);
          int   * n = data->A->ibegin(i);
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



double Adj::q_bb(int   i, int   j)
{
  double* ib;
  double* ie;
  int   * in;

  double* jb = data->A->begin(j);
  double* je = data->A->end(j);
  int   * jn = data->A->ibegin(j);

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
  const int N = chol.rows();
  const int b = chol.bandWidth();

  for (int m, j, i=1; i<=N; i++)
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
  const int N = chol.rows();
  const int b = chol.bandWidth();

  for (int m, i=1; i<=N; i++)
    {
      if (i > b+1) m = i - b;
      else         m = 1;
      for (int j=m; j<i; j++) v(i) -= chol(i,j)*v(j);

      v(i) /= chol(i,i);
    }
}

