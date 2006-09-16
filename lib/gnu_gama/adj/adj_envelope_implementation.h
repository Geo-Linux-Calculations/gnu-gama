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
 *  $Id: adj_envelope_implementation.h,v 1.4 2006/09/16 10:20:39 cepek Exp $
 */

#ifndef GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h
#define GNU_Gama_gnu_gama_adj_envelope_implementationenvelope__implementation_h

#include <gnu_gama/adj/adj_envelope.h>
#include <gnu_gama/adj/homogenization.h>

namespace 
{
  static GaMaLib::Vec b;
  static GaMaLib::Mat A;
}

namespace GNU_gama {

  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::reset(const AdjInputData *data) 
  {
    this->input = data;
    this->stage = 0;

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

      std::cout << "MatVec = " << inv(trans(A)*A)*(trans(A)*b);
    }
  }


  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::solve_ordering() 
  {
    if (this->stage >= stage_ordering) return;

    Homogenization    <Float, Index>   hom(this->input);
    const SparseMatrix<Float, Index>*  mat = hom.mat();
    const Vec         <Float>&         rhs = hom.rhs();
    SparseMatrixGraph <Float, Index>   graph(mat);

    ordering.reset(&graph);

    // std::cout<<"\n****  rusim ordering .... pracuji v puvodni soustave\n";
    // for (Index i=1; i<=ordering.nodes(); i++)
    //   {
    //     ordering.perm(i) = i;
    //     ordering.invp(i) = i;
    //   }
    // std::cout<<"****************************************************\n\n";
    // std::cout << "\nPERM = ";
    // for (Index i=1; i<=ordering.nodes(); i++)
    //   {
    //     std::cout << ordering.perm(i) << " ";
    //   }
    // std::cout << "\n";
    // std::cout << "INVP = ";
    // for (Index i=1; i<=ordering.nodes(); i++)
    //   {
    //     std::cout << ordering.invp(i) << " ";
    //   }
    // std::cout << "\n";

    const Index N = mat->columns();
    
    tmpvec.reset(N);
    tmpvec.set_zero();
    
    for (Index r=1; r<=mat->rows(); r++)
      {
        const Float* b=mat->begin (r);
        const Float* e=mat->end   (r);
        const Index* n=mat->ibegin(r);
        
        while (b != e)
          {
            const Index c = ordering.invp(*n++);
            const Float a = *b++;
            
            // absolute terms in normal equations
            tmpvec(c) +=  a * rhs(r);              
          }
      }

    std::cout << "\nRRHHSS = " << trans(tmpvec);
    
    envelope.set(mat, &graph, &ordering);   

    this->stage = stage_ordering;
  }


  template <typename Float, typename Index, typename Exc> 
  void AdjEnvelope<Float, Index, Exc>::solve_x0() 
  {
    if (this->stage >= stage_x0) return;
    solve_ordering();

    std::cout << "\npred choleskym : ";  envelope.write_xml(std::cout);

    /* !! */  Mat<> N(envelope.dim(), envelope.dim());
    /* !! */  N.set_zero();
    /* !! */  for (Index i=1; i<=envelope.dim(); i++)
    /* !! */    {
    /* !! */      N(i,i) = envelope.diagonal(i);
    /* !! */      double* b = envelope.begin(i);
    /* !! */      double* e = envelope.end(i);
    /* !! */      for (Index j=i-(e-b); j<i; j++)
    /* !! */        {
    /* !! */          N(i,j) = N(j,i) = *b++;
    /* !! */        }
    /* !! */    }
    /* !! */  std::cout << N;

    envelope.cholDec();
    std::cout << "\npo choleskym   : ";  envelope.write_xml(std::cout);

    /* !! */  Mat<> L(envelope.dim(), envelope.dim());
    /* !! */  Mat<> D(envelope.dim(), envelope.dim());
    /* !! */  L.set_zero();
    /* !! */  D.set_zero();
    /* !! */  for (Index i=1; i<=envelope.dim(); i++)
    /* !! */    {
    /* !! */      D(i,i) = envelope.diagonal(i);
    /* !! */      L(i,i) = 1;
    /* !! */      double* b = envelope.begin(i);
    /* !! */      double* e = envelope.end(i);
    /* !! */      for (Index j=i-(e-b); j<i; j++)
    /* !! */        {
    /* !! */          L(i,j) = *b++;
    /* !! */        }
    /* !! */    }
    /* !! */  std::cout << D << L;
    /* !! */  Mat<> ERROR = N - L*D*trans(L);
    /* !! */  std::cout << ERROR;
    /* !! */  double error = 0;
    /* !! */  for (Index i=1; i<=ERROR.rows(); i++)
    /* !! */    for (Index j=1; j<=ERROR.cols(); j++) 
    /* !! */      error = std::max(error, std::abs(ERROR(i,j)));
    /* !! */  std::cout << "\nCHOLESKY ERROR = " 
    /* !! */            << error << std::endl << std::endl;

    envelope.solve(tmpvec.begin(), tmpvec.dim());

    x0.reset(tmpvec.dim());
    for (Index i=1; i<=tmpvec.dim(); i++) 
      {
        x0(ordering.perm(i)) = tmpvec(i);
      }
    tmpvec.reset();

    std::cout << "\nVYSLEDKY Z ENVELOPE = " << x0;
    std::cout << "------------------------- ******\n";

    this->stage = stage_x0;
  }
    
}

#endif
