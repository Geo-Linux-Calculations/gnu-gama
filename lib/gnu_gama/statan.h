/* GNU Gama -- adjustment of geodetic networks
   Copyright (C) 1999, 2012  Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef GNU_gama_gnu_gama_StatAn_h
#define GNU_gama_gnu_gama_StatAn_h

#include <cmath>
#include <algorithm>

/** \file statan.h
 * \brief Basic statistic functions
 *
 * \author Ales Cepek
 */

namespace GNU_gama {

/** \brief For the given probability and number of degrees of fredom
 * computes critical value of Student's distribution
 *
 * \param alfa probability for which critical values of Student's
 *       distribution is computed; value of parameter alfa must be
 *       in interval (0, 1); function doesn;t check the value of the
 *       parameter
 *
 * \param N degrees of freedom
 */
double Student(double alfa, int N);

/** \brief For given probability computes critical value of normalized
 *  normal distribution N(0,1).
 *
 * \param alfa probability for which critical values of Normal
 *        distribution is computed; value of parameter alfa must be in
 *        interval (0, 1); function doesn;t check the value of the
 *        parameter
 */
double Normal(double alfa);

/** \brief Function computes in the given point x value D(x) of
 * distributive normalized normal distribution and the value of its
 * density function.
 *
 * \param x  argument
 * \param D  cumulative distribution
 * \param f  probability density function
 */
void NormalDistribution(double x, double& D, double& f);

/** \brief Kolmogorov-Smirnov probability function
 */
double KSprob(double);

/** \brief Kolmogorov-Smirnov test.
 *
 * \param Func   user-supplied cumulative probability distribution function
 * \param ks     K-S statistic
 * \param prob   significance level
*/

void KStest(double Data[], int n, double (*Func)(double),
            double& ks, double& prob);

/** \brief For the given probability and degrees of freedom computes
 * critical value of Chi-square distribution.  */
double Chi_square(double probability, int degrees_of_freedom);

}      /* namespace GNU_gama::local */

//---------------------------------------------------------------------------
#endif
