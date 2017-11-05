/* matvec_demo_006.cpp
   Copyright (C) 2012  Ales Cepek <cepek@gnu.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library (see COPYING.LIB); if not, write to the
   Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <fstream>
#include <sstream>
#include <matvec/bandmat.h>
#include <matvec/matvec.h>
#include <matvec/covmat.h>
#include <matvec/symmat.h>

int main()
{
  using namespace GNU_gama;
  using namespace std;

  cout << "\n   BandMat   .........   demo_006  matvec "
       << GNU_gama::matvec_version() << "\n"
    "------------------------------------------------------\n\n";

  {
    char bbb[] =
      "12 4    9     9    18    27    36"
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
      "    158.5                        ";
    ofstream o("matvec_.tmp");
    o << bbb;
  }

  BandMat<> B;
  {
    ifstream i("matvec_.tmp");
    i >> B;
  }

  CovMat<> C;
  {
    ifstream i("matvec_.tmp");
    i >> C;    
  }

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
  Vec<> b = b12;
  Vec<> c = b12;
  Vec<> s = b12;



  SymMat<> S(Mf12.rows(), Mf12.cols());
  for (unsigned int r=1; r<=S.rows(); r++)
    for (unsigned int s=r; s<=S.cols(); s++)
      {
	S(r,s) = Mf12(r,s);
      }

  S.cholDec();
  S.solve(s);

  // B.cholDec();
  // B.solve(b);

  C.cholDec();
  C.solve(c);

  // cout << Mf12;
  Vec<> eig;
  B.eigenVal(eig);
  cout << "eigenvalues ~ " << trans(eig);
  cout << "cond.number ~ " << eig(eig.dim())/eig(1) << "\n";

  Vec<> d = s - c;

  cout << "differences between cholesky "
       << "decomposition solution SymMat - CovMat\n";
  cout << "norm L1: "     << d.norm_L1()   << "   "
       << "norm_L2: "     << d.norm_L2()   << "   "
       << "norm_Linf(): " << d.norm_Linf() << "\n\n";
}
