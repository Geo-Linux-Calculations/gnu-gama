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

#include <matvec/covmat.h>
#include <gnu_gama/sparse/smatrix.h>
#include <gnu_gama/sparse/sbdiagonal.h>
#include <gnu_gama/sparse/intlist.h>
#include <gnu_gama/adj/adj_envelope.h>
#include <gnu_gama/adj/adj_svd.h>
#include <gnu_gama/adj/adj_gso.h>
#include <gnu_gama/adj/adj_chol.h>

#include <iostream>

#ifndef gama_local_Adj_input_data_h
#define gama_local_Adj_input_data_h


namespace GNU_gama {

  /** \brief Adjustment input data class.
   */

  class AdjInputData {
  public:

    AdjInputData();
    ~AdjInputData();

    void write_xml(std::ostream&) const;
    void read_xml(std::istream&);

    /** Sparse design matrix */
    const SparseMatrix <> * mat () const { return A;     }
    /** Block diagonal matrix of observtion covariances */
    const BlockDiagonal<> * cov () const { return pcov;  }
    /** Right-hand site*/
    const Vec          <> & rhs () const { return prhs;  }
    /** List of parameters indexes used in regulrization of singular systems */
    const IntegerList  <> * minx() const { return pminx; }

    void set_mat (SparseMatrix <> * p) { delete A;     A     = p; }
    void set_cov (BlockDiagonal<> * p) { delete pcov;  pcov  = p; }
    void set_rhs (Vec          <>   p) {               prhs  = p; }
    void set_minx(IntegerList  <> * p) { delete pminx; pminx = p; }

    /* Sparse project equations for uncorrelated observations. *
     * Defined here only for backward data compatibility       */
    void read_gama_local_old_format(std::istream&);


  private:

    friend class Adj;

    SparseMatrix <> * A;
    BlockDiagonal<> * pcov;
    Vec          <>   prhs;
    IntegerList  <> * pminx;

    void swap(AdjInputData *);
  };

}  // namespace GNU_gama

#endif
