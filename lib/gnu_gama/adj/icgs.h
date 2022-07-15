/* Class ICGS for adjustment of geodetic/surveying free networks
 * Copyright (C) 2022 Ales Cepek <cepek@gnu.org>
 *
 * Algorithm Iterated Classical Gram-Schmidt algorithm (ICGS)
 * Based on https://cs.wikipedia.org/wiki/Gramova-Schmidtova_ortogonalizace
 *
 * This file is part of GNU Gama project.
 *
 * ICGS is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * ICGS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ICGS. If not, see <https://www.gnu.org/licenses/>.
 */


/* Class ICGS operates on the block matrix A stored by columns in
 * the array declared as type double[].
 *
 *    A = [ A11 A12; A21 A22 ]
 *
 * where sizes of the blocks are
 *
 *    A11 (m1,n1), A12(m1,n2), A21(m2,n1) and A(m2,n2)
 *
 * m1>0, n1>0, m2>=0, n2>=0.
 *
 * For xample, with overdetermined system of linear equations Ax = b,
 * where A has more rows than columns, would be A11 = A, A12 = b,
 * A21 identity matrix and A22 zero vector.
 *
 * After orthogonalization we get vector of Least Squares residuals in A12
 * and unknowns x in A22. Adjustment weight coefficients correspond
 * to dot products of rows from blocks [A11; A21]
 */

#ifndef ICGS_H
#define ICGS_H

#include <limits>
#include <utility>
#include <set>

namespace GNU_gama {

class ICGS
{
public:
  ICGS() = default;
  ICGS(double* M, int m1, int n1, int m2=0, int n2=0)
  {
    reset(M, m1, n1, m2, n2);
  }
  ICGS(const ICGS&) = delete;
  ICGS& operator=(const ICGS&) = delete;
  ICGS(const ICGS&&) = delete;
  ICGS& operator=(const ICGS&&) = delete;
  ~ICGS();

  void reset(double* M, int m1, int n1, int m2=0, int n2=0);

  void icgs1();
  void icgs2();

  // for least squares solution Ax = b, i.e. block matrix (A b; E 0)
  const double* residuals_begin() const;
  const double* residuals_end() const;
  const double* unknowns_begin() const;
  const double* unknowns_end() const;

  const std::set<int>& lindep_columns { lindep };
  int defect() const { return lindep.size(); };
  int error()  const { return error_icgs2_defect; }

  double cdot1st(int i, int j) const;      // columns dot product (A11)
  double rowdot (int i, int j) const;      // rows    dot product (A11 and A12)

  double norm1st(double*) const;           // column vector norm ||.||_2
  double norm2nd(double*) const;           // column vector norm ||.||_2

  void   blocks (int& m1, int& n1, int& m2, int& n2) const;
  double tol() const { return tolerance; };
  void   tol(double t) { tolerance = t; }

  void min_x();
  void min_x(int, int[]);
  const std::set<int>& min_x_indices { minx } ;

private:

  double*  A {nullptr};                    // block matrix storeb by columns
  bool     internal_data { false };
  int      M1 {0}, N1 {0}, M2 {0}, N2 {0}, M12 {0}, N12 {0};

  double** column {nullptr};               // column pointers list
  double** row    {nullptr};               // row    pointers
  double*  p_ak   {nullptr};               // column working space

  double tolerance { std::numeric_limits<double>::epsilon()*1e5 };

  void cscale1st(double* col, double sc);  // col *= sc
  void cscale2nd(double* col, double sc);
  void ccopy1st (double* from, double* to);
  void ccopy2nd (double* from, double* to);

  std::set<int> lindep;

  bool icgs1_is_ready { false };
  bool min_x_use_all  { true  };
  std::set<int> minx;

  int error_icgs2_defect {0};

  /* the templates are not needed unless using GNU_gama matvec library */

public:
  template <class Mat> ICGS(const Mat& T, int m1=0, int n1=0);
  template <class Mat> void reset(const Mat& T, int m1=0, int n1=0);
  template <class Mat> void getMat(Mat& T) const;
};


template <class Mat> ICGS::ICGS(const Mat& T, int m1, int n1)
{
  set(T, m1, n1);
}

template <class Mat> void ICGS::reset(const Mat& T, int m1, int n1)
{
  icgs1_is_ready = false;
  {
    // A block matrix [A11, A12; A21, A22]
    //
    // A11  M1 x N1  design matrix
    // A12  M1 x N2  right hand side(s)
    // A21  M2 x N1  e.g. unit matrix
    // A22  M2 x N2  e.g. zero vector

    M1 = m1;
    M2 = T.rows() - M1;
    N1 = n1;
    N2 = T.cols() - N1;

    M12 = M1 + M2;
    N12 = N1 + N2;

    if (internal_data && A) delete A;
    A = new double[ M12*N12 ];
    internal_data = true;

    if (column) delete column;
    column = new double* [ N12+1 ];  // +1 : 1-based indexing
    column[0] = nullptr;             // unused
    double* t = A;                   // first column
    for (int j=1; j<=N12; j++)
      {
        column[j] = t;
        t += M12;
      }

    if (row) delete row;
    row = new double* [ M12 + 1 ];
    row[0] = nullptr;
    t = A;
    for (int i=1; i<=M12; i++)
      {
        row[i] = t++;
      }

    icgs1_is_ready = true;
  }

  double* a = A;
  for (int j=1; j<=N12; j++)
    for (int i=1; i<=M12; i++)
      *a++ = T(i,j);
}

template <class Mat> void ICGS::getMat(Mat& T) const
{
  T.reset(M12,N12);

  double* a = A;
  for (int j=1; j<=N12; j++)
    for (int i=1; i<=M12; i++)
      T(i,j) = *a++;
}

}
#endif // ICGS_H
