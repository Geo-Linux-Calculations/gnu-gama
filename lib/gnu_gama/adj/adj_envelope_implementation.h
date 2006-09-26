/*  
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU Gama C++ library
    
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
 *  $Id: adj_envelope_implementation.h,v 1.7 2006/09/26 18:02:09 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h
#define GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h

#include <gnu_gama/adj/adj_envelope.h>

namespace 
{
  static GaMaLib::Vec b;
  static GaMaLib::Mat A;
}

namespace GNU_gama {

  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::reset(const AdjInputData *data) 
  {
    observations = data->mat()->rows();
    parameters   = data->mat()->columns();
    this->input  = data;

    set_stage(stage_init);


    // ######  LADENI  ##########################################

    Homogenization<> hom(data);

    const SparseMatrix<>* mat = hom.mat();
    const Vec<Float>&     rhs = hom.rhs();

    {
      Index M = mat->rows();
      Index N = mat->columns();
    
      b.reset(M);
      for (Index i=1; i<=M; i++) b(i) = rhs(i);
      
      A.reset(M, N);
      A.set_zero();
      for (Index i=1; i<=M; i++)
        {
          Float *b = mat->begin(i);
          Float *e = mat->end(i);
          Index *n = mat->ibegin(i);
          while(b != e)
            {
              A(i, *n) = *b;
              
              b++;
              n++;
            }
        }
      
      chol->reset(A, b);
    }
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

    set_stage(stage_x0);
  }


  template <typename Float, typename Index, typename Exc> 
  const GNU_gama::Vec<Float, Exc>& 
  AdjEnvelope<Float, Index, Exc>::unknowns()
  {
    solve();
    if (defect() == 0) return x0;   // !!!!!!!!!!!!!!

    return chol->unknowns();
  }    


  template <typename Float, typename Index, typename Exc> 
  const GNU_gama::Vec<Float, Exc>& 
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

    return envelope.defect();
  }


  template <typename Float, typename Index, typename Exc> 
  Float AdjEnvelope<Float, Index, Exc>::q_xx(Index i, Index j)
  {
    return chol->q_xx(i,j);
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
            if (qk == 0) goto FULL_VECTOR;
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
  Float AdjEnvelope<Float, Index, Exc>::q_bx(Index i, Index j)
  {
    return chol->q_bx(i,j);
  }


  template <typename Float, typename Index, typename Exc> 
  bool AdjEnvelope<Float, Index, Exc>::lindep(Index i)
  {
    if (this->stage < stage_x0) solve_x0();

    return chol->lindep(i);  // envelope zlobi v bug-1.3.25-zpk.gkf
    //return (envelope.diagonal(i) == Float());
  }


  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::min_x()
  {
    chol->min_x();
  }


  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::min_x(Index n, Index m[])
  {
    chol->min_x(n, m);
  }


  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::solve()
  {
    solve_x0(); 
    chol->solve(); 
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
}

#endif
