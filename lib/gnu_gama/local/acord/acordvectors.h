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

#ifndef GNU_GAMA_LOCAL_ACORVECTORS_H_
#define GNU_GAMA_LOCAL_ACORVECTORS_H_

#include <map>
#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama {
  namespace local {

    class AcordVectors final : public AcordAlgorithm
    {
    public:
      AcordVectors(Acord2* acord2);

      virtual void prepare();
      virtual void execute();

      virtual const char* className() const { return "AcordVectors"; }

    private:
      Acord2& AC;
      PointData & PD;
      ObservationData& OD;

      struct pvector
      {
        PointID from;
        PointID to;
        double  dx, dy, dz;
        bool    active;
      };

      void remove_vectors_between_known_xyz();

      std::vector<pvector> vectors_;
      GNU_gama::local::PointData lpd_;
    };

  }
}
#endif
