/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2002  Ales Cepek <cepek@gnu.org>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
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
 *  $Id: adj.h,v 1.4 2007/03/29 12:31:36 cepek Exp $
 */

#include <matvec/covmat.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/sbdiagonal.h>
#include <gnu_gama/sparse/intlist.h>
#include <gnu_gama/adj/adj_envelope.h>
#include <gnu_gama/adj/adj_svd.h>
#include <gnu_gama/adj/adj_gso.h>
#include <gnu_gama/adj/adj_chol.h>

#include <iostream>

#ifndef GaMaLib_Adj__adjustment_class__h
#define GaMaLib_Adj__adjustment_class__h


namespace GNU_gama {


  class AdjInputData;

  class Adj 
  {
  public:
    
    enum algorithm {envelope, gso, svd, cholesky};
    
    Adj () : data(0), algorithm_(envelope), minx_dim(0), minx(0) { init(0); }
    virtual ~Adj();
    
    int n_obs() const { return n_obs_; }
    int n_par() const { return n_par_; }
    
    void set(const AdjInputData* inp) { init(inp); }

    void set_algorithm(Adj::algorithm);
    Adj::algorithm get_algorithm() const { return algorithm_; }
  
    int    defect() const { return least_squares->defect(); }
    double rtr   () const { return rtr_; }     /* weighted sum of squares */
    const Vec<>& x();                          /* adjusted parameters     */
    const Vec<>& r();                          /* adjusted residuals      */

    double q_xx(Index i, Index j) { return least_squares->q_xx(i,j); }
    double q_bb(Index i, Index j);
  
  private:
    
    const AdjInputData *data;

    typedef GNU_gama::AdjBase<double, Index, Vec<> >         AdjBase;
    typedef GNU_gama::AdjBaseFull<double, Exception::matvec> AdjBaseFull;
    typedef GNU_gama::AdjBaseSparse<double, Index, Vec<>,
                                    GNU_gama::AdjInputData>  AdjBaseSparse;

    AdjBase*       least_squares;

    bool      solved;
    algorithm algorithm_;
    int       n_obs_, n_par_;
    Mat <>    A_dot;
    Vec <>    b_dot;
    Vec <>    x_;
    Vec <>    r_;
    double    rtr_;
  
    void init(const AdjInputData*);
    void init_least_squares();
    void choldec (CovMat<>& chol);                            // move it away!
    void forwardSubstitution(const CovMat<>& chol, Vec<>& v); // move it away!

    Index  minx_dim;
    Index* minx;
  };
  

  class AdjInputData {
  public:

    AdjInputData();
    ~AdjInputData();
    
    void write_xml(std::ostream&) const;
    void read_xml(std::istream&);

    const SparseMatrix <> * mat () const { return A;     }
    const BlockDiagonal<> * cov () const { return pcov;  }
    const Vec          <> & rhs () const { return prhs;  }
    const IntegerList  <> * minx() const { return pminx; } 

    void set_mat (SparseMatrix <> * p) { delete A;     A     = p; }
    void set_cov (BlockDiagonal<> * p) { delete pcov;  pcov  = p; }
    void set_rhs (Vec          <>   p) {               prhs  = p; }
    void set_minx(IntegerList  <> * p) { delete pminx; pminx = p; } 

    void swap(AdjInputData *);

    /* Sparse project equations for uncorrelated observations. *
     * Defined here only for backward data compatibility       */
    void read_gama_local_old_format(std::istream&);


  private:

    friend class Adj;

    SparseMatrix <> * A;
    BlockDiagonal<> * pcov;
    Vec          <>   prhs;
    IntegerList  <> * pminx;

  };
  
}  // namespace GNU_gama

#endif









