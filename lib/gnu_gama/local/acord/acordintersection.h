/*
  GNU Gama -- Heights derived from zenith angles and distances
  Copyright (C) 2019  Ales Cepek <cepek@gnu.org>

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

#ifndef GAMA_LOCAL_ACORD2_INTERSECTION_H_
#define GAMA_LOCAL_ACORD2_INTERSECTION_H_

#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama {  namespace local
  {

    class AcordIntersection final : public AcordAlgorithm
    {
    public:

      AcordIntersection(Acord2* acord2);

      virtual void prepare();
      virtual void execute();
      virtual const char* className() const { return "AcordIntersection"; }

      inline bool completed() const
      {
        return completed_;
      }


    private:
      Acord2& AC;
      PointData & PD;
      ObservationData& OD;
    };

  }} //namespace GNU_gama::local

#endif
