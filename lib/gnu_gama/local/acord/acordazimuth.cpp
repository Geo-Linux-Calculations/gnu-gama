/*
  GNU Gama C++ library
  Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library

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

#include <gnu_gama/local/acord/acordazimuth.h>
#include <utility>
#include <algorithm>
#include <cmath>

using namespace GNU_gama::local;
using pair_ = std::pair<PointID,PointID>;
using std::sin;
using std::cos;

AcordAzimuth::AcordAzimuth(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
}


void AcordAzimuth::prepare()
{
  if (prepared_) return;

  // fetch all azimuths from the input data with possibly multiple values
  for (auto i=OD.begin(); i!=OD.end(); ++i)
    {
      if (Azimuth* a = dynamic_cast<Azimuth*>(*i))
        {
          PointID from = a->from();
          PointID to   = a->to();
          double  val  = a->value();

          if (to < from)
            {
              std::swap(from, to);
              val += M_PI;
              if (val > 2*M_PI) val -= 2*M_PI;
            }

          azimuths_[pair_(from, to)].values.push_back(val);
        }
    }

  remove_azimuths_between_known_xy();

  // get medians (attribute value)
  for (auto& a : azimuths_)
    {
      auto& v = a.second.values;
      std::sort(v.begin(), v.end());
      a.second.value = ( v[(v.size()-1)/2] + v[v.size()/2] )/2;

      v.clear(); // next we shall use the vector for storing distances
    }      

  // fetch all available horizontal distances (shall we also accept slope?)
  for (auto i=OD.begin(); i!=OD.end(); ++i)
    {
      if (Distance* a = dynamic_cast<Distance*>(*i))
        {
          PointID from = a->from();
          PointID to   = a->to();
          double  val  = a->value();
          if (to < from)
            {
              std::swap(from, to);
            }

          if (azimuths_.find(pair_(from, to)) == azimuths_.end()) continue;

          azimuths_[pair_(from, to)].values.push_back(val);
        }
    }

  // get medians (attribute distance)
  for (auto& a : azimuths_)
    {
      if (a.second.values.size() == 0) continue;
      
      auto& v = a.second.values;
      std::sort(v.begin(), v.end());
      a.second.distance = ( v[(v.size()-1)/2] + v[v.size()/2] )/2;
      v.clear();
    }

  prepared_ = true;
}


void AcordAzimuth::remove_azimuths_between_known_xy()
{
  // we cannot use std::remove_if for std::map container
  // https://en.cppreference.com/w/cpp/algorithm/remove

  std::vector<pair_> ids;
  for (auto iter : azimuths_)
    {
      if (PD[iter.first.first].test_xy() && PD[iter.first.second].test_xy())
        {
          ids.push_back(iter.first);
        }
    }

  for (auto p : ids) azimuths_.erase(p);
}


void AcordAzimuth::execute()
{
  if (!prepared_) prepare();

  for (auto iter : azimuths_)
    {
      const PointID& a = iter.first.first;
      const PointID& b = iter.first.second;

      bool axy = PD[a].test_xy();
      bool bxy = PD[b].test_xy();

      if ( axy &&  bxy) continue;    // nothing to do 


      if (axy && !bxy && iter.second.distance != 0)
        {
          double x0 =  PD[a].x();
          double y0 =  PD[a].y();
          double bb  = iter.second.value + PD.xNorthAngle();
          double x  = x0 + iter.second.distance*cos(bb);
          double y  = y0 + iter.second.distance*sin(bb);
          PD[b].set_xy(x, y);
          AC.missing_xy_.erase(b);
        }
      else if (!axy && bxy && iter.second.distance != 0)
        {
          double x0 =  PD[b].x();
          double y0 =  PD[b].y();
          double bb  = iter.second.value + M_PI + PD.xNorthAngle();
          double x  = x0 + iter.second.distance*cos(bb);
          double y  = y0 + iter.second.distance*sin(bb);
          PD[a].set_xy(x, y);
          AC.missing_xy_.erase(a);
        }
      else if (!axy && !bxy)
        {
          // should we try to set some orientation shifts?
        }
    }
  
  remove_azimuths_between_known_xy();

  if (azimuths_.empty()) completed_ = true;
}

/*
#include <iostream>
#include <iomanip>
using std::cerr;
using std::endl;
using std::setw;
using std::string;

void AcordAzimuth::dbg(string s)
{
  for (auto a : azimuths_)
    {
      cerr << setw(10) << s
           << " " << setw(3) << a.first.first  << "->"
           << setw(3) << a.first.second << " : ";
      cerr << "dist " << a.second.distance
           << " value " << a.second.value << " ~ ";
      for (auto v : a.second.values) cerr << v << " ";
      cerr << endl;
        
    }
}
*/
