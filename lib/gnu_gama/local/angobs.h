/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001, 2012  Ales Cepek <cepek@gnu.org>

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

#ifndef gama_local____Angular_Observations__h____
#define gama_local____Angular_Observations__h____


namespace GNU_gama { namespace local {

  class AngularObservations {
  public:

  AngularObservations()
      : left_handed_(true)
      {
      }

    void setAngularObservations_Lefthanded () { left_handed_ = true;  }
    void setAngularObservations_Righthanded() { left_handed_ = false; }

    bool left_handed_angles () const { return  left_handed_; }
    bool right_handed_angles() const { return !left_handed_; }

  private:
    bool left_handed_;
  };

}}   // namespace GNU_gama::local


#endif

















