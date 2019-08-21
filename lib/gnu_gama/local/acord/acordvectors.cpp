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

#include <gnu_gama/local/acord/acordvectors.h>
#include <gnu_gama/local/observation.h>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <cmath>

using namespace GNU_gama::local;


AcordVectors::AcordVectors(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
}


void AcordVectors::prepare()
{
  if (prepared_) return;

  double xyz_buffer[3];      // the order of vector's elements is not guaranteed
  int    xyz_state {0};      // initial state is empty

  std::unordered_set<PointID> point_ids;

  for (Vectors* vecc : AC.VectorsClusters_)
    {
      for (Observation* obs : vecc->observation_list)
        {
          if (Xdiff* xdiff = dynamic_cast<Xdiff*>(obs))
            {
              xyz_buffer[0] = xdiff->value();
	      xyz_state += 1;
            }
          else if (Ydiff* ydiff = dynamic_cast<Ydiff*>(obs))
            {
              xyz_buffer[1] = ydiff->value();
	      xyz_state += 2;
            }
          else if (Zdiff* zdiff = dynamic_cast<Zdiff*>(obs))
            {
	      xyz_buffer[2] = zdiff->value();
	      xyz_state += 4;
	    }

	  if (xyz_state == 7)
	    {
              pvector t;
              t.from = obs->from();
              t.to   = obs->to();
              t.dx = xyz_buffer[0];
              t.dy = xyz_buffer[1];
              t.dz = xyz_buffer[2];
              vectors_.push_back(t);

	      xyz_state = 0;

	      point_ids.insert(t.from);
	      point_ids.insert(t.to);
            }
        }
    }

  for (PointID pid : point_ids)
    {
      lpd_[pid] = AC.PD_[pid];
    }

  remove_vectors_between_known_xyz();

  prepared_ = true;
}


void AcordVectors::remove_vectors_between_known_xyz()
{
  auto f = [&](pvector h)
    {
      const LocalPoint& from = AC.PD_[h.from];
      const LocalPoint& to   = AC.PD_[h.to];

      return from.test_xy() && from.test_z()  &&  to.test_xy() && to.test_z();
    };

  using std::remove_if;
  vectors_.erase( remove_if(vectors_.begin(), vectors_.end(), f), vectors_.end() );
}


void AcordVectors::execute()
{
  if (!prepared_) prepare();

  bool success {};
  do
    {
      success = false;
      for (pvector& hd : vectors_)
        {
          bool bf = lpd_[hd.from].test_xy() && lpd_[hd.from].test_z();
          bool bt = lpd_[hd.to  ].test_xy() && lpd_[hd.to  ].test_z();

          if (bf == bt) continue;   // both true or both false

          if (bf)
            {
              double x_from = lpd_[hd.from].x();
              double x_to   = x_from + hd.dx;
              double y_from = lpd_[hd.from].y();
              double y_to   = y_from + hd.dy;
              lpd_[hd.to].set_xy(x_to, y_to);

              double z_from = lpd_[hd.from].z();
              double z_to   = z_from + hd.dz;
              lpd_[hd.to].set_z(z_to);
            }
          else
            {
              double x_to    = lpd_[hd.to].x();
              double x_from  = x_to - hd.dx;
              double y_to    = lpd_[hd.to].y();
              double y_from  = y_to - hd.dy;
              lpd_[hd.from].set_xy(x_from, y_from);

              double z_to   = lpd_[hd.to].z();
              double z_from = z_to - hd.dz;
              lpd_[hd.from].set_z(z_from);
            }

          success = true;
        }

      remove_vectors_between_known_xyz();
    }
  while (success && !vectors_.empty());

  // copy local heights to the Acord2 point list
  for (auto lp : lpd_)
    {
      AC.PD_[lp.first].set_xy(lp.second.x(),lp.second.y());
      AC.PD_[lp.first].set_z (lp.second.z());
    }

  if (vectors_.empty()) completed_ = true;
}
