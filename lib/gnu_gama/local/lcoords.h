/*
  GNU Gama C++ library
  Copyright (C) 2001, 2019  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Local_Coordinate_System_h_
#define gama_local_Local_Coordinate_System_h_


namespace GNU_gama { namespace local {

  class LocalCoordinateSystem {
  public:

    enum class CS
      {                    // orientation of axes x and y :
        EN, NW, SE, WS,    //   plane right-handed  systems
        NE, SW, ES, WN     //   plane left-handed systems
      }
      local_coordinate_system;

    LocalCoordinateSystem(CS cs=CS::NE)
      : local_coordinate_system(cs)
      {
      }

    bool right_handed_coordinates() const
      {
        return local_coordinate_system < CS::NE;
      }

    bool left_handed_coordinates () const
      {
        return local_coordinate_system > CS::WS;
      }

    const char* locos_str() const
      {
        return locos_tags[unsigned(local_coordinate_system)];
      }

    static const char* locos_tags[8];
    //  {
    //    "en", "nw", "se", "ws", "ne", "sw", "es", "wn"
    //  };

  };

}}   // namespace GNU_gama::local

#endif
