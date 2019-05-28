/*
  GNU Gama -- Approximate coordinates by Polar Method
  Copyright (C) 2018  Petra Millarova <petramillarova@gmail.com>

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

#ifndef GAMA_LOCAL_ACORDPOLAR_H
#define GAMA_LOCAL_ACORDPOLAR_H

#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>

namespace GNU_gama { namespace local
  {
    class AcordPolar final : public AcordAlgorithm
    {
    public:
      AcordPolar(Acord2* acord2);

      virtual void execute();
      virtual void prepare() {} /* to be implmented later */

      virtual const char* className() const { return "AcordPolar"; }

      void enable_slope_observations()  { AC.slope_observations_ = true; }
      void enable_weak_checks()         { weak_checks_ = true; }
      void disable_slope_observations() { AC.slope_observations_ = false; }
      void disable_weak_checks()        { weak_checks_ = false; }

    private:
      Acord2& AC;
      PointData & PD;
      ObservationData& OD;

      bool weak_checks_ = false;

      struct Measurement
      {
        PointID standpoint;
        PointID ori;
        double dir;
        double dist;
        StandPoint* sp; //for orientation shift

        Measurement(PointID standpoint_val, PointID ori_val,
                    double dir_val, double dist_val, StandPoint* sp_val)
          : standpoint(standpoint_val), ori(ori_val),
            dir(dir_val), dist(dist_val), sp(sp_val)
        {
        }
      };

      bool points_from_SPCluster(StandPoint* sp);
      LocalPoint calculate_polar(Measurement m);
      bool stub(PointID p);

      int execute_counter_ {0};
    };

 }} //namespace GNU_gama::local
#endif
