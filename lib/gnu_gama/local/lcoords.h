/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

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

#ifndef gama_local_Local_Coordinate_System_h_
#define gama_local_Local_Coordinate_System_h_


namespace GNU_gama { namespace local {

  class LocalCoordinateSystem {
  public:

    enum CS
    {
                                            // orientation of axes x and y :
      EN= 1, NW= 2, SE= 4, WS=  8,          //   plane right-handed  systems
      NE=16, SW=32, ES=64, WN=128,          //   plane left-handed systems

      right_handed = (EN | NW | SE | WS),
      left_handed  = (NE | SW | ES | WN)

    } local_coordinate_system;

    LocalCoordinateSystem(CS cs=NE)
      : local_coordinate_system(cs)
      {
      }
    bool right_handed_coordinates() const
      {
        return local_coordinate_system & right_handed;
      }
    bool left_handed_coordinates () const
      {
        return local_coordinate_system & left_handed;
      }

  };

}}   // namespace GNU_gama::local



#endif

















