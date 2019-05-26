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

#ifndef GAMA_LOCAL_ACORDTRAVERSE_H
#define GAMA_LOCAL_ACORDTRAVERSE_H

#include <set>

#include <gnu_gama/local/acord/acordalgorithm.h>
#include <gnu_gama/local/acord/acord2.h>
#include <gnu_gama/local/gamadata.h>


namespace GNU_gama { namespace local {

    class Acord2;
    
    class AcordTraverse final : public AcordAlgorithm
    {
    public:
      AcordTraverse(Acord2* acord2);
      void execute();
      virtual void prepare() {} /* to be implmented later */

      virtual const char* className() const { return "AcordTraverse"; }

    private:
      friend class AcordWeakChecks;

      Acord2& AC;
      PointData & PD;
      ObservationData& OD;


      Acord2::Traverse_type tr_type = AC.open_traverse;
      Acord2::Traverse traverse;

      std::set<PointID>    candidate_traverse_points_;
      std::vector<PointID> traverse_points_;

      std::set<PointID> get_neighbours(PointID pt);
      bool  candidateTraversePoint(PointID);
      void get_traverse_pts(PointID pt);
      Acord2::Traverse_type get_connecting_points();

      std::set<PointID> add_same_points();

      bool calculate_traverse();
    };

  }} //namespace GNU_gama::local
#endif
