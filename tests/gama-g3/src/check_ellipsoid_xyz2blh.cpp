#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <gnu_gama/ellipsoid.h>

using namespace std;
using namespace GNU_gama;

double PI = 3.14159265358979323846; // pi_v in <numbers> cince c++20
inline double int2rad(int a) { return double(a)/180.0*PI; }

int main()
{
  cout << "XYZ -> BLH -> XYZ\n" << endl
       << " B   L    H   |dx| |dy| |dz|" << endl;

  double maxdx = 0, maxdy = 0, maxdz = 0;

  Ellipsoid ellipsoid;
  // ellipsoid.set_af1(6378137, 298.257223563); // WGS 84

  for (int b=0; b<=90; b++)
    for (int l=0; l<=180; l += 10)
    for (int h=0; h<=7000; h += 10)
      {
        double X, Y, Z;
        ellipsoid.blh2xyz(int2rad(b),int2rad(l),double(h), X,Y,Z);
        // numerical noise in fractional part
        // X += double(5)/7;  Y += double(11)/13; Z += double(17)/19;

        double B,L,H;
        ellipsoid.xyz2blh(X,Y,Z,B,L,H);
        double x, y, z;
        ellipsoid.blh2xyz(B,L,H,x,y,z);

        if (0) cout << "... "
                    << X << " " << Y << " " << Z << " | "
                    << B << " " << L << " " << H << " | "
                    << x << " " << y << " " << z << endl;

        double dx = abs(X-x), dy = abs(Y-y),  dz = abs(Z-z);
        if (  dx > maxdx || dy > maxdy || dz > maxdz
              // || (b == 0 && l == 0 && h == 0)
           )
          {
            maxdx = max(maxdx,dx);
            maxdy = max(maxdy,dy);
            maxdz = max(maxdz,dz);
            cout << setw(2) << b << " " << setw(3) << l << " " << setw(4) << h << "   "
                 << dx << " " << dy << " " << dz << endl;
          }
        if (maxdx + maxdz > 0.1)
          {
            cout << "--- XYZ BLH xyz "
                 << X << " " << Y << " " << Z << " | "
                 << B << " " << L << " " << H << " | "
                 << x << " " << y << " " << z << endl;
            return 1;
          }
      }

  cout << endl << "max |dx| |dy| |dz|  "
       << maxdx << "   " << maxdy << "  " << maxdz << endl;

  return 0;
}
