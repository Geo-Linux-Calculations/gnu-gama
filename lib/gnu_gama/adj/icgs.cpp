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

#include <gnu_gama/adj/icgs.h>
#include <cmath>
#include <memory>
#include <algorithm>

using namespace GNU_gama;


ICGS::~ICGS()
{
  if (internal_data) delete[] A;
  delete[] p_ak;
  delete[] column;
  delete[] row;
}

const double* ICGS::residuals_begin() const
{
  return A + M12*N1;
}

const double* ICGS::residuals_end() const
{
  return A + M12*N1 + M1;
}

const double* ICGS::unknowns_begin() const
{
  return A + M12*N1 + M1;
}

const double* ICGS::unknowns_end() const
{
  return A + M12*N1 + M1 + N1;
}

void ICGS::min_x()
{
  min_x_use_all = true;
}

void ICGS::min_x(int n, int list[])
{
  min_x_use_all = false;
  minx.clear();
  for (int i=0; i<n; i++) minx.insert(list[i]);
}

void ICGS::reset(double* a, int m1, int n1, int m2, int n2)
{
  if (n2 == 0) n2 = 1;
  if (m2 == 0) m2 = n1;

  icgs1_is_ready = false;

  // A block matrix [A11, A12; A21, A22]
  //
  // A11  M1 x N1  design matrix
  // A12  M1 x N2  right hand side(s)
  // A21  M2 x N1  e.g. unit matrix
  // A22  M2 x N2  e.g. zero vector

  M1 = m1;
  M2 = m2;
  N1 = n1;
  N2 = n2;

  M12 = M1 + M2;
  N12 = N1 + N2;

  if (internal_data && A) delete[] A;
  A = a;
  internal_data = false;

  if (column) delete[] column;
  column = new double* [ N12+1 ];  // +1 : 1-based indexing
  column[0] = nullptr;             // unused
  double* t = A;                   // first column
  for (int j=1; j<=N12; j++)
    {
      column[j] = t;
      t += M12;
    }

  if (row) delete[] row;
  row = new double* [ M12 + 1 ];
  row[0] = nullptr;
  t = A;
  for (int i=1; i<=M12; i++)
    {
      row[i] = t++;
    }

  icgs1_is_ready = true;
}

double ICGS::cdot1st(int i, int j) const
{
  double* ci = column[i];
  double* cj = column[j];
  double* ci_end = ci + M1;

  double s = 0;
  while (ci != ci_end) s += *ci++ * *cj++;
  return s;
}

double ICGS::rowdot(int i, int j) const
{
  double* ri = row[i];
  double* rj = row[j];

  double s = 0;
  for (int n=1; n<=N1; n++, ri+=M12, rj+=M12) {
      s += *ri * *rj;
    }
  return s;
}

double ICGS::norm1st(double *r) const     // column vector norm, 1st orthog.
{
  double* r_end = r + M1;

  double t {0}, s = 0;
  while (r != r_end)
    {
      t  = *r++;
      s += t*t;
    }
  s = std::sqrt(s);

  return s;
}

double ICGS::norm2nd(double *r) const     // column vector norm, 2nd orthog.
{
  r += M1;
  double t {0}, s {0};
  for (int i : minx)
    {
      t = r[i-1];
      s += t*t;
    }

  return std::sqrt(s);
}

void ICGS::blocks(int& m1, int& n1, int& m2, int& n2) const
{
  m1 = M1; n1 = N1; m2 = M2; n2 = N2;
}

void ICGS::cscale1st(double* column, double sc)
{
  double* cend = column + M12;
  while (column < cend) *column++ *= sc;
}

void ICGS::cscale2nd(double* column, double sc)
{
  column += M1;

  double* cend = column + M2;
  while (column < cend) *column++ *= sc;
}

void ICGS::ccopy1st (double* from, double* to)
{
  double* from_end = from + M12;
  while (from < from_end) *to++ = *from++;
}

void ICGS::ccopy2nd (double* from, double* to)
{
  from += M1;
  to   += M1;

  double* from_end = from + M2;
  while (from < from_end) *to++ = *from++;
}

void ICGS::icgs1()
{
  lindep.clear();
  icgs1_is_ready = false;
  error_icgs2_defect = 0;

  if (p_ak) delete[] p_ak;
  p_ak = new double[M12+1];
  double* const p_end_M1  = p_ak + M1;
  double* const p_end_M12 = p_ak + M12;

  double* rjk = new double[N12+1];

  double r11 = norm1st(column[1]);
  if (r11 > tolerance) cscale1st(column[1], 1/r11);
  else
    {
      lindep.insert(1);
    }

  for (int k=2; k<=N12; k++)
    {
      ccopy1st(column[k], p_ak);

      int jmax = k <= N1 ? k-1 : N1;
      for (int iter=1; iter<=2; iter++)
        {
          for (int j=1; j<=jmax; j++)
            {
              // r_jk := q^T_j p = <p,q_j>
              double* p = p_ak;
              double* q_j  = column[j];
              double  s = 0;
              while (p != p_end_M1) s += *p++ * *q_j++;
              rjk[j] = s;
            }
          for (int j=1; j<=jmax; j++)
            {
              // p := = p - q_j*r_jk
              double* p = p_ak;
              double* q = column[j];
              double  r_jk = rjk[j];
              while (p < p_end_M12) *p++ -= r_jk * *q++;
            }
        }

      if (k <= N1)
        {
          double rkk = norm1st(p_ak);
          if (rkk > tolerance) cscale1st(p_ak, 1/rkk);
          else
            {
              lindep.insert(k);
            }
        }

      ccopy1st(p_ak, column[k]);
    }

  delete[] rjk;
  icgs1_is_ready = true;
}

void ICGS::icgs2()
{
    {
      if (!icgs1_is_ready) icgs1();
      if (defect() == 0) return;

      // move linearly dependent column left
      int t = 1;
      for (int z : lindep) std::swap(column[t++],column[z]);

      //  indices for 2nd orthogonalization columns dot products
      if (min_x_use_all)
        {
          minx.clear();
          for (int i=1; i<=N1; i++) minx.insert(i);
        }
    }

  double* const p_end_M12 = p_ak + M12;
  double* rjk = new double[N12+1];

  double r11 = norm2nd(column[1]);

  if (r11 > tolerance) cscale2nd(column[1], 1/r11);
  else
    {
      error_icgs2_defect++;
    }

  for (int k=2; k<=N12; k++)
    {
      ccopy2nd(column[k], p_ak);

      int jmax = k <= defect() ? k-1 : defect();
      for (int iter=1; iter<=2; iter++)
        {
          for (int j=1; j<=jmax; j++)
            {
              // r_jk := q^T_j p = <p,q_j>
              double* p = p_ak + M1 - 1;        // -1: 1-based indexing minx
              double* q_j = column[j] + M1 - 1; // -1: 1-based undexing minx

              double  s = 0;
              // icgs1: while (p != p_end_M12) s += *p++ * *q_j++;
              for (int i : minx) s += p[i]*q_j[i];
              rjk[j] = s;
            }
          for (int j=1; j<=jmax; j++)
            {
              // p := = p - q_j*r_jk
              double* p = p_ak + M1;
              double* q = column[j] + M1;
              double  r_jk = rjk[j];
              while (p < p_end_M12) *p++ -= r_jk * *q++;
            }
        }

      if (k <= defect())
        {
          double rkk = norm2nd(p_ak);
          if (rkk > tolerance) cscale2nd(p_ak, 1/rkk);
          else
            {
              error_icgs2_defect++;
            }
        }
      ccopy2nd(p_ak, column[k]);
    }

  /* weight coefficients of adjusted unknowns are computed as dot products
   * of rows and elements of linearly dependent columns must be set to zero
   */
  for(int i=1; i<=defect(); i++)
    {
      cscale2nd(column[i], 0);
    }

  delete[] rjk;
}
