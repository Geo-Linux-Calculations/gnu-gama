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

#ifndef K2GKF_INPUT_H
#define K2GKF_INPUT_H

#include <krumm/common.h>
#include <istream>

namespace GNU_gama { namespace local {

class Input {
public:
  Input(std::istream& input, Common& com);

  void input();

private:
  void coordinates(std::string line);
  void datum(std::string line);
  void datum_dyn();
  void spatial_distances(std::string line);
  void vertical_angles(std::string line);
  void zenith_angles(std::string line);
  void angles(std::string line, bool dms=false);
  void angles_dms(std::string line);
  void vectors(std::string line);
  void directions(std::string line);
  void distances(std::string line);
  void approximate_orientation(std::string line);
  void levelled_height_differences(std::string line);
  void azimuths(std::string line, bool dms=false);
  void azimuths_dms(std::string line);
  void grid_bearings_dms(std::string line);
  void sigma_0(const std::vector<std::string>& tokens);

  std::string utf8_to_dms(std::string line);
  std::vector<std::string> tokenize(std::string);

  std::istream& inp_;
  Common& common;
};

}} // namespace GNU_gama::local

#endif  // K2GKF_INPUT_H
