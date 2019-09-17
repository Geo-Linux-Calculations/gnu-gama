/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2006, 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

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

#ifndef GNU_Gama_gnu_gama_adj_envelope_gnugamaadjenvelope_adj_envelope_h
#define GNU_Gama_gnu_gama_adj_envelope_gnugamaadjenvelope_adj_envelope_h


#include <gnu_gama/adj/adj_basesparse.h>
#include <gnu_gama/adj/envelope.h>
#include <gnu_gama/sparse/smatrix_ordering.h>
#include <gnu_gama/adj/homogenization.h>
#include <gnu_gama/movetofront.h>
#include <vector>

namespace GNU_gama {


  template <typename Float=double,  typename Index=int,
            typename Exc=Exception::matvec>
  class AdjEnvelope
    : public AdjBaseSparse<Float, Index, Exc, AdjInputData>
  {
  public:

    AdjEnvelope() : min_x_list(nullptr) {}
    ~AdjEnvelope() override { delete[] min_x_list; }

    AdjEnvelope(const AdjEnvelope&) = delete;
    AdjEnvelope& operator= (const AdjEnvelope&) = delete;
    AdjEnvelope(const AdjEnvelope&&) = delete;
    AdjEnvelope& operator= (const AdjEnvelope&&) = delete;

    const GNU_gama::Vec<Float, Index, Exc>& unknowns() override;
    const GNU_gama::Vec<Float, Index, Exc>& residuals() override;
    Float sum_of_squares() override;
    Index defect() override;

    Float q_xx(Index i, Index j) override;
    Float q_bb(Index i, Index j) override;
    Float q_bx(Index i, Index j) override;

    Float q0_xx(Index i, Index j) override;

    bool lindep(Index i) override;
    void min_x() override;
    void min_x(Index n, Index m[]) override;

    void solve();

    void reset(const AdjInputData *data) override;

  private:

    ReverseCuthillMcKee<Index>   ordering;
    Homogenization<Float, Index>      hom;
    Envelope<Float, Index>       envelope;

    Index                    observations;
    Index                      parameters;
    const SparseMatrix<>*   design_matrix {nullptr};
    GNU_gama::Vec<Float, Index, Exc>   x0;    // particular solution
    GNU_gama::Vec<Float, Index, Exc>    x;    // unique or regularized solution
    GNU_gama::Vec<Float, Index, Exc>resid;        // residuals
    Float                         squares;        // sum of squares
    Envelope<Float, Index>             q0;        // weight coefficients for x0

    GNU_gama::Vec<Float, Index, Exc> tmpvec;
    GNU_gama::Vec<Float, Index, Exc> tmpres;         // used in q_bb

    std::vector<GNU_gama::Vec<Float, Index, Exc>> qxxbuf;
    GNU_gama::MoveToFront<3,Index,Index>          indbuf;

    enum Stage {
      stage_init,       // implicitly set by Adj_BaseSparse constuctor
      stage_ordering,   // permutation vector
      stage_x0,         // particular solution (dependent unknown set to 0)
      stage_q0
    };

    bool init_q_bb{};         // weight coefficieants of adjusted observations
    bool init_residuals{};    // residuals r = Ax - b
    bool init_q0{};           // weight coefficients of particular solution x0
    bool init_x{};            // unique or regularized solution

    void set_stage(Stage s);
    void solve_ordering();
    void solve_x0();
    void solve_x();
    void solve_q0();
    void T_row(GNU_gama::Vec<Float, Index, Exc>& row, Index i);

    Index nullity;
    Mat<Float, Index, Exc> G;
    Float dot(Index i, Index j) const;

    Index* min_x_list;
    Index  min_x_size;
  };

  // ---  Implementation  ------------------------------------------------

  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::reset(const AdjInputData *data)
  {
    observations = data->mat()->rows();
    parameters   = data->mat()->columns();
    this->input  = data;

    indbuf.erase();
    qxxbuf.resize(indbuf.size());
    for (Index i=0; i<size_to<Index>(qxxbuf.size()); i++)
      qxxbuf[i].reset();

    set_stage(stage_init);
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::set_stage(Stage s)
  {
    switch (s)
      {
      default:
      case stage_init:
      case stage_ordering:
      case stage_x0:
        init_residuals = true;
        init_q0        = true;
        init_x         = true;
      case stage_q0:
        init_q_bb      = true;
        ;
      }

    this->stage = s;
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::solve_ordering()
  {
    if (this->stage >= stage_ordering) return;

    hom.reset(this->input);
    design_matrix = hom.mat();

    SparseMatrixGraph <Float, Index> graph(design_matrix);
    ordering.reset(&graph);
    // std::cerr << "renumbering is suppressed!\n";
    // for (int i=1; i<=design_matrix->columns(); i++)
    //   ordering.perm(i) = ordering.invp(i) = i;

    const Vec<Float>& rhs = hom.rhs();
    const Index N = design_matrix->columns();
    tmpvec.reset(N);
    tmpvec.set_zero();

    for (Index r=1; r<=design_matrix->rows(); r++)
      {
        const Float* b=design_matrix->begin (r);
        const Float* e=design_matrix->end   (r);
        const Index* n=design_matrix->ibegin(r);

        while (b != e)
          {
            const Index c = ordering.invp(*n++);
            const Float a = *b++;

            // absolute terms in normal equations
            tmpvec(c) +=  a * rhs(r);
          }
      }

    envelope.set(design_matrix, &graph, &ordering);

    set_stage(stage_ordering);
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::solve_x0()
  {
    if (this->stage >= stage_x0) return;
    solve_ordering();

    // Cholesky decomposition L*D*L'

    envelope.cholDec();

    // particular solution x0

    envelope.solve(tmpvec.begin(), tmpvec.dim());

    x0.reset(tmpvec.dim());
    for (Index i=1; i<=tmpvec.dim(); i++)
      {
        x0(ordering.perm(i)) = tmpvec(i);
      }
    tmpvec.reset();

    // sum of squares of weighted residuals

    const SparseMatrix<Float, Index>*  mat = hom.mat();
    const Vec         <Float>&         rhs = hom.rhs();
    squares = 0;
    for (Index i=1; i<=mat->rows(); i++)
      {
        Float *b = mat->begin(i);
        Float *e = mat->end(i);
        Index *n = mat->ibegin(i);
        Float  s = Float();
        while(b != e)
          {
            s += *b++ * x0(*n++);
          }

        const Float t = s - rhs(i);
        squares += t*t;
      }

    nullity = envelope.defect();

    if (nullity)
      {
        qxxbuf.resize(indbuf.size());
        for (Index i=0; i<size_to<Index>(qxxbuf.size()); i++)
          qxxbuf[i].reset(parameters);
      }

    set_stage(stage_x0);
  }


  template <typename Float, typename Index, typename Exc>
  const GNU_gama::Vec<Float, Index, Exc>&
  AdjEnvelope<Float, Index, Exc>::unknowns()
  {
    if (init_x) solve_x();

    return x;
  }


  template <typename Float, typename Index, typename Exc>
  const GNU_gama::Vec<Float, Index, Exc>&
  AdjEnvelope<Float, Index, Exc>::residuals()
  {
    if (init_residuals)
      {
        if (this->stage < stage_x0) solve_x0();

        const SparseMatrix<Float, Index>* mat = this->input->mat();
        const Vec<>&                      rhs = this->input->rhs();
        const Index N = rhs.dim();
        resid.reset(N);

        for (Index i=1; i<=N; i++)        // residuals = Ax - rhs
          {
            Float *b = mat->begin(i);
            Float *e = mat->end(i);
            Index *n = mat->ibegin(i);
            Float  s = Float();
            while(b != e)
              {
                s += *b++ * x0(*n++);
              }

            resid(i) = s - rhs(i);
          }

        init_residuals = false;
      }

    return resid;
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::sum_of_squares()
  {
    if (this->stage < stage_x0) solve_x0();

    return squares;
  }


  template <typename Float, typename Index, typename Exc>
  Index AdjEnvelope<Float, Index, Exc>::defect()
  {
    if (this->stage < stage_x0) solve_x0();

    return nullity;
  }


  // T = I - alpha*inv(alpha'*alpha)*alpha'

  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>
    ::T_row(GNU_gama::Vec<Float, Index, Exc>& row, Index ii)
  {
    Float t;
    const Index i = ordering.invp(ii);
    for (Index jj=1; jj<=parameters; jj++)
      {
        const Index j = ordering.invp(jj);
        t = Float();
        if (i == j) t = Float(1);

        for (Index k=0; k<min_x_size; k++)
          if (min_x_list[k] == jj)
            {
              for (Index c=1; c<=nullity; c++) t -= G(i,c)*G(j,c);
              break;
            }

        row(j) = t;
      }
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::q_xx(Index i, Index j)
  {
    if (this->stage < stage_q0) solve_q0();

    if (nullity == 0) return q0_xx(i, j);

    // singular system

    if (init_x) solve_x();

    std::pair<Index,bool> pa = indbuf.get(i);
    std::pair<Index,bool> pb = indbuf.get(j);

    Vec<Float, Index, Exc>& a = qxxbuf[pa.first];
    Vec<Float, Index, Exc>& b = qxxbuf[pb.first];
    if (!pa.second)
      {
        T_row(a, i);
        envelope.lowerSolve(1, parameters, a.begin());
      }
    if (!pb.second)
      {
        T_row(b, j);
        envelope.lowerSolve(1, parameters, b.begin());
      }

    Float s = Float();
    for (Index i=1; i<=parameters; i++)
      if (const Float d = envelope.diagonal(i))
        s += a(i)/d*b(i);

    return s;
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::q0_xx(Index i, Index j)
  {
    if (this->stage < stage_q0) solve_q0();

    Float* q = q0.element(ordering.invp(i), ordering.invp(j));
    if (q) return *q;

    // elements outside the envelope (full solution)

    if (qxxbuf[0].dim() != parameters)
      {
        qxxbuf.resize(indbuf.size());
        for (Index i=0; i<size_to<Index>(qxxbuf.size()); i++)
          qxxbuf[i].reset(parameters);
      }

    Index ii = ordering.invp(i);
    Index jj = ordering.invp(j);
    if (ii < jj) std::swap(ii, jj);

    std::pair<Index,bool> pa = indbuf.get(ii);

    Vec<Float, Index, Exc>& a = qxxbuf[pa.first];
    if (!pa.second)
      {
        a.set_zero();
        a(ii) = 1;
        envelope.solve(a.begin(), a.dim());
      }

    return a(jj);
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::q_bb(Index i, Index j)
  {
    if (this->stage < stage_q0) solve_q0();

    // if i == j the test for null pointers is redundant

    const Float* b2;
    const Float* e2;
    const Index* n2;
    const Float* b = design_matrix->begin (i);
    const Float* e = design_matrix->end   (i);
    const Index* n = design_matrix->ibegin(i);
    const Float* qk;
    Float qbb = Float();
    while (b != e)
      {
        const Index k = ordering.invp(*n++);
        b2 = design_matrix->begin (j);
        e2 = design_matrix->end   (j);
        n2 = design_matrix->ibegin(j);
        Float s = Float();
        while (b2 != e2)
          {
            qk = q0.element(k, ordering.invp(*n2++));
            if (qk == nullptr) goto FULL_VECTOR;
            s += *qk * *b2++;
          }
        qbb += *b++ * s;
      }
    return qbb;


  FULL_VECTOR:

    if (init_q_bb)
    {
      tmpres.reset(parameters);
      init_q_bb = false;
    }

    tmpres.set_zero();
    b = design_matrix->begin (j);
    e = design_matrix->end   (j);
    n = design_matrix->ibegin(j);
    while (b != e)
      {
        tmpres(ordering.invp(*n++)) = *b++;
      }

    envelope.solve(tmpres.begin(), parameters);

    b = design_matrix->begin (i);
    e = design_matrix->end   (i);
    n = design_matrix->ibegin(i);
    Float s = Float();
    while (b != e)
      {
        s += *b++ * tmpres(ordering.invp(*n++));
      }

    return s;
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::q_bx(Index, Index)
  {
    throw Exc(Exception::BadRegularization,
              "q_bx not implemented");
    return 0;
  }


  template <typename Float, typename Index, typename Exc>
  bool AdjEnvelope<Float, Index, Exc>::lindep(Index i)
  {
    if (this->stage < stage_x0) solve_x0();

    return (envelope.diagonal(i) == Float());
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::min_x()
  {
    delete[] min_x_list;
    min_x_list = nullptr;

    init_x = true;
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::min_x(Index n, Index m[])
  {
    delete[] min_x_list;
    min_x_size = n;
    min_x_list = new Index[min_x_size];
    for (Index i=0; i<min_x_size; i++)
      min_x_list[i] = m[i];

    init_x = true;
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::solve()
  {
    solve_x();
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::solve_q0()
  {
    if (init_q0)
      {
        if (this->stage < stage_x0) solve_x0();

        q0.inverse(envelope);

        init_q0 = false;
        set_stage(stage_q0);
      }
  }


  template <typename Float, typename Index, typename Exc>
  Float AdjEnvelope<Float, Index, Exc>::dot(Index i, Index j) const
  {
    Float s = Float();
    for (Index n=0; n<min_x_size; n++)
      {
        const Index k = ordering.invp(min_x_list[n]);
        s += G(k,i)*G(k,j);
      }
    return s;
  }


  template <typename Float, typename Index, typename Exc>
  void AdjEnvelope<Float, Index, Exc>::solve_x()
  {
    if (init_x)
      {
        if (min_x_list == nullptr)   // regularization for all parameters
          {
            min_x_size = parameters;
            min_x_list = new Index[min_x_size];
            for (Index i=0; i<min_x_size; i++)
              min_x_list[i] = i+1;
          }

        if (this->stage < stage_x0) solve_x0();
        init_x = false;
        if (defect() == 0)
          {
            x = x0;
            return;
          }

        nullity = defect();
        const Index N1 = nullity+1;
        G.reset(parameters, N1);
        Vec<Float, Index, Exc> tmp(parameters);

        for (Index k=1, column=1; column<=parameters; column++)
          if (envelope.diagonal(column) == 0)
            {
              for (Index i=1; i<=parameters; i++)
                if (Float* e = envelope.element(i, column))
                  tmp(i) = *e;
                else
                  tmp(i) = Float();

              envelope.upperSolve(1, parameters, tmp.begin());

              tmp(column) = Float(-1);
              for (Index i=1; i<=parameters; i++)
                  G(i,k) = tmp(i);

              k++;
            }

        for (Index i=1; i<=parameters; i++)
          G(ordering.invp(i), N1) = x0(i);


        // Gramm-Schmidt orthogonalization

        static Float s_tol = Float();
        if (s_tol <= Float())
          {
            s_tol = std::sqrt( std::numeric_limits<Float>::epsilon() );
          }

        for (Index column=1; column<=nullity; column++)
          {
            const Float pivot = std::sqrt( dot(column, column) );
            if (pivot < s_tol)
              {
                init_x = true;
                throw Exc(Exception::BadRegularization,
                        "AdjEnvelope::solve_x() --- bad regularization");
              }
            for (Index i=1; i<=parameters; i++)
              G(i,column) /= pivot;

            for (Index col=column+1; col<=N1; col++)
              {
                const Float dp = dot(column, col);
                for (Index i=1; i<=parameters; i++)
                  G(i,col) -= dp*G(i, column);
              }
          }

        x.reset(parameters);
        for (Index i=1; i<=parameters; i++)
          x(ordering.perm(i)) = G(i, N1);
      }
  }

}  // namespace GNU_gama

#endif
