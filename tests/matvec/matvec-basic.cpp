#include <matvec/matvec.h>
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

}


int main()
{
  int error = 0;

  error += b01();

  return error;
}
