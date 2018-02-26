/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2002,2003  Jan Pytel  <pytel@gama.fsv.cvut.cz>
                  2018  Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local_acord_reduce_observations_h
#define gama_local_acord_reduce_observations_h

#include <gnu_gama/local/gamadata.h>
#include <fstream>
#include <list>
#include <cstddef>

namespace GNU_gama { namespace local {

  struct TypeOfZAngles;

  class ReducedObservations
    {
    public:

        enum TypeOfReduction {
            none_     = 1,
            approx_   = 2,
            precise_  = 4,
            nonexist_ = 8
        };

        class ReducedObs
        {
        public:

            ReducedObs(Observation* obs_, TypeOfReduction type_ = none_)
                : ptr_obs(obs_),type_of_reduction(type_)
            {
                orig_value_ = ptr_obs->value();
            }

            Observation*        ptr_obs;
            TypeOfReduction     type_of_reduction;

            Double  orig_value() const
            {
                return orig_value_;
            }

            bool reduced() const
                {
                    return !( type_of_reduction & (none_ | nonexist_) );
                }

        private:
            friend class ReducedObservations;
            Double orig_value_;
        };

        typedef std::list<ReducedObs> ListReducedObs;
        typedef std::list<ReducedObs>::iterator ListReducedObs_iter;
        typedef std::list<ReducedObs>::const_iterator ListReducedObs_c_iter;

        ListReducedObs_iter begin()
        {
            return list_reduced_obs.begin();
        }

        ListReducedObs_iter end()
        {
            return list_reduced_obs.end();
        }

        ListReducedObs_c_iter begin() const
        {
            return list_reduced_obs.begin();
        }

        ListReducedObs_c_iter end()const
        {
            return list_reduced_obs.end();
        }

    private:

        ReducedObs* giveReducedObs(const Observation* obs_)
        {
            for (ListReducedObs_iter i  = list_reduced_obs.begin();
                                     i != list_reduced_obs.end(); ++i)
                if (i->ptr_obs == obs_)
                    return &(*i);
            return 0;
        }


        struct RemoveNonActiveObs
        {
            bool operator()(const ReducedObs& red_obs)
            {
                return !red_obs.ptr_obs->active();
            }
        };


        PointData&          PD;
        ObservationData&    OD;

        ListReducedObs  list_reduced_obs;
        ObservationList list_obs;

    protected:

        void reduce(ReducedObs&);

        void reduce_sdistance( ReducedObs* );
        void reduce_zangle   ( ReducedObs* );

        size_t number_of_reduced_observations_with_attribute(size_t attrib) const
        {
            if ( ! list_reduced_obs.size() )
                return 0;

            size_t number = 0;

            for (ListReducedObs_c_iter ci  = list_reduced_obs.begin();
                 ci != list_reduced_obs.end(); ++ci)
                if ( ci->type_of_reduction & attrib )
                    number++;

            return number;
        }

        size_t number_of_not_reduced_observations() const
        {
            return number_of_reduced_observations_with_attribute(none_ | approx_);
        }

    public:

        ReducedObservations(PointData& b, ObservationData& m);

        size_t size() const
        {
            return list_reduced_obs.size();
        }

        size_t size_nonexist() const
        {
            return number_of_reduced_observations_with_attribute( nonexist_ );
        }

        void execute();
        void print(std::ostream&);
  };

}}   // namespace GNU_gama::local

#endif
