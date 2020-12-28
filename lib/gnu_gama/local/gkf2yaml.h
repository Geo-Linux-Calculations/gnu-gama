/* Gkf2yaml --- conversion from yaml to gkf input format
   Copyright (C) 2020 Ales Cepek <cepek@gnu.org>

   This file is part of the GNU Gama C++ library.

   Class Gkf2yaml is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Gkf2yaml is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU Gama.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef gama_local_Gkf2yaml_h_
#define gama_local_Gkf2yaml_h_

#include <iostream>
#include <string>
#include <gnu_gama/xml/gkfparser.h>

namespace GNU_gama { namespace local {

class Gkf2yaml {
public:
  Gkf2yaml(GNU_gama::local::GKFparser& gkfp, std::ostream& ostr);

  void run();

private:
  GNU_gama::local::GKFparser* gkfparser_;
  GNU_gama::local::LocalNetwork* locnet_;
  std::ostream& ostream_;

  void description();
  void defaults();
  void points();
  void observations();

  void stand_point(StandPoint*);
  void stand_point_obs(Observation*);
  void height_differences(HeightDifferences*);
  void vectors(Vectors*);
  void coordinates(Coordinates*);
  void cov_mat(const GNU_gama::local::Observation::CovarianceMatrix&,
               std::string::size_type indent);
};

}}  // namespace GNU_gama::local

#endif
