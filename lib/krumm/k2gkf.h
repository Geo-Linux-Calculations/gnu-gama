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

#ifndef K2GKF_H
#define K2GKF_H

#include <iostream>
#include <map>
#include <set>
#include <krumm/common.h>

namespace GNU_gama { namespace local {

class K2gkf
{
  void input();

public:
  K2gkf(std::istream &, std::ostream &);

  void run();
  int  dimension() { return dimension_; }
  int  error()     { return error_; }
  void set_error() { error_++; }
  void set_examples(bool b) { examples_ = b; }

  static std::string version();

private:

  std::istream& inp_;
  std::ostream& out_;
  bool examples_;

  int error_ {0};
  int dimension_{0};
};

}} // namespace GNU_gama::local

#endif // K2GKF_H
