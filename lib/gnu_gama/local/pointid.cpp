/*  
    Geodesy and Mapping C++ Library (GNU GaMa / GaMaLib)
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>,
                  2001  Ales Cepek <cepek@fsv.cvut.cz>,
                        Jan Pytel  <pytel@gama.fsv.cvut.cz>

    This file is part of the GNU GaMa / GaMaLib C++ Library.
    
    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <cstdlib>
#include <gnu_gama/local/pointid.h>

// typedef std::string PointID;  


using namespace GaMaLib;

PointID::PointID(const std::string& s)
{
  using namespace std;
  
  string::const_iterator b=s.begin(), e=s.end();
  GNU_gama::TrimWhiteSpaces(b, e);
  sid = std::string(b,e);
  iid = std::atoi(sid.c_str());
  
  if (iid < 0) iid = 0;
  
  int m10, tmp=iid;
  const std::string& cid = sid;
  const char ctab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  for (string::const_reverse_iterator i=cid.rbegin(); i!=cid.rend(); ++i)
    {
      m10  = tmp % 10;
      if (tmp == 0 || ctab[m10] != *i)   // check for long int overlow
        {
          iid = 0;
          return;
        }
      tmp /= 10;
    }
}

