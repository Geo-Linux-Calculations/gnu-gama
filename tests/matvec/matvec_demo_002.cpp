/*
  GNU Gama C++ library tests/matvec
  Copyright (C) 2017  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library tests/matvec
  
  GNU Gama is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  GNU Gama is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <sstream>
#include <limits>
#include <matvec/bandmat.h>
#include <matvec/matvec.h>
#include <matvec/svd.h>

int main()
{
  using namespace GNU_gama;
  using namespace std;

  const double dbl_epsilon = std::numeric_limits<double>::epsilon();

  int result = 0;

  cout << "\n   BandMat   .........   demo_002  matvec "
       << GNU_gama::matvec_version() << "\n"
    "------------------------------------------------------\n\n";


  BandMat<> A(4, 2);
  {
    istringstream input
      (" 4 3   2  4   6  10 "    // upper part of a band matrix
       "         13  32  50 "
       "            101 171 "
       "                381 ");
    input >> A;
  }
  cout.width(3);
  cout << A;
  BandMat<> B = A;
  BandMat<> C;
  C = B;
  C.cholDec();    // cholesky decomposition trans(U) * diagonal * U
  // cout << "\n" << C << "\n";

  Vec<> z;
  Mat<> U;
  Mat<> F;
  Mat<> W;
  Mat<> Q;
  {
    std::istringstream inp
      ("4      1.0  2.0  3.0  4.0 "    // right hand side
       " "
       "4  4   1.0  2.0  3.0  5.0 "    // upper part of Cholesky decomposition
       "       0.0  1.0  4.0  6.0 "
       "       0.0  0.0  1.0  7.0 "
       "       0.0  0.0  0.0  1.0 "
       " "
       "4  4   1.0  0.0  0.0  0.0 "    // lower part of Cholesky decomposition
       "       2.0  1.0  0.0  0.0 "
       "       3.0  4.0  1.0  0.0 "
       "       5.0  6.0  7.0  1.0 "
       " "
       "4  4   2.0  0.0  0.0  0.0 "    // diagonal of Cholesky decomposition
       "       0.0  5.0  0.0  0.0 "
       "       0.0  0.0  3.0  0.0 "
       "       0.0  0.0  0.0  4.0 "
       " "
       "4  4   2.0    4.0    6.0   10.0 "   // full matrix
       "       4.0   13.0   32.0   50.0 "   // cond. number = 1.6197e5
       "       6.0   32.0  101.0  171.0 "
       "      10.0   50.0  171.0  381.0 ");
    inp >> z >> U >> F >> W >> Q;
  }

  Vec<> x = inv(Q)*z;
  cout << trans(x);

  C.solve(z);
  cout << trans(z);

  cout << "diff = " << trans(x-z)  << "\n";

  if ((x-z).norm_Linf() > 100*1.6197e5*dbl_epsilon) result++;

  // --------------------------------------------------------------------------

  BandMat<> M;
  {
    istringstream inp
      ("12 4    9     9    18    27    36"
       "     17.5  26.5    44  61.5    34"
       "     52.5    79 113.5    58    32"
       "    130.5 182.5   107  54.5    30"
       "      267   172 100.5    51    28"
       "    251.5 161.5    94  47.5    26"
       "      236   151  87.5    44    24"
       "    220.5 140.5    81  40.5    22"
       "      205   130  74.5    37      "
       "    189.5 119.5    68            "
       "      174   109                  "
       "    158.5                        ");
    inp >> M;
  }
  M.cholDec();
  cout.width(3);
  cout << "M = " << M;

  Mat<> Mf12;
  Vec<> b12;
  {
    std::istringstream inp
      ("  12   9.0 8.5 8.0 7.5 7.0 6.5 6.0 5.5 5.0 4.5 4.0 3.5 "
       "  12  12 "
       "  9.0  9.0  18.0  27.0  36.0   0.0   0.0   0.0   0.0   0.0   0.0   0.0"
       "  9.0 17.5  26.5  44.0  61.5  34.0   0.0   0.0   0.0   0.0   0.0   0.0"
       " 18.0 26.5  52.5  79.0 113.5  58.0  32.0   0.0   0.0   0.0   0.0   0.0"
       " 27.0 44.0  79.0 130.5 182.5 107.0  54.5  30.0   0.0   0.0   0.0   0.0"
       " 36.0 61.5 113.5 182.5 267.0 172.0 100.5  51.0  28.0   0.0   0.0   0.0"
       "  0.0 34.0  58.0 107.0 172.0 251.5 161.5  94.0  47.5  26.0   0.0   0.0"
       "  0.0  0.0  32.0  54.5 100.5 161.5 236.0 151.0  87.5  44.0  24.0   0.0"
       "  0.0  0.0   0.0  30.0  51.0  94.0 151.0 220.5 140.5  81.0  40.5  22.0"
       "  0.0  0.0   0.0   0.0  28.0  47.5  87.5 140.5 205.0 130.0  74.5  37.0"
       "  0.0  0.0   0.0   0.0   0.0  26.0  44.0  81.0 130.0 189.5 119.5  68.0"
       "  0.0  0.0   0.0   0.0   0.0   0.0  24.0  40.5  74.5 119.5 174.0 109.0"
       "  0.0  0.0   0.0   0.0   0.0   0.0   0.0  22.0  37.0  68.0 109.0 158.5"
       );
    inp >> b12 >> Mf12;
  }

  Vec<> t = inv(Mf12)*b12;
  cout << endl << trans(t);

  M.solve(b12);
  cout << trans(b12 - t);

  SVD<> svd(Mf12);
  double cond = svd.SVD_W()(1)/svd.SVD_W()(svd.SVD_W().dim());
  cout <<"\ncond = "<< cond <<'\n';

  if ((b12 - t).norm_Linf() > 100*cond*dbl_epsilon) result++;

  return result;
}
