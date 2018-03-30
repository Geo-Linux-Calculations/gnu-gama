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

    AdjEnvelope() : min_x_list(0) {}
    ~AdjEnvelope() { delete[] min_x_list; }

    virtual const GNU_gama::Vec<Float, Index, Exc>& unknowns();
    virtual const GNU_gama::Vec<Float, Index, Exc>& residuals();
    virtual Float sum_of_squares();
    virtual Index defect();

    virtual Float q_xx(Index i, Index j);
    virtual Float q_bb(Index i, Index j);
    virtual Float q_bx(Index i, Index j);

    virtual Float q0_xx(Index i, Index j);

    virtual bool lindep(Index i);
    virtual void min_x();
    virtual void min_x(Index n, Index m[]);

    void solve();

    virtual void reset(const AdjInputData *data);

  private:

    ReverseCuthillMcKee<Index>   ordering;
    Homogenization<Float, Index>      hom;
    Envelope<Float, Index>       envelope;

    Index                    observations;
    Index                      parameters;
    const SparseMatrix<>*   design_matrix;
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

    bool init_q_bb;         // weight coefficieants of adjusted observations
    bool init_residuals;    // residuals r = Ax - b
    bool init_q0;           // weight coefficients of particular solution x0
    bool init_x;            // unique or regularized solution

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

}  // namespace GNU_gama

#include <gnu_gama/adj/adj_envelope_implementation.h>

#endif
