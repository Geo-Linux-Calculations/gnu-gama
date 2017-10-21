/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2006  Ales Cepek <cepek@gnu.org>

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

#ifndef GNU_gama_xml_localnetwork_gnugamaxmllocalnetwork_h
#define GNU_gama_xml_localnetwork_gnugamaxmllocalnetwork_h

#include <gnu_gama/local/gamadata.h>
#include <gnu_gama/local/network.h>
#include <gnu_gama/local/cluster.h>
#include <string>


namespace GNU_gama
{
  class LocalNetworkXML
  {
  public:

    LocalNetworkXML(GNU_gama::local::LocalNetwork* ln) : netinfo(ln) {}
    void write(std::ostream&) const;

  private:

    GNU_gama::local::LocalNetwork* netinfo;

    void coordinates_summary (std::ostream&) const;
    void observations_summary(std::ostream&) const;
    void equations_summary   (std::ostream&) const;
    void coordinates         (std::ostream&) const;
    void observations        (std::ostream&) const;

    void orientation_shifts(std::ostream&, std::vector<Index>&, Index&) const;
    void std_dev_summary(std::ostream&) const;
    template <typename T> void tagnl(std::ostream&, const char*, T) const;
    template <typename T> void tagsp(std::ostream&, const char*, T) const;
  };
}

#endif
