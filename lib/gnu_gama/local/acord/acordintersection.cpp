/*
  GNU Gama -- Abstract base class of Acord2 algorithms family
  Copyright (C) 2019  Petra Millarova <petramillarova@gmail.com>

  This file is part of the GNU Gama C++ library.

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

#include <gnu_gama/local/acord/acordintersection.h>
#include <cmath>
#include <algorithm>

using namespace GNU_gama::local;
using std::sin;
using std::tan;
using std::sort;

AcordIntersection::AcordIntersection(Acord2* acord2)
  : AC(*acord2), PD(acord2->PD_), OD(acord2->OD_),
    approxy_(PD, OD)
{
  try
    {
    }
  catch (...)
    {
      throw;
    }
}


void AcordIntersection::prepare()
{
  if (prepared_) return;

  completed_ =  AC.missing_xy_.empty();
  prepared_  = !AC.missing_xy_.empty() && AC.new_points_xy_ == 0;
}


void AcordIntersection::execute()
{
  if (completed_) return;

  if (!prepared_)
    {
      /* AcordIntersection is prepared if some xy coordinates are still missing
       * and all previous algorithms failed do solve at least one xy point */
      prepare();
      if (!prepared_) return;
    }

  approxy_.calculation();

  auto tmp = AC.missing_xy_;
  AC.missing_xy_.clear();
  for (auto pid : tmp)
    {
      if (!AC.PD_[pid].test_xy()) AC.missing_xy_.insert(pid);
    }

  completed_ = AC.missing_xy_.empty();
}
