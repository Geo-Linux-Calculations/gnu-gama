#include <iostream>
#include <limits>
#include <matvec/svd.h>
#include <matvec/matvec.h>
#include <matvec/sortvec.h>

using namespace std;
using namespace GNU_gama;

const long double dbl_epsilon = std::numeric_limits<double>::epsilon();

int result = 0;

template<class M, class N> void cmp(M a, N b)
{
  for (unsigned i=1; i<=a.rows(); i++)
    for (unsigned j=1; j<=a.cols(); j++)
      if (std::abs(a(i,j) - b(i,j)) > dbl_epsilon) result++;
}

int main()
{

  typedef long double T;

  const T T1 = 1.0;
  const T T2 = 2.0;
  const T T3 = 3.0;
  const T T4 = 4.0;
  const T T5 = 5.0;

  cout << "\n   GNU_gama   .........   demo_003  matvec "
       << GNU_gama::matvec_version() << "\n"
       << "------------------------------------------------------\n\n";
  Mat<T> A(2, 2);  A(1,1)=T1; A(1,2)=T2; A(2,1)=T3; A(2,2)=T4;

  Mat<T> X = A;
  Mat<T> Y = Mat<T>(trans(A));

  // ------------------------------------------------------------------------
  cout << "trans() ...\n";

  // cout <<       X  +       Y  <<       X  -       Y;
  // cout << trans(X) + trans(Y) << trans(X) - trans(Y);
  // cout << trans(X) +       Y  << trans(X) -       Y;
  // cout <<       X  + trans(Y) <<       X  - trans(Y);

  cmp(X+Y,Mat<T>({{2, 5},{5, 8}}));
  cmp(X-Y,Mat<T>({{0,-1},{1, 0}}));

  cmp(trans(X)+trans(Y),Mat<T>({{ 2, 5},{ 5, 8}}));
  cmp(trans(X)-trans(Y),Mat<T>({{ 0, 1},{-1, 0}}));

  cmp(trans(X)+Y,Mat<T>({{ 2, 6},{ 4, 8}}));
  cmp(trans(X)-Y,Mat<T>({{ 0, 0},{ 0, 0}}));

  cmp(X+trans(Y),Mat<T>({{ 2, 4},{ 6, 8}}));
  cmp(X-trans(Y),Mat<T>({{ 0, 0},{ 0, 0}}));

  // ------------------------------------------------------------------------
  cout << "inv(), trans() ... \n";

  // cout << inv(A) << inv(A)*A;
  // cout << trans(inv(A))*A;
  // cout << inv(A)*trans(A);
  // cout << trans(inv(A))*trans(A);
  // cout << trans(inv(X)) - inv(Mat<T>(trans(X)));

  cmp(inv(A)*A,                Mat<T>({{  1, 0}, {   0,  1 }}));
  cmp(trans(inv(A))*A,         Mat<T>({{2.5, 2}, {-0.5,  0 }}));
  cmp(inv(A)*trans(A),         Mat<T>({{  0,-2}, { 0.5, 2.5}}));
  cmp(trans(inv(A))*trans(A),  Mat<T>({{  1, 0}, {   0,  1 }}));
  cmp(trans(inv(A))-inv(Mat<T>(trans(A))), Mat<T>({{0,0},{0,0 }}));


  // ------------------------------------------------------------------------
  cout << "sort vec, SVD ...\n";
  {
    //Vec<T> W(3); W(1)=T3; W(2)=T5; W(3)=T1;
    Vec<T> W({T3, T5, T1});

    Vec<T> V1(W);
    Vec<T> V2(trans(W));
    sort(V1);
    //cout << trans(V1) << V2;
    if (V2(1) != 3 || V2(2) != 5 || V2(3) != 1) result++;
    if (V1(1) != 1 || V1(2) != 3 || V1(3) != 5) result++;
   }


   Vec<T> x, b(2); b(1)=T2; b(2)=T1;

   SVD<T> svd(A);
   svd.solve(b, x);

   if (std::abs(x(1)+3.0) > dbl_epsilon) return result++;
   if (std::abs(x(2)-2.5) > dbl_epsilon) return result++;

   return result;
}
