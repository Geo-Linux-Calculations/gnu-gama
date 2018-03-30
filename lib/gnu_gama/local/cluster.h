/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Cluster_of_observations_h
#define gama_local_Cluster_of_observations_h

#include <gnu_gama/obsdata.h>

#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/matvec.h>
#include <vector>
#include <cmath>

namespace GNU_gama { namespace local {


  // simple observation types: horizontal directions, distances and
  // angles


  typedef GNU_gama::ObservationData<Observation> ObservationData;


  class StandPoint : public GNU_gama::Cluster<Observation> {
  public:

    PointID  station;

    StandPoint(const ObservationData* od)
      :
      GNU_gama::Cluster<Observation>(od),
      test_or(false), indx_or(0)
      {
      }

    StandPoint* clone(const ObservationData*p) const
      {
        return new StandPoint(p);
      }

    double orientation() const
      {
        if (!test_or) throw GNU_gama::local::Exception(T_POBS_bad_data);
        return attr_or;
      }
    void   set_orientation(double p) { attr_or = p; test_or = true; }
    bool   test_orientation() const  { return test_or;              }
    void   delete_orientation()      { test_or = false;             }
    void   index_orientation(int n)  { indx_or = n;                 }
    int    index_orientation() const { return indx_or;              }

  private:

    double   attr_or;
    bool     test_or;
    int      indx_or;

  };


  // coordinate observations (observed coordinates) x, y, z

  class Coordinates : public GNU_gama::Cluster<Observation> {
  public:

    Coordinates(const ObservationData* od)
      : GNU_gama::Cluster<Observation>(od)
      {
      }
    Coordinates* clone(const ObservationData*p) const
      {
        return new Coordinates(p);
      }
  };


  // height differences (leveling)

  class HeightDifferences : public GNU_gama::Cluster<Observation> {
  public:

    HeightDifferences(const ObservationData* od)
      : GNU_gama::Cluster<Observation>(od)
      {
      }
    HeightDifferences* clone(const ObservationData*p) const
      {
        return new HeightDifferences(p);
      }
  };


  // vectors (coordinate differences  diff_x, diff_y, diff_z)

  class Vectors : public GNU_gama::Cluster<Observation> {
  public:

    Vectors(const ObservationData* od)
      : GNU_gama::Cluster<Observation>(od)
      {
      }
    Vectors* clone(const ObservationData*p) const
      {
        return new Vectors(p);
      }
  };


}}   // namespace GNU_gama::local

#endif









