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
 *  $Id: adj_envelope.h,v 1.4 2006/09/22 15:45:31 cepek Exp $
 */

#ifndef GNU_Gama___gnu_gama_adj_envelope___gnugamaadjenvelope___adj_envelope_h
#define GNU_Gama___gnu_gama_adj_envelope___gnugamaadjenvelope___adj_envelope_h


#include <gnu_gama/adj/adj_basesparse.h>
#include <gnu_gama/adj/adj_chol.h>
#include <gnu_gama/adj/envelope.h>
#include <gnu_gama/sparse/smatrix_ordering.h>
#include <gnu_gama/adj/homogenization.h>

namespace GNU_gama {


  template <typename Float=double,  typename Index=std::size_t,
            typename Exc=Exception::matvec> 
  class AdjEnvelope : public AdjBaseSparse<Float, Index, 
                                           GNU_gama::Vec<Float, Exc>, 
                                           AdjInputData >
  {
  public:

    AdjEnvelope() : chol(new AdjCholDec<Float, Exc>) {}
    ~AdjEnvelope() { delete chol; }

    virtual const GNU_gama::Vec<Float, Exc>& unknowns();
    virtual const GNU_gama::Vec<Float, Exc>& residuals();
    virtual Float sum_of_squares();
    virtual Index defect();
 
    virtual Float q_xx(Index i, Index j);
    virtual Float q_bb(Index i, Index j);
    virtual Float q_bx(Index i, Index j);
                                                                      
    virtual bool lindep(Index i);
    virtual void min_x();
    virtual void min_x(Index n, Index m[]);

    void solve();

    virtual void reset(const AdjInputData *data);

  private:

    AdjCholDec<Float, Exc>*          chol;  // #########################   

    ReverseCuthillMcKee<Index>   ordering;
    Homogenization<Float, Index>      hom;
    Envelope<Float, Index>       envelope;
    GNU_gama::Vec<Float, Exc>          x0;        // particular solution
    GNU_gama::Vec<Float, Exc>       resid;        // residuals
    Float                         squares;        // sum of squares

    GNU_gama::Vec<Float, Exc>     tmpvec;   

    enum { 
      stage_init,      // implicitly set by Adj_BaseSparse constuctor
      stage_ordering,  // permutation vector
      stage_x0,        // particular solution (dependent unknown set to 0)
      stage_residuals  // residuals r = Ax - b
    };

    void solve_ordering();
    void solve_x0();
  };

}  // namespace GNU_gama

#include <gnu_gama/adj/adj_envelope_implementation.h> 

#endif
