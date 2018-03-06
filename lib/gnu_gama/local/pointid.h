/*
  GNU Gama -- adjustment of geodetic networks
  Copyright (C) 2000  Ales Cepek <cepek@fsv.cvut.cz>,
                2001  Ales Cepek <cepek@fsv.cvut.cz>,
                      Jan Pytel  <pytel@gama.fsv.cvut.cz>
                2011, 2014, 2018  Ales Cepek <cepek@gnu.org>

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

// PointID - point identification

#ifndef gama_local_Point_Identification_h
#define gama_local_Point_Identification_h

#include <string>
#include <cstddef>

namespace GNU_gama { namespace local
{

  class PointID
    {
    public:

      PointID()                     { init(std::string("")); }
      PointID(const char* c)        { init(std::string(c));  }
      PointID(const std::string& s) { init(s); }

      bool operator==(const PointID& p) const;
      bool operator!=(const PointID& p) const;
      bool operator< (const PointID& p) const;

      std::string str() const { return sid; }
      std::size_t lengthUtf8() const;

    private:

      using PointInt = long;
      PointInt     iid;   // positive integer representation if available or 0
      std::string  sid;

      void init(const std::string& s);
    };


  inline std::ostream& operator<<(std::ostream& ostr, const PointID& p)
    {
      return ostr << p.str();
    }

}}      // namespace GNU_gama::local

#endif
