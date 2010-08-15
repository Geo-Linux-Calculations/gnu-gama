/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>,
                        Jan Pytel  <pytel@gama.fsv.cvut.cz>

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

#ifndef gama_local_acord___ApproximateHeights_ApproxHeights___header___h
#define gama_local_acord___ApproximateHeights_ApproxHeights___header___h

#include <gnu_gama/local/gamadata.h>
#include <fstream>
#include <algorithm>
#include <list>

namespace GNU_gama { namespace local {


  class ApproximateHeights
    {
    private:

      struct ObservedHData {
        std::list<H_Diff*>     HD;
        std::list<H_Diff>      tmpHD;
        std::list<Distance*>   DI;
        std::list<S_Distance*> SD;
        std::list<Z_Angle*>    ZA;
      };

      void make_heights();

      int                 missing_heights;
      PointData&          PD;
      ObservationData&    OD;

      ObservedHData      OHD;

    public:

      ApproximateHeights(PointData& b, ObservationData& m);
      void execute();
      void print(std::ostream&);
    };

 }}   // namespace GNU_gama::local

#endif
