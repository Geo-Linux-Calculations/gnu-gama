/*
  GNU Gama -- Approximate coordinates by Polar Method
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

#ifndef GAMA_LOCAL_ACORDWEAKCHECKS_H
#define GAMA_LOCAL_ACORDWEAKCHECKS_H

#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama { namespace local {

    class AcordWeakChecks final : public AcordAlgorithm
    {
    public:
      AcordWeakChecks(Acord2* acord2);
      void execute();
      virtual void prepare() {} /* to be implemented later */

      std::pair<bool, PointID> check_traverse_endpoint(Acord2::Point pt);
      bool check_point(Acord2::Point pt);

    private:
      Acord2& AC;
      PointData & PD;
      ObservationData& OD;
    };

}} //namespace GNU_gama::local

#endif
