/* matvec_demo_005.cpp
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

#include <matvec/matvec.h>

#include <iostream>
#include <sstream>

int main()
{
  using namespace GNU_gama;
  using namespace std;

  cout << "\n   Array   .........   demo_005  matvec "
       << GNU_gama::matvec_version() << "\n"
    "------------------------------------------------------\n\n";

  Array<int> A(8);
  for (Array<int>::iterator i=A.begin(); i!=A.end(); i++) *i = 1;
  for (int i=0; i<8; i++) A.entry(i) = A[i] + i;
  const Array<int>& CA = A;
  cout << "Array<int> A(8) : ";
  for (int i=0, j=7; i<8; i++, j--)
    {
      A.swap(i,j);
      if (i == 7)
	{
	  for (Array<int>::const_iterator i=CA.begin(); i!=CA.end(); i++)
	    {  
	      cout << *i << " ";
	    }
	  cout << "\n";
	}
    }

  {
    TransVec<> t0;
    TransVec<> t1(3);     t1(1) = 1;   t1(2) = 2;   t1(3) = 3;   
    t0.reset(t1.dim());
    TransVec<> t2(t1);
    t1 + t2;   t1 - t2;   t1*12.4;   12.4*t2;
    Vec<> v1(t1);
    Vec<> v2(v1.dim()); v2(1)=10.0; v2(2)=20.0; v2(3)=30.0;
    t1*v1;     // dot product
    // v1*t1;  not defined
    // t1*t1;
    // v1*v1;
    {
      std::ostringstream cout;
      cout << v2;
      cout << t1 << v1 << trans(t1) << trans(v1);
    }
  }
}
