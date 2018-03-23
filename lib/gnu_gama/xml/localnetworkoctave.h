/*
  GNU Gama C++ library
  Copyright (C) 2018  Ales Cepek <cepek@gnu.org>

  This file is part of the GNU Gama C++ library
  
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

#ifndef GNU_gama_localnetwork_octave_h
#define GNU_gama_localnetwork_octave_h

#include <gnu_gama/local/network.h>


namespace GNU_gama
{
  class LocalNetworkOctave
  {
  public:

    LocalNetworkOctave(GNU_gama::local::LocalNetwork* ln) : netinfo(ln) {}
    void write(std::ostream&) const;

  private:

    GNU_gama::local::LocalNetwork* netinfo;
  };
}

#endif
