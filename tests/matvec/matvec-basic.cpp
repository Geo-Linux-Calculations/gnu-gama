#include <matvec/matvec.h>
#include <matvec/svd.h>
#include <iostream>
#include <string>

using namespace GNU_gama;

int b01()
{
  double ver = std::stod(std::string(matvec_version()));

  Vec<> a, b{ {1,2,3,4,5,6,7,8,9,10} };
  unsigned bd = b.dim();

  a = std::move(b);
  std::cout << "01# matvec_version() " << matvec_version() << " --> "
            << " a.dim() b.dim() : " << a.dim() << " " << b.dim() << "\n";

  if (ver >= 2.0)
    {
      // move constructor
      if (a.dim() != bd || b.dim() != 0 ) return 1;
      for (unsigned i=1; i<=bd; i++)
        if (a(i) != i) return 1;
    }
  else
    {
      // fall back to copy constructor
      if (a.dim() != bd || b.dim() != bd) return 1;
      for (unsigned i=1; i<=bd; i++)
        if (a(i) != i || a(i) != b(i)) return 1;
    }

  return 0;
}


int b02()
{
  int result = 0;

  using std::cout;
  using std::endl;

  // code fragment from matvec_demo_001

  Mat<> A(5, 3);
  Vec<> b(5);

  try    // List initialisation for Mat<> and Vec<> --> move assignment
    {
       A = {{1.001, 0.006, 0.012},
            {0.002, 1.007, 0.013},
            {0.003, 0.008, 1.014},
            {1.004, 0.009, 1.015},
            {0.005, 1.011, 1.016}};

        b = {1.1, 1.9, 3.1, 4, 5.1};
    }
  catch (const Exception::matvec& e)
    {
      cout << e.description << endl;
      return 1;
    }

  return result;
}


int main()
{
  int error = 0;

  error += b01();
  error += b02();

  return error;
}
