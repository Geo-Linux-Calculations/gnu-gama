/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2002,2003  Jan Pytel  <pytel@gama.fsv.cvut.cz>
                2018, 2021 Ales Cepek <cepek@gnu.org>

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
  along with GNU Gama.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef gama_local_acord_reduce_observations_h
#define gama_local_acord_reduce_observations_h

#include <gnu_gama/local/gamadata.h>
#include <fstream>
#include <list>
#include <cstddef>
#include <numeric>

namespace GNU_gama {
  namespace local {

    class ReducedObservations
    {
    public:

      ReducedObservations(PointData& b, ObservationData& m);

      void execute();
      void print(std::ostream&);

    private:

      enum Reduction {
        not_defined   = 0,  // not defined for the given observation type
        not_available = 1,  // data not available, cannot be evaluated
        approximate   = 2,
        exact         = 4
      };

      class ReducedObs {
      public:

        ReducedObs(Observation* obs_, Reduction type = not_defined)
          : ptr_obs(obs_), reduction (type) {
          orig_value_ = ptr_obs->value();
        }

        Observation*  ptr_obs;
        Reduction     reduction;

        double  orig_value() const {
          return orig_value_;
        }

        bool reduced() const {
          return reduction == approximate || reduction == exact;
        }

      private:
        double orig_value_;
      };


      PointData&            PD;
      ObservationData&      OD;
      std::list<ReducedObs> reduced_obs;
      ObservationList       list_obs;

    protected:

      void reduce(ReducedObs&);

      void reduce_sdistance( ReducedObs* );
      void reduce_zangle   ( ReducedObs* );

      size_t reduced_observations(size_t attrib) const
      {
        return std::count_if(reduced_obs.cbegin(), reduced_obs.cend(),
               [&attrib](ReducedObs r){ return r.reduction == attrib;});
      }

      size_t unreduced_observations() const
      {
        return std::count_if(reduced_obs.cbegin(), reduced_obs.cend(),
               [](ReducedObs r){ return r.reduction == not_available ||
                                        r.reduction == not_defined;});
      }
    };

  }
}   // namespace GNU_gama::local

#endif
