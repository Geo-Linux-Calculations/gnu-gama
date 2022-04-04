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

#ifndef K2GKF_OUTPUT_H
#define K2GKF_OUTPUT_H

#include <krumm/common.h>
#include <krumm/k2gkf.h>
#include <ostream>

namespace GNU_gama { namespace local {

class Output {
public:
  Output(std::ostream& output, Common& com);

  void output();
  void xml_head();
  void description();
  void parameters();
  void points_observations();
  void points();
  void spatial_distances();
  void vertical_angles();
  void zenith_angles();
  void vectors();
  void directions();
  void distances();
  void angles();
  void levelled_hdiffs();
  void datum_dyn();
  void azimuths();

  void out_xml_tail();
private:
  std::ostream& out_;
  Common& common;
};

}}  // namespace GNU_gama::local

#endif  // K2GKF_OUTPUT_H
