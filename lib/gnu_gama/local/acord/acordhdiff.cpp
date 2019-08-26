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

#include <gnu_gama/local/acord/acordhdiff.h>
#include <gnu_gama/local/observation.h>
#include <utility>
#include <unordered_set>
#include <algorithm>
#include <cmath>

using namespace GNU_gama::local;


AcordHdiff::AcordHdiff(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_)
{
}


void AcordHdiff::prepare()
{
  if (prepared_) return;

  // local working copy of all height differences
  std::unordered_set<PointID> hdiff_ids;
  for (auto hdc : AC.HDiffClusters_)
    for (auto hd : hdc->observation_list)
      {
        hdiff t;
        t.from = hd->from();
        t.to   = hd->to();
        t.hd   = hd->value();
        hdiffs_.push_back(t);

        // set of points' IDs from heigth difference observations
        hdiff_ids.insert(hd->from());
        hdiff_ids.insert(hd->to());
      }

  // local working copy of points with active coordinate z from hd observations
  for (PointID pid : hdiff_ids)
    {
      lpd_[pid] = AC.PD_[pid];
    }

  remove_hdiffs_between_known_heights();

  prepared_ = true;
}


void AcordHdiff::remove_hdiffs_between_known_heights()
{
  auto f = [&](hdiff h)
    {
      return AC.PD_[h.from].test_z() && AC.PD_[h.to].test_z();
    };

  using std::remove_if;
  hdiffs_.erase( remove_if(hdiffs_.begin(), hdiffs_.end(), f), hdiffs_.end() );
}


void AcordHdiff::execute()
{
  if (!prepared_) prepare();

  bool success;
  do
    {
      success = false;
      for (hdiff& hd : hdiffs_)
        {
          bool bf = lpd_[hd.from].test_z();
          bool bt = lpd_[hd.to  ].test_z();

          if (bf == bt) continue;   // both true or both false

          if (bf)
            {
              double z_from = lpd_[hd.from].z();
              double z_to   = z_from + hd.hd;
              lpd_[hd.to].set_z(z_to);
            }
          else
            {
              double z_to   = lpd_[hd.to].z();
              double z_from = z_to - hd.hd;
              lpd_[hd.from].set_z(z_from);
            }

          success = true;
        }

      remove_hdiffs_between_known_heights();
    }
  while (success && !hdiffs_.empty());

  // copy local heights to Acord2 point list
  for (auto lp : lpd_)
    {
      AC.PD_[lp.first].set_z(lp.second.z());
      AC.missing_z_.erase(lp.first);
    }

  if (hdiffs_.empty()) completed_ = true;
}
