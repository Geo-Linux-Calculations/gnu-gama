/* Krumm2gama-local -- conversion from F. Krumm format to XML gama-local
   Copyright (C) 2022 Ales Cepek <cepek@gnu.org>

   This file is part of Krumm2gama-local.

   Krumm2gama-local is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   Krumm2gama-local is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Krumm2gama-local. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef K2GKF_COMMON_H
#define K2GKF_COMMON_H

#include <krumm/string_matrix.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <tuple>

class K2gkf;

class Common {
private:
    K2gkf* k2gkf;
public:

  Common(K2gkf* k) : k2gkf(k) {}
  void set_error();

  enum class State
  {
    unknown,
    project,
    source,
    coordinates,
    graphics,
    datum,
    sigma0,
    zenith_angles,
    spatial_distances,
    vertical_angles,
    angles,
    angles_dms,
    d3_base_line,
    directions,
    distances,
    approximate_orientation,
    levelled_height_differences,
    azimuth,
    azimuth_dms,
    grid_bearings_dms
  };

  State state {State::unknown};

  using Tuple = std::tuple<State,int>; // State, minimal project dimension

  std::map<std::string, Tuple> state_map{
    {"[Project]", Tuple(State::project,0)},
    {"[Quelle]", Tuple(State::source,0)},
    {"[Source]", Tuple(State::source,0)},
    {"[Coordinates]", Tuple(State::coordinates,0)},
    {"[Graphics]", Tuple(State::graphics,0)},
    {"[Datum]", Tuple(State::datum,0)},
    {"[Sigma0]", Tuple(State::sigma0,0)},
    {"[ZenithAngles]", Tuple(State::zenith_angles,3)},
    {"[SpatialDistances]", Tuple(State::spatial_distances,3)},
    {"[VerticalAngles]", Tuple(State::vertical_angles,3)},
    {"[Angles]", Tuple(State::angles,2)},
    {"[Angles,dms]", Tuple(State::angles_dms,2)},
    {"[Angles,dms,s]", Tuple(State::angles_dms,2)},
    {"[Winkel,dms,s]", Tuple(State::angles_dms,2)},
    {"[3DBasislinie]", Tuple(State::d3_base_line,3)},
    {"[3DBaseline]",   Tuple(State::d3_base_line,3)},
    {"[Direction]",  Tuple(State::directions,2)},
    {"[Directions]", Tuple(State::directions,2)},
    {"[Distances]", Tuple(State::distances,2)},
    {"[ApproximateOrientation]", Tuple(State::approximate_orientation,2)},
    {"[LevelledHeightDifferences]", Tuple(State::levelled_height_differences,1)},
    {"[Azimuth]", Tuple(State::azimuth,2)},
    {"[Azimuth,dms]", Tuple(State::azimuth_dms,2)},
    {"[GridBearings,dms,s]", Tuple(State::grid_bearings_dms,2)}
    // position angles not implemented
  };

  bool examples {false};

  int dimension {0};

  std::string description;
  std::string project, source;

  // implicit sigma values for angular and linear observations
  std::string s0_ang {"10"},    // implicit 10cc
              s0_lin {"10"};    // implicit 10mm
  std::string sigma0 {"10"};
  std::string distances_list, distances_sigma            {s0_lin};
  std::string spatial_dist_list, spatial_dist_sigma      {s0_lin};
  std::string vertical_angles_list, vertical_angles_sigma{s0_ang};
  std::string zenith_angles_list, zenith_angles_sigma    {s0_ang};
  std::string angles_list, angles_sigma                  {s0_ang};
  std::string azimuths_list, azimuths_sigma              {s0_ang};

  std::vector<std::string> vectors;

  //std::string datum {};
  std::vector<std::string> datum_tokens;

  StringMatrix datum_dyn_cov;
  //std::string  datum_dyn_cov_list;


  // points ................................................
  struct Point {
    std::string x, y, z;

    int dim {0};
    std::set<char> fix, adj;
    bool datum_dyn {false};
  };

  std::map<std::string, Point> point_map;

  // directions ............................................
  std::string directions_sigma {"0.0010"}; // gon

  struct DirectionSet {
    std::string orientation;
    std::vector<std::string> directions;
  };

  // levelled height differences ...........................
  std::string levell_hdiffs_list, levell_hdiffs_sigma{"0.010"};

  std::map<std::string, DirectionSet> direction_map;
};

std::string point(std::string id, const Common::Point& pt);
std::vector<std::string> get_tokens(std::string line);
std::string gon2cc(std::string);
std::string mgon2cc(std::string);
std::string m2mm(std::string);
std::string cm2mm(std::string);

#endif  // K2GKF_COMMON_H
