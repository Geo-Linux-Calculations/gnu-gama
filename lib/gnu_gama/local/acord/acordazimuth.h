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

#ifndef GNU_GAMA_LOCAL_ACORDAZIMUTH_H_
#define GNU_GAMA_LOCAL_ACORDAZIMUTH_H_

#include <map>
#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama {
  namespace local {

    class AcordAzimuth final : public AcordAlgorithm
    {
    public:
      AcordAzimuth(Acord2* acord2);
      
      virtual void prepare();
      virtual void execute();

    private:
      Acord2& AC;
      PointData & PD;
      ObservationData& OD;
      
      struct azimuth
      {
        double value;
        double distance {0};     // zero if no distance is available
        
        std::vector<double> values;
      };
      
      std::map<std::pair<PointID,PointID>, azimuth> azimuths_;

      void remove_azimuths_between_known_xy();

      void dbg(std::string s);
    };
    
  }
}
#endif
