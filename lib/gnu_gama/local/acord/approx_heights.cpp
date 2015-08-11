/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>,
                        Jan Pytel  <pytel@gama.fsv.cvut.cz>

    This file is part of the GNU Gama C++ library.

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

#include <gnu_gama/local/acord/approx_heights.h>
#include <iostream>

using namespace std;
using namespace GNU_gama::local;

ApproximateHeights::ApproximateHeights(PointData& b, ObservationData& m)
  : PD(b), OD(m)
{
  missing_heights = 0;

  for (PointData::const_iterator i=PD.begin(); i!=PD.end(); ++i)
    {
      const LocalPoint& p = (*i).second;
      if (p.active_z() && !p.test_z()) missing_heights++;
    }

  if (missing_heights)
    for (ObservationData::iterator i=OD.begin(), e=OD.end(); i!=e; ++i)
    {
      Observation* obs = *i;

      if (H_Diff* h = dynamic_cast<H_Diff*>(obs))
        OHD.HD.push_back(h);
      if (Distance* d = dynamic_cast<Distance*>(obs))
        OHD.DI.push_back(d);
      if (S_Distance* s = dynamic_cast<S_Distance*>(obs))
        OHD.SD.push_back(s);
      if (Z_Angle* z = dynamic_cast<Z_Angle*>(obs))
        OHD.ZA.push_back(z);
    }
}

void ApproximateHeights::make_heights()
{

  if (!missing_heights) return;

  for (std::list<Z_Angle*>::const_iterator iza = OHD.ZA.begin();
       iza != OHD.ZA.end(); ++iza)
    {

      const Double too_small_zenith_angle = 1 * G2R;

      bool is_heights = false;

      Z_Angle* zangle = *iza;

      const PointID& From   = zangle->from();
      const PointID& To     = zangle->to();
      Double  Vzenit  = (*iza)->value();

      for (std::list<Distance*>::const_iterator idi = OHD.DI.begin();
           idi != OHD.DI.end() && !is_heights; ++idi)
        {

          Distance* distance = *idi;
          const PointID& From2   = distance->from();
          const PointID& To2     = distance->to();
          const Double& Vdist  = distance->value();

          // I transfer value of Vzenit into interval <0,M_PI>
          while (Vzenit < 0)   Vzenit+=2*M_PI;
          while (Vzenit > 2*M_PI) Vzenit-=2*M_PI;
          if (Vzenit > M_PI)   Vzenit = 2*M_PI - Vzenit;

          if ( ( ( (From == From2) &&  (To == To2  ) ) ||
                 ( (From == To2)   &&  (To == From2) ) ) &&
               (fabs(Vzenit) > too_small_zenith_angle) &&
               (fabs(Vzenit) < M_PI-too_small_zenith_angle) )
            {
              OHD.tmpHD.push_back(H_Diff(From,To,Vdist / tan(Vzenit)));
              is_heights = true;
            }
        }

      for (std::list<S_Distance*>::const_iterator isd = OHD.SD.begin();
           isd != OHD.SD.end() && !is_heights; ++isd)
        {

          S_Distance* sdistance = *isd;

          const PointID& From2   = sdistance->from();
          const PointID& To2     = sdistance->to();
          const Double& Vdist  = sdistance->value();

          if ( ( (From == From2) &&  (To == To2  ) ) ||
               ( (From == To2)   &&  (To == From2) ) )
            {
              OHD.tmpHD.push_back(H_Diff(From,To,Vdist * cos(Vzenit)));
              is_heights = true;
            }
        }

      // I can't compute H_Diff from distances, now I try to compute
      // H_Diff from coordinates;
      if (!is_heights)
        {
          const LocalPoint& from = PD[From];
          const LocalPoint& to   = PD[To];
          if (from.test_xy() && to.test_xy())
            {
              Double dist = sqrt( (from.x() - to.x())*(from.x() - to.x()) +
                                  (from.y() - to.y())*(from.y() - to.y()));
              OHD.tmpHD.push_back(H_Diff(From,To, dist / tan(Vzenit)));
              is_heights = true;
            }
        }

      if (is_heights)
        {
          OHD.HD.push_back( &(*OHD.tmpHD.rbegin()) );
        }
    }
}

void ApproximateHeights::execute()
{
  if (!missing_heights) return;

  make_heights();

  typedef std::list<H_Diff*> LHD;
  LHD  tmp;
  LHD* A = &tmp;     // input  list of elevation differences
  LHD* B = &OHD.HD;  // output list of observations with both heights unknown

  bool updated;

  do {

    swap(A, B);      // swap pointers
    B->clear();
    updated = false;

    for (LHD::iterator i=A->begin(); i!=A->end(); ++i)
      {
        H_Diff* h = *i;
        LocalPoint& f = PD[h->from()];
        LocalPoint& t = PD[h->to()];

        if (!f.test_z() && !t.test_z())
          {
            B->push_back(h);
          }
        else if ( f.test_z() && !t.test_z())
          {
            t.set_z(f.z() + h->value());
            missing_heights--;
            updated = true;
            if (!missing_heights) return;
          }
        else if (!f.test_z() &&  t.test_z())
          {
            f.set_z(t.z() - h->value());
            missing_heights--;
            updated = true;
            if (!missing_heights) return;
          }
        else    // both heights, nothing to do
          {
          }
      }

  } while (missing_heights && updated);

}

void ApproximateHeights::print(ostream&)
{
}




