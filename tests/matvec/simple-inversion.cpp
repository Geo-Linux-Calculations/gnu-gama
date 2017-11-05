#include <matvec/matvec.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cfloat>

using namespace GNU_gama;

int main()
{
  std::vector<Mat<>> matrix,  inverse;

  matrix.push_back ({{1,  2, 0},
                     {0,  4, 0},
                     {7, -3, 9}});
  inverse.push_back({{  1,    -0.5,    0},
                     {  0,    0.25,    0},
                     {-7.0/9, 17.0/36, 1.0/9}});

  matrix.push_back ({{1, 2, 3, 4},
                     {0, 1, 0, 1},
                     {0, 2, 2, 2},
                     {3, 0, 0, 3}});
  inverse.push_back({{-1, -1,  3.0/2,  2.0/3},
                     {-1,  0,  3.0/2,  1.0/3},
                     { 0, -1,  1.0/2,  0    },
                     { 1,  1, -3.0/2, -1.0/3}});

  matrix.push_back ({{1, 2, 3, 4, 5},
                     {0, 1, 0, 1, 4},
                     {0, 2, 2, 2, 3},
                     {3, 0, 0, 3, 2},
                     {2,-1, 0,-6, 3}});
  inverse.push_back({{ -62.0/195, -43.0/195,  31.0/65, 73.0/195,  19.0/195},
                     {-202.0/195,  -8.0/195, 101.0/65, 68.0/195,  -1.0/195},
                     {   7.0/13,   -5.0/13,   -4.0/13, -3.0/13,    1.0/13 },
                     {  34.0/195,  11.0/195, -17.0/65,  4.0/195, -23.0/195},
                     {  14.0/65,   16.0/65,  -21.0/65, -6.0/65,    2.0/65 }});

  double maxdif = 0;
  for (unsigned i=0; i<matrix.size(); i++) {
    Mat<> e = inverse[i] - inv(matrix[i]);
    std::cout << e;
    for (auto i=e.begin(); i!=e.end(); i++) {
      if (std::abs(*i) > std::abs(maxdif)) maxdif = *i;
    }
    std::cout << "matvec - simple inversion maxdif = " << maxdif << "\n";
  }

  return std::abs(maxdif) < 10*DBL_EPSILON ? 0 : 1;
}
