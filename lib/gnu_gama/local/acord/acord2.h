/*
  GNU Gama -- Approximate coordinates version 2
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

#ifndef GAMA_LOCAL_ACORD2_H_
#define GAMA_LOCAL_ACORD2_H_

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/acord/acordalgorithm.h>
#include <cstddef>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <utility>

namespace GNU_gama { namespace local {

    class Acord2
    {
    public:
      Acord2(PointData&, ObservationData&);
      std::pair<std::size_t, std::size_t> execute();

    private:
      using size_type = std::size_t;

      friend class AcordAlgorithm;
      friend class AcordAzimuth;
      friend class AcordZderived;
      friend class AcordHdiff;
      friend class AcordVector;
      friend class AcordPolar;
      friend class AcordTraverse;
      friend class AcordIntersection;
      friend class AcordWeakChecks;

      struct Point
      {
        PointID id;
        LocalPoint coords;

        Point(PointID pt_id_, LocalPoint pt_xy_)
          : id(std::move(pt_id_)), coords(pt_xy_)
        {}
        Point() : id(PointID()), coords(LocalPoint())
        {}
        bool operator< (const Point &p)	{ return (id <  p.id); }
        bool operator==(const Point &p)	{ return (id == p.id); }
      };

      StandPoint* find_standpoint(const PointID& pt);
      double median(std::vector<double>);
      std::pair<double,bool> get_dist(Observation* o);
      std::pair<double, bool> get_dir(Observation* o);
      bool in_missingXY(const PointID& pt);

      // tolerance for rejecting median of xy / z
      double median_max_norm_ = 0.1;

      bool slope_observations_ = false;
      bool has_azimuths_ = false;

      PointData&       PD_;
      ObservationData& OD_;
      std::vector<StandPoint*> SPClusters_;
      std::vector<HeightDifferences*> HDiffClusters_;
      std::vector<Vectors*> VectorsClusters_;
      std::set<PointID> missing_xy_;
      std::set<PointID> missing_z_;
      std::multimap<PointID,LocalPoint> candidate_xy_;
      std::multimap<PointID,double>     candidate_z_;
      std::multimap<PointID,Observation*> obs_from_;
      std::multimap<PointID,Observation*> obs_to_;
      bool get_medians();
      void get_medians_z();
      size_type new_points_xy_{}, new_points_z_{};

      using Traverse = std::vector<Point>;
      enum Traverse_type
        {
          open_traverse = 0,
          closed_start_traverse = 1,
          closed_traverse = 2
        };
      bool transform_traverse(Acord2::Traverse& traverse);

      std::vector<std::pair<Traverse, Traverse_type>> traverses;
      std::vector<std::shared_ptr<AcordAlgorithm>> algorithms_;

#ifdef DEBUG_ACORD2
      void debug_info(std::string text) const;
#endif
    };

  }} //namespace GNU_gama::local
#endif
