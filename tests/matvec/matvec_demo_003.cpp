#include <iostream>
#include <matvec/svd.h>
#include <matvec/matvec.h>
#include <matvec/sortvec.h>

using namespace std;
using namespace GNU_gama;


int f(ostream& cout)
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

  cout << "trans() ...\n\n";

  cout <<       X  +       Y  <<       X  -       Y;
  cout << trans(X) + trans(Y) << trans(X) - trans(Y);
  cout << trans(X) +       Y  << trans(X) -       Y;
  cout <<       X  + trans(Y) <<       X  - trans(Y);

  cout << "\n*******************************************************\n\n";

  cout << "inv(), trans() ... \n\n";

  cout << inv(A) << inv(A)*A;
  cout << trans(inv(A))*A;
  cout << inv(A)*trans(A);
  cout << trans(inv(A))*trans(A);
  cout << trans(inv(X)) - inv(Mat<T>(trans(X)));

  cout << "\n*******************************************************\n\n";
  {
    Vec<T> W(3); W(1)=T3; W(2)=T5; W(3)=T1;

    Vec<T> V1(W);
    Vec<T> V2(trans(W));
    sort(V1);
    cout << trans(V1) << V2;
   }


   Vec<T> x, b(2); b(1)=T2; b(2)=T1;

   SVD<T> svd(A);
   svd.solve(b, x);

   cout << trans(x) << trans(inv(A)*b);

   return 0;
}

int main() { f(cout); }
