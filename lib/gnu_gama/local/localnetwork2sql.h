/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2010, 2012, 2014  Ales Cepek <cepek@gnu.org>

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  $
*/

#ifndef GNU_gama_localnetwork_2_sql__gnugamalocalnetwork2sql_h
#define GNU_gama_localnetwork_2_sql__gnugamalocalnetwork2sql_h

#include <istream>
#include <string>

#include <gnu_gama/local/network.h>

namespace GNU_gama { namespace local
{
  class LocalNetwork2sql {
  public:

    LocalNetwork2sql(LocalNetwork& lnet);

    void readGkf(std::istream& istr);
    void write  (std::ostream& ostr, std::string conf);
    void setDelete(bool del) { delrec = del;  }
    bool getDelete() const   { return delrec; }

  private:
    GNU_gama::local::LocalNetwork&    localNetwork;
    GNU_gama::local::PointData&       points;
    GNU_gama::local::ObservationData& observations;
    std::string config;
    bool        delrec;

    typedef GNU_gama::Cluster<GNU_gama::local::Observation> Cluster;

    void write_cluster(std::ostream& ostr, const Cluster* c,
                       int cluster, std::string tag);
    int  rejected(const GNU_gama::local::Observation* m) const
      {
        return m->active() ? 0 : 1;
      }
    std::string cnfg() const
      {
        return
          "(select new_id from (select conf_id as new_id"
          " from gnu_gama_local_configurations"
          " where conf_name='" + config + "')x)";
      }
  };

}}

#endif
